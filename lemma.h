/***********************************************/
/**  MAC 0323  -  Estrutura de Dados          **/
/**  IME-USP   -  Primeiro  Semestre de 2013  **/
/**  Turma 45  -  Yoshiharu Kohayakawa        **/
/**                                           **/
/**  Terceiro  Exercício-Programa             **/
/**  Arquivo:  lemma.h                        **/
/**                                           **/
/**  Renato Cordeiro Ferreira        7990933  **/
/***********************************************/ 

#ifndef H_LEMMA_DEFINED
#define H_LEMMA_DEFINED

/* Definição de palavra */
typedef struct lemma *Lemma;

/* Funções para manipulação da tabela */
Lemma lemma_table_get    (char *lemma);
void  lemma_table_init   (void);
void  lemma_table_free   (void);
void  lemma_table_insert (char *lemma, char *word);

/* Funções para manipulação da palavra */
void  lemma_list_words         (char *lemma, void(*visit)(char *));
void  lemma_print_words        (Lemma);
int   lemma_total_lemmas       (void);
void  lemma_print_lemmas       (void);
void  lemma_print_lemma_word   (void);

#endif
