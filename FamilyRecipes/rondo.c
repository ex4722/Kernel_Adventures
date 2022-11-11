#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdlib.h>

#define CHALLENGE_DRIVER "/dev/chall"
// Commands
#define CMD_ALLOC 1337
#define CMD_DELETE 1338
#define CMD_READ 1339
#define CMD_INFO 1340
#define CMD_TOGGLE 1341

static int fd;

struct alloc {
    unsigned long idx;
    char* buf;
    unsigned long bufsize;
    unsigned int public;
};

struct info {
    unsigned long idx;
    unsigned long bufsize;
    unsigned int public;
    uid_t owner_uid;
};

struct delete {
    unsigned long idx;
};

struct read {
    unsigned long idx;
    char* buf;
    unsigned long bufsize;
};

struct toggle {
    unsigned long idx;
};

int alloc(unsigned long idx, char* buf, unsigned long bufsize, unsigned int public)
{
    struct alloc req ={
        .idx=idx,
        .buf=buf,
        .bufsize=bufsize,
        .public=public
    };
    ioctl(fd,CMD_ALLOC,&req);
    printf("Created\n");
    printf("Info about index %ld\n",req.idx);
    printf("\tBuf: %s\n",req.buf);
    printf("\tBUfSize: %lu\n",req.bufsize);
    printf("\tPublic: %d\n",req.public);
    return 0;
}
int info(unsigned long idx)
{
    struct info req = {
        .idx=idx
    };
    ioctl(fd,CMD_INFO,&req);
    printf("Info about index %ld\n",idx);
    printf("\tBuf Size: %ld\n",req.bufsize);
    printf("\tPublic: %d\n",req.public);
    printf("\tOwner: %d\n",req.owner_uid);
    return 0;
}
int delete(unsigned long idx)
{
    struct delete req = {
        .idx=idx
    };
    ioctl(fd,CMD_DELETE,&req);
    printf("Index %ld deleted!\n",idx);
    return 0;
}
int _read(unsigned long idx,unsigned long bufsize)
{
    char* ret_buf=malloc(bufsize);
    struct read req = {
        .idx=idx,
        .buf=ret_buf,
        .bufsize=bufsize
    };
    ioctl(fd,CMD_READ,&req);
    printf("Index %ld:\n",idx);
    write(1,ret_buf,bufsize);
    puts("");
    return 0;
}
int toggle(unsigned long idx)
{
    struct toggle req = {
        .idx=idx
    };
    ioctl(fd,CMD_TOGGLE,&req);
    printf("Toggling %ld!\n",idx);
    return 0;
}
int main(int argc, char **argv) {

    // Open challenge driver
    printf("[*] Opening challenge driver...\n");
    fd = open(CHALLENGE_DRIVER, O_RDWR | O_NONBLOCK);

    // ------- Exploit here -------
    alloc(0, "AAAAAAAABBBBBBBBB",31,1);
    alloc(1, "CCCCCCCCDDDDDDDDD",30,1);
    _read(0,30);
    /* delete(0); */
    /* _read(0,30); */



    /* alloc(1, "BYE BYE",30,1); */
    /* toggle(1); */
    /* _read(1,30); */
    /* toggle(1); */
    /* delete(0); */
    /* alloc(2, "H",30,1); */
    /* alloc(3, "BYE",30,1); */

    char cmd[0x100];
    char index[0x100];
    char data[0x100];
    char data_len[0x100];
    int idx;
    while (true){
        memset(cmd, 0, 0x100);
        memset(index, 0, 0x100);
        memset(data, 0, 0x100);
        memset(data_len, 1, 0x100);
        puts("CMD> ");
        gets(cmd);
        puts("INDX> ");
        gets(index);

        idx = atoi(index);

        if (strcmp(cmd, "alloc") == 0){
            puts("DATA> ");
            gets(data);

            puts("DATA LEN> ");
            gets(data_len);
            alloc(idx,data,atoi(data_len),1);

        }
        else if (strcmp(cmd, "info") == 0){
            info(idx);
        }
        else if (strcmp(cmd, "delete") ==0){
            delete(idx);
        }
        else if (strcmp(cmd, "toggle") ==0){
            toggle(idx);
        }
        else if (strcmp(cmd, "read")==0){
            puts("BUFSIZE> ");
            gets(index);
            _read(idx, atoi(index));
        }
    }
    close(fd);
    return 0;

}
