#include "ourhelp.h"

extern int status;     //这个在下面有用到，从main.c搬过来用咯
//help -- 这里面只有1个纯粹的输出函数printf--后期可以将这些内容存到一个txt文件，然后通过从文件中读取的方式完成
//或者单独放到一个.c文件中也行——先这样
void printHelp() {
  printf("欢迎查看网安自研shell的用户手册！\n");
  printf("\n");
  printf("*******************************************************************************\n");

  printf("1. ourhelp\n");
  printf("命令示例：  ourhelp \n");
  printf("命令作用：  输出用户帮助手册\n");
  printf("使用示例：  ourhelp\n");
  printf("参数个数：  无参数\n");
  printf("\n");  

  printf("2. ourls\n");
  printf("命令示例：  ourls \n");
  printf("命令示例：  ourls [目录]\n");
  printf("命令示例：  ourls [选项] [目录]\n");
  printf("命令作用：  显示指定工作目录下之内容，[OPTION]可为-l（输出详细信息），[FILE]为目标目录，无参数则代表当前目录，..为上级目录\n");
  printf("使用示例：  ourls \n");
  printf("使用示例：  ourls -l\n");
  printf("使用示例：  ourls /dir1\n");
  printf("使用示例：  ourls -l /dir1\n");
  printf("使用示例：  ourls ..\n");
  printf("参数个数：  无参数或1个参数或2个参数\n");
  printf("\n");  

  printf("3. ourcd\n");
  printf("命令示例：  ourcd \n");
  printf("命令示例：  ourcd [目录]\n");
  printf("命令作用：  跳转到目标目录，无参数则默认为家目录，有参数则改变当前目录为参数内容，..号代表回到之前的目录\n");
  printf("使用示例：  ourcd\n");
  printf("使用示例：  ourcd /dir1\n");
  printf("使用示例：  ourcd ..\n");
  printf("参数个数：  无参数或1个参数\n");
  printf("\n");  

  printf("4. ourcp\n");
  printf("命令示例：  ourcp [源文件] [目标文件]... \n");
  printf("命令作用：  将源文件的内容复制到目标文件，可以复制文件到文件，文件夹到文件夹，多个文件到文件夹\n");
  printf("使用示例：  ourcp /file1 /file2\n");
  printf("使用示例：  ourcp /dir1 /dir2\n");
  printf("使用示例：  ourcp /dir1 /file1 /file2 /file3 /file4\n");
  printf("参数个数：  2个或以上参数\n");
  printf("\n"); 

  printf("5. ourmv\n");
  printf("命令示例：  ourmv [源文件] [目标文件]... \n");
  printf("命令作用：  将源文件的内容移动到目标文件，可以移动文件到文件，文件夹到文件夹，多个文件到文件夹\n");
  printf("使用示例：  ourmv /file1 /file2\n");
  printf("使用示例：  ourmv /dir1 /dir2\n");
  printf("使用示例：  ourmv /dir1 /file1 /file2 /file3 /file4\n");
  printf("参数个数：  2个或以上参数\n");
  printf("\n"); 

  printf("6. ourrm\n");
  printf("命令示例：  ourrm [文件]... \n");
  printf("命令示例：  ourrm [选项] [目录]\n");
  printf("命令作用：  删除文件或目录，当选项为-r时是删除目录\n");
  printf("使用示例：  ourrm /file1\n");
  printf("使用示例：  ourrm /file1 /file2\n");
  printf("使用示例：  ourrm -r /dir1\n");
  printf("参数个数：  1个或以上参数\n");
  printf("\n"); 

  printf("7. ourpwd\n");
  printf("命令示例：  ourpwd\n");
  printf("命令作用：  显示当前所在工作目录的全路径\n");
  printf("使用示例：  ourpwd\n");
  printf("参数个数：  无参数\n");
  printf("\n"); 

  printf("8. ourhistory\n");
  printf("命令示例：  ourhistory\n");
  printf("命令示例：  ourhistory [数字]\n");
  printf("命令作用：  查看历史命令，不加参数则是输出全部历史命令，增加参数[数字]则输出最后执行的num条命令\n");
  printf("使用示例：  ourhistory\n");
  printf("使用示例：  ourhistory 42\n");
  printf("参数个数：  无参数或1个参数\n");
  printf("\n"); 

  printf("9. ourps\n");
  printf("命令示例：  ourps\n");
  printf("命令示例：  ourps [选项] [PID]\n");
  printf("命令作用：  显示当前进程的状态，不加选项则是输出全部进程，增加选项-p则显示查询的单个进程\n");
  printf("使用示例：  ourps\n");
  printf("使用示例：  ourps -p 123\n");
  printf("参数个数：  无参数或2个参数\n");
  printf("\n"); 

  printf("10. ourtree\n");
  printf("命令示例：  ourtree\n");
  printf("命令示例：  ourtree [目录]\n");
  printf("命令作用：  展示目录的文件结构，无参数则默认为当前目录，有参数则将参数内容作为目标目录\n");
  printf("使用示例：  ourtree\n");
  printf("使用示例：  ourtree /dir1\n");
  printf("参数个数：  无参数或1个参数\n");
  printf("\n"); 

  printf("11. 重定向 \n");
  printf("命令作用：  命令通常从标准输入读取输入和标准输出写入输出，使用重定向可以改变输入和输出的地方\n");
  printf("\n"); 
  printf("命令示例：  [命令] < [文件]\n");
  printf("命令作用：  将输入重定向到 [文件]\n");
  printf("使用示例：  wc -c < /file1\n");
  printf("\n"); 
  printf("命令示例：  [命令] > [文件]\n");
  printf("命令作用：  将输出重定向到 [文件]\n");
  printf("使用示例：  ourhelp > /file1\n");
  printf("\n"); 
  printf("命令示例：  [命令] >> [文件]\n");
  printf("命令作用：  将输出以追加的方式重定向到 [文件]\n");
  printf("使用示例：  ourls >> /file1\n");
  printf("\n"); 

  printf("12. 管道 | \n");
  printf("命令示例：  [命令] | [命令]");
  printf("命令作用：  将一个命令的输出作为另一个命令的输入，实现命令之间的数据流传递\n");
  printf("使用示例：  ls | wc -l\n");

  printf("13. 后台运行程序 & \n");
  printf("命令示例：  [命令] &\n");
  printf("命令作用：  使得某个程序在后台运行，允许用户继续输入其他命令而不阻塞 Shell\n");
  printf("使用示例：  sleep 3 &\n");
  
  printf("14. ourexit\n");
  printf("命令示例：  ourexit\n");
  printf("命令作用：  退出Shell\n");
  printf("使用示例：  ourexit\n");
}

void callHelp(Command *command) {
    //无参数时输出重定向到less分页程序显示
    //创建两个子进程执行即可
    //(1)只有“help”这一个参数的情况 -- 利用
    //这里的使用了pipe管道，第一个（firk得到的pid=0的子进程）进程将printHelp输出的结果写入管道，第二个进程
    //也就是父进程，负责调用more也就是less的方式，将从printHelp中读取的内容分页输出到屏幕
    if (command->cnt == 1) {
        int fd[2];
        if (pipe(fd) < 0) {perror("pipe()"); return; }
        pid_t pid1 = fork();
        if (pid1 < 0) ERROR_FORK;
        else if (pid1 == 0) {
            close(fd[0]);
            dup2(fd[1], 1);
            printHelp();
            close(fd[1]);
            exit(0);
        } 
        else {
            wait(&status);  //调用wait，父进程等待-同时会修改全局变量status的值，之后用WIFEXITED宏检查子进程的退出状态
            if (!WIFEXITED(status)) ERROR_EXIT;
            pid_t pid2 = fork();
            if (pid2 < 0) ERROR_FORK;
            else if (pid2 == 0) {
                close(fd[1]);
                dup2(fd[0], 0);
                //execl("/usr/bin/less", "less", NULL);
                execl("/bin/more", "more", NULL);
                close(fd[0]);
                exit(0);
            }
            else {
                //先关闭文件描述符防止阻塞
                close(fd[0]);
                close(fd[1]);
                wait(&status);
                if (!WIFEXITED(status)) ERROR_EXIT;
                return ;
            }
        }
    }
    //参数不符合条件 —— 参看help指示可见，只有1个参数 或者具有 重定向输出的3个参数的情况
    else if (command->cnt > 4 || command->cnt == 2 || command->read || (!command->write && !command->overwrite)) {ERROR_FMT(command->cmd); }
    //3个参数，有参数时直接重定向至文件
    //
    else if (command->cnt == 3) {
        int fd;
        int out = dup(1); //暂存文件描述符 —— 因为后面会改用fd作为标准输出，用完printHelp后，还要改回到out
        char *file = command->write ? command->write : command->overwrite;
        //分覆盖 和 追加
        if (command->write) {
            fd = open(file, O_WRONLY | O_TRUNC | O_CREAT, 0666); 
        }
        else if (command->overwrite) {
            fd = open(file, O_WRONLY | O_APPEND | O_CREAT, 0666); 
        }
        if (fd < 0) {fprintf(stderr, "\033[1;31mError:\033[0mFailed to open file %s\n", file); return; }
        dup2(fd, 1);
        printHelp();
        dup2(out, 1); //恢复文件描述符
        close(fd);
        close(out);
        return ;
    }
}