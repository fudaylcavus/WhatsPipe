#include "local.h"
#include "colors.h"
#include <iostream>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <vector>
#include <csignal>


using namespace std;

int n, privatepipe;
char selfpipe_path[50];
static char buffer[PIPE_BUF];
struct message msg;
int publicpipe;

void on_messsage(int signum)
{
    if ((privatepipe = open(selfpipe_path, O_RDONLY)) == -1)
    {
        perror(selfpipe_path);
        return;
    }


    while ((n = read(privatepipe, buffer, PIPE_BUF)) > 0)
    {
        write(fileno(stdout), buffer, n);
    }
    cout << endl;

    close(privatepipe);
}

void handle_target()
{
    // TODO: daha cok herkesin kendi room'u gibi yapalim
    // degilse, ahmet dedikten sonra da araya mehmet girebiliyor
    
    string target_user;
    cout << "\nWho do you want to send message?\n"
         << ">";
    cin >> target_user;
    sprintf(msg.target_user, "%s", target_user.c_str());
}

void server_init()
{
    // Send pid to
    sprintf(msg.target_user, "SERVER-INIT");
    sprintf(msg.content, "%d", getpid());
    write(publicpipe, (char *)&msg, sizeof(msg));
}

int main()
{
    signal(10, on_messsage);

    string from_user;
    cout << "Hi, What's your name?\n" << ">";
    cin >> from_user;

    sprintf(msg.from_user, "%s", from_user.c_str());
    sprintf(selfpipe_path, "/tmp/pipe%d", getpid());

    // write(fileno(stdout), "code working so far1", 20);
    if (mknod(selfpipe_path, S_IFIFO | 0666, 0) < 0)
    {
        perror(selfpipe_path);
        return 1;
    }

    // write(fileno(stdout), "code working so far2", 20);
    if ((publicpipe = open(PUBLIC, O_WRONLY)) == -1)
    {
        perror(PUBLIC);
        return 2;
    }
    // write(fileno(stdout), "code working so far3", 20);

    server_init();

    handle_target();

    while (1)
    { 
        memset(msg.content, 0x0, B_SIZ);

        n = read(fileno(stdin), msg.content, B_SIZ);


        if (!strcmp("<-\n", msg.content))
        {
            handle_target();
            continue;
        }
        write(publicpipe, (char *)&msg, sizeof(msg));
    }
    close(publicpipe);
    unlink(selfpipe_path);
    return 0;
}
