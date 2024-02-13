#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"


int main(int argc, char* argv[]){
    int p[2];
    pipe(p);
    if (fork() != 0){
        sleep(5);
        char buff[1] = "a";
        write(p[1], buff , sizeof(buff));
        close(p[1]);
        char buff2[1];
        sleep(1);
        read(p[1], buff2, sizeof(buff2));
        if (strcmp(buff2, buff)){
            printf("%d: received pong\n", getpid());
        }
        close(p[0]);
        exit(0);
    } else {
        sleep(5);
        char buff[1];
        read(p[1], buff, sizeof(buff));
        printf("%d: received ping\n", getpid());
        close(p[0]);
        write(p[1], buff, sizeof(buff));
        close(p[1]);
        exit(0);
    }
}