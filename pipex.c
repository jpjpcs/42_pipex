/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jode-jes <jode-jes@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/20 10:57:05 by joaosilva         #+#    #+#             */
/*   Updated: 2024/03/27 19:17:35 by jode-jes         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// Conceitos globais para entender o projeto: Conceitos teóricos de
// file descriptor, pipes, fork, execve, envp,
// here-doc e EOF (2 últimos no bónus).
// 1ª conceito - Um descritor de arquivo (file descriptor) é um número inteiro
// não negativo que é usado para acessar um arquivo ou um fluxo de
// entrada/saída (I/O) em sistemas operacionais Unix e semelhantes,
// incluindo Linux e macOS.
// Os descritores de arquivo são identificativos,
// são um sistema de identificação,
// usados para identificar os arquivos abertos por um processo.
// Basicamente Quando um processo abre um arquivo, o sistema operacional retorna
// um descritor de arquivo que o processo pode usar para ler, escrever ou
// modificar o arquivo. Cada processo tem sua própria tabela de descritores de
// arquivo, e os descritores de arquivo são usados para rastrear quais arquivos
// o processo abriu. Além de arquivos regulares,
// os descritores de arquivo também
// podem se referir a outros tipos de recursos de I/O,
// como pipes,
// sockets e dispositivos.
// Existem três descritores de arquivo padrão que são abertos automaticamente
// para cada processo:
// 0: Entrada padrão (STDIN)
// 1: Saída padrão (STDOUT)
// 2: Erro padrão (STDERR)
// Esses descritores de arquivo permitem que o processo leia a entrada do
// teclado (ou de outro lugar, se a entrada padrão for redirecionada),
// escreva a saída para a tela (ou para outro lugar, se a saída padrão
// for redirecionada) e escreva mensagens de erro para a
// tela (ou para outro lugar, se o erro padrão for redirecionado).
// 2ª conceito
// - No contexto de pipes em sistemas operacionais Unix-like c/o Linux,
// um pipe é uma forma de redirecionamento que é usada em programação
// de shell para canalizar uma stream de dados de um processo para outro.
// Basicamente o pipe é a forma da shell comunicar entre um comando e outro.
// E na realidade é um ficheiro temporário que é criado na memória do sistema,
// como se fosse um buffer na memória,
//	um buffer c/ 2 files descriptor: um de escrita,
// outro de leitura. O processo pai escreve no file descriptor de escrita,
// e o processo filho lê do file descriptor de leitura.
// 3ª conceito - O fork é teres 2 processos ou mais: um pai e um filho.
// Fazemos fork pq quando chamamos o execve,
//	o processo que o chama é substituído...
// e nós não queremos que o programa termine por aí. 
// Logo usamos um truque: fazemos um
// fork, dividindo o programa em 2 processos,
//	e o processo filho executa o execve....
// executa e morre. Ou seja, o pai cria o filho, o filho chama e executa o ...
// execve e morre. O pai continua. E pode criar outro processo filho para um ...
// segundo comando. O segundo processo fillho executa esse comando e morre...
// e o pai continua. E assim sucessivamente. até terminarmos o pai, que é ...
// executando no main...qd chamamos e executamos o execve no main.
// tenho um processo parent e um child. O id do child é zero.
// O id do parente passa a ser o novo id atribuído ao child.
// 4ª conceito - O execve pára um processo e inicia outro.
// termina um processo e substitui por outro novo, atribuindo o id do
// processo antigo ao novo processo. Exemplo: estou a correr cd no terminal
// e dps faço pwd. O cd termina e começa o pwd, que fica com o id do
// processo antigo. Aqui entra o envp, que basicamente é uma array de
// strings que contém variáveis de ambiente. Neste caso contém o PATH,
// localização onde estará a lista de comandos que queremos aplicar.
// 5ª conceito - envp é um array de strings que contém variáveis de ambiente.
// É uma forma de passar variáveis de ambiente para um processo filho.
// O envp é passado para o execve, e o execve tenta executar o comando.
// Basicamente, o envp contém as variáveis de ambiente que o processo filho
// vai usar. Neste caso, o envp contém o caminho para o comando que queremos
// aplicar no fd[1] do pipe durante o processo filho. O envp apresentará o
// PATH, que é a localização onde estarão os comandos que queremos executar.
// 6ª e 7º conceitos - heredoc e EOF.
// heredocs é uma funcionalidade da Shell que nos permite
// incluir uma multiline string.They allow you to input a multiline
// string in Shell. Basicamente Heredoc é uma função da shell que
// permite que vários inputs sejam introduzidos na shell e lidos desta,
// até que o input seja terminado com um EOF. EOF significa End of File.
// Exemplo: Permite-nos incluir múltiplas linhas (multiline strings) no
// programa (sem ter que escrever diretamente no terminal usando
// “blink strings” ou ter que escrever num ficheiro....q terá de
// ser lido por um programa tipo o GNL). Ou seja, EOF é basicamente uma
// forma de transmitir ao terminal que o ficheiro chegou ao fim.
// Tb podia ser escrito como “ANTHONY” ou qqer outra palavra.
// O comando pára de contar nessa palavra. Exemplo: wc -l ANTHONY
// A Explicação:
// ANTHOY
//  5
// Conta 5, parando no ANTHONY.
// Função Main
// 1. A função main recebe 4 argumentos: o nome do ficheiro de input,
// o comando a executar, o nome do ficheiro de output e o envp.
// Exemplo: ./pipex infile "cat -e" "grep main" outfile
// ./pipex pipex.c cat "grep main" aqui ou 
// ./pipex pipex.c cat "grep main" aqui.txt
// 2. Funções open e dup2 chamadas na main:
//  O main abre o ficheiro de input, e redireciona o stdin a apontar
//  para esse ficheiro, usando para tal a função dup2. A chamada dup2()
//  é usada para duplicar um descritor de arquivo. A chamada
//  dup2(fd_file[0], STDIN_FILENO) faz com que o
//  STDIN_FILENO (normalmente o teclado) seja redirecionado para fd_file[0].
// O mesmo acontece com o STDOUT_FILENO(que normalmente é o ecrã). Ao invés
// de apresentar no ecrã, nós queremos que o output seja redirecionado
// para o fd_file[1] - que é o ficheiro de outuput do processo todo...ou seja,
// o resultado final. Voltando ao STDIN_FILENO, e o facto dele substituir
// o teclado, significa que qualquer input que seja feito no terminal,
// será redirecionado para o ficheiro de input. Ou seja, isso
// significa que qualquer leitura do STDIN_FILENO agora lê do fd_file[0].
// A mesma lógica aplica-se ao STDOUT_FILENO. Qualquer output que seria
// apresentado no ecrã, é redirecionado para o ficheiro de output.
//  A razão para fazer isso antes de chamar a função redirect() é para que o
//  processo filho criado dentro da função redirect() 
// herde esse redirecionamento
//  de entrada padrão. Isso significa que quando o 
// processo filho executa o comando,
//  o comando lê a entrada do fd_file[0] em vez do teclado. Depois, dentro da
// função redirect(), pode haver outra chamada dup2() para redirecionar a
// entrada ou saída padrão novamente, dependendo do que a função redirect()
// está fazendo. Isso não desfaz o redirecionamento feito no processo pai,
// porque o processo filho tem seus próprios descritores de arquivo.
// Basicamente a chamada dup2(fd_file[0], STDIN_FILENO); no main está
// redirecionando a entrada padrão (STDIN_FILENO) para o arquivo de
// entrada (fd_file[0]). Isso significa que qualquer leitura da entrada
// padrão no processo atual (e em qualquer processo filho criado
// posteriormente) lerá do arquivo de entrada em vez do teclado.
// Isso é útil se o comando que você está executando na função
//  exec_cmd() espera ler a entrada do teclado (ou seja, da entrada
// padrão). Ao redirecionar a entrada padrão para o arquivo de entrada,
// você está efetivamente fornecendo a entrada do arquivo para o comando.
// Por exemplo, se o comando for algo como sort, que lê da entrada padrão,
// fazer dup2(fd_file[0], STDIN_FILENO); fará com que sort leia do
// arquivo de entrada em vez do teclado. Além disso, qualquer processo
// filho criado pela função fork() herdará os descritores de arquivo
// do processo pai. Portanto, se você redirecionar a entrada padrão
// para um arquivo no processo pai, o processo filho também lerá
// desse arquivo quando ler da entrada padrão. Isso é útil se
// o processo filho também precisar ler do arquivo de entrada.
// 3. Função redirect chamada na main:
//  Depois chama a função redirect, passando o ficheiro de input,
//  o comando a executar e o envp.
//  A função redirect cria um pipe, verifica se foi devidamente criado com a
//  função check, faz um fork, e sobre o fd[1] do pipe (que
//  é o input do processo filho e corresponde ao fd_file[0] proveniente do
//  main - para o qual o STDIN está a apontar - o que foi feito no dup2) executa
// o comando passado como av[2] do main para a função redirect.
// O resultado do comando av[2] sobre o ficheiro fd [1] é 
// redirecionado para o pipe,
// que fica a aguardar que o processo pai o leia e aplique o comando passado
// como av[3] do main para a função redirect.
// para a função redirect sobre o fd[1] do pipe,
//	que é o input do processo filho.
// 4. Função Open E FICHEIRO OUTPUT: O main abre o ficheiro de output,
//	faz o check pa ver se abriu,
// e redireciona o STDOUT para esse ficheiro.
// No ficheiro de output fd_file [1] escreve (através das Flags
// O_RDWR | O_TRUNC | O_CREAT) que lhe dão permissão para escrever,
// truncar/substituir o conteudo por outro apagando o que lá estava,
// e criar o ficheiro, respetivamente) o resultado da aplicação
// do comando av[3] - passado do main para a função redirect.
// 5. Função EXEC_CMD - Para execução do cmd sobre a pipe,
// exprimindo o resultado no STDOUT,
// a função exec_cmd é chamada. A função exec_cmd() executa um
// comando que lê da entrada padrão e/ou escreve para a saída
// padrão. Portanto, não é necessário passar o nome do arquivo
// para a função exec_cmd(), porque o comando lê do arquivo
// indiretamente através da entrada padrão. Antes de chamar
// exec_cmd(), o código chama dup2(fd_file[0], STDIN_FILENO);
// e dup2(fd_file[1], STDOUT_FILENO);. Essas chamadas
// redirecionam a entrada padrão e a saída padrão para os
// arquivos de entrada e saída, respectivamente. Portanto,
// quando exec_cmd() executa o comando, o comando lê a
// entrada do arquivo de entrada e escreve a saída para
// o arquivo de saída, mesmo que o nome do arquivo não
// seja passado para a função exec_cmd(). Em outras palavras,
// a função exec_cmd() não precisa saber sobre os arquivos
// porque ela opera na entrada padrão e na saída padrão,
// e o código que chama exec_cmd() é responsável por
// configurar a entrada padrão e a saída padrão para
// apontar para os arquivos corretos.
// Curiosidades: No main podiamos ter forçado o fd_1 (STDOUT) a
// ser fechado através da função close, para que ao invés de
// estarmos a redirecionar o STDOUT para o ficheiro de output,
// estivéssemos a escrever diretamente no ficheiro de output.
// Isso era uma das possibiidade, mas não foi a escolhida,
// pq o STDOUT é um file descriptor que é usado por muitos
// comandos, e fechá-lo poderia causar problemas, além de que
// teríamos outras questões por resolver, como as que elencamos
// seguidamente:
// Porque razão lemos dum arquivo em vez de diretamente do fd?
// Não lemos diretamente do descritor de arquivo por diversos
// motivos que podem depender do design do nosso programa.
// Algumas possíveis razões são:
// 1. Portabilidade: Os descritores de arquivo são específicos
// para o sistema operacional e podem não ser portáveis entre
// diferentes sistemas operacionais. Por outro lado, os arquivos
// são uma abstração de alto nível que é suportada por quase
// todos os sistemas operacionais.
// 2. Facilidade de uso: Trabalhar com arquivos pode ser mais
// fácil do que trabalhar com descritores de arquivo,
// especialmente para operações complexas. Por exemplo,
// a linguagem de programação pode fornecer funções de alto
// nível para ler e escrever arquivos que são mais fáceis de
// usar do que as funções de baixo nível para trabalhar
// com descritores de arquivo.
// 3. Flexibilidade: Trabalhar com arquivos pode ser mais
// flexível do que trabalhar com descritores de arquivo,
// especialmente se você precisar realizar operações
// complexas, como redirecionamento de entrada/saída,
// manipulação de arquivos temporários, etc.
// 4. Persistência de dados: Os dados em um arquivo
// persistem mesmo depois que o programa termina,
// enquanto os dados em um descritor de arquivo são
// perdidos quando o descritor de arquivo é fechado.
// 5. Segurança e controle de acesso: Trabalhar com
// arquivos pode fornecer mais segurança e controle de
// acesso aos dados do que trabalhar diretamente com
// descritores de arquivo. Por exemplo, você pode definir
// permissões de acesso em arquivos para controlar quem
// pode ler, escrever ou executar o arquivo.
// 6.Facilidade de depuração: Trabalhar com arquivos
// pode facilitar a depuração do programa, pois você
// pode inspecionar o conteúdo do arquivo com ferramentas
// de linha de comando ou editores de texto.
// Em resumo, ler de um arquivo em vez de diretamente de um
// descritor de arquivo pode ser mais portátil, fácil de usar,
// flexível, seguro, persistente e fácil de depurar, dependendo das
// necessidades do seu programa. Por estas razões optamos por não
// fechar o STDOUT no main, e dps abri-lo (usando a open) e trabalhar
//  nele diretamente. Ao invés, optamos por utiizar o dup2 e
// redirecionar o STDOUT para o fd_file[1] e trabalhar com esse ficheiro.
// Main, função redirect e exec_cmd
//	- File descriptors e dup2 chamados nessas funções
// O dup2 é uma função que duplica um file descriptor. Por exemplo, no caso onde
// é chamado no main, o STDIN_FILENO passa a apontar para o fd_file[0]. Ou seja,
// mete o conteúdo do fd_file[0] no STDIN_FILENO. O mesmo acontece em relação
// ao STDOUT_FILENO. O STDOUT_FILENO passa a apontar para o fd_file[1].
// Nota: Os file descriptors são independentes do processo, e são usados para
// identificar os ficheiros abertos pelo processo.
// No caso de um novo processo, o fd aberto será o num. q vem a seguir.
// Se já temos o fd 0 (stdin), 1 (stdout) e 2 (stderror) atribuídos, então
// o nosso processo abrirá o num.3. Os processos filhos herdam os
// file descriptors dos processos pais. Os fd do processo (pai ou filho)
// são passados para o execve.
// Funçaõ redirect - se a função redirect não fechar o fd [0] durante a
// execução do processo filho, há algum problema que possa acontecer?
// Sim, pode haver problemas se você não fechar o descritor de
// arquivo fd[0] no processo filho após a chamada dup2().
// Quando você cria um pipe com a chamada pipe(fd), você
// obtém dois descritores de arquivo: fd[0] para leitura
// e fd[1] para escrita. Quando você faz dup2(fd[1],
// STDOUT_FILENO), você está dizendo ao sistema operacional
// para escrever qualquer saída que normalmente iria para a
// saída padrão (ou seja, a tela) para fd[1] em vez disso.
// Se você não fechar fd[0] no processo filho, o processo
// filho ainda terá um descritor de arquivo aberto para
// leitura do pipe. Isso pode causar problemas porque o
// processo filho pode não terminar corretamente.
// Quando o processo filho termina de escrever para o pipe,
// ele deve fechar fd[1] para que o processo pai saiba
// que não há mais dados a serem lidos. Se fd[0] ainda
// estiver aberto no processo filho, o processo pai
// pode nunca receber essa indicação e pode ficar
// bloqueado indefinidamente tentando ler do pipe.
// Portanto, é uma boa prática fechar descritores
// de arquivo que você não precisa mais, para evitar
// esse tipo de problema.
// Diferença entre dup e dup2:
// O dup e o dup2 são funções que duplicam um file descriptor.
// dup e dup2 são chamadas de sistema em Unix e sistemas operacionais
// semelhantes que são usadas para duplicar descritores de arquivo.
// Ambas têm usos semelhantes, mas há uma diferença chave entre elas:
// A diferença entre o dup e o dup2 é que o dup simplesmente duplica
// o file descriptor mais baixo disponível, enquanto o dup2 permite
// que você especifique o número do file descriptor que deseja duplicar.
// Ou seja, o dup2 é uma versão mais avançada do dup.
// O dup2 permite que você especifique o número do file 
// descriptor que você deseja
// duplicar, enquanto o dup simplesmente duplica o file descriptor mais baixo.
// Por exemplo, se você chamar dup(fd), ele duplicará o file descriptor fd e
// retornará um novo file descriptor que é o número mais baixo disponível.
// Se você chamar dup2(fd, new_fd), ele duplicará o file descriptor fd e
// retornará um novo file descriptor com o número new_fd. Se new_fd já estiver
// aberto, ele será fechado antes de duplicar fd. Isso é útil se você deseja
// especificar o número do file descriptor que deseja usar.
// O dup2 é útil quando você deseja redirecionar um file descriptor específico
// para outro file descriptor específico. Por exemplo,
//	se você deseja redirecionar
// a entrada padrão para um arquivo específico,
//	você pode usar dup2 para redirecionar
// o file descriptor 0 (entrada padrão) para o file descriptor do arquivo.
// O dup2 é útil quando você deseja redirecionar um file descriptor específico
// para outro file descriptor específico.
// EXEMPLO PRÀTICO:
// Na prática dup(int oldfd) cria uma cópia do descritor de arquivo
// oldfd. O novo descritor de arquivo retorna o número do menor
// descritor de arquivo não utilizado. dup2(int oldfd, int newfd)
// também cria uma cópia do descritor de arquivo oldfd, mas faz
// isso no descritor de arquivo especificado por newfd. Se newfd
// já estiver aberto, ele será fechado antes de ser reutilizado.
// Se oldfd e newfd forem iguais, dup2 retornará newfd sem fechá-lo.
// Aqui está um exemplo para ilustrar a diferença:
// int fd1 = open("file1.txt", O_RDONLY);
// int fd2 = dup(fd1); // fd2 é agora uma cópia de fd1, e é o menor
// número de descritor de arquivo não utilizado.
// int fd3 = open("file2.txt", O_RDONLY);
// dup2(fd1, fd3); // fd3 é agora uma cópia de fd1. Se fd3 já estava
// aberto, ele foi fechado antes de ser reutilizado.
// Em resumo, dup2 permite que você controle o número do descritor
// de arquivo da cópia, enquanto dup não.
// Função redirect - como é lida uma pipeline na prática...função redirect
// Agora vejamos como é lida uma pipeline. Normalmente, em um pipeline:
// 1. O processo filho executa um comando e redireciona o output para
// o pipe. Isso é feito para que o resultado desse comando possa ser
// usado como entrada para outro comando.
// 2. Um segundo processo filho (ou o processo pai, dependendo
// da implementação) executa um segundo comando, pegando o input
// do pipe. Isso significa que ele usa o resultado do
// primeiro comando como sua entrada. Então, para esclarecer,
// um processo filho está redirecionando o output para o pipe,
// e um segundo processo está pegando esse output (agora
// transformado em input) do pipe.
// EXEMPLO:
// na função redirect o processo filho tem sempre id == 0,
// enquanto o processo pai herda o id do processo filho.
// logo, se id == 0 estamos a correr o processo filho.
// no exemplo dado no pipex_scheme_explication,
// o processo filho corre o comando cmd e
// redireciona o output para o pipe.
// o processo pai usa esse output como o seu input
// e corre o comando cmd, redirecionando o output
// para o ficheiro, e terminando o processo.
// o processo pai espera pelo processo filho terminar
// para terminar o processo pai.
// função redirect - Tipo de variável pid_t na função redirect
// pid_t é um tipo de dado usado para representar
// IDs de processo em C e C++. É um tipo de dado
// inteiro e é usado para armazenar o valor de
// identificação de um processo. O tipo exato de
// pid_t (por exemplo, int, long, etc.) pode variar
// dependendo do sistema operacional e da
// implementação da biblioteca C.
// função redirect - Sub-função Pipe na função redirect
// A função pipe() é uma chamada de sistema que é usada
// para criar um pipe, que é uma forma de comunicação
// interprocessos (IPC). Um pipe é um mecanismo que
// permite que dois ou mais processos troquem
// dados de maneira organizada. A função pipe() é
// declarada em C da seguinte maneira: int pipe(int pipefd[2]);
// A função pipe() recebe um array de dois inteiros como
// argumento. Após a chamada bem-sucedida à função pipe(),
// o array contém dois novos descritores de arquivo:
// pipefd[0] é o descritor de arquivo de leitura do pipe.
// pipefd[1] é o descritor de arquivo de escrita do pipe.
// Os dados escritos no descritor de arquivo de
// escrita (pipefd[1]) podem ser lidos do descritor
// de arquivo de leitura (pipefd[0]). Isso permite
// que um processo escreva dados no pipe e outro
// processo leia esses dados.
// EXEMPLO no código:
// No contexto do código comentado, a função pipe()
//  é usada para criar um pipe entre dois processos
// criados por fork(). O processo filho escreve no
// pipe e o processo pai lê do pipe. Isso permite que
// os dois processos troquem dados.
// função redirect - sub_função access e flags de acesso
//  A função access() é uma chamada de sistema que é usada
// para verificar se um arquivo existe e se o processo
// atual tem permissão para acessá-lo. A função access()
// é declarada em C da seguinte maneira:
// int access(const char *pathname, int mode);
// A função access() recebe o caminho do arquivo como
// argumento e um modo de acesso. O modo de acesso
// pode ser F_OK, R_OK, W_OK ou X_OK, que são usados
// para verificar se o arquivo existe, se pode ser lido,
// se pode ser escrito ou se pode ser executado, respectivamente.
// Existem quatro flags que podem ser usadas com access():
// F_OK: verifica se o arquivo existe.
// R_OK: verifica se o arquivo pode ser lido.
// W_OK: verifica se o arquivo pode ser escrito.
// X_OK: verifica se o arquivo pode ser executado.
// A função access() retorna 0 se o arquivo existe e o
// processo atual tem permissão para acessá-lo. Caso
// contrário, retorna -1 e define errno para indicar o
// tipo de erro que ocorreu.
// EXEMPLO:
// No contexto do código comentado, a função access()
// é usada para verificar se o arquivo de entrada e o
// arquivo de saída existem e se o processo atual tem
// permissão para acessá-los. A função access() é chamada
// duas vezes: uma vez para verificar se o arquivo de
// entrada existe e outra vez para verificar se o arquivo
// de saída existe. Se o arquivo existir e o processo
// atual tiver permissão para acessá-lo, a função access()
// retornará 0. Caso contrário, retornará -1 e definirá
// errno para indicar o tipo de erro que ocorreu.
// função redirect - Sub_função waitpid e Flag WNOHANG
// A flag WNOHANG é usada com a função waitpid() em C.
// waitpid() é uma chamada de sistema que é usada para
// fazer o processo pai esperar até que um de seus
// processos filhos termine.
// A flag WNOHANG faz com que a chamada waitpid() não
// bloqueie se o processo filho especificado ainda
// não tiver terminado. Em outras palavras, waitpid()
// retorna imediatamente se o processo filho ainda
// estiver em execução. Isso é útil se você deseja
// verificar o status de um processo filho sem
// bloquear o processo pai.
// EXEMPLO:
// No contexto do código comentado, a flag WNOHANG
// é usada com a função waitpid() para verificar
// se o processo filho terminou sem bloquear o
// processo pai. Isso permite que o processo pai
// continue executando outras tarefas enquanto
// aguarda o término do processo filho.
// Basicamente esta flag não espera por tempo indefinido.
// O Walter meteu esta flag para não dar timeout num teste
// que ele e Balsa fizeram. mas eu não sei qual foi o teste.
// Função Check
// esta função verifica se tudo dá erro. é uma função standard de erro.
// em vez de estarmos a fazer sempre para cada função if (xpt == -1) -> sair,
// fazemos uma q faz isso para tudo. por exemplo no main utilizamos a função
// para verificar se o ficheiro fd stdout (q é o av [ac -1] passado na main)
// abriu corretamente. Se não abriu, dá erro, e perror identica esse erro e
// mostra-o no ecrã, acrescentando ainda a msg que lhe passamos que queremos
// que também seja mostrada. Neste caso a msg é o nome do ficheiro stdout.
// Função exec_cmd
// void exec_cmd (char *cmd, char **envp)
// args = ft_split(cmd, ' '); // cria os args e separa-os.
// como? No exemplo ./pipex infile "cat -e" "grep main" outfile,
// o que é passado da função redirect ou do main (q é passado
// para a função redirect) é só o cmd1 (o tal cat -e), que
// corresponde ao av[2] do script inserido (cat -e está na
// posição 2 no script ./pipex infile "cat -e" "grep main" outfile).
// Ou seja, o split recebe apenas o av[2] que vem do
// main (ou é passado para a redirect que dps chama a exec_cmd).
// O split divide o "cat -e" em cat e em -e, e passa para o execve.
// O execve sabe que o -e é um argumento do cat, assim como o main
// é um argumento do grep.
// if (ft_strchr(args[0], '/')) // se o cmd tiver uma /,
// então é pq contém o caminho do comando. Se tiver o
// caminho do comando posso igualar o path ao caminho do
// comando. Faço um strdup unicamente pq dps tenho de
// dar free a um ponteiro (path). E tenho de dar free
// pq qd construo o path, faço malloc (na função get_path).
// E qd faço malloc, tenho de dar free.
// path = ft_strdup(args[0]); // O path pode ser igual ao
// args[0] se o cmd enviado tiver o caminho do comando.
// path = get_path(args[0], envp); // se não tiver o caminho do
// comando, então tenho de procurar o caminho do comando no envp.
// Para isso uso a função get_path que me devolve o caminho do
// comando. Como não sei o tamanho do caminho, tenho de fazer
// alocações de memória. Caso não encontre o caminho do
// comando (não encontra o PATH, o get_path devolve uma
// cópia do cmd usando o strdup), então o path é igual
// ao cmd. se encontrar, reconstruímos o caminho do
// comando no while da função get_path.
// Função exec_cmd - Pq não passo nenhum ficheiro para a função exec_cmd()?
// Não passo pq a exec_cmd executa um comando que
// lê da entrada padrão e escreve para a saída
// padrão. Como? Através da execve.
// Portanto, não é necessário passar o nome do
// arquivo para a função exec_cmd(), porque o comando lê
// do arquivo indiretamente através da entrada padrão.
// Antes de chamar exec_cmd(), o código chama
// dup2(fd_file[0], STDIN_FILENO); e
// dup2(fd_file[1], STDOUT_FILENO);. Essas chamadas
// redirecionam a entrada padrão e a saída padrão para
// os arquivos de entrada e saída, respectivamente.
// Portanto, quando exec_cmd() executa o comando,
// o comando é executado verdadeiramente pela execve.
// Assim, a função execve() é a que realmente executa o
// novo programa que lê da entrada padrão e escreve
// para a saída padrão. A função exec_cmd() é apenas
// uma função auxiliar que prepara os argumentos e o
// ambiente para a chamada execve(). A função execve()
// substitui o processo atual pelo novo programa
// especificado. Esse novo programa herda a entrada
// padrão e a saída padrão do processo original. Portanto,
// se a entrada padrão foi redirecionada para um
// arquivo (ou pipe) antes da chamada execve(), o novo
// programa lerá desse arquivo (ou pipe). Da mesma
// forma, se a saída padrão foi redirecionada para
// um arquivo (ou pipe), o novo programa escreverá
// nesse arquivo (ou pipe). Portanto, embora seja a
// função execve() que executa o novo programa, é o
// código que chama exec_cmd() (e, portanto, execve())
// que determina de onde o novo programa lê e para
// onde ele escreve, redirecionando a entrada e a saída
// padrão conforme necessário antes da chamada exec_cmd().
// Em resumo, a função exec_cmd() não precisa saber
// sobre os arquivos de entrada e saída porque ela
// lê (indiretamente através da execve) em que
// ficheiro deve ser aplicado o comando respetivo.
// Função exec_cmd - Sub-Função execve
// execve (path, args, envp) // Se não retornar nada
// é pq o execve foi bem sucedido. Se retornar -1 é
// pq o execve falhou. O execve sabe que o -e é um
// argumento do cat, assim como o main é um argumento
// do grep.
// A função execve() é uma chamada de sistema em Unix
// e sistemas operacionais semelhantes que substitui o
// programa atual em execução por um novo programa.
// A assinatura da função é: int execve(const char *filename,
// char *const argv[], char *const envp[]);
// Os parâmetros são:
// filename: O caminho para o arquivo executável que deve
// ser executado. Este é o novo programa que substituirá
// o programa atual.
// argv: Uma matriz de strings que representam os
// argumentos que serão passados para o novo programa. O
//  primeiro argumento (índice 0) é o nome do programa. O
// último elemento da matriz deve ser um ponteiro NULL.
// envp: Uma matriz de strings que formam o ambiente para
// o novo programa. Cada string é uma definição de variável
// de ambiente no formato name=value. O último elemento
// da matriz deve ser um ponteiro NULL.
// A função execve() não retorna se for bem-sucedida,
// porque o programa atual é completamente substituído
// pelo novo programa. Se a função execve() retornar, isso
// indica que ocorreu um erro (por exemplo, o arquivo
// especificado por filename não existe ou não é um
// executável válido). Nesse caso, a função retorna -1
// e a variável global errno é definida para indicar o erro.
// continuação da função exec_cmd
// dup2 (STDERR_FILENO, STDOUT_FILENO); // se o execve
// falhar, redireciona o erro para o stdout.
// ft_putstr_fd("Error: command not found\n",
// STDERR_FILENO); // se o execve falhar, dá erro.
// be escreve no stderr, apresentando a msg de erro no ecrã.
// ft_free_array(args); // liberta a memória alocada para os args.
// free (path); // liberta a memória alocada para o path.
// exit (127); // se o execve falhar, dá erro. O 127 é
// um código de erro que indica que o comando não foi encontrado.
// Função get_path
// A função get_path tenta encontrar o caminho completo
// para o comando especificado. Ela faz isso verificando
// cada diretório no PATH do ambiente para ver se o
// comando está presente lá. Se o comando for encontrado
// em um dos diretórios do PATH, a função retorna o
// caminho completo para o comando. Caso contrário, a
// função retorna uma cópia do comando original.
// Como ela não sabe o tamanho do caminho (PATH) temos
// de usar malloc para fazer uma alocação de memória.
// Como o fazemos, temos de dar free. E como temos de dar
// free do PATH, precisamos de dar free mesmo qd não o
// encontramos, senão ficamos com leaks de memória.
// Como tal, usamos a função strdup para devolver o
// próprio comando que nos é passado, caso não tenhamos
// encontrado o comando no caminho onde estamos a procurar.
// Função get_path linha a linha:
// Seguidamente iremos ver a função linha a linha:
//  while (envp[i] && ft_strncmp(envp[i], "PATH=", 5) != 0)
// i++; - Enquanto enp[i] existir e o ft_strncmp de
// enp[i] com "PATH=" for diferente de 0, incrementa i.
// get_path - subfunção strncmp:
// PATH é uma variável de ambiente que contém uma lista
// de diretórios separados por dois pontos (:). Portanto,
// envp contém toda a string do PATH, enquanto envp[i]
// acessa uma entrada específica nessa lista de diretórios.
// Aqui está a diferença prática entre usar envp e envp[i]
// no contexto do PATH:
// Usando envp:
// Se você usasse envp, estaria acessando a string completa
// do PATH, ou seja, toda a lista de diretórios.
// Isso não seria útil para verificar diretamente se o PATH
// começa com "PATH=".
// Usando envp[i]:
// Ao usar envp[i], você está acessando uma entrada
// específica na lista de diretórios do PATH.
// Isso permite que você verifique cada diretório
// individualmente para determinar se ele começa com "PATH=".
// Portanto, ao usar envp[i], você está verificando diretamente
// cada diretório na lista do PATH para determinar
// se ele começa com "PATH=". Isso é fundamental para
// encontrar a entrada específica do PATH no array envp.
// Se você usasse envp, estaria lidando com toda a lista
// de diretórios do PATH, o que não seria útil para a
// comparação específica que você está tentando fazer.
// continuação da funcao get_path
// if (!envp[i])
// return (ft_strdup(cmd)); // se não encontrar o PATH,
// então retorna o cmd. Como testar isso? Colocar unset
// PATH no terminal e correr o pipex - unset apaga a
// variável PATH (para repor é fazer export PATH=/usr/bin/).
// Se não encontrar o PATH, então diz que não encontrou o comando.
// envp[i] += 5; // o envp[i] += 5 é usado para passar o PATH=
// e ficar a apontar para o que vem a seguir. O que é o PATH=?
// O PATH= é uma variável de ambiente que contém o caminho
// para os comandos. O envp[i] é uma string que contém o PATH.
// O envp[i] += 5 é usado para passar o PATH= e ficar a apontar
// para o que vem a seguir, no caso uma barra /.
// while (*envp[i]) // qd o conteudo deste ponteiro deixar de
// existir/for nulo, a função pára. esta função basicamente vai
// procurar dentro do caminho/diretório PATH, onde está o comando
// que estamos a executar. No caso do cat -e, ele está no :/usr/bin,
// num PATH que é composto por PATH=/home/joao/.local/bin:
// /usr/local/sbin:/usr/local/bin:/usr/sbin:/->>>>>usr/bin:<<<<<----
// /sbin:/bin:/usr/games:/usr/local/games:/snap/bin:/snap/bin.
// O que faz o while (*envp[i])? O while (*envp[i]) é uma forma
// de percorrer o PATH. O envp[i] é uma string que contém o PATH.
// O *envp[i] é o primeiro caracter do PATH. O while (*envp[i])
// percorre o PATH até encontrar o caracter nulo. O que faz o *envp[i]?
// O *envp[i] é o primeiro caracter do PATH. O while (*envp[i])
// percorre o PATH até encontrar o caracter nulo.
// path = ft_substr(envp[i], 0, ft_strclen(envp[i], ':')); //o substring
// duplica a string para ir buscar o subcaminho (ou o caminho todo),
// fazendo uma alocação de memória do subcaminho. envp [i] é o ponteiro
// para o inicío (digo qual o inicío da frase que quero replicar. o
// ponteiro para o inicío), 0 é a posição da qual queremos começar
// a copiar (nós queremos começar a copiar desde a posição zero
// uma vez que já estamos a apontar para onde queremos copiar),
// e dps dizemos qual a extensão do que queremos
// copiar (no caso até ao final da frase - se for o último
// caminho da frase - ou até aos : - se for um caminho
// intermédio. e para isso usamos o strclen (envp[i], ':')). dps
// pegamos na substr (que basicamente é o subcaminho do caminho,
// isto é, é o caminho até encontrar o fim da frase ou os : (e
// procuramos até ao fim da frase ou os : usando o strclen). o
// que faz o ft_strclen? O strclen é uma função que conta o
// número de caracteres até encontrar um caracter específico.
// Neste caso, o caracter específico é o ':'. Usamos o Strclen
// em vez do split de forma a não fazermos diversas alocações
// de memória. O que faz o ft_substr? O ft_substr é uma função
// que cria uma substring a partir de uma string. Neste caso, a
// substring é criada a partir do envp[i], começando na posição
// 0 e terminando na posição do ':'.
// aux = ft_strjoin(path, "/"); // o que faz o ft_strjoin? O
// ft_strjoin é uma função que junta duas strings. Neste caso,
// junta o path com uma barra. O path é o caminho do comando.
// A barra é o separador de diretórios. Metemos a barra para
// meter o comando a seguir.
// free (path); // pq fazemos free do path? pq já não precisamos
// deste path...ele está/faz parte do aux. Nós dps recuperamos
// o path, metendo-o a apontar para outro lado (no caso usando
// o strjoin juntamos o aux ao cmd e metemos a apontar para aí.
// O aux é o subcaminho ao qual juntamos o cmd, ficando com o
// caminho todo do comando). Fazemos free do path pq o path é
// uma variável que foi alocada dinamicamente. E qd alocamos
// dinamicamente, temos de dar free.
// path = ft_strjoin(aux, cmd); // aqui reciclamos o path,
// usando o strjoin juntamos o aux ao cmd e metemos a
// apontar para aí. O aux é o subcaminho ao qual juntamos o cmd,
// ficando com o caminho todo do comando. o que faz o ft_strjoin?
// O ft_strjoin é uma função que junta duas strings. Neste caso,
// junta o aux com o cmd. O aux é o caminho do comando com a barra.
// O cmd é o comando.
// free (aux); // pq fazemos free do aux? Fazemos free do
// aux pq o aux é uma variável que foi alocada dinamicamente.
// E qd alocamos dinamicamente, temos de dar free.
// if (access(path, R_OK) == 0) // A flag F_Ok ou R_OK ou
// W_Ok ou X_OK diz-nos o tipo de acesso que queremos testar.
// Neste caso pode ser apenas F_OK, uma vez que só
// queremos verificar se o path existe. Não queremos
// verificar se conseguimos ler do mesmo. contudo meti
// R_OK caso o avaliador por alguma razão ache que devemos
// ler do caminho, além de verificarmos que ele
// existe. o que faz o access? tenta aceder ao
// path (que já tem o comando incorporado) com a flag
// que lhe passamos, e retorna -1 se não consegue
// e 0 se consegue. 
// É neste ponto que testamos se efetivamente 
// existe o comando que pretendemos executar 
// naquele sub-caminho. Se existir: 
// return (path); // se o path existir, retorna o path. Se
// não existir, continua a procurar através do 
// envp[i] += ft_strclen(envp[i], ':') +
// (ft_strclen(envp[i], ':') != ft_strlen
// (envp[i]); // esta função é usada para
// apontar para o próximo diretório dentro do
// caminho total, ou seja, aponta para o próximo
// diretório que contém o comando, caso este não
// tenha sido encontrado no diretório anterior. Se
// foi, retorna o path logo, se não foi procura
// no próximo diretório/subcaminho. Para tal,
// faz o i andar até apontar po novo caminho, para
// que no próximo while seja procurada a existência
// do comando nesse novo subcaminho. e por aí fora.
// A função ft_strlen() é uma função que 
// retorna o comprimento total de uma string, 
// enquanto ft_strclen() retorna o comprimento 
// de uma string até um caractere específico.
// A linha de código envp[i] += ft_strclen(envp[i], ':') 
// + (ft_strclen(envp[i], ':') != ft_strlen(envp[i])); 
// está incrementando o ponteiro envp[i] pelo 
// comprimento da string até o caractere ':'.
// A expressão (ft_strclen(envp[i], ':') != ft_
//strlen(envp[i])) verifica se o caractere ':' 
// está presente na string. Se o caractere ':' 
// estiver presente, ft_strclen(envp[i], ':') 
// será menor que ft_strlen(envp[i]), então a 
// expressão será verdadeira (ou 1 em C). Se 
// o caractere ':' não estiver presente, 
// ft_strclen(envp[i], ':') será igual a 
// ft_strlen(envp[i]), então a expressão 
// será falsa (ou 0 em C), e já não 
// entrará no loop. Portanto, se o 
// caractere ':' estiver presente na string, 
// o ponteiro envp[i] será incrementado pelo 
// comprimento da string até o caractere ':' 
// mais 1, efetivamente movendo o ponteiro 
// para o caractere após o ':'. Se o caractere ':' 
// não estiver presente, o ponteiro envp[i] será 
// incrementado pelo comprimento total da string, 
// efetivamente movendo o ponteiro para o final 
// da string. 
// free (path); // pq fazemos free do path? Fazemos free
// do path pq o path é uma variável que foi alocada
// dinamicamente. E qd alocamos dinamicamente, temos
// de dar free.
// return (ft_strdup(cmd)): //Se o acesso NUNCA existir 
// durante o caminho todo, 
// então a função devolve o comando que lhe passamos,
// usando o strdup para devolver um ponteiro,
// possibilitando que lhe demos free. Temos de passar
// um ponteiro pq como já referido qd estamos a
// reconstruir o path para testar se o comando existe
// nos sub-caminhos ou não, não sabemos o tamanho do 
// caminho. Como não sabemos temos de usar malloc 
// (através da strjoin e da substr), alocando memória
// dinamicamente. E qd usamos malloc, temos de
// dar free. Por isso qd não encontramos o comando,
// devolvemos um ponteiro obtido através do strdup.
// Desta forma damos sp free sem que haja qqer memory
// leak durante o processo. E desta forma qd o
// comando não existe em nenhum dos sub-caminhos,
// qd o passamos usando a strdup, o exec_cmd
// já sabe que o comando não existe, dando
// free à memória alocada pelo strdup no exec_cmd, 
// uma vez que esse retorno é o retorno do 
// path = get_path(args[0], envp) na funçao exec_cmd.
// Se não fizesse strdup iria dar free de uma
// alocação de memória que não tinha sido feita, o
// que iria originar um erro de memória. Erro esse
// que era detetado pelo valgrind e que é evitado
// através do strdup.
// Bónus do pipex
// No bónus do pipex, usamos o GNL para ler o input do heredoc,
// e guardá-lo num ficheiro temporário, o qual vamos copiar e ler
// para o stdin.
// Na prática
// CAT na prática: Exemplo de execução do pipex na shell usando o
// script/comando:  cat < examgpt.sh | grep bash
// Nota: na shell cat > aqui cria um ficheiro chamado aqui.
// O cat simples serve para ler do stdin (0) e escreve no stdout (1).
// O cat lê o input do stdin e escreve no ficheiro aqui (o ficheiro output).
// É importante ele escrever no stdout (1) por causa do pipe.
// pq a comunicação entre os comandos vai ser feita pelos fd do pipe.
// Mas escreve apenas na primeira linha. Apaga o resto. E se
// o usarmos somente como cat > aqui sem escrevermos nada de seguida,
// ele fica à espera que escrevamos algo. Se não escrevemos nada,
// ele fica à espera que pressionemos ctrl + d para terminar o input.
// e o ficheiro aqui fica sem nada dentro.
// Se já existir um ficheiro com algo dentro, ele lê e apaga.
// O cat >> aqui acrescenta o que introduzimos no final do
// do ficheiro criado. No caso, no ficheiro aqui.
// só cat lê do file descriptor 0 (input) e escreve
// no file descriptor 1 (output). Não cria ficheiro nenhum.

#include "pipex.h"

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

	if (ac == 5)
	{
		fd_file[0] = open(av[1], O_RDONLY, 0644);
		dup2(fd_file[0], STDIN_FILENO);
		redirect(av[1], av[2], envp);
		fd_file[1] = open(av[ac - 1], O_RDWR | O_TRUNC | O_CREAT, 0644);
		check(fd_file[1], av[ac - 1]);
		dup2(fd_file[1], STDOUT_FILENO);
		exec_cmd(av[3], envp);
	}
	else
		ft_putstr_fd("Error: invalid number of arguments\n", STDERR_FILENO);
	return (1);
}
