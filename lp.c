/*
////////////////////////////////////////////////////////////////////////
-----------------------------------------------------------------------
                             BIBLIOTECAS
-----------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

/* Bibliotecas-padrão */
#include <math.h>
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

/* Estrutura para o HASH */
struct st {
    int N, M;
    Item *item;
    void *NULLitem;
    void  (*free_item) (Item);
    void *(*key) (Key);
    int   (*eq)  (Key, Key);
    int   (*less)(Key, Key);
};

#define null(A) ((st->item[A]) == (st->NULLitem))

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

/* int hash(char *v, int M) */
/* {  */
    /* int h = 0, a = 127; #<{(| a = 128; |)}># */
    /* for (; *v != '\0' && *v != ' ' && *v != ']'; v++) */
    /* { putchar(*v); h = (a*h + *v) % M; } */
    /* return h; */
/* } */

/***********************************************************************
 * Outra funcao de hashing; devida a Knuth (aparece no Stanford GraphBase) 
 * http://www-cs-staff.Stanford.EDU/~knuth/sgb.html
 ***********************************************************************/

#define HASH_MULT 314159 /* random multiplier */
#define HASH_PRIME 516595003 /* the 27182818th prime; it's less than $2^{29}$ */

long hash_Knuth(char *t)
{
    register long h;
    for (h=0; *t; t++) 
    {
        h+=(h^(h>>1))+HASH_MULT*(unsigned char)*t;
        while (h>=HASH_PRIME) h-=HASH_PRIME;
    }
    return h;
}

#define hash(v,M) (hash_Knuth(v)%M)

/*
////////////////////////////////////////////////////////////////////////
-----------------------------------------------------------------------
                      IMPLEMENTAÇÃO DA BIBLIOTECA
-----------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

/* Inicialização */
static ST init(ST st, int M)
{ 
    int i; st->N = 0;
    st->item = malloc(M * sizeof(Item)); 
    for (i = 0; i < M; i++) st->item[i] = st->NULLitem;
    return st;
}

ST STinit(
    Item NULLitem, 
    void (*free_item)(Item),
    Key  (*key)(Item),
    int  (*eq)(Key, Key),
    int  (*less)(Key, Key) )
{
    ST new = (ST) malloc(sizeof(*new));
    new->free_item = free_item; new->key = key;
    new->eq = eq; new->less = less; 
    new->NULLitem = NULLitem;
    new->M = 50; new = init(new, new->M);
    return new;
}

/* Contagem */
int STcount(ST st) { return st->N; }

/* Inserção (com expansão dinâmica) */
static void expand(ST st);

void STinsert(ST st, Item item)
{ 
    Key v = st->key(item);
    int i = hash(v, st->M);
    while (!null(i)) i = (i+1) % st->M;
    st->item[i] = item;
    if (st->N++ > st->M/2) expand(st);
}

static void expand(ST st)
{ 
    int i, MM = st->M; Item *t = st->item;
    st->M = prime_m(2 * st->M);
    st = init(st, st->M);
    for (i = 0; i < MM; i++) 
        if (t[i] != st->NULLitem)
            STinsert(st, t[i]); 
    free(t);
}

/* Busca */
Item STsearch(ST st, Key v)
{ 
    int i = hash(v, st->M);
    while (!null(i))
        if (st->eq(v, st->key(st->item[i])))
            return st->item[i];
        else i = (i+1) % st->M;
    return st->NULLitem;
}

/***********************************************************************
 * Programa 14.5
 ***********************************************************************/

/* Remoção */
void STdelete(ST st, Key k)
{   
    long j, i = hash(k, st->M); Item v;
    while (!null(i))
        if (st->eq(k, st->key(st->item[i]))) break; 
        else i = (i+1) % st->M;
    if (null(i)) return;
    st->item[i] = st->NULLitem; st->N--;
    for (j = i+1; !null(j); j = (j+1) % st->M, st->N--)
    { v = st->item[j]; st->item[j] = st->NULLitem; STinsert(st, v); }
}

/* Liberação de memória */
void STfree(ST st)
    { /* st->free_item(st->item); */ free(st); }

/* Ordenação */
static ST global_st;
static int cmp(const void *a, const void *b)
{
    if(global_st->less((Key) a, (Key) b)) return -1;
    if(global_st->eq((Key) a, (Key) b)) return 0;
    return 1;
}

void STsort(ST st, void(*visit)(Item))
{
    int i = 0, j = 0, M = st->M; 
    Key *keys = (Key *) malloc(st->N * sizeof(*keys));
    for(i = 0; i < M; i++)
    {
        if(st->item[i] != st->NULLitem) 
            keys[j++] = st->key(st->item[i]);
    }
    
    global_st = st;
    qsort((void *) keys, st->N, sizeof(*keys), cmp);
}
