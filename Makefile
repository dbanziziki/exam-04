NAME = microshell


all:
	@gcc -g exec_cmd.c microshell.c -o $(NAME)

fclean:
	@rm -f $(NAME)
	@rm -rf $(NAME).dSYM