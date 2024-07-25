#pragma once

// 구조체로 배열 만들기
typedef struct jaggedArr
{
    int *pInt;    // 시작 주소
    int count;    // 현재 배열의 크기
    int maxCount; // 최대 배열의 크기
} jArr;

void InitJArr(jArr *_jArr);           // 힙 메모리 할당 함수
void RelaseJArr(jArr *_jArr);         // 힙 메모리 해제 함수
void PushJArr(jArr *_jArr, int data); // 힙 메모리에 저장 함수

// 클래스로 배열 만들기
class CArr
{
private:
    int *pData;
    int count;
    int maxCount;

public:
    void RPush(int data);
    void resize(int resizeCount);
    int &operator[](int idx);

public:
    CArr();
    ~CArr();
};