#pragma once

#include <thread>
#include <functional>
#include <vector>
#include <mutex>

using namespace std;

class ThreadManager
{
public:
	ThreadManager();
	~ThreadManager();

	void Launch(function<void(void)> callback);
	void Join();

	void InitTLS();

private:
	mutex			lock;
	vector<thread>	threads;
};
