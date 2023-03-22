#include "AppInfo.h"
#include "OSDetection.h"

std::string get_app_version() {
	return "1.0.0";
}

std::string get_app_dir(const char* firstArg) {
	std::string dir = "";

#if defined (OS_WIN)
	char buf[MAX_PATH] = {0};
	// the result of "GetModuleFileNameA" is not null-terminated, so it is good to rezero buf everytime we use it;
	memset((void*)buf, 0, MAX_PATH);
	GetModuleFileNameA(NULL, buf, MAX_PATH);
	dir = buf;
#endif

	return dir;
}