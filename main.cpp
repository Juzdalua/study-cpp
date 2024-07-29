#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>

vector<int> v;
mutex m;

// RAII (Resource Acquisition is Initialization)
template <typename T>
class LockGuard {
public:
	LockGuard(T& m) {
		_mutex = &m;
		_mutex->lock();
	}
	~LockGuard() {
		_mutex->unlock();
	}

private:
	T* _mutex;
};

void Push() {
	for (int i = 0; i < 10'000; i++) {
		//LockGuard<mutex> lockGuard(m);
		lock_guard<mutex> lockGuard(m); // std 표준에도 존재하는 클래스 템플릿이다.
		v.push_back(i);
	}
}

int main()
{
	thread t1(Push);
	thread t2(Push);

	t1.join();
	t2.join();

	cout << v.size() << endl;

	return 0;
}
