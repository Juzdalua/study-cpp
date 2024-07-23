#include <stdio.h>
#include "func.h"
#include "common.h"

int g_random = 42;
int Add(int num1, int num2)
{
    return num1 + num2;
}

void Print()
{
    printf("Random: %d \n", g_random);
}
