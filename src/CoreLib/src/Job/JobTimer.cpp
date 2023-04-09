#include "../pch.h"
#include "JobTimer.h"
#include "JobQueue.h"

void JobTimer::Reserve(unsigned long long tickAfter, weak_ptr<JobQueue> owner, shared_ptr<Job> job)
{
#ifdef linux
	const unsigned long long executeTick = GetTickCount() + tickAfter;
#elif _WIN32
	const unsigned long long executeTick = ::GetTickCount64() + tickAfter;
#endif
	JobData* jobData = new JobData(owner, job);

	lock_guard<mutex> lock(mtx);

	items.push(TimerItem{ executeTick, jobData });
}

void JobTimer::Distribute(unsigned long long now)
{
	if (distributing.exchange(true) == true)
		return;

	vector<TimerItem> _items;

	{
		lock_guard<mutex> lock(mtx);

		while (items.empty() == false)
		{
			const TimerItem& timerItem = items.top();
			if (now < timerItem.executeTick)
				break;

			_items.push_back(timerItem);
			items.pop();
		}
	}

	for (TimerItem& item : _items)
	{
		if (shared_ptr<JobQueue> owner = item.jobData->owner.lock())
			owner->Push(item.jobData->job);

		delete item.jobData;
	}

	distributing.store(false);
}

void JobTimer::Clear()
{
	lock_guard<mutex> lock(mtx);

	while (items.empty() == false)
	{
		const TimerItem& timerItem = items.top();
		delete timerItem.jobData;
		items.pop();
	}
}