#include "../pch.h"
#include "JobQueue.h"
#include "GlobalQueue.h"

void JobQueue::Push(shared_ptr<Job> job, bool pushOnly)
{
	const int prevCount = _jobCount.fetch_add(1);
	_jobs.Push(job);

	if (prevCount == 0)
		if (LCurrentJobQueue == nullptr && pushOnly == false)
			Execute();
		else
			GGlobalQueue->Push(shared_from_this());
}

void JobQueue::Execute()
{
	LCurrentJobQueue = this;

	while (true)
	{
		vector<shared_ptr<Job>> jobs;
		_jobs.PopAll(jobs);

		const int jobCount = static_cast<int>(jobs.size());
		for (int i = 0; i < jobCount; i++)
			jobs[i]->Execute();

		if (_jobCount.fetch_sub(jobCount) == jobCount)
		{
			LCurrentJobQueue = nullptr;
			return;
		}

#ifdef linux
		const unsigned long long now = GetTickCount();
#elif _WIN32
		const unsigned long long now = ::GetTickCount64();
#endif
		if (now >= LEndTickCount)
		{
			LCurrentJobQueue = nullptr;
			GGlobalQueue->Push(shared_from_this());
			break;
		}
	}
}
