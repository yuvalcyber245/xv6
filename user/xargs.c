#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"
#include "kernel/param.h"

#define MAX_BUFFER_SIZE 256

int main(int argc, char* argv[])
{
    if (argc < 2){
        exit(0);
    }
    int fd = 0;
    char letter_holder[1];
    char temp_buff[MAX_BUFFER_SIZE];
    char* words[MAXARG];
    for(int i = 1; i < argc; i++)
    {
        words[i - 1] = argv[i]; 
    }
    int word_index = argc - 1;
    int index_in_word = 0;
    int read_return_val;
    while(42 != 3){
        read_return_val = read(fd, letter_holder, sizeof(letter_holder));
        if (read_return_val == 0){
            exit(0);
        }
        if (*letter_holder == ' '){
            // allocate memory for the current word, put it in the array and start a new word
            temp_buff[index_in_word] = '\0';
            words[word_index] = malloc(sizeof(char) * (index_in_word + 1));
            strcpy(words[word_index], temp_buff);
            word_index++;
            index_in_word = 0;
            //temp_buff[0] = '\0';
            for (int i = 0; i < MAX_BUFFER_SIZE; i++)
            {
                temp_buff[i] = '\0';
            }
            continue;
        }
        if (*letter_holder == '\n'){
            temp_buff[index_in_word] = '\0';
            words[word_index] = malloc(sizeof(char) * (index_in_word + 1));
            strcpy(words[word_index], temp_buff);
            // exec and start again with new line
            if (fork() == 0){
                // child process executes
                exec(argv[1], words);
                exit(0);
            }
            else{
                // parent process frees words, returns variables to initial state and continues
                wait((int*) 0);
                for(int i = argc - 1; i < word_index; i++)
                {
                    free(words[i]);
                }
                word_index = argc - 1;
                index_in_word = 0;
                continue;
            }
        }
        // if the letter was not \n or \0 or ' ' it was part of the current word
        temp_buff[index_in_word] = *letter_holder;
        index_in_word++;
    }
    exit(0);
}