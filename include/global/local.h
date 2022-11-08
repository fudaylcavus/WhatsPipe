#include <cstdio>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <string>

using namespace std;

/* ---- Sizes ---- */
const int C_PIPE_BUF = 8192;
const int MSGCONTENT_SIZ = (C_PIPE_BUF - 100);
const int C_NAME_SIZ = 50;

/* ---- Variables ---- */
const string WHITESPACE = " \n\r\t\f\v";
const char PUBLIC[] = "/tmp/PUBLIC";
const string ERRORS[] = {"SERVER_DUPL_NAME_ERROR", "SERVER_TARGET_NOT_FOUND_ERROR"}; 

/* ---- Structs ---- */
struct message {
    char target_user[C_NAME_SIZ]; 
    char content[MSGCONTENT_SIZ];
    char from_user[C_NAME_SIZ];
};

