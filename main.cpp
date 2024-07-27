#include <stdio.h>
#include <iostream>
#include "./include/Template/arrayList.h"

using namespace std;

int main()
{
    CList<int> list;

    list.RPush(1);
    list.RPush(2);
    list.RPush(3);

    return 0;
}