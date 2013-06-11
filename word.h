/***********************************************/
/**  MAC 0323  -  Estrutura de Dados          **/
/**  IME-USP   -  Primeiro  Semestre de 2013  **/
/**  Turma 45  -  Yoshiharu Kohayakawa        **/
/**                                           **/
/**  Terceiro  Exercício-Programa             **/
/**  Arquivo:  word.h                         **/
/**                                           **/
/**  Renato Cordeiro Ferreira        7990933  **/
/***********************************************/ 

#ifndef H_WORD_DEFINED
#define H_WORD_DEFINED

/* Definição de palavra */
typedef struct word *Word;

/* Funções para manipulação da tabela */
Word  word_table_get    (char *word);
void  word_table_init   (void);
void  word_table_free   (void);
void  word_table_insert (char *word, 
                         char *lemma, 
                         char *identif, 
                         char *sentence, 
                         char *annotated);

/* Funções para manipulação da palavra */
char *word_lemma               (Word);
int   word_total_words         (void);
void  word_print_words         (void);
int   word_total_tokens        (void);
void  word_print_tokens        (void);
void  word_print_sentences     (Word, int verbosity);

#endif
