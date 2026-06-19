*Este projeto foi criado como parte do currículo da 42 por vde-alme.*

# Get Next Line

O **Get Next Line (GNL)** é um projeto da 42 que consiste em implementar uma função capaz de devolver, a cada chamada, uma linha lida de um *file descriptor* (FD) — terminada pelo carácter de nova linha (`\n`), exceto quando o ficheiro termina sem `\n`.

O desafio central não é ler um ficheiro, mas sim ler **o mínimo possível em cada chamada**, sem usar `lseek()` e sem variáveis globais, preservando o estado da leitura entre chamadas através de uma variável estática.

---

## Descrição

A função tem o seguinte protótipo:

```c
char *get_next_line(int fd);
```

Chamadas sucessivas a `get_next_line()` devolvem o ficheiro apontado por `fd`, uma linha a cada vez. Quando não há mais nada para ler, ou ocorre um erro, a função devolve `NULL`.

O algoritmo é baseado numa **stash** (resíduo): a cada chamada, lê-se em blocos de `BUFFER_SIZE` bytes e vai-se acumulando o conteúdo numa string alocada na *heap*, até encontrar um `\n` ou atingir o fim do ficheiro (EOF). A linha é depois extraída dessa stash, e o que sobra é guardado — através de uma variável `static` — para ser reaproveitado na próxima chamada.

## Funcionalidades

- **Leitura mínima e eficiente:** cada chamada só lê o necessário, em blocos de `BUFFER_SIZE` (definido em tempo de compilação), nunca relendo o ficheiro desde o início.
- **Suporte a qualquer `BUFFER_SIZE`:** testado com valores extremos (`1`, `42`, `9999`, `10000000`), incluindo ficheiros maiores e menores que o buffer.
- **Funciona com qualquer FD:** ficheiros regulares e a entrada padrão (`stdin`).
- **Sem fugas de memória (*leak-free*):** todo o conteúdo intermédio é libertado corretamente, mesmo nos casos de erro ou EOF.
- **Bónus — múltiplos *file descriptors* em simultâneo:** usando uma única variável estática (um array `stash[FD_MAX]`), é possível intercalar chamadas a `get_next_line()` para FDs diferentes sem perder o estado de leitura de cada um.

---

## Instruções

### Compilação

**Parte obrigatória**

```bash
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line.c get_next_line_utils.c main.c -o gnl
./gnl
```

> O projeto também deve compilar sem a flag `-D BUFFER_SIZE`, caso em que assume o valor padrão definido no `.h` (`42`).

**Parte bónus** (múltiplos FDs)

```bash
cc -Wall -Wextra -Werror -D BUFFER_SIZE=42 get_next_line_bonus.c get_next_line_utils_bonus.c main.c -o gnl_bonus
./gnl_bonus
```

### Estrutura de ficheiros

```
get_next_line/
├── get_next_line.h            # protótipos + BUFFER_SIZE (mandatório)
├── get_next_line.c            # lógica principal (stash única)
├── get_next_line_utils.c      # ft_strlen, ft_strchr, ft_strjoin
├── get_next_line_bonus.h      # protótipos + BUFFER_SIZE + FD_MAX (bónus)
├── get_next_line_bonus.c      # lógica principal (stash por FD)
└── get_next_line_utils_bonus.c
```

### Exemplo de uso

```c
#include "get_next_line.h"
#include <fcntl.h>
#include <stdio.h>

int main(void)
{
    int     fd;
    char    *line;

    fd = open("ficheiro.txt", O_RDONLY);
    while ((line = get_next_line(fd)) != NULL)
    {
        printf("%s", line);
        free(line);
    }
    close(fd);
    return (0);
}
```

---

## Algoritmo e justificativa técnica

A implementação divide-se em três funções estáticas, chamadas por `get_next_line()`:

```
get_next_line(fd)
 ├─ read_to_stash(fd, stash)   → lê em blocos de BUFFER_SIZE até existir '\n' na stash, ou EOF
 ├─ get_line(stash)            → extrai a 1ª linha completa da stash (até e incluindo o '\n')
 └─ clean_stash(stash)         → descarta a linha já extraída e devolve o resto, para a próxima chamada
```

**`read_to_stash`** só entra no `while` se ainda não existir `\n` na stash atual — ou seja, se já houver uma linha completa pendente de uma leitura anterior, não se faz nenhuma chamada a `read()`. Isto garante que o número de bytes lidos por chamada é o mínimo necessário, independente do `BUFFER_SIZE` escolhido.

**`get_line`** percorre a stash até ao primeiro `\n` (ou até ao fim, se o ficheiro terminar sem nova linha) e copia esse intervalo para uma nova string, que é devolvida ao utilizador.

**`clean_stash`** localiza esse mesmo `\n` e devolve apenas o que vem depois dele. Se não houver mais conteúdo (string vazia ou só `\0`), liberta a stash e devolve `NULL` — sinalizando que, na próxima chamada, se deve começar com uma stash "limpa".

### Por que uma variável estática?

Sem persistir o resíduo da leitura entre chamadas, seria necessário reler o ficheiro do início a cada chamada (usando `lseek()`, que é proibido pelo enunciado) ou carregar o ficheiro inteiro em memória de uma só vez (o que viola o requisito de ler o mínimo possível e não escala para ficheiros grandes). A variável `static` resolve isto guardando, entre chamadas, apenas o excedente da última leitura — normalmente uma fração de `BUFFER_SIZE`.

### Por que funciona com qualquer `BUFFER_SIZE`

O `BUFFER_SIZE` controla apenas o tamanho de **cada chamada a `read()`**, não o tamanho máximo de uma linha. Como o conteúdo lido é sempre concatenado (`ft_strjoin`) à stash já acumulada, uma linha pode crescer além de um único `BUFFER_SIZE` sem problema — apenas implica mais iterações do `while` em `read_to_stash` quando `BUFFER_SIZE` é pequeno (ex.: `1`), ou poucas iterações quando é grande (ex.: `10000000`).

### Extensão para múltiplos FDs (bónus)

A versão bónus substitui a variável `static char *stash;` por `static char *stash[FD_MAX];`. Continua a ser **uma única variável estática** (um array), mas agora cada posição guarda o resíduo de leitura de um FD diferente, indexado pelo próprio valor do `fd`. Isto permite alternar chamadas entre, por exemplo, `fd 3`, `fd 4` e `fd 5`, sem que a leitura de um interfira na do outro.

---

## Recursos

### Documentação e referências

- [man 2 read](https://man7.org/linux/man-pages/man2/read.2.html) — comportamento da syscall `read()`, incluindo o caso de retorno `0` (EOF)
- [man 3 malloc / man 3 free](https://man7.org/linux/man-pages/man3/malloc.3.html) — gestão de memória dinâmica
- [42 Docs — get_next_line](https://harm-smits.github.io/42docs/projects/get_next_line) — explicação da abordagem por *stash*
- *The C Programming Language* — Kernighan & Ritchie, Cap. 5 (Ponteiros e Arrays) e Cap. 7 (I/O)

### Uso de IA neste projeto

A IA (Claude) foi utilizada apenas na **redação e organização deste README** — estruturação das secções exigidas pelo enunciado, formatação em Markdown e revisão da explicação técnica do algoritmo a partir do código já implementado. A lógica das funções (`read_to_stash`, `get_line`, `clean_stash` e a extensão para múltiplos FDs) foi desenvolvida e implementada pelo aluno.
