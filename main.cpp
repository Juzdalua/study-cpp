#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>

atomic<bool> ready;
int value;

void Producer() {
	value = 10;
	ready.store(true, memory_order::memory_order_seq_cst);
}

void Consumer() {
	while (ready.load(memory_order::memory_order_seq_cst) == false)
		;
	cout << value << endl;
}

void Producer_Relase() {
	value = 10;
	ready.store(true, memory_order::memory_order_release);
	// ------------- 절취선 --------------------
}

void Consumer_Acquire() {
	// ------------- 절취선 --------------------
	while (ready.load(memory_order::memory_order_acquire) == false)
		;
	cout << value << endl;
}

int main()
{
	ready = false;
	value = 0;

	thread t1(Producer);
	thread t2(Consumer);
	t1.join();
	t2.join();

	atomic_thread_fence(memory_order::memory_order_release);
	atomic_thread_fence(memory_order::memory_order_acquire);
}
