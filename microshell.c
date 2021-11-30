#include "microshell.h"

void reset() { printf("\033[0m"); }

t_list *new_list(char *content) {
  t_list *new;

  new = malloc(sizeof(t_list));
  if (!new)
    return NULL;
  new->content = content;
  new->next = NULL;
  return new;
}

void ft_putstr_fd(char *str, int fd) {
  if (!str)
    return;
  int i = -1;
  while (str[++i])
    write(fd, &str[i], STDOUT_FILENO);
}

void put_error_msg(char *msg, char *arg) {
  ft_putstr_fd(msg, STDERR_FILENO);
  ft_putstr_fd(arg, STDERR_FILENO);
  ft_putstr_fd("\n", STDERR_FILENO);
}

void lst_addback(t_list **list, t_list *new) {
  t_list *temp;

  if (*list == NULL) {
    *list = new;
    return;
  }
  temp = *list;
  while (temp->next)
    temp = temp->next;
  temp->next = new;
}

char **list_to_array(t_list *lst) {

  if (!lst)
    return NULL;
  t_list *temp = lst;
  int size = 0;
  char **res = NULL;
  int i = -1;

  while (temp) {
    size++;
    temp = temp->next;
  }
  res = (char **)malloc(sizeof(char *) * (size + 1));
  if (!res) {
    put_error_msg("error: fatal", NULL);
    exit(EXIT_FAILURE);
    return NULL;
  }
  temp = lst;
  while (++i < size) {
    res[i] = temp->content;
    temp = temp->next;
  }
  res[size] = 0;
  return res;
}

void *ft_memset(void *s, int c, size_t n) {
  char *ptr;
  size_t i;

  ptr = s;
  i = 0;
  while (i < n)
    *(ptr + i++) = c;
  return (s);
}

t_ms *new_ms() {
  t_ms *new;

  new = malloc(sizeof(t_ms));
  if (!new)
    return NULL;
  ft_memset(new, 0, sizeof(t_ms));
  return (new);
}

void ms_addback(t_ms **ms, t_ms *new) {
  t_ms *temp;

  if (*ms == NULL) {
    *ms = new;
    return;
  }
  temp = *ms;
  while (temp->next)
    temp = temp->next;
  temp->next = new;
  temp->prev = temp;
}

int is_modifier(char *s1) { return (!strcmp(s1, "|") || !strcmp(s1, ";")); }

int parse_cmd_and_arg(char *arg, t_ms **temp) {
  if (!arg)
    return -1;

  t_ms *ms = *temp;
  while (ms->next) {
    ms = ms->next;
  }
  if (ms->cmd) {
    lst_addback(&ms->args, new_list(arg));
    return (1);
  }
  ms->cmd = arg;
  lst_addback(&ms->args, new_list(arg));
  return 1;
}

//";" ";" /bin/echo OK ";" ";" ";" /bin/echo OK
int parse(int argc, char *argv[], t_ms **ms) {
  int i;

  i = -1;
  while (++i < argc) {
    if (argv[i] && is_modifier(argv[i])) {
      t_ms *new = new_ms();
      if (!strcmp(argv[i], "|")) {
        new->type = PIPE;
      } else {
        if ((*ms)->type == START) {
          free(new);
          (*ms)->type = SEMI_COLON;
          continue;
        }
        new->type = SEMI_COLON;
      }
      ms_addback(ms, new);
    } else {
      parse_cmd_and_arg(argv[i], ms);
    }
  }
  return 1;
}

void lst_clear(t_list **list) {
  t_list *temp;

  if (!list)
    return;
  while (*list) {
    temp = (*list)->next;
    free(*list);
    *list = temp;
  }
  *list = NULL;
}

void clear_ms(t_ms **ms) {
  t_ms *temp;

  while (*ms) {
    temp = (*ms)->next;
    lst_clear(&((*ms)->args));
    free(*ms);
    *ms = temp;
  }
  *ms = 0;
}

void print_cmd_args(t_list *args) {
  while (args) {
    printf("[%s]", args->content);
    args = args->next;
  }
  printf("\n");
}

void print_cmd(t_ms **temp) {

  t_ms *ms = *temp;
  while (ms) {
    t_list *args = ms->args;
    if (ms->type == PIPE) {
      printf("[ PIPE ]\n");
      printf("cmd: %s\n", ms->cmd);
      print_cmd_args(args);
    } else if (ms->type == SEMI_COLON) {
      printf("[ SEMI-COLON ]\n");
      printf("cmd: %s\n", ms->cmd);
      print_cmd_args(args);
    } else {
      printf("[ First cmd ]\n");
      printf("cmd: %s\n", ms->cmd);
      print_cmd_args(args);
    }
    ms = ms->next;
  }
}

int main(int argc, char *argv[], char *envp[]) {
  t_ms *ms;

  if (argc < 2)
    return 1;
  ms = new_ms();
  if (!ms)
    return (1);
  ms->envp = envp;
  ms->type = START;
  if (parse(argc, &argv[1], &ms) == -1)
    return (127);
  start(ms);
  // print_cmd(&ms);
  clear_ms(&ms);
  return (0);
}
