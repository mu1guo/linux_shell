//--------------------------包含必要的库文件
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <pwd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include<time.h>
#include <dirent.h>

//--------------------------包含自己的.h文件
#include "parameter.h"
#include "printsetting.h"
#include "cmd_file.h"
#include "ourls.h"
#include "ourps.h"
#include "ourtree.h"
#include "ourhistory.h"
#include "ourhelp.h"


//-------------------------------------定义参数
FILE *fp;                            //文件操作变量
char **environ;
char buf[BUF_SIZE];                  //用于存放这一次读入的所有字符串
char *start;                         //一个工具变量
char **args;                         //字符串数组--存放一个个参数字符串
int commandsCount = 0;               //记录指令的个数 —— 这一次输入的指令个数，也就是buf中包含的指令个数  --全局
int status = 0;                      //子进程退出状态  -- 先设置为0，方便后面使用wait（&status）
int cmdnum = 0;                      //记录历史命令的个数  --全局
char history[MAX_HISTORY][BUF_SIZE]; //存放历史命令  --全局 
char pathbak[BUF_SIZE];              //存放工作路径

//函数声明：---------------------------
//工具函数：
void prompt();                                                  //打印输入左边的东西
void createCommand(Command *cmd);                               //初始化cmd指针
void freeCommand(Command *command);                             //释放cmd指针
bool fetchFileName(char **bufAddr, char **cmdFileAddr);         //buf中的某个位置提取连续文件名给cmdFileAddr参数

//核心函数：
bool splitCommands(char *buf, Command *commands);               //解析buf提取commands
void forkToExecute(Command *command, int fd_in, int fd_out);    //负责if-else if：匹配执行
void executeCommands(Command *commands);                        //分情况调用forkToExecute


//解析main函数-入口+while循环
int main () {
    //(1)输入进入时候的提示页面
	printf("\033[1;33m-------------------------------------------------\n"); // \033 是 ASCII 字符转义的开始符号，[1;33m 是设置输出文本颜色的代码。其中 1 表示粗体，33 表示黄色。
	printf("\033[5;33m|   	    Welcome to WANG-AN's shell :)		|\n");
	printf("\033[1;33m-------------------------------------------------\n");
    //（2）while无限循环检查输入的命令
    while (1) {
        prompt();                                                    //打印提示信息
        if (fgets(buf, BUF_SIZE, stdin) != NULL) {                   //从标准输入-键盘读入-char字符串到buf这个char[]数组中
            saveHistory(buf);                                        //将读取的内容通过调用saveHistory函数保存下来-保存到二维的history的char[][]中
            Command *cmds = malloc(MAX_COMMANDS * sizeof(Command));  //创建一个Command结构体类型的数组cmds[],但是这里的cmds主要作为指向第一个命令的指针，这个结构体见上，主要包含参数字符串数组argv[][]和命令名char[]
            
            commandsCount = 1;                                       //这一次进入splitCommands函数的时候，是以命令数=1进入的，至于到底有多少，见里面的命令解析
            
            if (splitCommands(buf, cmds)) executeCommands(cmds);     //splitCommands函数，输入：buf-char[]当前输入命令的字符串 ， 这里的cmds该指令数组的基址 返回：bool变量
            free(cmds);
        }
    }
}

//---------------------------------------------------------------------

//（1）打印输入命令左边的东西
void prompt() {
    char *path = NULL;
    char host[1024];
    struct passwd *pwd;       //保存用户账户信息的结构体
    pwd = getpwuid(getuid()); //通过uid来获取用户信息结构体
    path = getcwd(NULL, 0);   //getcwd获取到的内容是存放在堆的
    gethostname(host, 1024);
    //add some special
    printf("(WANG-AN)");
    printf("\033[1;34m%s%s%s\033[0m%s\033[1;36m%s\033[0m $ ", pwd->pw_name, "@", host, ":", path); //彩色显示
    free(path);               //获取完就释放内存
}

//（2）初始化cmd指针
void createCommand(Command *cmd) {
    cmd->read = cmd->overwrite = cmd->write = NULL;
    cmd->args = NULL;
    cmd->cmd = NULL;
    cmd->cnt = 0;
    //--is_background
    cmd->is_background = 0;
}

//（3）释放cmd指针
void freeCommand(Command *command) {
    free(command->read);
    free(command->write);
    free(command->overwrite);
    free(command->cmd);
    for (char **args = command->args; *args; ++args) free(*args);
    free(command->args);
}

//（4）提取buf中的文件名
//从这个bufAddr作为起始位置，提取“空格/。”之前的完整文件名,将这个文件名字符串外加一个\0给到cmdFileAddr，成功提取返回true
bool fetchFileName(char **bufAddr, char **cmdFileAddr) {
    start = (*bufAddr); //记录bufAddr的起点位置
    char *buf = *bufAddr;
    while ((*buf != '\n') && (!isspace(*buf)) && (*buf != '|') && (*buf != '<') && (*buf != '>')) buf = ++(*bufAddr);
    if (buf == start) return false; //while之后，buf应该指向的是文件名的最后一个字符

    (*cmdFileAddr) = malloc(sizeof(char) * (buf - start + 1));
    memcpy((*cmdFileAddr), start, sizeof(char) * (buf - start)); //从start指针位置开始复制buf-start个字符过去
    (*cmdFileAddr)[buf - start] = '\0'; //最后一个字符设为 \0结尾
    return true;
}

//（5）逐个解析buf
//在main函数中，接收了从键盘输入的字符串char[]buf之后，创建了一个commands的数组元素是Command类型的结构体
bool splitCommands(char *buf, Command *commands) {
    int waitCommand = 1;                                           //标志位——1代表正在等待新的命令
    createCommand(commands);                                       //初始化commands指针指向的那个Command结构体
    args = commands->args = malloc(sizeof(char *) * MAX_ARGS);     //给全局的char**args和当前指针指向的结构体comands里面的args -char**分配足够的空间
    
    //整个while(1)无限循环就是包含了一个switch函数：
    while (1) {
        //buf-char*就是一个字符串咯，下面就是一个个的解析里面的东西
        switch(*buf) {
            case ' ' : 
                while ((*buf != '\n') && isspace(*buf)) {++buf;}   //遇到非换行的空白字符就继续后移
                break;

            case '\n': 
                //我们的shell仅支持单行命令，因此遇到换行符就表明命令结束，如果此时仍然处于等待命令状态则报错，除非没有输入过命令
                
                //如果是第一“个”命令commandsCount==1,已经完成的命令cnt==0,直接返回false，表示解析失败，返回false到main里面后，就不会继续执行，而是等下下一次输入
                if (commandsCount == 1 && commands->cnt == 0) return false;
                //如果waitCommand==1,说明还在等待命令，但是又用回车收尾，所以啊，直接报错就好了
                //--------------------------------
                //针对command & 这种情况，我们不得不多进行一些考虑
                if(commandsCount > 1)
                {
                    //为什么我不直接去检查&符号后面的回车呢！！真愚蠢 —— 不过，这里可以允许&之后的回车咯
                    Command* tmp = (commands-1);                      //上一条指令是background而且这一次
                    if(tmp->is_background == 1 && waitCommand == 1)   //因为waitCommand==1说明连命令名都没遇到
                    {
                        commandsCount--;
                        //模仿下面的代码
                        *args = malloc(sizeof(char));
                        *args = 0;                //NULL结尾
                        //commands++;              //commands指针后移一位--background已经后移过了
                        commands->cmd = NULL;
                        return true; //可以解析，只是不用解析更多命令了
                    }
                }

                //---------------------------------
                if (waitCommand) ERROR_EMPTY("\033[1;31mError:\033[0m Next command shouldn't be empty!\n"); //这个宏，其实就是替换成了固定的“语句序列”,这里直接返回false了
                //如果waitCommand!=1,说明没有在等待命令，那就说明在 这个“\n”换行符“之前”已经解析到命令了，我们只要给全局args分配一个char空间，等待下一个命令即可
                *args = malloc(sizeof(char));
                *args = 0;                 //NULL结尾
                commands++;                //commands指针后移一位
                commands->cmd = NULL;     //初始化下一条命令字符串名
                return true;              //返回true到main里面——允许解析, ！！！不会再看buf之后的内容了，当然，之后也不应该有内容了
            
            //对于管道|命令的实现原理，在help那里有详细的说明
            case '|' : 
                if (waitCommand) ERROR_EMPTY("\033[1;31mError:\033[0m Pipe should be used after a command!\n"); //管道符|前面没有命令可不行哈
                
                waitCommand = 1; //继续等待“管道符|后面的那个命令的获取”，所有还是要把waitCommand设置为1
                                 
                buf++;           //检查buf中的下一个char字符
                *args = malloc(sizeof(char));
                *args = 0;             //NULL结尾
                commandsCount++;       //这个buf中包含的指令数+1，你可以去main里面看，每次检查buf进来的时候都是commandsCount=1
                commands++;            //commands指针后移一位 - 后面2个初始化commands指针的操作和这个splitCommands开始相同
                createCommand(commands);
                args = commands->args = malloc(sizeof(char *) * MAX_ARGS);
                break; 

            //输入重定向处理
            case '<' : 
                if (waitCommand) ERROR_EMPTY("\033[1;31mError:\033[0m I/O redirection should be used after a command!\n"); //不说了，反正这个< 或者 >前面都要有命令
                
                buf++;
                while ((*buf != '\n') && isspace(*buf)) ++buf;                     //跳过所有的空格
                if (fetchFileName(&buf, &(commands->read)) == false) ERROR_IOFILE; //传递此时跳过空格的buf，按道理应该是一个文件名的起点，同时成功将文件名字符串给到了commands->read指针
                commands->cnt += 2;                                                //commands->cnt记录这个command命令的参数的个数+2 （包括这个< 和 这个文件名）
                break;
            
            //输出重定向，包括>覆盖 和>>追加
            case '>' : 
                if (waitCommand) ERROR_EMPTY("\033[1;31mError:\033[0m I/O redirection should be used after a command!\n");
                buf++;
                while ((*buf != '\n') && isspace(*buf)) ++buf;            //同样跳过所有的空格
                //对于>覆盖输出重定位：  
                //—— 它和追加重定位的区别是：这个的文件名写在了commands->write指针中，追加的那个写在了commands->overwrite指针中
                if (*(buf) != '>') { if(fetchFileName(&buf, &(commands->write)) == false) ERROR_IOFILE; commands->cnt += 2; break; }
                
                buf++;
		        while ((*buf != '\n') && isspace(*buf)) {++buf;}
                //对于>>追加输出重定位：
                if (fetchFileName(&buf, &(commands->overwrite)) == false) ERROR_IOFILE; commands->cnt += 2; break;
            

            //code:is_background
            case '&' : 
                if (waitCommand) ERROR_EMPTY("\033[1;31mError:\033[0m & should be used after a command!\n");
                waitCommand = 1;              //这里设置waitCommand=1，说明之后可能还有命令
                                              //和|管道不一样，管道后面一定有东西，这里后面可能就没有了
                commands->is_background = 1;  //设置后台执行标识
                buf++;
                //------------------
                //如果下一个buf是回车
                if((*buf) == '\n')
                {
                    *args = malloc(sizeof(char));
                    *args = 0;                // NULL 结尾
                    commands++;               //commands指针后移一位
                    commands->cmd = NULL;     //初始化下一条命令字符串名
                    return true;
                }
                //------------------
                *args = malloc(sizeof(char));
                *args = 0;          // NULL 结尾
                commandsCount++;    
                commands++;
                createCommand(commands);
                args = commands->args = malloc(sizeof(char *) * MAX_ARGS);
                break;

            case '\'': 
            case '\"':
                if (waitCommand) ERROR_EMPTY("\033[1;31mError:\033[0m String parameter should be used after a command!\n");
                
                start = buf++;                                         //记录这个 引号“引号”的起点，英文的引号“不分左右”
                while ((*buf != '\n') && (*buf != *start)) { buf++; }  //虽然遇到\n也会停下来，但是，只有遇到“和start相同的引号才正确”

                if ((*buf == '\n') || (buf - start - 1 < 0)) ERROR_STR; //如果*buf==\n说明没有“引号”
                
                //将“引号内（不含）引号的内容”全部给到全局args指针
                *args = malloc(sizeof(char) * (buf - start));
                memcpy(*args, start + 1, sizeof(char) * (buf - start - 1));
                (*args)[buf - start - 1] = '\0';

                commands->cnt++; //这一条命令的参数+1
                args++;          //args指针后移
                buf++;           //buf指针后移到下一个字符
                break;

            //上面的是处理buf字符串中包括空格，回车，管道|，重定向在内的特殊符号的匹配
            //下面，就是处理2种对象——“命令名”或者”参数名“
            default :
                //(1)start作为名称开头，buf作为结尾的下一个
                start = buf;
                while ((*buf != '\n') && (!isspace(*buf)) && (*buf != '|') && (*buf != '<') && (*buf != '>')) {++buf;}
                //----------------
                
                //（2）将这个参数名 整个+给到args指针，然后args指针给到新创建的addr指针，并且args指针后移一位
                *args = malloc(sizeof(char) * (buf - start + 1));  //注意'\0'
                char *addr = *args;
                args++;

                //(3)如果还在等待命令，则第一个识别到的参数名一定就是命令名，记得将waitCommand标志设置为0
                //一定是指令名
                if (waitCommand) {
                    //后续可以是特殊字符
                    waitCommand = 0;
                    //获取指令名
                    commands->cmd = malloc(sizeof(char) * (buf - start + 1));
                    addr = commands->cmd;
                }
                //（4）addr此时和args指向的是同一个地址，所以，改变addr中的内容，就是改变args中的内容
                //无论是参数还是指令名都需要保存
                memcpy(addr, start, sizeof(char) * (buf - start));
                addr[buf - start] = '\0'; //指令的每部分都要以\0结尾
                commands->cnt++;  //这个命令的 参数个数+1 ，命令名本身也会作为一个argv[i]的元素，cnt中+1
                break;
        }
    }
}

//（6）if-else匹配执行命令
//这个函数主要就是由一些if语句进行字符匹配——从而可以找到需要执行的命令

void forkToExecute(Command *command, int fd_in, int fd_out) {
    //-----------------------------------------------------
    //if-else指向命令匹配之前，必须考虑命令的预处理——比如说，需要考虑是否需要对这条命令的输入和输出进行重定向
    int original_in = dup(0);         //保留原来的标准输入输出位置
    int original_out = dup(1);
    //（1）如果有输入重定向，就将0通过dup2函数给到command->read这个文件名的文件描述符句柄
    if (command->read) {
        int in = open(command->read, O_RDONLY, 0666);
        if (in < 0) ERROR_OPEN;
        dup2(in, 0);
        close(in);
    } 
    //-否则，如果fd_in>0，就将0给到fd_in文件描述符，也就是这个函数的参数fd_in作为输入重定向咯
    else if (fd_in > 0) {
        dup2(fd_in, 0);
    }
    //（2）下面的输出重定向也是同理可得咯
    if (command->write) {
        int out = open(command->write, O_WRONLY | O_TRUNC | O_CREAT, 0666);
        if (out < 0) ERROR_OPEN;
        dup2(out, 1);
        close(out);
    } 
    else if (command->overwrite) {
        int append = open(command->overwrite, O_WRONLY | O_CREAT | O_APPEND, 0666);
        if (append < 0) ERROR_OPEN;
        dup2(append, 1);
        close(append);
    } 
    else if (fd_out > 0) {
        dup2(fd_out, 1);
    }

    //(1)如果从command指针当前指向的命令的“名称”command->cmd中匹配到exit字符串——输出退出信息后退出
    if (strcmp(command->cmd, "ourexit") == 0) {printf("\033[1;36mGood Bye!\033[0m\n");freeCommand(command);dup2(original_out,1) ; dup2(original_in,0) ;exit(0);}

    //(2)->cmd == "ourcd":调用callCd函数，并且把command结构体数组传进入
    else if (strcmp(command->cmd, "ourcd") == 0) { 
        char *path;
        if (command->args[1] == NULL || command->args[1][0] == ' ') {
            struct passwd *pwd;
            pwd = getpwuid(getuid());
            path = pwd->pw_dir;
        } else
            path = command->args[1];
        if (chdir(path) < 0) {
            char error[MAX_CHAR_SIZE];
            sprintf(error, "No such dir or file: %s !\n", path);
        }
        dup2(original_out,1) ; dup2(original_in,0) ;
        return;
    }

    //（3）->cmd=="history"
    else if (strcmp(command->cmd, "ourhistory") == 0 && commandsCount == 1) {
        callHistory(command, history);
        dup2(original_out,1) ; dup2(original_in,0) ;
        return ;
    }

    //(4)->cmd == "help"
    else if (strcmp(command->cmd, "ourhelp") == 0 && commandsCount == 1) { 
        callHelp(command); 
        dup2(original_out,1) ; dup2(original_in,0) ;
        return ;
    }
    //(5)->cmd == "ourls"
    else if (strcmp(command->cmd, "ourls") == 0 && commandsCount == 1) {
        callmyls(command);
        dup2(original_out,1) ; dup2(original_in,0) ;
        return ;
    }

    //(5)文件操作的3个命令cp,mv,rm
    else if (strcmp(command->cmd, "ourcp") == 0 && commandsCount == 1) {
        ourcp(command);
        dup2(original_out,1) ; dup2(original_in,0) ;
        return ;
    }

    else if (strcmp(command->cmd, "ourmv") == 0 && commandsCount == 1) {
        ourmv(command);
        dup2(original_out,1) ; dup2(original_in,0) ;
        return ;
    }

    else if (strcmp(command->cmd, "ourrm") == 0 && commandsCount == 1) {
        ourrm(command);
        dup2(original_out,1) ; dup2(original_in,0) ;
        return ;
    }
    //(6)利用unistd.h库中的getcwd获取当前路径，实现pwd命令
    else if (strcmp(command->cmd, "ourpwd") == 0 && commandsCount == 1) {
        char cwd[1024];          // 用于存储当前工作路径的缓冲区

        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("Current working directory: %s\n", cwd);
        } else {
            perror("getcwd() error");
            dup2(original_out,1) ; dup2(original_in,0) ;
            return ;
        }
        dup2(original_out,1) ; dup2(original_in,0) ;
        return ;
    }

    //（7）ps输出进程信息——也是要实现输出重定向的之后
    else if (strcmp(command->cmd, "ourps") == 0 && commandsCount == 1) {
        ourps(command);
        dup2(original_out,1) ; dup2(original_in,0) ;
        return ;
    }
    //（8）tree命令就是输出那个树结构即可 
    if (strcmp(command->cmd, "ourtree") == 0 && commandsCount == 1) {
		const char *directory_path = (command->args[1] != NULL) ? command->args[1] : "."; // Default to current directory if no path is provided
		show_directory_structure(directory_path);
        dup2(original_out,1) ; dup2(original_in,0) ;
		return;
	}
    
    

    //如果是单条指令-直接在上面的if语句中就已经return了
    //---------------------------------------------------------
    //到了这里，就一定是多条指令了，会针对多指令的问题，调用fork函数的方式予以实现
    //补充说明：之所以上面用return——是因为上面是parent这个shell进程，不能杀死，只能回到上一个调用的
    //下面用exit(0)——就是为了结束子进程，回到parent进程中

    pid_t pid = fork();
    if (pid < 0) ERROR_FORK;  //这部分主要是配合管道命令的重定向需求
    else if (pid == 0) {      //child
        //（1）如果有输入重定向，就将0通过dup2函数给到command->read这个文件名的文件描述符句柄
        if (command->read) {
            int in = open(command->read, O_RDONLY, 0666);
	        if (in < 0) ERROR_OPEN;
            dup2(in, 0);
            close(in);
        } 
        //-否则，如果fd_in>0，就将0给到fd_in文件描述符，也就是这个函数的参数fd_in作为输入重定向咯
        else if (fd_in > 0) {
            dup2(fd_in, 0);
        }
        //（2）下面的输出重定向也是同理可得咯
        if (command->write) {
            int out = open(command->write, O_WRONLY | O_TRUNC | O_CREAT, 0666);
            if (out < 0) ERROR_OPEN;
            dup2(out, 1);
            close(out);
        } 
        else if (command->overwrite) {
            int append = open(command->overwrite, O_WRONLY | O_CREAT | O_APPEND, 0666);
            if (append < 0) ERROR_OPEN;
            dup2(append, 1);
            close(append);
        } 
        else if (fd_out > 0) {
            dup2(fd_out, 1);
        }
        //(3)child进程中还有额外的命令需要执行，那就继续匹配这个command->cmd好了
        //单独处理history
        if (strcmp(command->cmd, "ourhistory") == 0) {callHistory(command, history); exit(0);}
        //单独处理help
        if (strcmp(command->cmd, "ourhelp") == 0) {printHelp(); exit(0); }
        if (strcmp(command->cmd, "ourcp") == 0) {ourcp(command); exit(0);}
        if (strcmp(command->cmd, "ourmv") == 0) {ourmv(command); exit(0);}
        if (strcmp(command->cmd, "ourrm") == 0) {ourrm(command); exit(0);}
        //ourpwd
        if (strcmp(command->cmd, "ourpwd") == 0) {
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) != NULL) {
                printf("Current working directory: %s\n", cwd);
                } else {
                perror("getcwd() error");
                exit(1) ;
                }
                exit(0);
                }
        if (strcmp(command->cmd, "ourps") == 0) {ourps(command); exit(0);}
        if (strcmp(command->cmd, "ourtree") == 0) {
            const char *directory_path = (command->args[1] != NULL) ? command->args[1] : "."; 
		    show_directory_structure(directory_path);
            exit(0);}
        if (strcmp(command->cmd, "ourls") == 0) {callmyls(command); exit(0); } 

        //（4）
        //首先，这个execvp函数就是执行外部环境变量中匹配到的外部命令程序，参数是args
        //虽然这个外部调用会替代掉当前的“子进程”，但是，那又有什么关系呢，只是一个用来执行一个命令的子进程而已哈
        if((status = execvp(command->cmd, command->args)) < 0) ERROR_EXECUTE(command->cmd);  

    } 
    else { //parent
        wait(&status);                          //父进程等待，直到子进程的状态-变成退出状态
        if (!(WIFEXITED(status))) ERROR_EXIT;   //如果子进程没有正常退出，会调用error宏
    }
    dup2(original_out,1) ; dup2(original_in,0); //执行完命令-需要返还标准输入和输出位置
}

//-----------------------------------------------------------------------------

//（7）作用：分情况调用forkToExecute
void executeCommands(Command *commands) {
    //(1)如果解析出来，总共只有一个命令，直接调用这个forkToExecute函数即可
    if (commandsCount == 1) {
        //forkToExecute(commands, -1, -1);
        if (commands->is_background == 1) {
            pid_t pid = fork();

            if (pid < 0) {
            perror("fork");
            exit(1);
            } else if (pid == 0) {
            // Child process
            forkToExecute(commands, -1, -1);
            printf("\n%d Finished\n", getpid());
            exit(0);                                    //0--正常退出，1--异常退出-退出子进程
            } else {
            // Parent process
            printf("PID: %d\n", pid);
            }
        } else {
            forkToExecute(commands, -1, -1);           
        }
        freeCommand(commands);                  
    }
    //（2）如果解析出来有2条命令 —— 采用pipe创建 写-读管道，就非常容易理解了
    //第一个命令的输出 写入到pd[1] ,第二个命令的标准输入是pd[0]
    else if(commandsCount >=2 && commands->is_background==1)  
    {
        for(int i =1;i<=commandsCount;i++)
        {
            //forkToExecute(commands, -1, -1);
            if (commands->is_background == 1) {
                pid_t pid = fork();

                if (pid < 0) {
                perror("fork");
                exit(1);
                } else if (pid == 0) {
                // Child process
                forkToExecute(commands, -1, -1);
                printf("\n%d Finished\n", getpid());
                exit(0);                                    
                } else {
                // Parent process
                printf("PID: %d\n", pid);
                }
            } else {
                forkToExecute(commands, -1, -1);
            }
            freeCommand(commands++);                         
            }
        
    }
    //-------------------------
    else if (commandsCount == 2) {
        int fd[2];
        pipe(fd);
        if (commands->is_background == 1) {
            pid_t pid = fork();

            if (pid < 0) {
            perror("fork");
            exit(1);
            } else if (pid == 0) {
            // Child process
            forkToExecute(commands, -1, fd[1]);
            printf("\n%d Finished\n", getpid());
            exit(0); 
            } else {
            // Parent process
            printf("PID: %d\n", pid);
            }
        } else {
            forkToExecute(commands, -1, fd[1]);
        }
        //----
        close(fd[1]);
        freeCommand(commands++);
        //forkToExecute(commands, fd[0], -1);
        //----
        if (commands->is_background == 1) {
            pid_t pid = fork();

            if (pid < 0) {
            perror("fork");
            exit(1);
            } else if (pid == 0) {
            // Child process
            forkToExecute(commands, fd[0], -1);
            printf("\n%d Finished\n", getpid());
            exit(0); 
            } else {
            // Parent process
            printf("PID: %d\n", pid);
            }
        } else {
            forkToExecute(commands, fd[0], -1);
        }
        //----
        close(fd[0]);
        freeCommand(commands);
    } 
    //（3）如果有多条命令（全局变量中有说-最多8条）-问一下chat，你就能够理解下面的代码的逻辑
    else {
        int *pipes[2];
        pipes[0]= malloc(sizeof(int) * 2);
        pipes[1]= malloc(sizeof(int) * 2);
        int newPoint = 0;
        pipe(pipes[newPoint]);
        //--第一条命令从只需要将输入写入到管道中
        //forkToExecute(commands, -1, (pipes[newPoint])[1]);
        if (commands->is_background == 1) {
            pid_t pid = fork();

            if (pid < 0) {
            perror("fork");
            exit(1);
            } else if (pid == 0) {
            // Child process
            forkToExecute(commands, -1, (pipes[newPoint])[1]);
            printf("\n%d Finished\n", getpid());
            exit(0); 
            } else {
            // Parent process
            printf("PID: %d\n", pid);
            }
        } else {
            forkToExecute(commands, -1, (pipes[newPoint])[1]);
        }
        //----
        close((pipes[newPoint])[1]);
        freeCommand(commands++);
        
        //--中间的命令需要从另一个管道读取，并写入到当前管道
        for (int i = 1; i < (commandsCount - 1); ++i) {
            newPoint = 1 - newPoint;
            pipe(pipes[newPoint]);
            //forkToExecute(commands,(pipes[1-newPoint])[0],(pipes[newPoint])[1]);
            if (commands->is_background == 1) {
                pid_t pid = fork();

                if (pid < 0) {
                perror("fork");
                exit(1);
                } else if (pid == 0) {
                // Child process
                forkToExecute(commands,(pipes[1-newPoint])[0],(pipes[newPoint])[1]);
                printf("\n%d Finished\n", getpid());
                exit(0); 
                } else {
                // Parent process
                printf("PID: %d\n", pid);
                }
            } else {
                forkToExecute(commands,(pipes[1-newPoint])[0],(pipes[newPoint])[1]);
            }
            //----
            close((pipes[1-newPoint])[0]);
            close((pipes[newPoint])[1]);
            freeCommand(commands++);
        }
        //--最后一条命令只要从另一个管道读即可，输出到标准输出端口-屏幕
        //forkToExecute(commands,(pipes[newPoint])[0],-1);
        if (commands->is_background == 1) {
            pid_t pid = fork();

            if (pid < 0) {
            perror("fork");
            exit(1);
            } else if (pid == 0) {
            // Child process
            forkToExecute(commands,(pipes[newPoint])[0],-1);
            printf("\n%d Finished\n", getpid());
            exit(0); 
            } else {
            // Parent process
            printf("PID: %d\n", pid);
            }
        } else {
            forkToExecute(commands,(pipes[newPoint])[0],-1);
        }
        //----
        close((pipes[newPoint])[0]);
        freeCommand(commands);
    }
    return;
}

