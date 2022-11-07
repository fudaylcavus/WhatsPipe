#include "local.h"
#include <iostream>
#include <string.h>
#include <signal.h>
#include <map>

using namespace std;

struct message msg;
struct message serverMessage;
map<string, int> user_pid;
char targetpipe_path[C_NAME_SIZ];
string ERRORS[] = {"SERVER_DUPL_NAME_ERROR", "SERVER_TARGET_NOT_FOUND_ERROR"};

enum Location
{
    FROM,
    CONTENT,
    TARGET
};

bool is_user_exists(Location location)
{
    switch (location)
    {
    case FROM:
        return user_pid[msg.from_user] != 0;
    case CONTENT:
        return user_pid[msg.content] != 0;
    case TARGET:
        return user_pid[msg.target_user] != 0;
    default:
        return false;
    }
}

void user_init()
{
    // Pid parser:
    int pid;
    sscanf(msg.content, "%d", &pid);

    // char[] to string and map insertion
    string from(msg.from_user);
    user_pid[from] = pid;
    kill(pid, 15);

    cout << "New User<" << from << "," << user_pid[from] << "> saved!" << endl;
}

void handle_send_error(int targetPid)
{
    kill(targetPid, 10);
    snprintf(targetpipe_path, sizeof(targetpipe_path), "/tmp/pipe%d", targetPid);
    int targetPrivatePipe;
    int done = 0;
    int n = 0;
    do
    {
        if ((targetPrivatePipe = open(targetpipe_path, O_WRONLY | O_NDELAY)) == -1)
        {
            sleep(3);
        }
        else
        {

            write(targetPrivatePipe, serverMessage.content, C_PIPE_BUF - 100);
            close(targetPrivatePipe);
            done = 1;
        }
    } while (++n < 5 && !done);
    if (!done)
    {
        write(fileno(stderr),
              "\nNOTE: SERVER ** NEVER ** accessed private FIFO\n", 48);
    }
}
void error_handler(int errorType, int targetPid)
{

    snprintf(serverMessage.from_user, sizeof(serverMessage.from_user), "%s", "SERVER");
    snprintf(serverMessage.content, sizeof(serverMessage.content), "%s", ERRORS[errorType].c_str());
    snprintf(serverMessage.target_user, sizeof(serverMessage.target_user), "%d", targetPid);
    handle_send_error(targetPid);
}

void handle_taken_name()
{
    // pid of faulty user
    int targetPid;
    sscanf(msg.content, "%d", &targetPid);
    error_handler(0, targetPid);
}
void handle_user_disconnect()
{

    if (is_user_exists(CONTENT))
    {
        // string messageToUser = std::string("User ") + msg.from_user+ (" has been disconnected, You can not message to them anymore");
        int pid = user_pid[msg.content];
        kill(pid, 4);
    }

    user_pid.erase(msg.from_user);
}



int main()
{
    int n, done, dummypipe, publicpipe, privatepipe;

    mknod(PUBLIC, S_IFIFO | 0666, 0);

    if ((publicpipe = open(PUBLIC, O_RDONLY)) == -1 ||
        (dummypipe = open(PUBLIC, O_WRONLY | O_NDELAY)) == -1)
    {
        perror(PUBLIC);
        return 1;
    }

    while (read(publicpipe, (char *)&msg, C_PIPE_BUF) > 0)
    {
        cout << "From: " << msg.from_user << "\nTo: " << msg.target_user << "\nContent: " << msg.content << endl;
        if (!strcmp(msg.target_user, "SERVER-INIT"))
        {
            if (is_user_exists(FROM))
            {
                cout << "inside control" << endl;
                handle_taken_name();
                cout << "This map has user" << msg.from_user << endl;
                cout << "This map has user with pid" << msg.content << endl;
            }
            // shouldnt init if it has in map
            else
                user_init();
            // TODO: send error to sender if name is used
            // client should re-take server_init action
            continue;
        }
        else if (!strcmp(msg.target_user, "USER-DISCONNECT"))
        {
            if (is_user_exists(FROM))
            {
                handle_user_disconnect();
            }

            continue;
        }
        else
        {
            n = done = 0;
            string target(msg.target_user);
            int target_pid = user_pid[target];
            if (!target_pid)
            {
                kill(user_pid[msg.from_user], 4);
                // TODO: send to sender:
                // SERVER: Target user is offline!
                continue;
            }

            if (target_pid != 0)
                kill(target_pid, 10);
            snprintf(targetpipe_path, sizeof(targetpipe_path), "/tmp/pipe%d", target_pid);
            cout << "Target pipe path: " << targetpipe_path << endl;

            do
            {
                if ((privatepipe = open(targetpipe_path, O_WRONLY | O_NDELAY)) == -1)
                {
                    sleep(3);
                }
                else
                {
                    // TODO, su anda client sadece msg.content aliyor
                    //  tum mesaj objesini gonder.
                    write(privatepipe, (char *)&msg, C_PIPE_BUF);

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
    }
    return 0;
}