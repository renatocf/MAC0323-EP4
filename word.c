/***********************************************/
/**  MAC 0323  -  Estrutura de Dados          **/
/**  IME-USP   -  Primeiro  Semestre de 2013  **/
/**  Turma 45  -  Yoshiharu Kohayakawa        **/
/**                                           **/
/**  Terceiro  Exercício-Programa             **/
/**  Arquivo:  word.c                         **/
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
#include "Item.h"
#include "list.h"
#include "word.h"

/* Tabela de símbolos */
#if defined(W_LP)
    #include "lp.h"
#elif defined(W_ENC)
    #include "enc.h"
#endif

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

/* Estrutura para informações sobre sentença */
typedef struct sent *Sent;
struct sent
{
    char *identifier;
    char *sentence;
    char *annotated;
};

/* Estrutura para informações sobre palavra */
struct word
{
    char *word;
    char *lemma;
    List sentences;
};

/* Tabela de símbolos de palavras */
ST words;

/*
////////////////////////////////////////////////////////////////////////
-----------------------------------------------------------------------
                  FUNÇÕES PARA A TABELA DE SÍMBOLOS
-----------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

static Key key(Item w)
    { Word aux = (Word) w; return (Key) aux->word; }

static int eq(Key w1, Key w2)
{
    char W1[BUF_CMP_SIZE], W2[BUF_CMP_SIZE]; int s = 0;
    for(s = 0; w1[s] != ' ' && w1[s] != '\0'; s++) 
        W1[s] = w1[s]; W1[s] = '\0';
    for(s = 0; w2[s] != ' ' && w2[s] != '\0'; s++) 
        W2[s] = w2[s]; W2[s] = '\0';
    return strcmp(W1, W2) == 0;
}

static int less(Key w1, Key w2)
{ 
    char W1[BUF_CMP_SIZE], W2[BUF_CMP_SIZE]; int s = 0;
    for(s = 0; w1[s] != ' ' && w1[s] != '\0'; s++) 
        W1[s] = w1[s]; W1[s] = '\0';
    for(s = 0; w2[s] != ' ' && w2[s] != '\0'; s++) 
        W2[s] = w2[s]; W2[s] = '\0';
    return strcmp(W1, W2) < 0;
}

/*
////////////////////////////////////////////////////////////////////////
-----------------------------------------------------------------------
                            FUNÇÕES INTERNAS
-----------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

/* Libera a estrutura da sentença */
static void sent_free(void *sent) { free(sent); }

/* Libera a estrutura relacionada a uma palavra */
static void word_free(Item word)
{
    Word w = (Word) word; 
    list_select(w->sentences, sent_free);
    list_free(w->sentences); free(w);
}

/* Imprime sentenças com um nível de verbosidade 
 * característico (protótipo a ser usado na lista/fila */
static int verbosity;
static void print_sentence(void *phrase)
{
    Sent init = (Sent) phrase; 
    int s = 0; size_t size = 6 * sizeof(char);
    printf("\n-------------------------------------------------------\n");
    
    if(verbosity) for(s = 0; init->identifier[s] != '\n'; s++) 
        putchar(init->identifier[s]);
    
    for(s = 0; init->sentence[s] != '[' 
        && strncmp(&init->sentence[s], "[Text=", size) != 0; s++) 
        putchar(init->sentence[s]);
    
    if(verbosity > 1) for(s = 0; init->annotated[s] != '\n'; s++) 
        putchar(init->annotated[s]); 
    
    putchar('\n');
}

/* Imprime todos os tokens da sentença 
 * (todos os termos da tabela de símbolos) */
static void print_tokens(void *word)
{
    int s; Word w = (Word) word;
    for(s = 0; w->word[s] != ' '; s++) putchar(w->word[s]); 
    putchar(PRT_SEPARATOR);
}

/* Imprime todas as palavras da sentença
 * (termos formados pelas letras a-z e/ou A-Z */
static void print_words(void *word)
{
    int s; Word w = (Word) word;
    for(s = 0; w->word[s] != ' '; s++)
        if(w->word[s] < 'A' || (w->word[s] > 'Z' && w->word[s] < 'a')
        || w->word[s] > 'z') return;
    printf("%.*s", s, w->word); putchar(PRT_SEPARATOR);
}

/* Contador para número de palavras e checador
 * se um token é ou não uma palavra */
static int n_words = 0;
static void count_words(void *word)
{
    int s; Word w = (Word) word; 
    for(s = 0; w->word[s] != ' '; s++)
        if(w->word[s] < 'A' || (w->word[s] > 'Z' && w->word[s] < 'a')
        || w->word[s] > 'z') return;
    n_words++;
}

/*
////////////////////////////////////////////////////////////////////////
-----------------------------------------------------------------------
                  FUNÇÕES DE MANIPULAÇÃO DA TABELA
-----------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

void word_table_init()
    { words = STinit(NULLitem, word_free, key, eq, less); }

void word_table_free()
    { STfree(words); }

Word word_table_get(char *query)
    { return (Word) STsearch(words, query); }

void word_table_insert(char *word, char *lemma, 
    char *identifier, char *sentence, char *annotated)
{
    Word query;
    
    Sent s = (Sent) malloc(sizeof(*s));
    s->identifier = identifier;
    s->sentence = sentence;
    s->annotated = annotated;
    
    if((query = STsearch(words, word)) == NULLitem)
    {
        /* Cria nova palavra a ser inserida na tabela */
        query = (Word) malloc(sizeof(*query));
        query->word = word; query->lemma = lemma;
        
        /* Insere sentença */
        query->sentences = list_init();
        list_insert(query->sentences, (void *) s);
        
        STinsert(words, query);
    }
    else 
    {
        /* Separa se o lema já existe para a frase */
        int current_n, last_n;
        Sent last = list_item(list_last(query->sentences));
        char *last_id = last->identifier;
        sscanf(identifier, "Sentence #%d", &current_n);
        sscanf(last_id, "Sentence #%d", &last_n);
        
        /* Insere a nova sentença na lista 
         * de um lema já existente */
        if(current_n != last_n)
            list_insert(query->sentences, s);
    }
}

/*
////////////////////////////////////////////////////////////////////////
-----------------------------------------------------------------------
                FUNÇÕES DE MANIPULAÇÃO DE PALAVRAS
-----------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

/* Imprime todas as sentenças de uma palavra */
void word_print_sentences(Word word, int verb)
{ 
    if(word != NULL) 
    {
        verbosity = verb;
        list_select(word->sentences, print_sentence); 
    }
}

/* Para uma dada palavra, pega seu lema */
char *word_lemma(Word word)
    { if(word != NULL) return word->lemma; else return NULL; }

/* Funções para impressão de palavras/tolkens */
void word_print_words() { STsort(words, print_words); }
void word_print_tokens() { STsort(words, print_tokens); }

/* Funções para contagem de paalvras/tolkens */
int word_total_words()  
    { n_words = 0; STsort(words, count_words); return n_words; }
int word_total_tokens() 
    { return STcount(words); }
