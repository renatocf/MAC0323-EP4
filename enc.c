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
#include "enc.h"
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

/* Contador para número de tabelas de símbolo */
static int STn = 0;

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
    int i; st->N = 0; st->M = M;
    st->heads = malloc(M * sizeof(link));
    for (i = 0; i < M; i++) st->heads[i] = z; 
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
    ST new = (ST) malloc(sizeof(*new)); STn++;
    
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
    st->heads[i] = NEW(item, st->heads[i]);
    if (st->N++ > st->M/2) resize(st, 2);
}

static void resize(ST st, float factor)
{ 
    int i, MM = st->M; link aux, *t = st->heads;
    st->M = prime_m(factor * st->M);
    st = init(st, st->M);
    for (i = 0; i < MM; i++) 
        for(aux = t[i]; aux != z; aux = aux->next) 
        { STinsert(st, aux->item); /* free(aux);*/ }
    free(t);
}

/* Busca */
Item searchR(ST st, link t, Key v)
{ 
    if(t == z) return st->NULLitem;
    if(st->eq(st->key(t->item), v)) return t->item;
    return searchR(st, t->next, v);
}

Item STsearch(ST st, Key v)
{ return searchR(st, st->heads[hash(v, st->M)], v); }

/* Remoção */
void STdelete(ST st, Key k)
{   
    int i = hash(k, st->M);
    link s = st->heads[i], t = st->heads[i]->next;
    
    /* Caso base: vazio e remoção no primeiro elemento */
    if(s == z) return;
    if(st->eq(st->key(t->item), k))
    { st->heads[i] = s->next; free(s); st->N--; return; }
    
    /* Caso geral: remoção no n-ésimo elemento */
    while(t != z)
        if(st->eq(st->key(t->item), k))
        { s->next = t->next; free(t); return; }
}

/* Ordenação */
static ST global_st;
static int cmp(const void *a, const void *b)
{
    if(global_st->less(*(Key *) a, *(Key *) b)) return -1;
    if(global_st->eq(*(Key *) a, *(Key *) b)) return 0;
    return 1;
}

void STsort(ST st, void(*visit)(Item))
{
    /* Aloca vetor de chaves para ordenação */
    int i = 0, j = 0, N = st->N; link t;
    Key *keys = (Key *) malloc(N * sizeof(*keys));
    
    /* Preenche vetor de chaves */
    for(i = 0; i < st->M; i++) 
        if(st->heads[i] != z)
            for(t = st->heads[i]; t != z; t = t->next)
                keys[j++] = st->key(t->item);
    
    /* Ordena as chaves com a função qsort */
    global_st = st;
    qsort((void *) keys, N, sizeof(*keys), cmp);
    
    /* Seleciona e realiza a ação em cada item */
    for(i = 0; i < N; i++)
        visit(STsearch(st, keys[i]));
    
    free(keys); /* Libera vetor auxiliar de chaves */
}

/* Liberação de memória */
void STfree(ST st)
{ 
    int i = 0; link t; STn--;
    for(i = 0; i < st->M; i++) 
        for(t = st->heads[i]; t != z; t = t->next) 
            { st->free_item(t->item); free(t); }
    free(st->heads); free(st); if(STn == 0) free(z);
}
