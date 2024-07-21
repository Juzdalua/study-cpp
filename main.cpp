#include <iostream>
#include <stdio.h>
#include "func.h"

using namespace std;

int main()
{
    printf("HELLO \n");

    int a = func::Add(1, 2);
    printf("A: %d \n", a);
    
    printf("WORLD \n");

    return 0;
}

