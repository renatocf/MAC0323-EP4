/***********************************************/
/**  MAC 0323  -  Estrutura de Dados          **/
/**  IME-USP   -  Primeiro  Semestre de 2013  **/
/**  Turma 45  -  Yoshiharu Kohayakawa        **/
/**                                           **/
/**  Terceiro  Exercício-Programa             **/
/**  Arquivo:  lemma.c                        **/
/**                                           **/
/**  Renato Cordeiro Ferreira        7990933  **/
/***********************************************/ 

/*
////////////////////////////////////////////////////////////////////////
-----------------------------------------------------------------------
                               BIBLIOTECAS 
-----------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

/* Bibliotecas-padrão */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Bibliotecas internas */
#include "ST.h"
#include "list.h"
#include "lemma.h"

/*
////////////////////////////////////////////////////////////////////////
-----------------------------------------------------------------------
                           MACROS/ESTRUTURAS
-----------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

#define SEPARATOR     ' '
#define NULLitem      NULL
#define BUF_CMP_SIZE  500
#define PRT_SEPARATOR '\n'

/* Definição de estrutura para o lema */
struct lemma { char *lemma; List words; };

/* Tabela de símbolos dos lemas */
ST lemmas;

/*
////////////////////////////////////////////////////////////////////////
-----------------------------------------------------------------------
                  FUNÇÕES PARA A TABELA DE SÍMBOLOS
-----------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

static Key key(Item l)
    { Lemma aux = (Lemma) l; return (Key) aux->lemma; }

static int eq(Key lemma1, Key lemma2)
{    
    char *l1 = (char *) lemma1, *l2 = (char *) lemma2; 
    char L1[BUF_CMP_SIZE], L2[BUF_CMP_SIZE]; int s = 0;
    for(s = 0; l1[s] != ' '; s++) L1[s] = l1[s]; L1[s] = '\0';
    for(s = 0; l2[s] != ' '; s++) L2[s] = l2[s]; L2[s] = '\0';
    return strcmp(L1, L2) == 0;
}

static int less(Key lemma1, Key lemma2)
{ 
    char *l1 = (char *) lemma1, *l2 = (char *) lemma2; 
    char L1[BUF_CMP_SIZE], L2[BUF_CMP_SIZE]; int s = 0;
    for(s = 0; l1[s] != ' '; s++) L1[s] = l1[s]; L1[s] = '\0';
    for(s = 0; l2[s] != ' '; s++) L2[s] = l2[s]; L2[s] = '\0';
    return strcmp(L1, L2) < 0;
}

/*
////////////////////////////////////////////////////////////////////////
-----------------------------------------------------------------------
                     FUNÇÕES INTERNAS AUXILIARES 
-----------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

/* Libera as estruturas na tabela de símbolos */
static void lemma_free(void *lemma)
{
    Lemma l = (Lemma) lemma;
    list_free(l->words); free(l);
}

/* Imprime uma palavra buscando seu separador característico */
static void print_word(void *phrase)
{
    char *init = (char *) phrase; int s = 0;
    printf("\n-------------------------------------------------------\n");
    for(s = 0; init[s] != ' '; s++) putchar(init[s]);
}

/* Ponteiros para funções que auxiliam na transformação
 * do protótipo 'char *' (que esconde as implementações 
 * de lista/árvore para o usuário de lemma.c) para um
 * protótipo 'void *' (requerido pela lista) */
static void (*word_visit)(char *);
static void v(void *var) { word_visit((char *) var); }

/* Função para impressão das palavras dos lemas */
static void print_word2(void *word)
{
    int s = 0; char *w = (char *) word;
    while(w[s] != ' ') putchar(w[s++]); putchar(' ');
}

/* Função para impressão das palavras dos lemas */
static void print_lemma_word(void *lemma)
{ 
    int s; Lemma l = (Lemma) lemma;
    for(s = 0; l->lemma[s] != ']'; s++) putchar(l->lemma[s]);
    printf(": "); list_select(l->words, print_word2); 
    putchar(PRT_SEPARATOR);
}

/* Função para imprimir um único lema */
static void print_lemmas(void *lemma)
{
    int s; Lemma l = (Lemma) lemma;
    for(s = 0; l->lemma[s] != ']'; s++) putchar(l->lemma[s]);
    putchar(PRT_SEPARATOR);
}

/* Função para comparação entre lemas */
static int cmp(void *a, void *b)
{ 
    char *l1 = (char *) a, *l2 = (char *) b; 
    char L1[BUF_CMP_SIZE], L2[BUF_CMP_SIZE]; int s = 0;
    for(s = 0; l1[s] != ' '; s++) L1[s] = l1[s]; L1[s] = '\0';
    for(s = 0; l2[s] != ' '; s++) L2[s] = l2[s]; L2[s] = '\0';
    return strcmp(L1, L2);
}

/*
////////////////////////////////////////////////////////////////////////
-----------------------------------------------------------------------
                  FUNÇÕES DE MANIPULAÇÃO DA TABELA
-----------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

void lemma_table_init()
    { lemmas = STinit(NULLitem, lemma_free, key, eq, less); }

void lemma_table_free()
    { STfree(lemmas); }

Lemma lemma_table_get(char *lemma)
    { return (Lemma) STsearch(lemmas, key(lemma)); }

void lemma_table_insert(char *lemma, char *word)
{
    Lemma query;
    if((query = STsearch(lemmas, lemma)) == NULLitem)
    {
        /* Cria nova palavra a ser inserida na tabela */
        query = (Lemma) malloc(sizeof(*query));
        query->lemma = lemma;
        
        /* Insere palavra cujo lema 
         * é a chave desta estrutura */
        query->words = list_init();
        list_insert(query->words, word);
        
        STinsert(lemmas, query);
    }
    else if(!list_search(query->words, word, cmp))
        list_insert(query->words, word);
}

/*
////////////////////////////////////////////////////////////////////////
-----------------------------------------------------------------------
                  FUNÇÕES DE MANIPULAÇÃO DOS LEMAS  
-----------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

/* Dado um lema, imprime suas palavras */
void lemma_print_words(Lemma lemma)
    { list_select(lemma->words, print_word); }

/* Realiza ação sobre as palavras dos lemas */
void lemma_list_words(char *lemma, void(*visit)(char *))
{
    Lemma l = (Lemma) STsearch(lemmas, lemma);
    word_visit = visit; if(l == NULL) return; 
    list_select(l->words, v);
}

/* Imprime todos os lemas e todos os lemas com palavras */
void lemma_print_lemmas() { STsort(lemmas, print_lemmas); }
void lemma_print_lemma_word() { STsort(lemmas, print_lemma_word); }

/* Contagem do total de lemas */
int lemma_total_lemmas() { return STcount(lemmas); }
