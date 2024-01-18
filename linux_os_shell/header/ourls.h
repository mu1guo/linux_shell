#ifndef _OURLS_H_
#define _OURLS_H_

//---------ourls.c所需的库 和 头文件
//借鉴的库和宏定义：
#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "parameter.h"  //包含参数头文件-里面有宏定义和Command结构体定义
#include "printsetting.h"  //这个也包含上吧

//---------函数声明:
//函数声明：
void print_file_info(const char *filename, const struct stat *fileStat);
int callmyls(Command  *commands);

#endif