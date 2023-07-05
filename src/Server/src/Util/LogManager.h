#pragma once

#include <JobQueue.h>
#include <iomanip>
#include <iostream>
#include <ctime>
#include <sstream>

class LogManager : public JobQueue
{
private:
	void AppendToString(std::stringstream& ss) { return; }

	template <typename String, typename... Strings>
	void AppendToString(std::stringstream& ss, String&& s, Strings&&... strs) {
		ss << s;
		AppendToString(ss, strs...);
	}

public:
	LogManager(boost::asio::io_context& ioc) : JobQueue(ioc)
	{}

	template <typename String, typename... Strings>
	void Log(String&& s, Strings&&... strs)
	{
		std::stringstream ss;

		auto currentTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
		std::tm bt;
#ifdef linux
		currentTime += 32400;
		localtime_r(&currentTime, &bt);
#elif _WIN32
		localtime_s(&bt, &currentTime);
#endif
		ss << "[" << std::put_time(&bt, "%H:%M:%S") << "] ";

		AppendToString(ss, s, strs...);

		Post(&LogManager::PrintLog, std::move(ss.str()));
	}

private:
	void PrintLog(std::string log)
	{
		std::cout << log << "\n";
	}
};