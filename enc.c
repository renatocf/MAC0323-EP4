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

/* Bibliotecas internas */
#include "ST.h"
#include "Item.h"

/*
////////////////////////////////////////////////////////////////////////
-----------------------------------------------------------------------
                          MACROS E ESTRUTURAS
-----------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

/* Definição de nódulo para lista */
typedef struct STnode *link;
struct STnode { Item item; link next; };

/* Cauda padrão para todas as listas */
static link z;

/* Estrutura para o HASH */
struct st {
    int N, M;
    link *heads;
    void *NULLitem;
    Key   (*key) (Item);
    int   (*eq)  (Key, Key);
    int   (*less)(Key, Key);
    void  (*free_item)(Item);
};

/*
////////////////////////////////////////////////////////////////////////
-----------------------------------------------------------------------
                              NÚMEROS PRIMOS 
-----------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

/* Tabela de números primos */
int p[] = {
    251,
    509,
    1021,
    2039,
    4093,
    8191,
    16381,
    32749,
    65521,
    131071,
    262139,
    524287,
    1048573,
    2097143,
    4194301,
    8388593,
    16777213,
    33554393,
    67108859,
    134217689,
    268435399,
    536870909,
    1073741789,
    2147483647
};

int prime_m(int m)
{
    int x;
    x = log(m)/log(2.0) + 1;
    if (x < 8)
        return p[0];
    else 
        return p[x - 8];
}

/*
////////////////////////////////////////////////////////////////////////
-----------------------------------------------------------------------
                            FUNÇÃO DE HASH
-----------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

/* Esta função de hash considera os caracteres como
 * números na base 127. A soma dos valores dos caracteres
 * cria um número inteiro que é usado como chave para o hash */
int hash(char *v, int M)
{ 
    int h = 0, a = 127; /* a = 128; */
    for (; *v != '\0' && *v != ' ' && *v != ']'; v++)
        h = (a*h + *v) % M;
    return h;
}

/*
////////////////////////////////////////////////////////////////////////
-----------------------------------------------------------------------
                      IMPLEMENTAÇÃO DA BIBLIOTECA
-----------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

/* Cria novo nódulo de lista */
static link NEW(Item item, link next)
{
    link x = malloc(sizeof *x);
    x->item = item; x->next = next;
    return x;
}

/* Inicialização */
static ST init(ST st, int M)
{ 
    int i; st->N = 0;
    st->heads = malloc(M * sizeof(link));
    for (i = 0; i < M; i++) heads[i] = z; 
    return st;
}

ST STinit(
    Item NULLitem, 
    void (*free_item)(Item),
    Key  (*key)(Item),
    int  (*eq)(Key, Key),
    int  (*less)(Key, Key) )
{
    /* Inicializa nova tabela de símbolos */
    ST new = (ST) malloc(sizeof(*new));
    
    /* Inicializa 'z' (se não feito antes) */
    if(z == NULL) z = NEW(NULLitem, NULL);
    
    /* Inicializa itens referentes às tabelas */
    new->NULLitem = NULLitem; new->free_item = free_item; 
    new->eq = eq; new->less = less; new->key = key;
    new->M = 251; new = init(new, new->M);
    return new;
}

/* Contagem */
int STcount(ST st) { return st->N; }

/* Inserção (com expansão/retração dinâmica) */
static void resize(ST st, float factor);

void STinsert(ST st, Item item)
{ 
    Key v = st->key(item);
    int i = hash(v, st->M);
    heads[i] = NEW(item, heads[i]); N++; 
    if (st->N++ > st->M/2) resize(st, 2);
}

static void resize(ST st, float factor)
{ 
    int i, MM = st->M; link aux, *t = st->heads;
    st->M = prime_m(factor * st->M);
    st = init(st, st->M);
    for (i = 0; i < MM; i++) 
        for(aux = t[i]; aux != z; aux = aux->next) 
            STinsert(st, aux->item); 
    free(t);
}

/* Busca */
Item searchR(link t, Key v)
{ 
    if (t == z) return NULLitem;
    if (eq(key(t->item), v)) return t->item;
    return searchR(t->next, v);
}

Item STsearch(ST st, Key v)
{ return searchR(st->heads[hash(v, st->M)], v); }

void STsort(ST st, void(*visit)(Item))
{
    int i; link t;
    for(i = 0; i < st->M; i++) 
    {
        if(st->heads[i] != z)
            for(t = st->heads[i]; t != z; t = t->next) 
                visit(t->item);
    }
}
