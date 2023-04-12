#include "../pch.h"
#include "JobQueue.h"

void JobQueue::Push(shared_ptr<Job> job, bool pushOnly)
{
	const int prevCount = jobCount.fetch_add(1);
	jobs.Push(job);

	if (prevCount == 0)
		if (LCurrentJobQueue == nullptr && pushOnly == false)
			Execute();
		else
			GPendingJobQueues->Push(shared_from_this());
}

void JobQueue::Execute()
{
	LCurrentJobQueue = this;

	while (true)
	{
		vector<shared_ptr<Job>> _jobs;
		jobs.PopAll(_jobs);

		const int _jobCount = static_cast<int>(_jobs.size());
		for (int i = 0; i < _jobCount; i++)
			_jobs[i]->Execute();

		if (jobCount.fetch_sub(_jobCount) == _jobCount)
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
			GPendingJobQueues->Push(shared_from_this());
			break;
		}
	}
}
