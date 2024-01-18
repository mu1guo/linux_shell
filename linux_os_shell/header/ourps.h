#ifndef _OURPS_H_
#define _CMDPS_H_

//-------------------库
//ps命令的库：
#include <dirent.h>
#include <limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#include "parameter.h"
#include "printsetting.h"

//-------------------宏


//-------------------函数声明
//ps命令的函数声明

void ps_pid(int pid);
void ps() ;
int ourps(Command* commands) ;

#endif