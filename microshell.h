#ifndef __MICROSHELL_H__
#define __MICROSHELL_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct s_list {

  char *content;
  struct s_list *next;
} t_list;

typedef struct s_ms {
  enum {
    PIPE,
    SEMI_COLON,
    START,
  } type;
  pid_t id;
  char *cmd;
  t_list *args;
  char **envp;
  int fds[2];
  struct s_ms *next;
  struct s_ms *prev;
} t_ms;

int parse_cmd_and_arg(char *arg, t_ms *ms);
int start(t_ms *ms);
char **list_to_array(t_list *lst);
#endif