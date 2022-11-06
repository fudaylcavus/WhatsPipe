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
string from_user;
string target_user;
string ERRORS[] = {"SERVER_DUPL_NAME_ERROR", "SERVER_TARGET_NOT_FOUND_ERROR"};

void server_init()
{
    // Send pid to
    sprintf(msg.target_user, "SERVER-INIT");
    sprintf(msg.content, "%d", getpid());
    write(publicpipe, (char *)&msg, sizeof(msg));
}
void handle_target(int signum = 15)
{
    // TODO: daha cok herkesin kendi room'u gibi yapalim
    // degilse, ahmet dedikten sonra da araya mehmet girebiliyor

    cout << "\nWho do you want to send message?\n"
         << ">";
    cin >> target_user;
    cout << "new target user name is " << target_user << "\n";
    sprintf(msg.target_user, "%s", target_user.c_str());
}
void handle_duplicate_name()
{
    string tempName;
    tempName.assign(from_user);
    while (!tempName.compare(from_user))
    {
        cout << "This name has already been taken please choose new name\n";
        cout << "Your new name is:?\n"
             << ">";
        cin >> from_user;
    }
    sprintf(msg.from_user, "%s", from_user.c_str());
    server_init();
    cout << "Waiting for server to verify your name...";
    pause();
}
void on_messsage(int signum)
{
    if ((privatepipe = open(selfpipe_path, O_RDONLY)) == -1)
    {
        perror(selfpipe_path);
        return;
    }

    while ((n = read(privatepipe, buffer, PIPE_BUF)) > 0)
    {
        if (buffer == ERRORS[0])
        {
            handle_duplicate_name();
        }
        else
            write(fileno(stdout), buffer, n);
    }
    cout << endl;

    close(privatepipe);
}
void handle_offline(int signum)
{
    cout << "User is offline! Please try again later!\n";
    handle_target();
}
void handle_disconnect(int signum)
{
    sprintf(msg.target_user, "USER-DISCONNECT");
    sprintf(msg.content, "%d", getpid());
    write(publicpipe, (char *)&msg, sizeof(msg));
    cout<<"Successfully disconnected";
    exit(0);
}


int main()
{
    signal(2, handle_disconnect);
    signal(4, handle_offline);
    signal(10, on_messsage);
    signal(15, handle_target);

    cout << "Hi, What's your name?\n"
         << ">";
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
    cout << "Waiting for server to verify your name...\n";
    pause();

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
