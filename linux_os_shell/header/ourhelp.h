#ifndef _OURHELP_H_
#define _CMDHELP_H_

//-------------------库
#include <stdio.h>
#include <stdlib.h>

#include <sys/wait.h>
#include <sys/types.h>

#include <fcntl.h>
#include <unistd.h>

#include "parameter.h"
#include "printsetting.h"
//-------------------宏


//-------------------函数声明
void printHelp() ;
void callHelp(Command *command);


#endif