#include <iostream>
#include <thread>
#include <vector>

void HelloThread() {
	cout << "Hello Thread" << endl;
}

void HelloThread2(int num) {
	cout << num << endl;
}

void HelloThread3(int num, int num2) {
	cout << num << " " << num2 << endl;
}

int main() // 메인 스레드
{
	//thread t(HelloThread);
	thread t;
	// t.get_id(); == 0
	t = thread(HelloThread); // 실제 스레드 생성. id 배정

	cout << "Hello Main" << endl;

	int count = t.hardware_concurrency(); // CPU 코어 개수 추측 함수.
	auto id = t.get_id(); // 스레드마다 고유하게 부여되는 id
	if (t.joinable()) // 실제 스레드가 연결이 되어있는지 확인.
		t.join(); // t 스레드가 종료될 때까지 메인 스레드를 종료하지 않고 기다린다.
	t.detach(); // 스레드 객체 t에서 실제 스레드를 분리한다. -> 이후 t의 상태나 정보를 활용할 수 없게된다. 거의 사용하지 않음.

	// 스레드는 병렬로 실행되기때문에 순서를 보장하지 않는다.
	vector<thread> v;
	for (int i = 0; i < 5; i++) {
		v.push_back(thread(HelloThread2, i));
	}

	for (int i = 0; i < 5; i++) {
		if (v[i].joinable()) v[i].join();
	}

	thread t3(HelloThread3, 3, 4);
	t3.join();

    return 0;
}
