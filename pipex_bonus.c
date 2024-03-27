/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex_bonus.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jode-jes <jode-jes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/21 12:18:59 by joaosilva         #+#    #+#             */
/*   Updated: 2024/03/27 19:17:43 by jode-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Diferenças do subject do bónus po mandatório:
// 1 - O bonús tem de Ler múltiplos comandos e ...:
//$> ./pipex file1 cmd1 cmd2 cmd3 ... cmdn file2
// Should behave like this (a shell faz isto):
// < file1 cmd1 | cmd2 | cmd3 ... | cmdn > file2
// 2 - ... tem de suportar o here_doc (<< LIMITER) e (>> file)
// que é um comando que lê do stdin até encontrar uma palavra
//  que é passada como argumento
// Support « and » when the first parameter is "here_doc".
// This:
// $> ./pipex here_doc LIMITER cmd cmd1 file
// Should behave like:
// cmd << LIMITER | cmd1 >> file
// ...............................................
// Diferenças de implementaçãp do bónus para o mandatório:
// As diferenças no código entre o bónus e o mandatório
// encontram-se no:
// 1 - MAIN (as restantes funções permanecem TODAS iguais);
// 2 - O acréscimo de uma função ft_here_doc.
//-----------
// 1 - MAIN:
// No Main o código do bónus terá de ser diferente do
// código do mandatório nos seguintes pontos:
// 1.1 - if (ac >= 6 || ac == 5 && ft_strncmp(av[1], "here_doc", 8) != 0)
// O if (ac == 5) passa a poder receber
// vários argumentos ou o here_docs:
//        CODE: o if (ac == 5) passa a if (ac >= 6 || ac == 5 &&
// ft_strncmp(av[1], "here_doc", 8) != 0) // Ou seja,
// se tivermos 6 argumentos ou se tivermos 5 argumentos
// e o primeiro argumento não for "here_doc".
// 1.2 - here_doc = (ac >= 6) && ft_strncmp(av[1], "here_doc", 8) == 0;
// ft_here_doc (av[2]);
// fd_file [0] = open (av[1], O_RDONLY, 0644);
// Temos de fazer com que o programa consiga ler do heredocs
// caso este exista. Caso exista precisemos ler por aí:
// Temos de fazer com que o programa considera o here_docs e leia
// o que está no stdin até encontrar o LIMITER (que costuma ser EOF).
// Para tal acrescentamos
//        CODE: o here_doc = (ac >= 6) && ft_strncmp(av[1],
// "here_doc", 8) == 0; // Se tivermos 6 argumentos
// e o primeiro argumento for "here_doc" (here_doc = 1,
// senão here_doc = 0), então vamos ler do here_doc.
// Se o here_doc existir, vamos chamar a função ft_here_doc(av[2]);
// através do:
//       CODE: if (here_doc)
//        ft_here_doc (av[2]); // Como referido, se o here_doc
// existir, ou seja, here_doc = 1, vamos criamos um ficheiro. Um ficheiro que
// se chama here_doc. E pq passamos o av[2]? Pq o av[2] é o limiter,
// e nós queremos ler até encontrar o limiter. Portanto, ao criarmos
// um ficheiro, vamos "copiar" o conteúdo do stdin para esse ficheiro
// até encontrarmos o limiter. E é isso que a função ft_here_doc faz.
// A seguir abrimos esse ficheiro através do:
//       CODE: fd_file [0] = open (av[1], O_RDONLY, 0644);
// Aliás, a apartir do if (here_doc), o open vai abrir o ficheiro
// here_doc (que estará como av[1]), ou o ficheiro que lá estará, pq
// se o av[1] não for here_doc, vai ser o ficheiro infile, e o open
// vai abrir na mesma esse ficheiro que lhe passamos.
// Ou seja, Se o programa não for here_doc, ou seja, here_doc = 0,
// vamos abrir o ficheiro que está em av[1] em modo de leitura.
// 1.3 - while (i < ac - 2)
//  redirect(av[i], av[i++], envp);
// Função redirect com While:
// O programa tem de correr vários comandos filhos.
// Para tal temos q Fazer um While com a função redirect
// onde passamos os vários comandos filhos. Temos q fazer uma
// Função Redirect com um While até chegarmos a
// ac - 2. Qd chegarmos paramos.
// E porquê ac - 2? pq o último argumento é o
// ficheiro de destino, o penúltimo comando é o comando do processo pai,
// logo o ante-penúltimo comando é o último comando do processo filho.
// E é esse último comando que queremos correr como último durante o while.
// 1.4 - i = 2 + here_doc;
// While aceitar infile ou here_docs qd passamos o av [1]
// Para que o while corra corretamente independentemente se o av[1] é
// o infile ou o here_docs, programa aceite os vários filhos ou here_doc,
// temos de colocar o programa a aceitar os dois argumentos (caso o
// argumento seja here_docs ou caso não seja). Para tal fazemos
//      CODE: i = 2 + here_docs;
// Se o here_docs for 0, então o i será igual a 2, como podemos ver
// no exemplo abaixo:
//$> ./pipex file1 cmd1 cmd2 cmd3 ... cmdn file2
// Se o here_docs for 1, então o i será igual a 3, e iremos correr
// o while a partir do av[3] (que será o primeiro comando filho),
// como podemos ver no exemplo abaixo:
// $> ./pipex here_doc LIMITER cmd cmd1 file
// 1.5 - fd_file[1] = open(av[ac - 1], O_RDWR | O_TRUNC  * !here_doc | O_APPEND
//		* here_doc | O_CREAT, 0644);
// O ficheiro de saída pode ter Truncate ou Append
// O fd_file[1] = open(av[ac - 1], O_RDWR | O_TRUNC | O_CREAT, 0644);
// No bonús temos de ter essas duas opções pq se o here_doc = 0,
// então O_TRUNC = 1 e O_APPEND = 0, se here_doc = 1,
//	então O_TRUNC = 0 e O_APPEND = 1.
// O fd_file[1] pode ter append em vez de truncate. 
// O truncate apaga e substitui. O append acrescenta
// ao ficheiro. Para ter uma ou outra multiplicamos por 0 ou 1. Se here_doc = 0,
//	então O_TRUNC = 1 e
// O_APPEND = 0, se here_doc = 1, então O_TRUNC = 0 e O_APPEND = 1.
// 1.6 - exec_cmd(av[ac - 2], envp);
// No mesmo sentido do ponto 3,
// temos de mudar o argumento passado
// para a função exec_cmd através do main:
// Deixa de ser exec_cmd(av[3], envp) e passa a ser
// exec_cmd(av[ac - 2], envp); onde vamos correr o
// último comando com o processo pai. Este último comando irá correr
// sobre o último pipe resultante do último processo filho, cujo resultado
// será escrito no ficheiro de saída / output do processo filho (vulgo pipe),
// e que estará dispnível para que o pai corra o último comando em cima deste.
// 2 - FUNÇÃO HERE_DOCS:
// No ponto 1.2 explica o que o a função ft_here_doc faz.
// A função ft_here_doc lê do stdin até encontrar uma palavra que é 
// passada como argumento e que funciona como um delimitador (LIMITER). 
// Geralmente a palavra usada é o EOF.
// A função ft_here_doc é chamada no main,
//	caso o primeiro argumento seja "here_doc".
// A função ft_here_doc cria um ficheiro chamado here_doc e o conteúdo 
// lido é escrito nesse ficheiro here_doc.
// Após ler o ficheiro todo até à palavra delimitadora,
//	a função ft_here_doc "devolve" o ficheiro here_doc.
// "Devolve" está entre aspas pq a função não devolve nada,
//	mas sim escreve o conteúdo do ficheiro here_doc no stdin.
// Função here_docs linha a linha:
// void	ft_here_doc(char *eof)
// {
//      fd_here_doc = open("here_doc", O_RDWR | O_CREAT | O_TRUNC, 0644);
// abre o ficheiro here_doc em modo de leitura e escrita,
//	cria o ficheiro se ele não existir e trunca o ficheiro para zero se ele 
// existir. O 0644 é o modo de permissão do ficheiro.
//      if (fd_here_doc == -1) // se o ficheiro here_doc não for aberto,
//	o programa dá erro e termina.
//      {
//           perror("Error");
// imprime a mensagem de erro associada ao valor da variável errno. A 
// mensagem de erro é escrita no standard error output (stderr).
//           exit(1);
// é o mesmo que EXIT_FAILURE. Basicamente termina o 
// programa com o código de erro 1.
//       }
//      ft_printf("> ");
// imprime o prompt no terminal para o user escrever 
// de forma a simular o terminal.
//      line = get_next_line(0);
// pq é que o get_nex_line recebe zero? Recebe zero porque 
// o 0 é o file descriptor do terminal (é o file descriptor 
// do standard input ou stdin). O que é que o get_next_line(0) faz? 
// ele lê a linha que o user escreveu no terminal e retorna essa linha.
//        while (line && ft_strncmp(line, eof, ft_strlen(eof))
//	|| ft_strclen(line, '\n') != ft_strlen(eof)
// enquanto a linha for diferente do eof ou o tamanho da linha 
// for diferente do tamanho do eof,
//	o programa continua a pedir input ao user
//       {
//          write(fd_here_doc, line, ft_strlen(line));
// escreve a linha no ficheiro here_doc (fd_here_doc). tb podia 
// usar ft_putstr_fd(line,
//	fd_here_doc);
//          write(fd_here_doc, "\n", 1);
// escreve uma nova linha no ficheiro here_doc (fd_here_doc). também podia 
// usar a função ft_putchar_fd('\n',
//	fd_here_doc);
//          free(line); // liberta a memória alocada para a linha
//          ft_printf("> ");
// imprime o prompt no terminal para o user escrever de 
// forma a simular o terminal.
//          line = get_next_line(0);
// lê a linha que o user escreveu no terminal e retorna essa linha.
//        }
//        free(line); // liberta a memória alocada para a linha
//        close(fd_here_doc); // fecha o ficheiro here_doc
//

// ----------------------------------------
// O que fica igual?
// O dup2 (fd_file[0], STDIN_FILENO); é igual - Vamos na mesma
// apontar o STDIN_FILENO para o fd_file[0]. Ou seja, o que estiver
// no ficheiro fd_file[0] vai ser lido pelo STDIN_FILENO.
// Todas as outras funções.

#include "pipex_bonus.h"

void	check(int result, char *msg)
{
	if (result <= -1)
	{
		ft_putstr_fd("pipex: ", STDERR_FILENO);
		perror(msg);
		exit(EXIT_FAILURE);
	}
}

char	*get_path(char *cmd, char **envp)
{
	char	*path;
	char	*aux;
	int		i;

	i = 0;
	while (envp[i] && ft_strncmp(envp[i], "PATH=", 5) != 0)
		i++;
	if (!envp[i])
		return (ft_strdup(cmd));
	envp[i] += 5;
	while (*envp[i])
	{
		path = ft_substr(envp[i], 0, ft_strclen(envp[i], ':'));
		aux = ft_strjoin(path, "/");
		free(path);
		path = ft_strjoin(aux, cmd);
		free(aux);
		if (access(path, R_OK) == 0)
			return (path);
		free(path);
		envp[i] += ft_strclen(envp[i], ':') + (ft_strclen(envp[i],
					':') != ft_strlen(envp[i]));
	}
	return (ft_strdup(cmd));
}

void	exec_cmd(char *cmd, char **envp)
{
	char	**args;
	char	*path;

	args = ft_split(cmd, ' ');
	if (ft_strchr(args[0], '/'))
		path = ft_strdup(args[0]);
	else
		path = get_path(args[0], envp);
	execve(path, args, envp);
	dup2(STDERR_FILENO, STDOUT_FILENO);
	ft_putstr_fd("Error: command not found\n", STDERR_FILENO);
	ft_free_array(args);
	free(path);
	exit(127);
}

void	redirect(char *file, char *cmd, char **envp)
{
	pid_t	pid;
	int		fd[2];

	check(pipe(fd), "pipe");
	pid = fork();
	check(pid, "fork");
	if (pid == 0)
	{
		check(access(file, R_OK), file);
		dup2(fd[1], STDOUT_FILENO);
		close(fd[0]);
		exec_cmd(cmd, envp);
	}
	else if (pid > 0)
	{
		dup2(fd[0], STDIN_FILENO);
		close(fd[1]);
		waitpid(pid, NULL, WNOHANG);
	}
	else
	{
		ft_putstr_fd("Error: fork failed\n", STDERR_FILENO);
		exit(EXIT_FAILURE);
	}
}

int	main(int ac, char **av, char **envp)
{
	int	fd_file[2];
	int	here_doc;
	int	i;

	if (ac >= 6 || (ac == 5 && ft_strncmp(av[1], "here_doc", 8) != 0))
	{
		here_doc = (ac >= 6) && ft_strncmp(av[1], "here_doc", 8) == 0;
		if (here_doc)
			ft_here_doc(av[2]);
		fd_file[0] = open(av[1], O_RDONLY, 0644);
		dup2(fd_file[0], STDIN_FILENO);
		i = 2 + here_doc;
		while (i < ac - 2)
			redirect(av[1], av[i++], envp);
		fd_file[1] = open(av[ac - 1], O_RDWR | O_TRUNC * !here_doc | O_APPEND
				* here_doc | O_CREAT, 0644);
		check(fd_file[1], av[ac - 1]);
		dup2(fd_file[1], STDOUT_FILENO);
		exec_cmd(av[ac - 2], envp);
	}
	else
		ft_putstr_fd("Error: Invalid number of arguments\n", 2);
	return (1);
}
