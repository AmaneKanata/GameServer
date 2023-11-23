#pragma once

#include <queue>
#include <boost/asio.hpp>
#include <iostream>

struct DelayedJob
{
	DelayedJob(std::chrono::system_clock::time_point time, std::function<void()> func)
		: time(time)
		, func(func)
	{}

	std::chrono::system_clock::time_point time;
	std::function<void()> func;
	bool isValid = true;
};

struct DelayedJobComparator {
	bool operator()(const std::shared_ptr<DelayedJob>& lhs, const std::shared_ptr<DelayedJob>& rhs) const {
		return lhs->time > rhs->time;
	}
};

class JobQueue : public std::enable_shared_from_this<JobQueue>
{
public:
	JobQueue(boost::asio::io_context& ioc)
		: ioc(ioc)
		, jobs(ioc.get_executor())
		, delayedJob_timer(ioc)
	{}

	template<typename T, typename Ret, typename... Args>
	void Post(Ret(T::* memFunc)(Args...), Args... args)
	{
		std::shared_ptr<T> owner = std::static_pointer_cast<T>(shared_from_this());
		boost::asio::post(jobs, [owner, memFunc, args...]() {
			(owner.get()->*memFunc)(args...);
			});
	}

	void Post(std::function<void()> func)
	{
		boost::asio::post(jobs, [func]() {
			func();
			});
	}

	template<typename T, typename Ret, typename... Args>
	std::shared_ptr<DelayedJob> DelayPost(int milli, Ret(T::* memFunc)(Args...), Args... args)
	{
		std::shared_ptr<T> owner = std::static_pointer_cast<T>(shared_from_this());

		auto delayedJob = std::make_shared<DelayedJob>(
			std::chrono::system_clock::now() + std::chrono::milliseconds(milli),
			[owner, memFunc, args...]()
			{
				(owner.get()->*memFunc)(args...);
			}
		);

		std::lock_guard<std::recursive_mutex> lock(delayedJob_mtx);

		delayedJobs.push(delayedJob);

		if (delayedJobs.top()->time >= delayedJob->time)
		{
			delayedJob_timer.cancel();
			delayedJob_timer.expires_at(delayedJobs.top()->time);
			delayedJob_timer.async_wait([this](const boost::system::error_code& error)
				{
					if (!error)
					{
						DoDelayedJob();
					}
				}
			);
		}

		return delayedJob;
	}

	std::shared_ptr<DelayedJob> DelayPost(int milli, std::function<void()> func)
	{
		auto delayedJob = std::make_shared<DelayedJob>(
			std::chrono::system_clock::now() + std::chrono::milliseconds(milli),
			func
		);

		std::lock_guard<std::recursive_mutex> lock(delayedJob_mtx);

		delayedJobs.push(delayedJob);

		if (delayedJobs.top()->time >= delayedJob->time)
		{
			delayedJob_timer.cancel();
			delayedJob_timer.expires_at(delayedJobs.top()->time);
			delayedJob_timer.async_wait([this](const boost::system::error_code& error)
				{
					if (!error)
					{
						DoDelayedJob();
					}
				}
			);
		}

		return delayedJob;
	}

	void DoDelayedJob()
	{
		std::chrono::system_clock::time_point current = std::chrono::system_clock::now();

		std::lock_guard<std::recursive_mutex> lock(delayedJob_mtx);

		if (!delayedJobs.empty() && delayedJobs.top()->time <= current)
		{
			auto job = delayedJobs.top();
			delayedJobs.pop();
			if (job->isValid)
			{
				boost::asio::post(jobs, [job]() {
					job->func();
					});
			}
		}

		if (!delayedJobs.empty())
		{
			auto nextJob = delayedJobs.top();
			delayedJob_timer.cancel();
			delayedJob_timer.expires_at(nextJob->time);
			delayedJob_timer.async_wait([this](const boost::system::error_code& error)
				{
					if (!error)
					{
						DoDelayedJob();
					}
				}
			);
		}
	}

	void Clear()
	{
		std::lock_guard<std::recursive_mutex> lock(delayedJob_mtx);

		delayedJob_timer.cancel();
		while (!delayedJobs.empty())
			delayedJobs.pop();
	}

	boost::asio::io_context& GetIoC()
	{
		return ioc;
	}

private:
	boost::asio::io_context& ioc;
	
	boost::asio::strand<boost::asio::io_context::executor_type> jobs;
	
	std::priority_queue<std::shared_ptr<DelayedJob>, std::vector<std::shared_ptr<DelayedJob>>, DelayedJobComparator> delayedJobs;
	std::recursive_mutex delayedJob_mtx;
	boost::asio::system_timer delayedJob_timer;
};