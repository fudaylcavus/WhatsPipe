#include <iostream>
#include "colors.h"
#include <map>
#include <vector>
#include <string>

using namespace std;

map<string, vector<string> > user_msg;

enum MessageStatus
{
    RECEIVED,
    SENT
};

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

        //TODO: all of those cout codes are duplicate and works for printing,
        // even space. Create a function for it
        switch (status)
        {
        case RECEIVED:
            cout << SetBackBBLK << "   ";
            for (int j = 0; j < message.length(); j++)
            {
                cout << " ";
            }
            cout << "   " << RESETTEXT << endl;

            cout << SetBackBBLK << "   " << message << "   " << RESETTEXT <<endl;

            cout << SetBackBBLK << "   ";
            for (int j = 0; j < message.length(); j++)
            {
                cout << " ";
            }
            cout << "   " << RESETTEXT << endl << endl;

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
            cout << "   " << RESETTEXT << endl << endl;
        default:
            break;
        }
    }
}

int main()
{
    insert_message("Fudayl", "0Merhaba!");
    insert_message("Fudayl", "0Nasilsin! Abi ben gece lc cozmeyi unuttum ya!");
    insert_message("Fudayl", "1Iyi, sen?");
    insert_message("Fudayl", "1Hay allah ya, ben cozdun sanmistim");
    insert_message("Fudayl", "1:(");
    print_messages("Fudayl");

    do {
        char line_input[100];
        cout << "----------------Input----------------" << endl;
        cin.getline(line_input, 100);
        string new_message(line_input);

        insert_message("Fudayl", "1" + new_message);
        print_messages("Fudayl");
    } while (1);
    

    return 0;
}
