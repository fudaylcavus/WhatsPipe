#include "client/shared.h"
#include "client/screen.h"
#include "client/events.h"
#include <stdio.h>
#include <iostream>

using namespace std;

void on_winresize(int signum)
{
    handle_resize();
    if (signum != 0)
        print_messages(target_user);
}

void on_serverinit()
{
    snprintf(msg.target_user, sizeof(msg.target_user), "%s", "SERVER-INIT");
    snprintf(msg.content, sizeof(msg.content), "%d", getpid());
    write(publicpipe, (char *)&msg, C_PIPE_BUF);

    cout << "Waiting for server to verify your name...\n";
    pause();
}

void on_duplicatename()
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
    on_serverinit();
    cout << "Waiting for server to verify your name...";
    pause();
}

void on_disconnect(int signum)
{
    snprintf(msg.content, sizeof(msg.content), "%s", msg.target_user);
    snprintf(msg.target_user, sizeof(msg.target_user), "%s", "USER-DISCONNECT");
    write(publicpipe, (char *)&msg, C_PIPE_BUF);
    cout << "Successfully disconnected";
    exit(0);
}

void on_message(int signum)
{
    if ((privatepipe = open(selfpipe_path, O_RDONLY)) == -1)
    {
        perror(selfpipe_path);
        return;
    }

    message temp_msg;
    int n;
    while ((n = read(privatepipe, (char *)&temp_msg, C_PIPE_BUF)) > 0)
    {
        if (temp_msg.content == ERRORS[0])
        {
            on_duplicatename();
        }
        else
        {
            insert_message(temp_msg.from_user, "0" + string(temp_msg.content));
            print_messages(temp_msg.from_user);
        }
    }
    close(privatepipe);
}

void on_newtarget(int signum = 15)
{
    cout << "\nWho do you want to send message?\n"
         << ">";
    cin >> target_user;
    snprintf(msg.target_user, sizeof(msg.target_user), "%s", target_user.c_str());
    print_messages(target_user);
}

void on_useroffline(int signum)
{
    cout << "User is offline!\n";
    on_newtarget(0);
}
