/***********************************************/
/**  MAC 0323  -  Estrutura de Dados          **/
/**  IME-USP   -  Primeiro  Semestre de 2013  **/
/**  Turma 45  -  Yoshiharu Kohayakawa        **/
/**                                           **/
/**  Terceiro  Exercício-Programa             **/
/**  Arquivo:  ST.c                           **/
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

/* Bibliotecas padrão */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* Bibliotecas internas */
#include "arne.h"

/*
////////////////////////////////////////////////////////////////////////
-----------------------------------------------------------------------
                                CONSTANTES 
-----------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/
const char no_externo[] = "\\Tn";
const char no_interno[] = "\\Tc*{3pt}";
const char tree_sep[] = "3mm";
const char level_sep[] = "5mm";

/*
////////////////////////////////////////////////////////////////////////
-----------------------------------------------------------------------
                          MACROS/ESTRUTURAS 
-----------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

/* Estruturas para facilitar ponteiros */
#define hl  (h->l)
#define hr  (h->r)
#define hrl (h->r->l)
#define hll (h->l->l)

/* Nódulo de uma tabela de símbolos */
typedef struct STnode* link;
struct STnode { 
    Item item;  /* Conteúdo da tabela */
    link l, r;  /* Ponteiro para os filhos das ARNEs */
    int red;    /* 'Cor' do ponteiro da ARNE (vermelho/preto) */
    int N;      /* Nº de filhos na subárvore cuja raiz é este nódulo */
};

/* Ponteiro para a raiz da ARNE e nódulo externo */
struct st {
    link head;
    void *NULLitem;
    Key   (*key) (Item);
    int   (*eq)  (Key, Key);
    int   (*less)(Key, Key);
    void  (*free_item)(Item);
};

/* Link genérico que representa nó externo */
static link z;

static int TSn = 0;

/*
////////////////////////////////////////////////////////////////////////
-----------------------------------------------------------------------
                                FUNÇÕES
-----------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

static link NEW(Item item, link l, link r, int N, int red)
{ 
    link x = malloc(sizeof *x); 
    x->item = item; x->l = l; x->r = r; x->N = N; x->red = red;
    return x;
}

ST STinit(
    Item NULLitem, 
    void (*free_item)(Item),
    Key  (*key) (Item),
    int  (*eq)  (Key, Key),
    int  (*less)(Key, Key) )
{ 
    ST new = (ST) malloc(sizeof(*new)); TSn++;
    if(z == NULL) z = NEW(NULLitem, 0, 0, 0, 0); 
    new->head = z; new->free_item = free_item;
    new->key = key; new->eq = eq; new->less = less;
    new->NULLitem = NULLitem;
    return new;
}

int STcount(ST st) { return st->head->N; }

int STempty(ST st) { return st->head->N == 0; }

static Item searchR(ST st, link h, Key v)
{ 
    Key t;
    if (h == z) return st->NULLitem;
    t = st->key(h->item);
    if (st->eq(v, t)) return h->item;
    if (st->less(v, t)) return searchR(st, hl, v);
    else return searchR(st, hr, v);
}

Item STsearch(ST st, Key v) 
{ return searchR(st, st->head, v); } 

static link fixNr(link h)
{ 
    h->N = hl->N + hr->N +1;
    return h;
}

static link rotL(link h)
{ 
    link x = hr; hr = x->l; x->l = fixNr(h); 
    x->red = x->l->red; x->l->red = 1;
    return fixNr(x);
}

static link rotR(link h)
{ 
    link x = hl; hl = x->r; x->r = fixNr(h);
    x->red = x->r->red; x->r->red = 1;
    return fixNr(x);
}

void colorFlip(link h)
{ 
    h->red  = 1 - h->red;
    hl->red = 1 - hl->red;
    hr->red = 1 - hr->red;    
}

link mvRedL(link h)
{
    colorFlip(h);
    if (hrl->red) { hr = rotR(hr); h = rotL(h); }
    return h;
}

link mvRedR(link h)
{
    colorFlip(h);
    if (hll->red) { h = rotR(h); }
    return h;
}

link balance(link h)
{
    if (hr->red) h = rotL(h);
    if (hl->red && hll->red) h = rotR(h);
    if (hl->red && hr->red) colorFlip(h);
    return fixNr(h);
}

/*
////////////////////////////////////////////////////////////////////////
-----------------------------------------------------------------------
                              INSERÇÕES        
-----------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

link LLRBinsert(ST st, link h, Item item)
{ 
    Key v = st->key(item);
    /* Insert a new node at the bottom*/
    if (h == z) return NEW(item, z, z, 1, 1);  

    if (st->less(v, st->key(h->item)))
        hl = LLRBinsert(st, hl, item); 
    else
        hr = LLRBinsert(st, hr, item); 

    /* Enforce left-leaning condition */
    if (hr->red && !hl->red) h = rotL(h);
    if (hl->red && hll->red) h = rotR(h);
    if (hl->red && hr->red) colorFlip(h);

    return fixNr(h);
}

void STinsert(ST st, Item item)
{ st->head = LLRBinsert(st, st->head, item); st->head->red = 0; }

/*
////////////////////////////////////////////////////////////////////////
-----------------------------------------------------------------------
                         SELEÇÃO E ORDENAÇÃO     
-----------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

Item selectR(link h, int r)
{
    int t = hl->N;
    if (t > r) return selectR(hl, r);
    if (t < r) return selectR(hr, r-t-1);
    return h->item;
}

Item STselect(ST st, int r)
{
    return selectR(st->head, r);
}

void sortR(link h, void(*visit)(Item))
{
    if (h!=z) {
        sortR(hl, visit);
        visit(h->item);
        sortR(hr, visit);
    }
}

void STsort(ST st, void(*visit)(Item))
{ sortR(st->head, visit); }

/*
////////////////////////////////////////////////////////////////////////
-----------------------------------------------------------------------
                                DELEÇÃO            
-----------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

Item getMin(link h)
{
    if (hl == z) return h->item;
    else return getMin(hl);
}

link deleteMin(ST st, link h)
{
    if (hl == z) { st->free_item(h->item); free(h); return z; }
    if (!hl->red && !hll->red) h = mvRedL(h);
    hl = deleteMin(st, hl);
    return balance(h);
}

void STdeleteMin(ST st)
{
    if (STcount(st) == 0) 
    { printf("Underflow"); exit(1); }

    if (!st->head->l->red && !st->head->r->red) st->head->red = 1;

    st->head = deleteMin(st, st->head);
    if (STcount(st) > 0) st->head->red = 0;
}

link deleteMax(ST st, link h)
{
    if (hl->red) h = rotR(h);
    if (hr == z) { st->free_item(h->item); free(h); return z; }
    if (!hr->red && !hrl->red) h = mvRedR(h);
    hr = deleteMax(st, hr);
    return balance(h);
}

void STdeleteMax(ST st)
{
    if (STcount(st) == 0) 
    { printf("Underflow"); exit(1); }

    if (!st->head->l->red && !st->head->r->red) st->head->red = 1;

    st->head = deleteMax(st, st->head);
    if (STcount(st) > 0) st->head->red = 0;
}

link deleteR(ST st, link h, Key v)
{
    Key t = st->key(h->item);
    if (st->less(v,t)) {
        if (!hl->red && !hll->red) h = mvRedL(h);
        hl = deleteR(st, hl, v);
    } else {
        if (hl->red) h = rotR(h);
        if (st->eq(v,st->key(h->item)) && hr == z)
        { st->free_item(h->item); free(h); return z; }
        if (!hr->red && !hrl->red) h = mvRedR(h);
        if (st->eq(v,st->key(h->item))) {
            h->item = getMin(hr);
            hr = deleteMin(st, hr);
        } else hr = deleteR(st, hr, v);
    }
    return balance(h);
}

void STdelete(ST st, Key v)
{
    if (!st->head->l->red && !st->head->r->red) st->head->red = 1;
    st->head = deleteR(st, st->head, v);
    if (STcount(st) > 0) st->head->red = 0;
}

void STfree(ST st)
{
    while(!STempty(st)) STdeleteMin(st);
    free(st); TSn--; if(TSn == 0) free(z);
}
