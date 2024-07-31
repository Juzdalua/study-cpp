#pragma once
#include <mutex>

template <typename T>
class LockStack {
public:
	LockStack() {}
	LockStack(const LockStack&) = delete;
	LockStack& operator=(const LockStack&) = delete;

	void Empty() {
		lock_guard<mutex> lock(_mutex);
		return _stack.empty();
	}

	void Push(T value) {
		lock_guard<mutex> lock(_mutex);
		_stack.push(move(value));
		_cv.notify_one();
	}

	bool TryPop(T& value) {
		lock_guard<mutex> lock(_mutex);
		if (_stack.empty())
			return false;

		value = move(_stack.top());
		_stack.pop();
		return true;
	}

	void WaitPop(T& value) {
		unique_lock<mutex> lock(_mutex);
		_cv.wait(lock, [this] {return _stack.empty() == false;});
		value = std::move(_stack.top());
		_stack.pop();
	}
private:
	stack<T> _stack;
	mutex _mutex;
	condition_variable _cv;
};