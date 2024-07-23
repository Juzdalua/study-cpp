#pragma once

typedef struct jaggedArr
{
    int *pInt;    // 시작 주소
    int count;    // 현재 배열의 크기
    int maxCount; // 최대 배열의 크기
} jArr;

void InitJArr(jArr *_jArr);           // 힙 메모리 할당 함수
void RelaseJArr(jArr *_jArr);         // 힙 메모리 해제 함수
void PushJArr(jArr *_jArr, int data); // 힙 메모리에 저장 함수
