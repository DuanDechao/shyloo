#include "../slcall_stack.h"
struct StrandTest{
public:
	template<typename F>
	void run(F f){
		SLCallStack<StrandTest>::Context ctx(this);
		f();
	}
};

StrandTest test;

void func(){
	printf("%s\n", SLCallStack<StrandTest>::contain(&test) ? "true" : "false");
}

int main(){
	func();			// false
	test.run(func);	//true
	return 0;
}
