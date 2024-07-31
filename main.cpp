#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>

class PointClass
{
public:
	PointClass(int _x, int _y) : x(_x), y(_y) {};
	bool operator==(PointClass &ref)
	{
		if (this->x == ref.x && this->y == ref.y)
			return true;
		return false;
	}
	PointClass operator++()
	{
		this->x++;
		this->y++;

		return *this;
	}

private:
	int x, y;
};

int main()
{
	PointClass p1(2, 3);
	PointClass p2(2, 3);

	if (p1 == p2)
		printf("true");

	++p1;

	return 0;
}