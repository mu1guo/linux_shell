#include "ourps.h"

//-----------------------------------------------------------------
//下面让我们实现那个简单的ps显示当前进程信息的函数

void ps_pid(int pid) {
  char path[1024];
  snprintf(path, sizeof(path), "/proc/%d/stat", pid);

  FILE *stat_file = fopen(path, "r");
  if (stat_file != NULL) {
    char comm[256];
    char state;
    unsigned long utime, stime;
    fscanf(stat_file,
           "%d %s %c %*s %*s %*s %*s %*s %*s %*s %*s %*s %*s %lu %lu", &pid,
           comm, &state, &utime, &stime);
    fclose(stat_file);

    printf("%-8d %-15s %-10c %-20lu %-20lu\n", pid, comm, state, utime, stime);
  } else {
    printf("Process with PID %d not found.\n", pid);
  }
}

void ps() {
  DIR *dir = opendir("/proc");
  struct dirent *entry;

  if (!dir) {
    perror("Unable to open /proc");
    exit(EXIT_FAILURE);
  }

  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type == DT_DIR) {
      if (atoi(entry->d_name) > 0) {
        ps_pid(atoi(entry->d_name));
      }
    }
  }

  closedir(dir);
}

int ourps(Command* commands) {
  int argc = commands->cnt;
  char** argv = commands->args;
  char *option = "";
  int pid = -1;

  if (argc > 1) {
    if (argv[1][0] != '-') {
      printf("Usage: myps [-p pid]\n");
      return 0;
    } else {
      option = argv[1] + 1;
      if (argc > 2) {
        pid = atoi(argv[2]);
      } else {
        printf("Usage: myps [-p pid]\n");
        return 0;
      }
    }
  }

  printf("%-8s %-15s %-10s %-20s %-20s\n", "PID", "Command", "State",
         "User Mode Time", "Kernel Mode Time");
  printf(
      "------------------------------------------------------------------------"
      "--\n");
  if (option[0] == '\0') {
    ps();
  } else if (strstr(option, "p") != NULL) {
    ps_pid(pid);
  } else {
    printf("Usage: myps [-p pid]\n");
    return 0;
  }

  return 0;
}

//------------------------------------------------------------------