/***********************************************/
/**  MAC 0323  -  Estrutura de Dados          **/
/**  IME-USP   -  Primeiro  Semestre de 2013  **/
/**  Turma 45  -  Yoshiharu Kohayakawa        **/
/**                                           **/
/**  Terceiro  Exercício-Programa             **/
/**  Arquivo:  list.h                         **/
/**                                           **/
/**  Renato Cordeiro Ferreira        7990933  **/
/***********************************************/ 

#ifndef H_LIST_DEFINED
#define H_LIST_DEFINED

/* Lista e link para nódulo de lista */
typedef struct lnode *Link;
typedef struct list *List;

/* Funções para uso do cliente */
List  list_init   (void);
Link  list_last   (List);
Link  list_next   (Link);
void  list_free   (List);
void *list_item   (Link);
Link  list_first  (List);
int   list_empty  (List);
void *list_remove (List, Link);
void  list_insert (List, void *);
void  list_select (List, void(*visit)(void *));
int   list_search (List, void *item, int(*cmp)(void *,void *));

#endif
