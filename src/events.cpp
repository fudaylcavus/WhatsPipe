#include "client/shared.h"
#include "client/screen.h"
#include "client/events.h"
#include <stdio.h>
#include <iostream>
#include <thread>
#include <signal.h>

using namespace std;
int reader_pid;

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

void handle_message()

{

    while (1)
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
                if (temp_msg.from_user == target_user)
                {
                    insert_message(temp_msg.from_user, "0" + string(temp_msg.content));
                    print_messages(temp_msg.from_user);
                }
                else
                {
                    user_unreadc[temp_msg.from_user]++;
                    total_unreadc++;
                    string system_message = string("New Message: ") + temp_msg.from_user + " (" + to_string(user_unreadc[temp_msg.from_user]) + ")";
                    insert_message(temp_msg.from_user, "0" + string(temp_msg.content));
                    insert_message(target_user, "2" + string(system_message));
                    print_messages(target_user);
                    remove_last_message(target_user);
                }
            }
        }
        reader_pid = getpid();
        pause();
        close(privatepipe);
    }
}

void start_thread()
{
    thread(handle_message).detach();
}
void wake_up_process(int signum)
{
    kill(reader_pid, 14);
}
void on_newtarget(int signum = 15)
{
    cout << "\nWho do you want to send message?\n"
         << ">";
    cin >> target_user;
    snprintf(msg.target_user, sizeof(msg.target_user), "%s", target_user.c_str());

    total_unreadc -= user_unreadc[msg.target_user];
    user_unreadc[msg.target_user] = 0;

    print_messages(target_user);
}

void on_useroffline(int signum)
{
    cout << "User is offline!\n";
    on_newtarget(0);
}
void ignore_signal(int signum) {}
