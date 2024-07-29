#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>

class SpinLock {
public:
	void lock() {
		bool expected = false; // before
		bool desired = true; // after

		while (_locked.compare_exchange_strong(expected, desired) == false) {
			expected = false;
		}
	}
	void unlock() {
		//_locked = false;
		_locked.store(false);
	}
private:
	atomic<bool> _locked = false;
};

SpinLock spinLock;
int sum = 0;

void Add() {
	for (int i = 0; i < 1'000'000; i++) {
		lock_guard<SpinLock> guard(spinLock);
		sum++;
	}
}

void Sub() {
	for (int i = 0; i < 1'000'000; i++) {
		lock_guard<SpinLock> guard(spinLock);
		sum--;
	}
}


int main()
{
	thread t1(Add);
	thread t2(Sub);
	t1.join();
	t2.join();
	cout << "SpinLock: " << sum << endl;

	return 0;
}
