#include <iostream>
#include <thread>
#include <atomic>

atomic<int> sum = 0;

void Add() {
	for (int i = 0; i < 1'000'000; i++) {
		sum.fetch_add(1);
	}
}

void Sub() {
	for (int i = 0; i < 1'000'000; i++) {
		sum.fetch_sub(1);
	}
}

int main() // 메인 스레드
{
	Add();
	Sub();
	cout << sum << endl;

	thread t1(Add);
	thread t2(Sub);
	t1.join();
	t2.join();
	cout << sum << endl;
}
