#include "Slave.h"
#include "sltools.h"
int32 Slave::StartNode(sl::api::IKernel* pKernel, const char* cmd){
	char process[256];
#ifdef SL_OS_WINDOWS
	SafeSprintf(process, sizeof(process)-1, "%s/shyloo.exe", sl::getAppPath());
	STARTUPINFO si = { sizeof(si) };
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.lpTitle = (char*)cmd;
	PROCESS_INFORMATION pi;
	BOOL ret = CreateProcess(process, (char*)cmd, nullptr, nullptr, false, CREATE_NEW_CONSOLE, nullptr, nullptr, &si, &pi);
	SLASSERT(ret, "create process failed");
	::CloseHandle(pi.hThread);
	::CloseHandle(pi.hProcess);
	return 0;
#endif

#ifdef SL_OS_LINUX
	//SafeSprintf(process, sizeof(process), "%s/%s", sl::getAppPath(), EXECUTE_NAME);

#endif
}