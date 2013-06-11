/***********************************************/
/**  MAC 0323  -  Estrutura de Dados          **/
/**  IME-USP   -  Primeiro  Semestre de 2013  **/
/**  Turma 45  -  Yoshiharu Kohayakawa        **/
/**                                           **/
/**  Terceiro  Exercício-Programa             **/
/**  Arquivo:  getline.h                      **/
/**                                           **/
/**  Renato Cordeiro Ferreira        7990933  **/
/***********************************************/ 

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define INITIAL_BUF_SIZE 100

char *getline(FILE *file, char separator);

/* Fazemos um define que representa um tamanho de código
 * passando para ele um valor. Ele substituirá a entrada.
 * Isso tem uma VANTAGEM sobre criar uma função, pois
 * evita o tempo de chamada de uma delas. */
#define out_of_memory(x) \
    { fprintf(stderr, "Erro: Acabou a memória!\n"); exit(x); }
