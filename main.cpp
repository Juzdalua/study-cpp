#include <iostream>
#include <thread>
#include <mutex>

mutex m;
queue<int> q;
condition_variable cv;

void Producer() {
	while (true) {
		{
			/*
				1. Lock을 잡고
				2. 공유 변수 값을 수정
				3. Lock을 풀고
				4. 조건변수를 통해 다른 스레드에게 통지
			*/
			unique_lock<mutex> lock(m);
			q.push(100);
		}
		cv.notify_one(); // wait중인 스레드 1개를 깨운다.
	}

}

void Consumer() {
	while (true) {
		unique_lock<mutex> lock(m);
		cv.wait(lock, []() {return q.empty() == false;});

		int data = q.front();
		q.pop();
		cout << data << endl;
	}
}

int main()
{
	thread t1(Producer);
	thread t2(Consumer);

	t1.join();
	t2.join();

	return 0;
}
