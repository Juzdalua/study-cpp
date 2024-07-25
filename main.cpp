#include <stdio.h>
#include <iostream>
#include "./include/Array/arr.h"

using namespace std;

class MyClass
{
public:
    int age;
    char *name;

private:
    float pi;

public:
    MyClass() // 생성자
        : age(1), name((char *)"AB"), pi(3.14f)
    {
    }

    ~MyClass() // 소멸자
    {
        printf("%s, %d, %f \n", name, age, pi);
    }

public:
    void SetAge(int _age)
    {
        this->age = _age;
    }
};

int main()
{
    // 기본 사용법
    MyClass newClass;
    newClass.SetAge(11);
    newClass.name = (char *)"ab";

    // 클래스 동적할당
    MyClass *newClass2 = new MyClass;
    delete newClass2;

    CArr cArr;
    cArr.RPush(1);
    cArr.RPush(2);
    cArr.RPush(3);

    int data = cArr[2];
    cArr[2] = 22;
    printf("%d \n", cArr[2]);

    return 0;
}