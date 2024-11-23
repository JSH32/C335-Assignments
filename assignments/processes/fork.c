#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFFER_SIZE 256
#define READ_END 0
#define WRITE_END 1

static const char* items[][2] = {
  { "id", "/usr/bin/id" },
  { "pwd", "/usr/bin/pwd" },
  { "date", "/usr/bin/date" }
};

int execute_command(const char* cmd) {
  for (int i = 0; i < sizeof(items) / sizeof(items[0]); i++) {
    if (strcmp(cmd, items[i][0]) == 0) {
      return execl(items[i][1], items[i][0], NULL);
    }
  }
  return -1;
}

int main(void) {
  int pipe_fd[2];
  pid_t pid;
  char command[BUFFER_SIZE];

  if (pipe(pipe_fd) == -1) {
    printf("Failed to create pipe.\n");
    return 1;
  }

  pid = fork();

  if (pid < 0) {
    printf("Failed to create fork.\n");
    return 1;
  }

  if (pid > 0) {
    // Parent
    close(pipe_fd[READ_END]);

    printf("prompt$ ");
    scanf("%255s", command);

    write(pipe_fd[WRITE_END], command, strlen(command) + 1);
    close(pipe_fd[WRITE_END]);

    wait(NULL);
    printf("Done - %s\n", command);
  } else {
    // Child
    char received_cmd[BUFFER_SIZE];

    close(pipe_fd[WRITE_END]);
    read(pipe_fd[READ_END], received_cmd, BUFFER_SIZE);
    close(pipe_fd[READ_END]);

    execute_command(received_cmd);
  }

  return 0;
}
