#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

void receive_from_pipe_and_send(int receive[2]){
    int buff[1];
    close(receive[1]);
    int read_result = read(receive[0], buff, sizeof(buff));
    if (read_result == 0){
        close(receive[0]);
        close(receive[1]);
        exit(0);
    }
    int new_pipe[2];
    int prime = buff[0];
    printf("prime %d\n", prime);
    pipe(new_pipe);
    if (fork() == 0){
        close(receive[0]);
        receive_from_pipe_and_send(new_pipe);
        exit(0);
    }
    else{
        close(new_pipe[0]);
        read_result = read(receive[0], buff, sizeof(buff));
        while (read_result == sizeof(buff)){
            if (buff[0] % prime != 0){
                if (write(new_pipe[1], buff, sizeof(buff)) != sizeof(buff)){
                    break;
                }
            }
            read_result = read(receive[0], buff, sizeof(buff));
            if (read_result != sizeof(buff)){
                break;
            }
        }
        close(receive[0]);
        close(new_pipe[1]);
        wait(((int*) 0));
        kill(getpid());
    }
}

int main(int argc, char* argv[]){
    int first_pipe[2];
    pipe(first_pipe);
    int buff[1];
    if(fork() == 0){
        receive_from_pipe_and_send(first_pipe);
    }
    else{
        close(first_pipe[0]);
        for (int i = 2; i < 35; i++){
            buff[0] = i;
            if (write(first_pipe[1], buff, sizeof(buff)) != sizeof(buff)){
                break;
            }
        }
        close(first_pipe[1]);
        wait((int *) 0);
    }
    exit(0);
}


