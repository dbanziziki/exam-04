#include "microshell.h"

int exec_cmd(t_ms **ms, char **envp) {
  t_ms *next;
  pid_t id;
  char **argv;
  int status;

  if (!ms || !(*ms))
    return -1;
  argv = list_to_array((*ms)->args);
  next = (*ms)->next;
  t_ms *temp = *ms;
  if (next && next->type == PIPE) {
    if (pipe(next->fds) == -1) {
      put_error_msg("error: fatal", NULL);
      exit(EXIT_FAILURE);
      return -1;
    }
  }
  id = fork();
  if (id < 0) {
    return -1;
  }
  if (id == 0) {
    if (next && next->type == PIPE) {
      dup2(next->fds[1], STDOUT_FILENO);
    }
    if ((*ms)->type == PIPE) {
      dup2((*ms)->fds[0], STDIN_FILENO);
    }
    close((*ms)->fds[1]);
    close((*ms)->fds[0]);
    if (!(*ms)->cmd)
      exit(EXIT_SUCCESS);
    execve((*ms)->cmd, argv, envp);
    put_error_msg("error: cannot execute ", (*ms)->cmd);
    exit(EXIT_FAILURE);
  }
  if ((*ms)->type == PIPE) {
    close((*ms)->fds[1]);
    close((*ms)->fds[0]);
  }
  free(argv);
  waitpid(id, &status, 0);
  int exit_status = WEXITSTATUS(status);
  return exit_status;
}

int start(t_ms *ms) {

  if (!ms)
    return -1;
  char **envp = ms->envp;
  t_ms *temp = ms;
  int status = 0;

  while (ms) {
    if (ms->cmd) {
      if (!strcmp("cd", ms->cmd)) {
        status = ft_cd(ms);
        ms = ms->next;
        continue;
      }
    }
    status = exec_cmd(&ms, envp);
    ms = ms->next;
  }
  return (status);
}