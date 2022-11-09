#include "global/enums.h"
#include <string>
#include <map>
#include <vector>

using namespace std;

string from_user;
string target_user;
map<string, vector<string> > user_msg;
map<string, int> user_unreadc;
int total_unreadc;
int publicpipe;
int privatepipe;
message msg;
char selfpipe_path[C_NAME_SIZ];
