#include <string>
#include "Utils.h"
#include "../slstrand.h"
#include "Semaphore.h"

#include <assert.h>
#include <map>
#include <iostream>
#include <atomic>
#include "../slstrand_work_queue.h"

//#include <windows.h>
#include "Remotery/lib/Remotery.h"
#include <random>

long ranged_rand(long a, long b)
{
	return a + random_at_most(b - a);
}

struct ThreadInfo
{
	std::string name;
	std::thread th;
	double totalTime = 0;
	double totalWork = 0;
	double totalBlocked = 0;
};

Spinner gSpinner;

struct Foo {
	explicit Foo(int n, const char* colour, SLStrandWorkQueue& wq) : strand(&wq)
	{
		name = "Conn " + std::to_string(n);
		rmt_SetColour(name.c_str(), colour);
	}
	void doWorkLocked(int durationMs) {
		using namespace std::chrono;

		auto workStart = nowMs();
		auto th = SLCallStack<ThreadInfo>::top()->key();

		rmt_BeginCPUSampleDynamic(name.c_str());

		// Do the blocking, and time it
		auto blockingStart = nowMs();
		rmt_BeginCPUSample(Blocked);
		mtx.lock();
		rmt_EndCPUSample();
		auto blockingEnd = nowMs();

		// Work
		rmt_BeginCPUSample(Work);
		gSpinner.spinMs(durationMs);
		mtx.unlock();
		rmt_EndCPUSample();

		rmt_EndCPUSample();
		auto workEnd = nowMs();

		auto blocked = blockingEnd - blockingStart;
		auto work = (workEnd - workStart) - blocked;
		totalWork += work;
		totalBlocked += blocked;
		th->totalWork += work;
		th->totalBlocked += blocked;
	}

	void doWorkUnlocked(int durationMs)
	{
		using namespace std::chrono;
		auto workStart = nowMs();
		auto th = SLCallStack<ThreadInfo>::top()->key();

		rmt_BeginCPUSampleDynamic(name.c_str());

		// Work
		rmt_BeginCPUSample(Work);
		gSpinner.spinMs(durationMs);
		rmt_EndCPUSample();

		rmt_EndCPUSample();
		auto workEnd = nowMs();

		auto work = (workEnd - workStart);
		totalWork += work;
		th->totalWork += work;
	}

	std::mutex mtx;
	std::string name;
	double totalWork = 0;
	double totalBlocked = 0;

	SLStrand strand;
};


// What is available to run
#define WHAT_NOSTRANDS 1
#define WHAT_STRANDS 2
#define WHAT_SAMPLE 3

// Set this to one of the above, to specify what code to run 
#define WHAT WHAT_STRANDS

#define NUM_THREADS 4
#define NUM_OBJECTS 8
#define WORKDURATION_MIN 5
#define WORKDURATION_MAX 15

// Distinct colours generated with http://phrogz.net/css/distinct-colors.html
#define NUM_COLOURS 44
const char* distinctColours[44] = {
    "#005dff", "#ff001a", "#bb00ff", "#04ff00", "#40fcff", "#00f281", "#f28d79",
    "#f279a6", "#c3b6f2", "#e600bb", "#2200e6", "#e58a00", "#e3e639", "#73bbe6",
    "#b5e6ac", "#007ed9", "#d94c00", "#d93654", "#cc9766", "#8b66cc", "#99ccbd",
    "#b28c00", "#989973", "#23288c", "#77698c", "#730080", "#006f80", "#048000",
    "#7f004d", "#7f4120", "#205680", "#806067", "#00735e", "#736056", "#664b00",
    "#660a00", "#50661a", "#34004d", "#003c40", "#403038", "#331a00", "#330d15",
    "#1a1e33", "#1d331a"};

void strandSample();

int main()
{
#if WHAT==WHAT_SAMPLE
	strandSample();
	return 0;
#endif

	time_t t;
	srand((unsigned)time(&t));
	//timeBeginPeriod(1);

    Remotery* rmt;
    rmt_CreateGlobalInstance(&rmt);
	rmt_SetCurrentThreadName("MainThread");

	SLStrandWorkQueue wq;

	std::vector<ThreadInfo> ths;
	using namespace cz;
	ZeroSemaphore threadsReady;
	ZeroSemaphore threadsRunning;

	ths.resize(NUM_THREADS);
	for (int i = 0; i < NUM_THREADS; i++)
	{
		threadsReady.increment();
		threadsRunning.increment();
		ths[i].th = std::thread([&wq, &threadsReady, &threadsRunning, this_=&ths[i], i]
		{
			this_->name = formatStr("WorkerThread %d", i);
			rmt_SetCurrentThreadName(this_->name.c_str());
			threadsReady.decrement();
			auto start = nowMs();
			SLCallStack<ThreadInfo>::Context ctx(this_);
			wq.run();
			threadsRunning.decrement();
			this_->totalTime = nowMs() - start;
		});
	}

	std::vector<std::unique_ptr<Foo>> objs;
	for (int i = 0; i < NUM_OBJECTS; i++)
	{
		objs.push_back(std::make_unique<Foo>(i, distinctColours[i % NUM_COLOURS], wq));
	}

	rmt_SetColour("Blocked", "#800000");
	rmt_SetColour("Work", "#008000");

	int totalWorkMs = 0;

	std::atomic<int> itemsDone(0);
	int itemsTodo = 1000;

	// Prepare items beforehand, so it doesn't interfere with the actual work timing
	std::vector<std::function<void()>> items;
	for (int i = 0; i < itemsTodo; i++)
	{
		auto& obj = objs[ranged_rand(0, objs.size() - 1)];
		int ms = ranged_rand(WORKDURATION_MIN, WORKDURATION_MAX);
		totalWorkMs += ms;
		items.push_back([&obj, ms, &itemsDone]{
#if WHAT==WHAT_STRANDS
			obj->strand.dispatch([&obj, ms, &itemsDone] {
				itemsDone++;
				obj->doWorkUnlocked(ms);
			});
#else
			itemsDone++;
			obj->doWorkLocked(ms);
#endif
		});
	}

	threadsReady.wait();
	auto start = nowMs();
	for (auto&& item : items)
		wq.push(std::move(item));
	wq.stop();
	threadsRunning.wait();
	auto end = nowMs();

	double mainThreadTotalTime = end - start;
	assert(itemsDone.load() == itemsTodo);

	for (auto&& t : ths)
		t.th.join();

	double totalWork = 0;
	double totalBlocked = 0;
	for(auto& obj : objs)
	{
		totalWork += obj->totalWork;
		totalBlocked += obj->totalBlocked;
		printf("%s: totalWork=%5.2f totalBlocked=%5.2f . BlockingOverhead=%3.2f\n",
			obj->name.c_str(), obj->totalWork, obj->totalBlocked, (obj->totalBlocked * 100) / (obj->totalWork + obj->totalBlocked));
	}

	{
		printf("THREADS\n");
		double totalWork = 0;
		double totalBlocked = 0;
		double totalTime = 0;
		for (auto& t : ths)
		{
			totalWork += t.totalWork;
			totalBlocked += t.totalBlocked;
			totalTime += t.totalTime;
			printf("\t%s: totalTime=%5.2f totalWork=%5.2f totalOverhead=%3.2f%%\n",
				t.name.c_str(), t.totalTime, t.totalWork, ((t.totalTime-t.totalWork) * 100) / (t.totalTime));
		}
		printf("\tTOTAL: totalTime=%5.3f totalWork=%5.3f totalOverhead=%3.3f%%\n",
			totalTime, totalWork, ((totalTime-totalWork) * 100) / (totalTime));
	}

	double totalOverhead = (1 - ((totalWork / NUM_THREADS) / mainThreadTotalTime)) * 100;
	printf("MAINTHREAD: totaTime=%5.2f totalWork=%5.2f totalOverhead=%3.2f%%\n",
		mainThreadTotalTime, totalWork, totalOverhead);

	CSLEEP(3000);
	printf("Done\n");
	rmt_DestroyGlobalInstance(rmt);
}
