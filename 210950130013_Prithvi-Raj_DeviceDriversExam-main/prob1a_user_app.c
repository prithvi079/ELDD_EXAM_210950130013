#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<sys/ioctl.h>

#include "_ioctl.h"

int main(int argc,char* argv[])
{
    int fd=open("/dev/exam_driver",O_RDWR);
    if(fd<0)
    {
        perror("error while opening\n");
        exit(EXIT_FAILURE);
    }
    char buffer[100]="prob1-a question of device driver exam";
    char data_read[100];
    ssize_t nwrite=write(fd,(char*)buffer,strlen(buffer));
    if(nwrite<0)
    {
        perror("error while writing\n");
        exit(EXIT_FAILURE);
    }

    ssize_t nread=read(fd,data_read,nwrite);

    int len;

    ioctl(fd,string_getlen,&len);

    return 0;
}