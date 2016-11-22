#ifndef _SL_EVENT_DISPATCHER_H_
#define _SL_EVENT_DISPATCHER_H_
#include "sltask.h"
#include "slinterfaces.h"
#include "sltimer.h"
#include "sltasks.h"
#include "sltimestamp.h"
namespace sl
{
namespace network
{
class DispatcherCoupling;
class ErrorReporter;
class EventPoller;

class EventDispatcher
{
public:
	enum EVENT_DISPATCHER_STATUS
	{
		EVENT_DISPATCHER_STATUS_RUNNING = 0,
		EVENT_DISPATCHER_STATUS_WAITING_BREAK_PROCESSING = 1,
		EVENT_DISPATCHER_STATUS_BREAK_PROCESSING = 2
	};

	EventDispatcher();
	virtual ~EventDispatcher();

	int processOnce(bool shouldIdle = false);
	void processUntilBreak();

	inline bool hasBreakProcessing() const;
	inline bool waitingBreakProcessing() const;

	void breakProcessing(bool breakState = true);
	inline void setWaitBreakProcessing();

	void addTask(Task* pTask);
	bool cancelTask(Task* pTask);

	inline double maxWait() const;
	inline void maxWait(double seconds);

	bool registerReadFileDescriptor(int fd, InputNotificationHandler* handler);
	bool deregisterReadFileDescriptor(int fd);
	bool registerWriteFileDescriptor(int fd, OutputNotificationHandler* handler);
	bool deregisterWriteFileDescriptor(int fd);

	inline TimerHandle addTimer(int64 microseconds, TimerHandler* handler, void* arg = NULL);

	uint64 getSpareTime() const;
	void clearSpareTime();

	ErrorReporter& errorReporter() {return *m_pErrorReporter;}

	inline EventPoller* createPoller();
	EventPoller* pPoller() {return m_pPoller;}

	int processNetwork(bool shouldIdle);

private:
	TimerHandle addTimerCommon(int64 microseconds, TimerHandler* handler, void* arg,
		bool recurrent);

	void processTasks();
	void processTimers();
	void processStats();

	double calculateWait() const;
protected:
	int8			m_breakProcessing;
	double			m_maxWait;
	uint32			m_numTimeCalls;

	///statistics
	TimeStamp		m_accSpareTime;
	TimeStamp		m_oldSpareTime;
	TimeStamp		m_totSpareTime;
	TimeStamp		m_lastStatisticsGathered;

	Tasks*			m_pTasks;
	ErrorReporter*	m_pErrorReporter;
	Timers64*		m_pTimers;
	EventPoller*	m_pPoller;

};
}
}
#include "slevent_dispatcher.inl"
#endif