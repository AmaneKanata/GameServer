#pragma once

#include "Job.h"

#include <queue>
#include <memory>
#include <atomic>
#include <boost/thread.hpp>

using namespace std;

class JobQueue;

struct JobData
{
	JobData(weak_ptr<JobQueue> owner, shared_ptr<Job> job) : owner(owner), job(job)
	{
	}

	weak_ptr<JobQueue>	owner;
	shared_ptr<Job>	job;
};

struct TimerItem
{
	bool operator<(const TimerItem& other) const
	{
		return executeTick > other.executeTick;
	}

	unsigned long long executeTick = 0;
	JobData* jobData = nullptr;
};

class JobTimer
{
public:
	void Reserve(unsigned long long tickAfter, weak_ptr<JobQueue> owner, shared_ptr<Job> job);
	void Distribute(unsigned long long now);
	void Clear();

private:
	mutex mtx;
	priority_queue<TimerItem> items;
	atomic<bool> distributing = { false };
};
