#ifndef _OURTREE_H_
#define _CMDTREE_H_

//-------------------库
//ourtree命令的库
#include <stdio.h>
#include <string.h>
#include <dirent.h>


//-------------------宏


//-------------------函数声明
void show_directory_structure(const char *path);
void show_directory_structure_recursive(const char *path, int depth);



#endif