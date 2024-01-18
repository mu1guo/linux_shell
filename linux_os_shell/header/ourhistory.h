#ifndef _OURHISTORY_H_
#define _CMDHISTORY_H_

//-------------------库
//ourtree命令的库
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <fcntl.h>
#include <unistd.h>

#include "parameter.h"
#include "printsetting.h"
//-------------------宏


//-------------------函数声明

void saveHistory(char *str);
void printHistory(Command *command);
void callHistory(Command *command, char history[][BUF_SIZE]);


#endif