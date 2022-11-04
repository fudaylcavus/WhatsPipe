#include "local.h"
#include <iostream>
#include <cstdio>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <unistd.h> 
#include <string.h>
#include <stdlib.h>
#include <csignal>

using namespace std;

int n, privatepipe;
char name[50];
static char buffer[PIPE_BUF];

void signalHandler( int signum ) {
   if ((privatepipe = open(name, O_RDONLY)) == -1) {
        perror(name);
        return;
    }       
    cout << "\n\n";
    while ((n = read(privatepipe, buffer, PIPE_BUF)) > 0) {
        write(fileno(stderr), buffer, n);
    }
    close(privatepipe);
}

int main() {
    signal(10, signalHandler);  
    int publicpipe;

    char target_name[50];

    struct message msg;

    write(fileno(stdout), "Adiniz nedir?\n>", 16);
    n = read(fileno(stdin), name, 50);
    write(fileno(stdout), "Kime mesaj atmak istiyorsun?\n>", 31);
    n = read(fileno(stdin), target_name, 50);

    sprintf(msg.username, "/tmp/pipe%s", target_name);

    char temp[10];
    strcpy(temp, name);
    sprintf(name, "/tmp/pipe%s", temp);


    // write(fileno(stdout), "code working so far1", 20);
    if (mknod(name, S_IFIFO | 0666, 0) < 0) {
        perror(name);
        return 1;
    }

    // write(fileno(stdout), "code working so far2", 20);
    if ((publicpipe = open(PUBLIC, O_WRONLY)) == -1) { 
        perror(PUBLIC);
        return 2;
    }
    // write(fileno(stdout), "code working so far3", 20);

    while (1) {
        write(fileno(stdout), "\n>", 3);
        memset(msg.content, 0x0, B_SIZ);
        n = read(fileno(stdin), msg.content, B_SIZ);
        if (!strcmp("quit\n", msg.content)) {
            break;
        }
        write(publicpipe, (char *) &msg, sizeof(msg));
    }
    close(publicpipe);
    unlink(name);
    return 0;
}
