#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[])
{
    long long size = atol(argv[1]);

    for(long long x=0;x<size;x++)
    {
        printf("%c",((char)(random()%26 + 'a')));
    }
}