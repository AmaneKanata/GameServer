#pragma once

#include <set>
#include <boost/asio.hpp>

class JobQueue : public std::enable_shared_from_this<JobQueue>
{
public:
	JobQueue(boost::asio::io_context& ioc)
		: ioc(ioc)
		, jobs(ioc.get_executor())
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
	std::shared_ptr<boost::asio::steady_timer> DelayPost(int milli, Ret(T::* memFunc)(Args...), Args... args)
	{
		auto timer = std::make_shared<boost::asio::steady_timer>(ioc, std::chrono::milliseconds{ milli });
		{
			std::lock_guard<std::recursive_mutex> lock(delayedJob_mtx);
			delayedJobs.insert(timer);
		}

		std::shared_ptr<T> owner = std::static_pointer_cast<T>(shared_from_this());
		timer->async_wait([this, timer, owner, memFunc, args...](const boost::system::error_code& error)
			{
				if (!error)
				{
					boost::asio::post(jobs, [owner, memFunc, args...]() {
						(owner.get()->*memFunc)(args...);
						});
				}

				std::lock_guard<std::recursive_mutex> lock(delayedJob_mtx);
				delayedJobs.erase(timer);
			}
		);

		return timer;
	}

	std::shared_ptr<boost::asio::steady_timer> DelayPost(int milli, std::function<void()> func)
	{
		auto timer = std::make_shared<boost::asio::steady_timer>(ioc, std::chrono::milliseconds{ milli });
		{
			std::lock_guard<std::recursive_mutex> lock(delayedJob_mtx);
			delayedJobs.insert(timer);
		}

		timer->async_wait([this, timer, func](const boost::system::error_code& error)
			{
				if (!error)
				{
					boost::asio::post(jobs, [func]() {
						func();
						});
				}

				std::lock_guard<std::recursive_mutex> lock(delayedJob_mtx);
				delayedJobs.erase(timer);
			}
		);

		return timer;
	}

	void Cancel(std::shared_ptr<boost::asio::steady_timer> timer)
	{
		timer->cancel();

		std::lock_guard<std::recursive_mutex> lock(delayedJob_mtx);
		delayedJobs.erase(timer);
	}

	void Clear()
	{
		std::lock_guard<std::recursive_mutex> lock(delayedJob_mtx);

		for (auto& timer : delayedJobs)
			timer->cancel();
		delayedJobs.clear();
	}

	boost::asio::io_context& GetIoC()
	{
		return ioc;
	}

private:
	boost::asio::io_context& ioc;
	boost::asio::strand<boost::asio::io_context::executor_type> jobs;
	std::set<std::shared_ptr<boost::asio::steady_timer>> delayedJobs;
	std::recursive_mutex delayedJob_mtx;
};