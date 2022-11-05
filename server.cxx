#include "local.h"
#include <iostream>
#include <string.h>
#include <signal.h>

using namespace std;



void number_parser(char arr[], int size) {
    int j = 0;
    for (int i = 0; i < size; i++) {
        if (arr[i] >= '0' && arr[i] <= '9') {
            arr[j++] = arr[i];
        }
    }
    arr[j] = '\0';
}

void array_copier(char s[], char d[], int size) {
    for (int i = 0; i < size; i++) {
        d[i] = s[i];
    }
}

int main() {
    int n, done, dummypipe, publicpipe, privatepipe;
    struct message msg;
    FILE *fin;
    static char buffer[PIPE_BUF];

    mknod(PUBLIC, S_IFIFO | 0666, 0);

    if ((publicpipe = open(PUBLIC, O_RDONLY)) == -1 ||
            (dummypipe = open(PUBLIC, O_WRONLY | O_NDELAY)) == -1)
    {
        perror(PUBLIC);
        return 1;
    }

    while (read(publicpipe, (char *)&msg, sizeof(msg)) > 0)
    {
        cout << "POST";
        n = done = 0;

        int size = sizeof(msg.username)/sizeof(char);
        int pid;
        char pid_string[size];

        array_copier(msg.username, pid_string, size);
        number_parser(pid_string, size);
        sscanf(pid_string, "%d", &pid);

        kill(pid, 10);

        do {

            if ((privatepipe = open(msg.username, O_WRONLY | O_NDELAY)) == -1) {
                sleep(3);
            } else {
                write(privatepipe, msg.content, B_SIZ);
                close(privatepipe);
                done = 1;
            }
        } while (++n < 5 && !done);
        if (!done)
        {
            write(fileno(stderr),
                "\nNOTE: SERVER ** NEVER ** accessed private FIFO\n", 48);
            return 2;
        }
    }
    return 0;
}