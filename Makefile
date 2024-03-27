# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: joaosilva <joaosilva@student.42.fr>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/03/20 11:51:16 by joaosilva         #+#    #+#              #
#    Updated: 2024/03/26 23:23:49 by joaosilva        ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

#######################  Variables ######################

# Variables name
NAME = pipex
# NAME_BONUS = pipex_bonus - não podemos usar este. temos usar o debaixo par no bonus o executavel ser pipex e não pipex_bonus.
RULE = .all
RULE_BONUS = .bonus
# Header files ###########
HEADER = pipex.h
HEADER_BONUS = pipex_bonus.h
# Other Variables
INCLUDE = -I .
CC = cc
CFLAGS = -Wall -Wextra -Werror -O3 -g #-fsanitize=address
AR = ar rcs
RM = rm -f
#Libft
LIBFT = -L Libft_obj -lft
#LIBNAME = libft
#LIBDIR = libft/
#LIB_FLAGS = -L $(LIBDIR) -lft
###GNL
GET_NEXT_LINE = -L get_next_line_obj -lgnl
#  Sources to objects files ###########
SRC_FILES = pipex.c
SRC_BONUS_FILES = pipex_bonus.c pipex_utils_bonus.c
OBJS = ${SRC_FILES:.c=.o}
OBJS_BONUS = ${SRC_BONUS_FILES:.c=.o}


#######################   Rules ######################
### cria o pipex. congrega todos os ficheiros objetos, o header, e o libft.
all: ${NAME}

${NAME}: ${RULE} ${OBJS} ${HEADER}
	@make -s -C Libft
	@make -s -C get_next_line
	@${CC} ${CFLAGS} ${INCLUDE} -o ${NAME} ${OBJS} ${LIBFT} ${GET_NEXT_LINE}
	@echo "$(GREEN)\n${NAME} created.$(DEF_COLOR)"

${RULE}:
	@touch ${RULE}
	@${RM} ${RULE_BONUS}

bonus: ${RULE_BONUS}

${RULE_BONUS}: ${OBJS_BONUS} ${HEADER_BONUS}
	@touch ${RULE_BONUS}
	@${RM} ${RULE}
	@make -s -C Libft
	@make -s -C get_next_line
	@${CC} ${CFLAGS} ${INCLUDE} -o ${NAME} ${OBJS_BONUS} ${LIBFT} ${GET_NEXT_LINE}
	@echo "$(GREEN)\n${NAME} created with success.$(DEF_COLOR)"

###criar a diretoria obj e cria os objetos a partir dos ficheiros .c
%.o: %.c 
	@mkdir -p $(dir $@)
	@${CC} ${CFLAGS} ${INCLUDE} -c $< -o $@
	@echo "$(GRAY) Object $(basename $(notdir $@)) files created with success.$(DEF_COLOR)"

clean:
	@make fclean -s -C Libft
	@make fclean -s -C get_next_line
	@${RM} ${OBJS} ${OBJS_BONUS} ${RULE} ${RULE_BONUS}
	@${RM} -r obj
	@echo "$(RED)\nObject files removed.$(DEF_COLOR)"


###remove o pipex
fclean: clean
	@${RM} ${NAME} here_doc
	@echo "$(RED)\n${NAME} removed.$(DEF_COLOR)"

### primeiro faz o fclean (ou seja, o clean, eliminando os objetos, e dps elimina o libft.a), e a seguir torna a compilar tudo com o all (os objetos e o libft.a)
re: fclean all

tests:
	@rm -f infile infile.txt outfile outfile.txt no_permissions.txt
	@echo "\n\n$(GREEN)------NORM ERRORS------$(END)"
	@norminette | grep Error
	@echo "\n\n$(GREEN)------WITHOUT FILE PERMISSIONS------$(END)\n"
	@echo "a a a" > no_permissions.txt
	@chmod 000 no_permissions.txt
	@echo "./pipex no_permissions.txt \"grep a\" \"wc -w\" outfile.txt"
	@echo "\n Shell result:"
	@< no_permissions.txt grep a | wc -w > outfile.txt; echo "Error: $$?"
	@rm -f no_permissions.txt outfile.txt
	@echo "a a a" > no_permissions.txt
	@chmod 000 no_permissions.txt
	@echo "\n Your result:"
	@./pipex no_permissions.txt "grep a" "wc -w" outfile.txt; echo "Error: $$?"
	@echo "\n\n$(GREEN)------WITH FILE PERMISSIONS------$(END)\n"
	@echo "a a a" > permissions.txt
	@chmod 777 permissions.txt
	@echo "./pipex permissions.txt \"grep a\" \"wc -w\" outfile.txt"
	@echo "\n Shell result:"
	@< permissions.txt grep a | wc -w > outfile.txt; echo "Error: $$?"
	@cat outfile.txt | cat -e
	@rm -f permissions.txt outfile.txt
	@echo "a a a" > permissions.txt
	@chmod 777 permissions.txt
	@echo "\n Your result:"
	@./pipex permissions.txt "grep a" "wc -w" outfile.txt; echo "Error: $$?"
	@cat outfile.txt | cat -e
	@rm -f permissions.txt outfile.txt
	@echo "\n\n$(GREEN)------TEST N1------$(END)\n"
	@echo "./pipex infile.txt \"cat\" \"wc -l\" outfile.txt"
	@echo "\n$(YELLOW)WITHOUT INFILE$(END)\n"
	@echo "Shell result:"
	@< infile.txt cat | wc -l > outfile.txt; echo "Error: $$?"
	@cat outfile.txt | cat -e
	@echo "Your result:"
	@./pipex infile.txt "cat" "wc -l" outfile.txt; echo "Error: $$?"
	@cat outfile.txt | cat -e
	@echo "\n$(YELLOW)WITH INFILE$(END)\n"
	@echo "ola" > infile.txt
	@echo "Shell result:"
	@< infile.txt cat | wc -l > outfile.txt; echo "Error: $$?"
	@cat outfile.txt | cat -e
	@rm -f outfile.txt
	@echo "Your result:"
	@./pipex infile.txt "cat" "wc -l" outfile.txt; echo "Error: $$?"
	@cat outfile.txt | cat -e
	@rm -f outfile.txt
	@echo "\n\n$(GREEN)------TEST N2------$(END)\n"
	@echo "./pipex infile.txt \"grep a1\" \"wc -w\" outfile.txt\n"
	@echo "Shell result:"
	@< infile.txt grep a1 | wc -w > outfile.txt; echo "Error: $$?"
	@cat outfile.txt | cat -e
	@rm -f outfile.txt
	@echo "Your result:"
	@./pipex infile.txt "grep a1" "wc -w" outfile.txt; echo "Error: $$?"
	@cat outfile.txt | cat -e
	@rm -f outfile.txt infile.txt
	@echo "\n\n$(GREEN)------TEST N3------$(END)\n"
	@echo "./pipex infile.txt \"grep a1\" \"LOL -w\" outfile.txt\n"
	@echo "Shell result:"
	@< infile.txt grep a1 | LOL -w > outfile.txt; echo "Error: $$?"
	@cat outfile.txt | cat -e
	@rm -f outfile.txt
	@echo "Your result:"
	@./pipex infile.txt "grep a1" "LOL -w" outfile.txt; echo "Error: $$?"
	@cat outfile.txt | cat -e
	@rm -f outfile.txt no_permissions.txt

# Colors
DEF_COLOR = \033[0;39m
GRAY = \033[0;90m
RED = \033[0;91m
GREEN = \033[0;92m
YELLOW = \033[0;93m
BLUE = \033[0;94m
MAGENTA = \033[0;95m
CYAN = \033[0;96m
WHITE = \033[0;97m

git :
	@ clear && sleep 1 && echo "$(RED)\n\n\n\nStart cleanning...\n\n" && sleep 1
	@ make fclean
	@ echo "$(CYAN)\n\n\n\nEverything clean\n\n\n$(GREEN)Git status:\n\n$(DEF_COLOR)"
	@ git status && sleep 1 && echo "$(RED)Press Enter to confirm" && read enter
	@ echo "$(DEF_COLOR)\nGit add everything\n\n"
	@ git add . && git status
	@ echo "$(YELLOW)" && read -p "Enter commit message: " message && read -p "Enter commit comment: " comment && git commit -m "$message" -m "$comment"
	@ echo "$(GRAY)" && git push
	@ echo "$(GREEN)\n\nGit push done with sucess\n\n$(DEF_COLOR)"

### phony faz com que o nome destes comandos não sejam confundidos com ficheiros. ou seja, se houver um ficheiro com esse nome para ser compilado ou atualizado, o compilador ignora.
.PHONY: all clean fclean re bonus tests

### o silent faz com que as ações ocorram, mas que não apareçam no terminal. Daí o nome silent, pq apesar de ocorreram as ações, elas são silenciosas...não aparecem no terminal.
#.IGNORE:
#.SILENT:


