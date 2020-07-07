#include <stdlib.h>
#include <stdio.h>
int main() 
{
    printf("Running ps with system\n");
    system("ps ax &"); //background 에서 실행
    printf("Done\n");
    exit(0);
}