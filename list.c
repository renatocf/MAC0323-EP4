/***********************************************/
/**  MAC 0323  -  Estrutura de Dados          **/
/**  IME-USP   -  Primeiro  Semestre de 2013  **/
/**  Turma 45  -  Yoshiharu Kohayakawa        **/
/**                                           **/
/**  Terceiro  Exercício-Programa             **/
/**  Arquivo:  list.c                         **/
/**                                           **/
/**  Renato Cordeiro Ferreira        7990933  **/
/***********************************************/ 

/*
////////////////////////////////////////////////////////////////////////
-----------------------------------------------------------------------
                      BIBLIOTECAS E ESTRUTURAS
-----------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/
/* Bibliotecas */
#include <stdlib.h>
#include <stdio.h>
#include "list.h"

/* Estruturas */
struct lnode { void *item; Link next; };
struct list  { Link head; Link last; };

/*
////////////////////////////////////////////////////////////////////////
-----------------------------------------------------------------------
                              FUNÇÕES
-----------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

Link list_first(List list)
    { return list->head->next; }

Link list_last(List list)
    { return list->last; }

void *list_item(Link node)
    { return node->item; }

int list_empty(List list)
    { return list->head == list->last; }

Link list_next(Link node)
{
    if(node == NULL) return NULL;
    return node->next;
}

List list_init()
{
    List new = (List) malloc(sizeof(*new));
    Link head = (Link) malloc(sizeof(*head));
    
    new->head = head;
    new->last = head;
    new->last->next = NULL;
    
    return new;
}

void list_insert(List list, void *item)
{
    Link new = (Link) malloc(sizeof(*new));
    Link last = list->last;
    
    last->next = new;
    new->next = NULL;
    list->last = new;
    
    new->item = item;
}

void *list_remove(List list, Link node)
{
    void *dead; Link aux = list->head;
    while(aux->next != node) aux = aux->next;
    
    if(node == list->last) list->last = aux;
    aux->next = node->next;

    dead = node->item; free(node); 
    return dead;
}

void list_free(List list)
{
    while(!list_empty(list)) list_remove(list, list->last);
    free(list->head); free(list);
}

void list_select(List list, void(*visit)(void *))
{
    Link aux = list->head;
    while(aux != list_last(list)) { aux = aux->next; visit(aux->item); }
}

int list_search(List list, void *item, int(*cmp)(void *,void *))
{
    Link aux = list->head;
    while(aux != list_last(list)) 
        { aux = aux->next; if(cmp(item, aux->item) == 0) return 1; }
    return 0;
}
