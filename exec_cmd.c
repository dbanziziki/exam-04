#include "microshell.h"

int exec_cmd(t_ms **ms, char **envp) {
  t_ms *next;
  pid_t id;
  char **argv;

  (void)envp;
  if (!ms)
    return -1;
  argv = list_to_array((*ms)->args);
  next = (*ms)->next;
  if (next && next->type == PIPE) {
    if (pipe(next->fds) == -1)
      return -1;
  }
  id = fork();
  if (id == 0) {
    if (next && next->type == PIPE) {
      dup2(next->fds[1], STDOUT_FILENO);
      close((*ms)->fds[1]);
      close((*ms)->fds[0]);
    }
    if ((*ms)->type == PIPE) {
      dup2((*ms)->fds[0], STDIN_FILENO);
      close((*ms)->fds[1]);
      close((*ms)->fds[0]);
    }
    if (!(*ms)->cmd)
      exit(EXIT_SUCCESS);
    execve((*ms)->cmd, argv, (*ms)->envp);
    printf("error %s not found\n", (*ms)->cmd);
    exit(EXIT_FAILURE);
  }
  if ((*ms)->type == PIPE) {
    close((*ms)->fds[1]);
    close((*ms)->fds[0]);
  }
  waitpid(id, NULL, 0);
  return 1;
}

int start(t_ms *ms) {

  if (!ms)
    return -1;
  char **envp = ms->envp;
  t_ms *temp = ms;
  int i = -1;

  while (ms) {
    exec_cmd(&ms, envp);
    ms = ms->next;
  }
  return (1);
}