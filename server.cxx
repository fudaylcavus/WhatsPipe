#include "local.h"
#include <iostream>
#include <string.h>
#include <signal.h>
#include <map>

using namespace std;

struct message msg;
map<string, int> user_pid;
char targetpipe_path[50];


int number_parser(char arr[], int size)
{
    int j = 0;
    for (int i = 0; i < size; i++)
    {
        if (arr[i] >= '0' && arr[i] <= '9')
        {
            arr[j++] = arr[i];
        }
    }
    arr[j] = '\0';

    int pid;
    sscanf(arr, "%d", &pid);
    return pid;
}

void array_copier(char s[], char d[], int size)
{
    for (int i = 0; i < size; i++)
    {
        d[i] = s[i];
    }
}

void user_init()
{
    // Pid parser:
    int pid;
    sscanf(msg.content, "%d", &pid);

    // char[] to string and map insertion
    string from(msg.from_user);
    user_pid.insert(pair<string, int>(from, pid));


    cout << "New User<" << from << "," << user_pid[from] << "> saved!" << endl;
}


int main()
{
    int n, done, dummypipe, publicpipe, privatepipe;
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
        cout << "From: " << msg.from_user << "\nTo: " << msg.target_user << "\nContent: " << msg.content << endl;
        if (!strcmp(msg.target_user, "SERVER-INIT"))
        {
            user_init();
            //TODO: send error to sender if name is used
            //client should re-take server_init action
            continue;
        }
        n = done = 0;


        string target(msg.target_user);
        int target_pid = user_pid[target];
        if (!target_pid) {
            // TODO: send to sender:
            // SERVER: Target user is offline!
            continue;
        }

        kill(target_pid, 10);
        sprintf(targetpipe_path, "/tmp/pipe%d", target_pid);
        cout << "Target pipe path: " << targetpipe_path << endl;

        do {
            if ((privatepipe = open(targetpipe_path, O_WRONLY | O_NDELAY)) == -1) {
                sleep(3);
            } else {
                //TODO, su anda client sadece msg.content aliyor
                // tum mesaj objesini gonder.
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