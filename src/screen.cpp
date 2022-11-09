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

void handle_resize()
{
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

string align_center(string printable_box)
{
    int column = w.ws_col;
    int box_w = printable_box.length();

    string output;

    int i = 0;
    while (i < column)
    {
        if (i == (column / 2 - (box_w / 2)))
        {
            output.append(printable_box);
            i += box_w;
        }
        else
        {
            output.append(" ");
            i++;
        }
    }
    return output;
}

void align_right(int box_width)
{
    for (int i = 0; i < TERM_WIDTH - box_width; i++)
    {
        cout << " ";
    }
}

void print_bubble_row(string content, string bg_color, BubbleRow row, Aligment align)
{
    string message_white_space;
    for (int j = 0; j < content.length() + 6; j++)
        message_white_space.append(" ");

    switch (row)
    {
    case MIDDLE:
        if (align == RIGHT)
            align_right(6 + content.length());
        cout << bg_color << "   " << content << "   " << RESETTEXT << endl;
        break;

    default:
        if (align == RIGHT)
            align_right(6 + content.length());
        cout << bg_color << message_white_space << RESETTEXT << endl;
        break;
    }

    if (row == BOTTOM)
        cout << endl;
}

void print_message_bubble(string message, MessageStatus status)
{
    switch (status)
    {
    case SENT:
        print_bubble_row(message, SetBackGRN, TOP, RIGHT);
        print_bubble_row(message, SetBackGRN, MIDDLE, RIGHT);
        print_bubble_row(message, SetBackGRN, BOTTOM, RIGHT);
        break;
    case RECEIVED:
        print_bubble_row(message, SetBackBBLK, TOP, LEFT);
        print_bubble_row(message, SetBackBBLK, MIDDLE, LEFT);
        print_bubble_row(message, SetBackBBLK, BOTTOM, LEFT);
        break;
    case SYSTEM:
        print_bubble_row(message, SetBackRED, TOP, LEFT);   
        print_bubble_row(message, SetBackRED, MIDDLE, LEFT);
        print_bubble_row(message, SetBackRED, BOTTOM, LEFT);
    }
}


void handle_header()
{
    int column = w.ws_col;
    gotoxy(0, 2);

    string input_border;
    string white_space;
    for (int i = 0; i < column; i++)
        white_space += " ";

    cout << SetBackWHT << SetForeBLK << white_space << RESETTEXT << endl;
    cout << SetBackWHT << SetForeBLK << align_center(target_user) << RESETTEXT << endl;
    cout << SetBackWHT << SetForeBBLK << align_center("Type '<-' to go back") << RESETTEXT << endl;
    cout << SetBackWHT << SetForeBLK << white_space << RESETTEXT << endl;
}

void handle_input_border()
{
    int column = w.ws_col;
    gotoxy(0, column - 1);

    string input_box;
    for (int i = 0; i < column; i++)
        input_box.append("–");

    cout << input_box << endl;
}

void print_messages(string user)
{
    // Erases Display
    cout << "\e[2J";

    vector<string> messages = user_msg[user];
    for (int i = 0; i < messages.size(); i++)
    {
        string prefix = messages[i].substr(0, 1);
        string message = messages[i].substr(1);
        int status = atoi(prefix.c_str());

        switch (status)
        {
        case RECEIVED:
            print_message_bubble(message, RECEIVED);
            break;
        case SENT:
            print_message_bubble(message, SENT);
            break;
        case SYSTEM:
            print_message_bubble(message, SYSTEM);
            break;
        default:
            break;
        }
    }
    handle_header();
    handle_input_border();
    create_input_gap();
}