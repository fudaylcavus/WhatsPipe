#include <cstdio>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <unistd.h> 
#include <string.h>
#include <stdlib.h>
using namespace std;
const int PIPE_BUF = 8192;
const char *PUBLIC = "/tmp/PUBLIC"; 
const int B_SIZ = (PIPE_BUF / 2);
struct message {
    char username[B_SIZ]; 
    char content[B_SIZ];
};