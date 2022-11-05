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
struct message msg;

void signalHandler( int signum ) {
   if ((privatepipe = open(msg.username, O_RDONLY)) == -1) {
        perror(name);
        return;
    }       
    cout << "\r";
    while ((n = read(privatepipe, buffer, PIPE_BUF)) > 0) {
        write(fileno(stdout), buffer, n);
    }
    cout << "\n\n>";
    close(privatepipe);
}

int main() {
    signal(10, signalHandler);  
    int publicpipe;

    char target_name[50];

    sprintf(msg.username, "/tmp/pipe%d", getpid());


    // write(fileno(stdout), "code working so far1", 20);
    if (mknod(msg.username, S_IFIFO | 0666, 0) < 0) {
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
