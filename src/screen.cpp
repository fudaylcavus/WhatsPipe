#include "global/colors.h"
#include "global/enums.h"
#include "client/screen.h"
#include "client/shared.h"
#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>
#include <map>
#include <vector>

using namespace std;

struct winsize w;

int TERM_WIDTH;

#define gotoxy(x, y) printf("\033[%d;%dH", (y), (x))

void handle_resize() {
    ioctl(0, TIOCGWINSZ, &w);
    TERM_WIDTH = w.ws_col;
}

void create_input_gap()
{
    switch (fork())
    {
    case 0:
        execl("/usr/bin/tput", "tput", "cup", "9999", "0", NULL);
        break;
    default:
        break;
    }
}


void align_right(int box_width)
{
    for (int i = 0; i < TERM_WIDTH - box_width; i++)
    {
        cout << " ";
    }
}

void handle_input_border()
{
    int column = w.ws_col;
    int t_len = target_user.length();

    gotoxy(0, column - 1);

    string input_border;
    int i = 0;
    while (i < column)
    {
        if (i == (column / 2 - (t_len / 2)))
        {
            input_border.append(target_user);
            i += t_len;
        }
        else
        {
            input_border.append("=");
            i++;
        }
    }

    cout << input_border << endl;
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
            align_right(6 + message.length());
            cout << SetBackGRN << "   ";
            for (int j = 0; j < message.length(); j++)
            {
                cout << " ";
            }
            cout << "   " << RESETTEXT << endl;

            align_right(6 + message.length());
            cout << SetBackGRN << "   " << message << "   " << RESETTEXT << endl;

            align_right(6 + message.length());
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
    handle_input_border();
    create_input_gap();
}