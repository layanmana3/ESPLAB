#include "Util.h"

#define SYS_WRITE 4
#define STDOUT 1
#define SYS_OPEN 5
#define O_RDWR 2
#define SYS_SEEK 19
#define SEEK_SET 0
#define SHIRA_OFFSET 0x291
#define STDERR 2
#define SYS_EXIT 1
#define SYS_READ 3
#define bufferSize 8129

extern int system_call();
extern void infection();
extern void infector(char*);
int main(int argc, char *argv[], char *envp[])
{
    int i;
    char* fileName = 0;
    int flag=0;
    for(i = 0; i < argc && !flag ; i++){
        if(strncmp(argv[i], "-a", 2) == 0){
            fileName = argv[i] + 2; 
            flag = 1;
        }
    }
    if(fileName != 0){
        int fd = system_call(SYS_OPEN, fileName, O_RDWR);
        if(fd == -1){
            system_call(SYS_WRITE, STDERR, "Could not open the file!\n", strlen("Could not open the file!\n"));
            system_call(SYS_EXIT, 0x55);
        }
        char buffer[bufferSize];
        int num = system_call(SYS_READ, fd, buffer, bufferSize);
        for(i = 0; i < num; i++){
            system_call(SYS_WRITE, STDOUT, &buffer[i], 1);
        }
        infection();
        infector(fileName);
    }
    return 0;
}