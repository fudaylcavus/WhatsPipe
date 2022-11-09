#include "global/local.h"
#include <map>
#include <vector>
#include <string>

extern int publicpipe;
extern message msg;
extern map<string, vector<string> > user_msg;

extern string from_user;
extern string target_user;

extern int privatepipe;
extern char selfpipe_path[C_NAME_SIZ];

extern void insert_message(string, string);
extern void remove_last_message(string);

