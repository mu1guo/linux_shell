#ifndef _CMDFILE_H_
#define _CMDFILE_H_

//----------------------------这个头文件：只包含cmd_file.c函数需要的.h头文件库 和 函数声明
#include <dirent.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "parameter.h"  //包含参数头文件-里面有宏定义和Command结构体定义
#include "printsetting.h"  //这个也包含上吧


//---函数声明

int is_directory(const char *path);

/* fucntion `is_path_contained`:
      If path1 contains path2, return 1, else 0.
*/
int is_path_contained(const char *path1, const char *path2);

void remove_file(const char *filename);

int remove_directory(const char *path);

void copy_file_to_file(const char *source, const char *destination);

void copy_directory(const char *src_path, const char *dest_path);


//--------------
//mv,cp,rm函数声明
int ourcp(Command* commands);
int ourmv(Command* commands);
int ourrm(Command* commands);

#endif