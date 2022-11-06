#include "local.h"
#include <iostream>
#include <string.h>
#include <signal.h>
#include <map>

using namespace std;

struct message msg;
struct message serverMessage;
map<string, int> user_pid;
char targetpipe_path[50];
string ERRORS[] = {"SERVER_DUPL_NAME_ERROR", "SERVER_TARGET_NOT_FOUND_ERROR"};
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
    kill(pid, 15);
    cout << "New User<" << from << "," << user_pid[from] << "> saved!" << endl;
}

void handle_send_error(int targetPid)
{
    kill(targetPid, 10);
    sprintf(targetpipe_path, "/tmp/pipe%d", targetPid);
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

            write(targetPrivatePipe, serverMessage.content, B_SIZ);
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

    sprintf(serverMessage.from_user, "%s", "SERVER");
    sprintf(serverMessage.content, "%s", ERRORS[errorType].c_str());
    sprintf(serverMessage.target_user, "%d", targetPid);
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
    printf("BEFOREEE");
    for (auto it = user_pid.cbegin(); it != user_pid.cend(); ++it)
    {
        std::cout << "{" << (*it).first << ": " << (*it).second << "}\n";
    }
    user_pid.erase(msg.from_user);
    printf("AFTERRR");
    for (auto it = user_pid.cbegin(); it != user_pid.cend(); ++it)
    {
        std::cout << "{" << (*it).first << ": " << (*it).second << "}\n";
    }
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
            // checks if users map has username!
            // unless it's multidimensional map, map.count(key) returns 1 if it contains the key.
            bool isUserInMap = user_pid.count(msg.from_user) == 1 ? true : false;
            if (isUserInMap)
            {
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
            bool isUserInMap = user_pid.count(msg.from_user) == 1 ? true : false;
            if (isUserInMap)
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
            sprintf(targetpipe_path, "/tmp/pipe%d", target_pid);
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
    }
    return 0;
}