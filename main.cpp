#include <iostream>
#include <thread>
#include <mutex>
#include <future>
#include <windows.h>

int buffer[10'000][10'000];

int main()
{
	memset(buffer, 0, sizeof(buffer));
	{
		unsigned int start = GetTickCount64();

		int sum = 0;
		for (int i = 0; i < 10'000; i++)
			for (int j = 0; j < 10'000; j++)
				sum += buffer[i][j];

		unsigned int end = GetTickCount64();

		cout << "Elapsed Tick: " << (end - start) << endl;
	}

	{
		unsigned int start = GetTickCount64();

		int sum = 0;
		for (int i = 0; i < 10'000; i++)
			for (int j = 0; j < 10'000; j++)
				sum += buffer[j][i];

		unsigned int end = GetTickCount64();

		cout << "Elapsed Tick: " << (end - start) << endl;

		return 0;
	}
}
