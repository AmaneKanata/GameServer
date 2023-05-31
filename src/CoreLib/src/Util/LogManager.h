#pragma once

#include <chrono>
#include <ctime>
#include <iomanip>
#include <memory>

#include "../Job/JobQueue.h"

using namespace std;

#define LOG_SIZE 200

class LogObj
{
public:
	LogObj() : size(0) {}

	void Init();
	bool Add(const string& s);

public:
	char content[LOG_SIZE];
	int size;
};

class LogManager : public JobQueue
{
public:
	LogManager(boost::asio::io_context& ioc) : JobQueue(ioc)
	{}

private:
	void AppendToString(shared_ptr<LogObj> log) { return; }

	template <typename String, typename... Strings>
	void AppendToString(shared_ptr<LogObj> log, const String& s, Strings... strs) {
		log->Add(s);
		AppendToString(log, strs...);
	}

public:
	template <typename String, typename... Strings>
	void Log(const String& s, Strings... strs)
	{
		auto log = make_shared<LogObj>();
		log->Init();
		AppendToString(log, s, strs...);

		//log manager 는 싱글톤이라서 this 로 해도 문제없다?
		jobs.post(std::bind(&LogManager::PrintLog, this, log));
	}

	void PrintLog(shared_ptr<LogObj> log);
};