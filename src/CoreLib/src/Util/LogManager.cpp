#include "LogManager.h"

void LogObj::Init()
{
	auto now = std::chrono::system_clock::now();
	std::time_t t = std::chrono::system_clock::to_time_t(now);
	std::tm bt;

#ifdef linux
	localtime_r(&t, &bt);
#elif _WIN32
	localtime_s(&bt, &t);
#endif
	strftime(content, 12, "[%H:%M:%S] ", &bt);
	size = 11;
}

bool LogObj::Add(const string& s)
{
	try
	{
#ifdef linux
		strcpy(&content[size], s.c_str());
#elif _WIN32
		strcpy_s(&content[size], s.size() + 1, s.c_str());
#endif
	}
	catch (...)
	{
		return false;
	}
	size += s.size();
	return true;
}

void LogManager::PrintLog(shared_ptr<LogObj> log)
{
	std::cout << log->content << std::endl;
}