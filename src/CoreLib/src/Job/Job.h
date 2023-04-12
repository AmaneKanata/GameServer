#pragma once

#include <memory>
#include <functional>

using namespace std;

using CallbackType = std::function<void()>;

class Job
{
public:
	Job(CallbackType&& callback) : callback(std::move(callback))
	{}

	template<typename T, typename Ret, typename... Args>
	Job(shared_ptr<T> owner, Ret(T::* memFunc)(Args...), Args&&... args)
	{
		callback = [owner, memFunc, args...]()
		{
			(owner.get()->*memFunc)(args...);
		};
	}

	void Execute()
	{
		callback();
	}

private:
	CallbackType callback;
};
