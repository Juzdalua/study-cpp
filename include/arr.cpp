#include "arr.h"
#include <iostream>

void InitJArr(jArr *_jArr)
{
    _jArr->maxCount = 1;
    (*_jArr).count = 0;
    _jArr->pInt = (int *)malloc(sizeof(int) * _jArr->maxCount);
};

void RelaseJArr(jArr *_jArr)
{
    _jArr->maxCount = 0;
    _jArr->count = 0;
    free(_jArr->pInt);
};

 // 힙 메모리 주소 재정의 함수
void RelocateJArr(jArr *_jArr)
{
    // 새로운 힙 영역 생성
    int *pNewInt = (int *)malloc((_jArr->maxCount + 1) * sizeof(int));

    // 기존 데이터 복사
    for (int i = 0; i < _jArr->count; i++)
    {
        pNewInt[i] = _jArr->pInt[i];
    }

    // 기존 힙 영역 삭제
    free(_jArr->pInt);

    // 기존 메모리 주소 변경
    _jArr->pInt = pNewInt;

    // 배열 최대 사이즈 증가
    _jArr->maxCount++;
};

void PushJArr(jArr *_jArr, int data)
{
    if (_jArr->maxCount <= _jArr->count)
    {
        RelocateJArr(_jArr);
    }

    _jArr->pInt[_jArr->count] = data;
    _jArr->count++;
}