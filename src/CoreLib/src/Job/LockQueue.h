#pragma once

#include <queue>
#include <vector>
#include <mutex>
#include <memory>

using namespace std;

template<typename T>
class LockQueue
{
public:
	void Push(T item)
	{
		lock_guard<recursive_mutex> lock(mtx);
		items.push(item);
	}

	T Pop()
	{
		lock_guard<recursive_mutex> lock(mtx);
		if (items.empty())
			return T();

		T ret = items.front();
		items.pop();
		return ret;
	}

	void PopAll(std::vector<T>& items)
	{
		lock_guard<recursive_mutex> lock(mtx);
		while (T item = Pop())
			items.push_back(item);
	}

	void Clear()
	{
		lock_guard<recursive_mutex> lock(mtx);
		items = queue<T>();
	}

private:
	recursive_mutex mtx;
	queue<T> items;
};