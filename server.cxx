#include "local.h"
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

        n = done = 0;
        do {
            if ((privatepipe = open(msg.username, O_WRONLY | O_NDELAY)) == -1)
                sleep(3);
            else {
                write(privatepipe, msg.content, B_SIZ);
                close(privatepipe);
                done = 1;
            }
        } while (!done);
        if (!done)
        {
            write(fileno(stderr),
                "\nNOTE: SERVER ** NEVER ** accessed private FIFO\n", 48);
            return 2;
        }
    }
    return 0;
}