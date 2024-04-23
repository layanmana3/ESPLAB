#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int count(char* input){

    int cnt=0;
    for (int i = 0; *(input+i)!='\0'; i++)
    {
       if(input[i] >= '0' && input[i] <= '9')
            cnt++;
    }
    return cnt;
    
}


int main(int argc, char **argv)
{
    if(argc<2)
        return  0;
    printf("The number of digits in the string is:%d\n",count(argv[1]));
}//0x804816c Entry
//8048156 start offset
//Size 88 add 56d our func
// 