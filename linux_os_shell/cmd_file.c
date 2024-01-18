//所有和文件操作相关的，我都放到这里了：
#include "cmd_file.h"   

//---------------------------------------------------------------------
//第一部分：简单的6个文件操作的功能函数

// 判断是否为目录
int ifDirectory(const char *path) {
  struct stat path_stat;

  if (stat(path, &path_stat) != 0) {
    perror("打开目录错误");
    exit(EXIT_FAILURE);
  }

  return S_ISDIR(path_stat.st_mode);
}

// 判断目录一是否包含目录二
int pathIncluded(const char *path1, const char *path2) {
  size_t len1 = strlen(path1);
  size_t len2 = strlen(path2);

  if (len2 < len1) {
    return 0;
  }

  // 比较两个字符串的前len1个字符是否相等。
  if (strncmp(path2, path1, len1) == 0) {
    if (path2[len1] == '/' || path2[len1] == '\0') {
      return 1;
    }
  }

  return 0;
}

// 使用C语言的库函数删除给定的文件名 filename，以便它不再被访问
void removeFile(const char *filename) {
  if (remove(filename) == 0) {
    return;
  }
  perror("删除文件失败");
}

// 迭代删除目录中的全部文件
int removeDirectory(const char *path) {
  DIR *d = opendir(path);
  size_t path_len = strlen(path);
  int r = -1;

  if (d) {
    struct dirent *p;
    r = 0;

    // 调用readdir遍历目录子文件
    while (!r && (p = readdir(d))) {
      int r2 = -1;
      char *buf;
      size_t len;

      // 跳过 "." 和 ".."
      if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, "..")) continue;

      len = path_len + strlen(p->d_name) + 2;
      buf = malloc(len);

      if (buf) {
        struct stat statbuf;

        snprintf(buf, len, "%s/%s", path, p->d_name);

        if (!stat(buf, &statbuf)) {
          if (S_ISDIR(statbuf.st_mode)) {
            r2 = removeDirectory(buf);
          } else {
            r2 = unlink(buf);     // 删除链接文件，成功返回0，失败则返回-1
          }
        }

        free(buf);
      }

      r = r2;
    }

    closedir(d);
  }

  if (!r) {
    r = rmdir(path); 
  }

  return r;
}

// 循环使用fread和fwrite复制文件
void copyFile(const char *source, const char *destination) {
  FILE *source_file, *destination_file;
  char buffer[1024];
  size_t bytesRead;

  source_file = fopen(source, "rb");
  if (source_file == NULL) {
    perror("源文件打开失败");
    exit(EXIT_FAILURE);
  }

  destination_file = fopen(destination, "wb");
  if (destination_file == NULL) {
    perror("目标文件打开失败");
    fclose(source_file);
    exit(EXIT_FAILURE);
  }

  while ((bytesRead = fread(buffer, 1, sizeof(buffer), source_file)) > 0) {
    fwrite(buffer, 1, bytesRead, destination_file);
  }

  fclose(source_file);
  fclose(destination_file);
}

// 复制整个目录
void copyDirectory(const char *src_path, const char *dest_path) {
  DIR *dir;
  struct dirent *entry;
  struct stat src_stat;
  char src_file_path[MAX_CHAR_SIZE];
  char dest_file_path[MAX_CHAR_SIZE];

  // 打开源目录
  if ((dir = opendir(src_path)) == NULL) {
    perror("源目录打开失败");
    exit(EXIT_FAILURE);
  }

  // 如果目标目录不存在就创建目标目录
  mkdir(dest_path, 0755);

  // Read directory entries
  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) {
      sprintf(src_file_path, "%s/%s", src_path, entry->d_name);
      sprintf(dest_file_path, "%s/%s", dest_path, entry->d_name);

      // 获取文件状态
      if (stat(src_file_path, &src_stat) == -1) {
        perror("获取文件状态失败");
        closedir(dir);
        exit(EXIT_FAILURE);
      }

      // 确认是文件还是目录
      if (S_ISDIR(src_stat.st_mode)) {
        // 复制目录
        copyDirectory(src_file_path, dest_file_path);
      } else {
        // 复制文件
        copyFile(src_file_path, dest_file_path);
      }
    }
  }
}


// 调用上述函数接口，实现下面3条命令

// 复制文件或目录
int ourcp(Command* commands) {
  int argc = commands->cnt;
  char** argv = commands->args;
  // 必须要3个参数
  if (argc < 3) {
    printf("帮助: ourcp 源文件 目标文件...\n");
    exit(EXIT_FAILURE);
  }

  // F_OK 值为0，判断文件是否存在
  if (access(argv[argc-1], F_OK) != 0) {
    FILE *file = fopen(argv[argc-1], "w");
    fclose(file);
  } 

  // 目标文件不是目录
  if (!ifDirectory(argv[argc-1])) {
    // 源是目录
    if (ifDirectory(argv[1])) {
      printf("%s 不存在或者尝试复制目录到一个文件中\n", argv[argc-1]);
      remove(argv[1]);
      exit(EXIT_FAILURE);
    }
    // 源文件也不是目录就复制文件
    copyFile(argv[1], argv[argc-1]);
    //----------------------------------------------------------------
    //---------------------------------------------------------------
    //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //exit(EXIT_SUCCESS);                
    return 0;
    //-------------------------------------------
  }

  // 判断是否存在包含关系
  if (pathIncluded(argv[1], argv[argc-1])) {
    printf("%s 包含 %s, 不能复制!\n", argv[1], argv[argc-1]);
    exit(EXIT_FAILURE);
  }

  // 1是目录，就复制目录到目录
  if (ifDirectory(argv[1])) {
    const char *filename = basename(argv[1]);
    char path_in_dest[MAX_CHAR_SIZE];
    snprintf(path_in_dest, sizeof(path_in_dest), "%s/%s", argv[argc-1], filename);
    copyDirectory(argv[1], path_in_dest);
  } else {
    // 如果不是目录说明后面的都是文件，复制后面很多个文件到argv[argc-1]中
    for (int i = 1; i < argc-1; ++i) {
      if (ifDirectory(argv[i])) {
        PRINT_COLOR(argv[i], RED, 0);
        PRINT_COLOR(" 是目录 ", RED, 0);
        continue;
      }
      const char *filename = basename(argv[i]);
      char path_in_dest[MAX_CHAR_SIZE];
      snprintf(path_in_dest, sizeof(path_in_dest), "%s/%s", argv[argc-1], filename);
      copyFile(argv[i], path_in_dest);
    }
  }

  return 0;
}

// 移动文件，代码逻辑和cp类似，增加了复制文件后删除文件
int ourmv(Command* commands) {
  int argc = commands->cnt;
  char** argv = commands->args;
  if (argc < 3) {
    printf("帮助: ourmv 源文件 目标文件...\n");
    //exit(EXIT_FAILURE);
    //----------------------------------------------------
    return 1;
    //---------------------------------------
  }

  if (access(argv[argc-1], F_OK) != 0) {
    FILE *file = fopen(argv[argc-1], "w");
    fclose(file);
  }

  if (pathIncluded(argv[1], argv[argc-1])) {
    printf("%s 包含 %s, 不能复制!\n", argv[1], argv[argc-1]);
    exit(EXIT_FAILURE);
  }

  if (!ifDirectory(argv[argc-1])) {
    if (ifDirectory(argv[1])) {
      printf("%s not exits or try to move a directory to a file\n", argv[argc-1]);
      remove(argv[1]);
      exit(EXIT_FAILURE);
    }
    copyFile(argv[1], argv[argc-1]);
    removeFile(argv[1]);

    //---------------------------------------------------------------
    //-----------------------------------------------------------------
    //exit(EXIT_SUCCESS);
    return 0;
    //---------------------------------------
  }

  if (ifDirectory(argv[1])) {
    const char *filename = basename(argv[1]);
    char path_in_dest[MAX_CHAR_SIZE];
    snprintf(path_in_dest, sizeof(path_in_dest), "%s/%s", argv[argc-1], filename);
    copyDirectory(argv[1], path_in_dest);
    removeDirectory(argv[1]);
  } else {
    for (int i = 1; i < argc-1; ++i) {
      if (ifDirectory(argv[i])) {
        PRINT_COLOR(argv[i], RED, 0);
        PRINT_COLOR(" 是目录 ", RED, 0);
        continue;
      }
      const char *filename = basename(argv[i]);
      char path_in_dest[MAX_CHAR_SIZE];
      snprintf(path_in_dest, sizeof(path_in_dest), "%s/%s", argv[argc-1], filename);
      if (access(path_in_dest, F_OK) == 0) {
        continue;
      }
      copyFile(argv[i], path_in_dest);
      removeFile(argv[i]);
    }
  }

  return 0;
}

// 删除文件
int ourrm(Command* commands) {
  int argc = commands->cnt;
  char** argv = commands->args;
  char *path = ".";
  char *option = "";

  if (argc < 2) {
    printf("帮助: ourrm [-r] 文件/目录\n");
  }

  if (argc < 4) {
    if (argc == 3) {
      if (argv[1][0] == '-') {
        option = argv[1] + 1;
        path = argv[2];
        // 加了-r就是删除文件夹
        if (strstr(option, "r") != NULL) {
          removeDirectory(path);
        } else {
          printf("帮助: ourrm [-r] 文件/目录\n");
        }
        return 0;
      }
    }
  }

  for (int i = 1; i < argc; ++i) {
    removeFile(argv[i]);
  }

  return 0;
}


//-----------------------------------------------------------------