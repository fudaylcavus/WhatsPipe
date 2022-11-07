#include <cstdio>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <unistd.h> 
#include <stdlib.h>
using namespace std;
const int C_PIPE_BUF = 8192;
const char *PUBLIC = "/tmp/PUBLIC"; 
const int C_B_SIZ = (C_PIPE_BUF / 2);
const int C_NAME_SIZ = 50;

struct message {
    char target_user[C_NAME_SIZ]; 
    char content[C_PIPE_BUF - 100];
    char from_user[C_NAME_SIZ];
};