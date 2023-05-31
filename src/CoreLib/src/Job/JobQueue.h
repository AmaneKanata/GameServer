#pragma once

#include <set>
#include <boost/asio.hpp>

class JobQueue : public std::enable_shared_from_this<JobQueue>
{
public:
	JobQueue(boost::asio::io_context& ioc)
		: ioc(ioc)
		, jobs(ioc)
	{}

	void Push(std::function<void()> job)
	{
		jobs.post(job);
	}

protected:
	boost::asio::io_context& ioc;
	boost::asio::io_context::strand jobs;
	std::set<std::shared_ptr<boost::asio::steady_timer>> delayedJobs;
};