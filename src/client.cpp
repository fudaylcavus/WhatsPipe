#include "global/colors.h"
#include "client/shared.h"
#include "client/screen.h"
#include "client/client.h"
#include "client/events.h"
#include <iostream>

using namespace std;

string rtrim(const char charstring[])
{
    string s(charstring);
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == string::npos) ? "" : s.substr(0, end + 1);

}

void insert_message(string user, string message)
{
    user_msg[user].push_back(message);
}

int init_clientside() {
    cout << "Hi, What's your name?\n"
         << ">";
    cin >> from_user;

    //Generate Relative Pipe Path
    cout << "Welcome, " << from_user;
    snprintf(selfpipe_path, sizeof(selfpipe_path), "/tmp/pipe%d", getpid());

    
    //Generate Pipe
    if (mknod(selfpipe_path, S_IFIFO | 0666, 0) < 0)
    {
        perror(selfpipe_path);
        return 1;
    }

    //Establish a connection with Server
    if ((publicpipe = open(PUBLIC, O_WRONLY)) == -1)
    {
        perror(PUBLIC);
        return 2;
    }

    return 0;
}

void update_message_route() {
    snprintf(msg.target_user, sizeof(msg.target_user), "%s", target_user.c_str());
    snprintf(msg.from_user, sizeof(msg.from_user), "%s", from_user.c_str()); 
}

void handle_new_content() {
    memset(msg.content, 0x0, MSGCONTENT_SIZ);
    read(fileno(stdin), msg.content, MSGCONTENT_SIZ);
    snprintf(msg.content, sizeof(msg.content), "%s", rtrim(msg.content).c_str());
}


int main()
{
    on_winresize(0);
    signal(2, on_disconnect);
    signal(4, on_useroffline);
    signal(10, on_message);
    signal(15, on_newtarget);
    signal(28, on_winresize);

    init_clientside();
    update_message_route();

    on_serverinit();


    while (1)
    {
        print_messages(msg.target_user);
        handle_new_content();
        if (!strcmp("<-", msg.content))
        {
            on_newtarget(0);
            continue;
        }
        update_message_route();
        write(publicpipe, (char *)&msg, C_PIPE_BUF);
        insert_message(msg.target_user, "1" + string(msg.content));
    }
    close(publicpipe);
    unlink(selfpipe_path);
    return 0;
}
