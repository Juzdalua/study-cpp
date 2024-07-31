#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>

thread_local int LThreadId = 0;

void ThreadMain(int threadId) {
	LThreadId = threadId;

	while (true) {
		cout << LThreadId << endl;
		this_thread::sleep_for(1s);
	}
}

int main()
{
	thread t;
	t.get_id();

	vector<thread> threads;
	for (int i = 0; i < 10; i++) {
		int threadId = i + 1;
		threads.push_back(thread(ThreadMain, threadId));
	}

	for (thread& t : threads)
		t.join();
}
