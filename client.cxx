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
#include <map>

using namespace std;

const string WHITESPACE = " \n\r\t\f\v";

int n, privatepipe;
char selfpipe_path[50];
static char buffer[PIPE_BUF];
struct message msg;
int publicpipe;
string from_user;
string target_user;
string ERRORS[] = {"SERVER_DUPL_NAME_ERROR", "SERVER_TARGET_NOT_FOUND_ERROR"};

map<string, vector<string> > user_msg;

enum MessageStatus
{
    RECEIVED,
    SENT
};

string rtrim(const char charstring[])
{
    string s(charstring);
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}

void insert_message(string user, string message)
{
    user_msg[user].push_back(message);
}

void print_messages(string user)
{
    cout << "\e[2J";
    vector<string> messages = user_msg[user];
    for (int i = 0; i < messages.size(); i++)
    {
        string prefix = messages[i].substr(0, 1);
        string message = messages[i].substr(1);
        int status = atoi(prefix.c_str());

        // TODO: all of those cout codes are duplicate and works for printing,
        //  even space. Create a function for it
        switch (status)
        {
        case RECEIVED:
            cout << SetBackBBLK << "   ";
            for (int j = 0; j < message.length(); j++)
            {
                cout << " ";
            }
            cout << "   " << RESETTEXT << endl;

            cout << SetBackBBLK << "   " << message << "   " << RESETTEXT << endl;

            cout << SetBackBBLK << "   ";
            for (int j = 0; j < message.length(); j++)
            {
                cout << " ";
            }
            cout << "   " << RESETTEXT << endl
                 << endl;

            break;
        case SENT:
            cout << SetBackGRN << "   ";
            for (int j = 0; j < message.length(); j++)
            {
                cout << " ";
            }
            cout << "   " << RESETTEXT << endl;

            cout << SetBackGRN << "   " << message << "   " << RESETTEXT << endl;

            cout << SetBackGRN << "   ";
            for (int j = 0; j < message.length(); j++)
            {
                cout << " ";
            }
            cout << "   " << RESETTEXT << endl
                 << endl;
        default:
            break;
        }
    
    }
}

void server_init()
{
    // Send pid to
    snprintf(msg.target_user, sizeof(msg.target_user), "%s", "SERVER-INIT");
    snprintf(msg.content, sizeof(msg.content), "%d", getpid());
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
    snprintf(msg.target_user, sizeof(msg.target_user), "%s", target_user.c_str());
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
    snprintf(msg.from_user, sizeof(msg.from_user), "%s", from_user.c_str());
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

    message tempMsg;
    while ((n = read(privatepipe, (char *)&tempMsg, sizeof(tempMsg))) > 0)
    {
        if (tempMsg.content == ERRORS[0])
        {
            handle_duplicate_name();
        }
        else
        {
            insert_message(tempMsg.from_user, "0" + string(tempMsg.content));
            print_messages(tempMsg.from_user);
        }
    }
    close(privatepipe);
}
void handle_offline(int signum)
{
    cout << "User is offline!\n";
    handle_target();
}
void handle_disconnect(int signum)
{
    snprintf(msg.content, sizeof(msg.content), "%s", msg.target_user);
    snprintf(msg.target_user, sizeof(msg.target_user), "%s", "USER-DISCONNECT");
    write(publicpipe, (char *)&msg, sizeof(msg));
    cout << "Successfully disconnected";
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

    snprintf(msg.from_user, sizeof(msg.from_user), "%s", from_user.c_str());
    snprintf(selfpipe_path, sizeof(selfpipe_path), "/tmp/pipe%d", getpid());

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
        print_messages(msg.target_user);
        cout << "----------------Input----------------" << endl;
        memset(msg.content, 0x0, B_SIZ);
        read(fileno(stdin), msg.content, B_SIZ);
        snprintf(msg.content, sizeof(msg.content), "%s", rtrim(msg.content).c_str());
        cout << endl
             << msg.content << endl;
        if (!strcmp("<-", msg.content))
        {
            handle_target();
            continue;
        }
        snprintf(msg.target_user, sizeof(msg.target_user), "%s", target_user.c_str());
        snprintf(msg.from_user, sizeof(msg.from_user), "%s", from_user.c_str());
        write(publicpipe, (char *)&msg, sizeof(msg));
        insert_message(msg.target_user, "1" + string(msg.content));
    }
    close(publicpipe);
    unlink(selfpipe_path);
    return 0;
}
