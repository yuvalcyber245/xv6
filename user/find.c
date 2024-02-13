#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char* fmtname(char* path)
{
    static char buf[DIRSIZ+1];
    char *p;

    // Find first character after last slash.
    for(p=path+strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;

    // Return blank-padded name.
    if(strlen(p) >= DIRSIZ)
        return p;
    memmove(buf, p, strlen(p));
    memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
    //printf("fmtname: %s in path: %s. the buff is: %s\n", p, path, buf);
    return p;
    return buf;
}

void find(char *path, char* to_find)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if((fd = open(path, O_RDONLY)) < 0){
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if(fstat(fd, &st) < 0){
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }
    switch(st.type){
    case T_DEVICE:
    case T_FILE:
        //printf("path: %s, fmtpath: %s", path, fmtname(path));
        if (strcmp(fmtname(path), to_find) == 0)
            printf("%s\n", path);
        break;

    case T_DIR:
        //printf("im here");
        if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
            printf("find: path too long\n");
            break;
        }
        strcpy(buf, path);
        p = buf+strlen(buf);
        *p++ = '/';
        while(read(fd, &de, sizeof(de)) == sizeof(de)){
            if(de.inum == 0)
                continue;
            memmove(p, de.name, DIRSIZ);
            //printf("de is: %d %s", &de.inum, de.name);
            p[DIRSIZ] = 0;
            // printf("buf is %s", buf);
            if(stat(buf, &st) < 0){
                printf("find: cannot stat %s\n", buf);
                continue;
            }
            if ((strcmp(fmtname(buf),".")) == 0 || strcmp(fmtname(buf),"..") == 0){
                //printf("thrown out %s, formatted is %s\n", buf, fmtname(buf));
                continue;
            }
            // printf("find(%s,%s)\n",buf,to_find); 
            find(buf, to_find );
        }
        break;
    }
    close(fd);
}

int main(int argc, char *argv[])
{
    int i;

    if(argc < 2){
        exit(0);
    }
    if (argc == 2)
    {
        find(".", argv[1]);
        exit(0);
    }
    for(i=2; i<argc; i++)
        find(argv[1], argv[i]);
    exit(0);
}
