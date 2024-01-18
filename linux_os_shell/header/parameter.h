/* included only once*/
#ifndef _PARAMETER_H_
#define _PARAMETER_H_

//---------------------------这里面只包含：宏定义 和 结构体定义
//--原架构宏
#define MAX_ARGS 8            //单条命令的参数个数上限
#define MAX_COMMANDS 8        //管道连接的命令个数上限
#define BUF_SIZE 128          //命令行字符个数上限
#define MAX_HISTORY 100       //历史命令记录上限

//这里的宏都是直接替换成了指定的“语句序列”依次执行，比如第一个ERROR_EMPTY（errorStr）就是接收一个参数errorStr，然后执行输出语句，并且返回false，注意，这里不是“子函数”而是在“那个位置”返回false
#define ERROR_EMPTY(errorStr) do {fprintf(stderr,"%sPlease check and try again :D\n",errorStr); return false;} while(0)
#define ERROR_IOFILE do {fprintf(stderr,"\033[1;31mError:\033[0m I/O file format error! \nPlease check and try again :D\n"); return false;} while(0)
#define ERROR_STR do {fprintf(stderr,"\033[1;31mError:\033[0m Your str miss \' or \" ! \nPlease check and try again :D\n"); return false;} while(0)
#define ERROR_ENV do {fprintf(stderr,"\033[1;31mError:\033[0m Your variable miss ! \nPlease check and try again :D\n"); return false;} while(0)
#define ERROR_FORK do {fprintf(stderr,"\033[1;31mError:\033[0m Failed to fork process!"); return; } while(0)
#define ERROR_OPEN do {fprintf(stderr,"\033[1;31mError:\033[0m Failed to open file! \nPlease check and try again :D\n");exit(1); return;} while(0)
#define ERROR_EXECUTE(errorCmd) do {fprintf(stderr, "\033[1;31mError:\033[0m Failed to execute cmd %s!\n", errorCmd);exit(1); return; } while(0)
#define ERROR_EXIT do {fprintf(stderr, "\033[1;31mError:\033[0m Failed to exit with status %d!\n", WEXITSTATUS(status)); return; } while(0)
#define ERROR_FMT(errorCmd) do {fprintf(stderr, "\033[1;31mError:\033[0m Please read help doc and check the usage of %s!\n", errorCmd); return; } while(0)

//----------------------------
//李的宏
#define MAX_CHAR_SIZE 1024        // max char size
#define MAX_COMMAND_PER_LINE 8  // with pipe to seperate
#define MAX_ARGC 8              // max argument count each command
#define MAX_HISTORY_SIZE 100    // history log size

/* readable */
// standard file descriptor
#define STDIN 0
#define STDOUT 1
#define STDERR 2

// inial authoritation of a new file when automatically created
#define MODE 0666

// different oflags value when calling open()
#define OUTR_A_FLAG O_WRONLY | O_CREAT | O_APPEND
#define OUTR_FLAG O_WRONLY | O_CREAT | O_TRUNC

/* set foreground color */
#define RED 1
#define YEL 2
#define BLU 3
#define GRE 4

//指令结构体
typedef struct {
    char *read;         //输入重定向 的文件名
    char *write;        //追加输出重定向 的文件名
    char *overwrite;    //覆盖输出重定向 的文件名
    char **args;        //参数字符串数组——针对于这一条命令
    char *cmd;          //命令名
    int cnt;            //这条命令的参数个数
    //--为了判断每个命令是否需要后台执行--添加一个参数
    int is_background;  //is_background 初始为0，设置为1的时候说明需要后台执行
} Command;



#endif