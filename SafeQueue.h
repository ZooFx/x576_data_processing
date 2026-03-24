#ifndef SAFE_QUEUE_H
#define SAFE_QUEUE_H

#include <mutex>
#include <condition_variable>
#include <queue>

template <typename T>
class safe_queue
{
private:
	std::mutex              d_mutex;
	std::condition_variable d_condition;
	std::queue<T>           d_queue;

public:
	void push(T const& value)
	{
		{
			std::unique_lock<std::mutex> lock(this->d_mutex);
			d_queue.push(value);
		}
		this->d_condition.notify_one();
	}

	T pop()
	{
		std::unique_lock<std::mutex> lock(this->d_mutex);
		// 条件变量必须配合谓词使用，防止虚假唤醒
		this->d_condition.wait(lock, [this] { return !this->d_queue.empty(); });
		T rc(std::move(this->d_queue.front())); // 使用 move 提升性能
		this->d_queue.pop();
		return rc;
	}

	bool empty()
	{
		bool tmp;
		std::unique_lock<std::mutex> lock(this->d_mutex);
		tmp = d_queue.empty();
		return tmp;
	}

	bool try_pop(T &value)
	{
		std::unique_lock<std::mutex> lock(this->d_mutex);
		if (d_queue.empty())
			return false;
		else
		{
			value = d_queue.front();
			d_queue.pop();
			return true;
		}
	}

	T front()
	{
		std::unique_lock<std::mutex> lock(this->d_mutex);
		this->d_condition.wait(lock, [=] { return !this->d_queue.empty(); });
		T rc(this->d_queue.front());
		return rc;
	}

	size_t size()
	{
		size_t tmp;
		{
			std::unique_lock<std::mutex> lock(this->d_mutex);
			tmp = d_queue.size();
		}
		return tmp;
	}
};

#endif


