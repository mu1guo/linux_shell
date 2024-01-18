#include "ourhistory.h"

//从main.c搬过来声明一下
extern int cmdnum;                          //记录历史命令的个数
extern char history[MAX_HISTORY][BUF_SIZE]; //存放历史命令
extern int status;                          //。。。。。。。。。。。。似乎这里没用到

//history
void saveHistory(char *str) {
    if (cmdnum >= MAX_HISTORY) {
        cmdnum = 0;
        memset(history, 0, sizeof history);
    }
    strcpy(history[cmdnum++], str);
}

void printHistory(Command *command) {
    //history
     if (command->cnt == 1 || command->cnt == 3) {
        for (int i = 0; i < cmdnum; ++i) {
            printf("%d  %s", i + 1, history[i]);
        }
        return ;
    }
    //history + num
    else if (command->cnt == 2 || command->cnt == 4) {
        //check the argument
        for (char *c = command->args[1]; *c; ++c) {
            if (*c < '0' || *c > '9') 
                ERROR_FMT(command->cmd);
        }
        int n = atoi(command->args[1]);
        if (n > cmdnum) n = cmdnum;
        for (int i = n; i > 0 && cmdnum - i >= 0; --i) {
            printf("%d  %s", cmdnum - i + 1, history[cmdnum - i]);
        }
        return;
    }
}

void callHistory(Command *command, char history[][BUF_SIZE]) {
    //history 或 history + num
    if (command->cnt == 1 || command->cnt == 2) {
        printHistory(command);
        return ;
    }
    //参数不符合要求
    else if (command->cnt > 4 || command->read || (!command->write && !command->overwrite)) { ERROR_FMT(command->cmd); }
    //重定向操作
    else if (command->cnt == 3 || command->cnt == 4) {
        int fd;
        int out = dup(1); //暂存文件描述符
        char *file = command->write ? command->write : command->overwrite;
        if (command->write) {
            fd = open(file, O_WRONLY | O_TRUNC | O_CREAT, 0666); 
        }
        else if (command->overwrite) {
            fd = open(file, O_WRONLY | O_APPEND | O_CREAT, 0666); 
        }
        if (fd < 0) {fprintf(stderr, "\033[1;31mError:\033[0m Failed to open file %s\n", file); return; }
        dup2(fd, 1);
        printHistory(command);
        dup2(out, 1); //恢复文件描述符
        close(fd);
        close(out);
        return ;
    }
}