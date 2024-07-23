#include <stdio.h>
#include <iostream>
#include "./include/LinkedList/linkedList.h"

using namespace std;

int main()
{
    LinkedList list = {};
    InitList(&list);

    LPushList(&list, 100);
    LPushList(&list, 200);
    LPushList(&list, 300);

    ReleaseList(&list);

    return 0;
}