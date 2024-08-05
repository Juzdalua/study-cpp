#include <iostream>
#include <atomic>
#include <thread>
#include <vector>

// 소수 구하기
// 1과 자기 자신으로만 나뉘면 그것을 소수라고 함.
using namespace std;

bool CalculateRepeatToSelf(int selfNum) {
	int count = 0;
	for (int i = 2; i <= selfNum; i++) {
		if (count > 2)
			return false;

		if (selfNum % i == 0) {
			count++;
		}
	}
	if (count == 1) 
		return true;
	
	return false;
}

int PreCalcuate(int order, int devision, int interval) {
	// 1, 10, 100'000
	// 2, 10, 100'000
	// 3, 10, 100'000
	// ...
	// 10, 10, 100'000

	int startNum = (order - 1) * interval + 1;
	int endNum = order * interval;
	int count = 0;
	for (int i = startNum; i <= endNum; i++) {
		if (CalculateRepeatToSelf(i))
			count++;
	}

	return count;
}

int main_function()
{
	const int MAX_NUMBER = 1'000'000;
	//const int MAX_NUMBER = 10'000; // 1229
	//const int MAX_NUMBER = 1'000; // 168

	// 1~MAX_NUMBER까지 소수 개수
	// 멀티스레드로 병렬로 구해서 덧셈한다.

	/*for (int i = 1; i <= 20; i++) {
		if (CalculateRepeatToSelf(i))
			count++;
	}
	cout << count << endl;*/

	// ------------------------

	vector<thread> threads;
	int devision = thread::hardware_concurrency();
	int interval = MAX_NUMBER / devision + 1;
	atomic<int> totalCount = 0;

	for (int i = 1; i <= devision; i++) {
		threads.push_back(thread([&totalCount, i, devision, interval](){
			totalCount+= PreCalcuate(i, devision, interval);
		}));
	}

	for (thread& t : threads) {
		t.join();
	}

	cout << "Total: " << totalCount.load() << endl;
}
