#include <stdio.h>
#include <iostream>
#include "./include/arr.h"
// #include "arr.h"

using namespace std;

int main()
{
    jArr _jArr = {};
    InitJArr(&_jArr);

    for (int i = 0; i < 10; i++)
    {
        PushJArr(&_jArr, i);
    }

    RelaseJArr(&_jArr);

    return 0;
}