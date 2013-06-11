/***********************************************/
/**  MAC 0323  -  Estrutura de Dados          **/
/**  IME-USP   -  Primeiro  Semestre de 2013  **/
/**  Turma 45  -  Yoshiharu Kohayakawa        **/
/**                                           **/
/**  Terceiro  Exercício-Programa             **/
/**  Arquivo:  main.c                         **/
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
#include "getline.h"

/* Bibliotecas internas */
#include "word.h"
#include "lemma.h"

/*
////////////////////////////////////////////////////////////////////////
-----------------------------------------------------------------------
                           MACROS/ESTRUTURAS
-----------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/

#define MAX_OPTION_SIZE 3

#define TRUE 1
#define FALSE 0
#define PROMPT ">"

static int verbosity = 0;

/*
////////////////////////////////////////////////////////////////////////
-----------------------------------------------------------------------
                            FUNÇÕES INTERNAS
-----------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/
static void visit(char *word)
{
    int s = 0; Word w = word_table_get(word);
    printf("\n===============================================\n");
    while(word[s] != ' ') putchar(word[s++]); putchar('\n'); 
    word_print_sentences(w, verbosity);
}

/*
////////////////////////////////////////////////////////////////////////
-----------------------------------------------------------------------
                                 MAIN
-----------------------------------------------------------------------
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
*/
int main(int argc, char **argv)
{
    /** VARIÁVEIS *****************************************************/
        int i;           /* Contador auxiliar                        */
        FILE *file;      /* Ponteiro para o arquivo de entrada       */
        char *buffer;    /* Buffer auxiliar para armazenar o texto   */
        char *file_name; /* Nome do arquivo com o texto para análise */
        
        /* Ponteiros para informações sobre as sentenças analisadas */
        char *identifier, *annotated, *sentence, *lemma, *word;
        
        /* Ponteiros para pegar opções do prompt */
        char *query, option[MAX_OPTION_SIZE];
        
        /* Contadores para estatísticas do texto */
        int total_words = 0, total_tokens = 0, total_sentences = 0;
        
    /** PRÉ-PROCESSAMENTO DO TEXTO ************************************/
        if(argc != 2 && argc != 3) 
        {
            /* Opções incorretas */
            fprintf(stderr, "Uso: ./ep3 -f<arquivo.txt.out>\n");
            return EXIT_FAILURE;
        }
        else if(argv[1][0] != '-' || argv[1][1] != 'f')
        {
            /* Arquivo não fornecido */
            fprintf(stderr, "Erro: arquivo requerido para análise!\n");
            fprintf(stderr, "Uso: ./ep3 -f<arquivo.txt.out>\n");
            return EXIT_FAILURE;
        }    
        else
        {
            if(argc == 3) file_name = argv[2];  /* Nome separado */
            else file_name = &argv[1][2]; /* Nome colado no '-f' */
            
            /* Inicializa tabelas de símbolos */
            word_table_init(); lemma_table_init(); 
            
            /* Abre texto */
            file = fopen(file_name, "r");
            if(file == NULL) 
                fprintf(stderr, "ERRO: arquivo %s não encontrado", file_name);
            else printf("Carregando o texto...\n");
            
            buffer = getline(file, EOF);
            for(i = 0; buffer[i] != '\0'; i++) 
            {
                /* 1: Identifica a sentença */
                if(strncmp(&buffer[i], "Sentence #", 10 * sizeof(char)) == 0)
                {
                    /* 2: Percorre o identificador até a sentença*/
                    for(identifier = &buffer[i]; buffer[i] != ':'; i++);
                    i++; sentence = &buffer[i]; total_sentences++;
                    
                    /* 3: Percorre a sentença até as anotações */
                    while(buffer[i] != '['
                    && strncmp(&buffer[i], "[Text=", 6 * sizeof(char)) != 0) i++;
                    annotated = &buffer[i];
                    
                    /* 4: Percorre as anotações até acabarem */
                    while(1)
                    {
                        /* 4.1: início da anotação e palavra anotada */
                        while(buffer[i] != '\n' && buffer[i] != '[' 
                        && strncmp(&buffer[i], "[Text=", 6 * sizeof(char))) i++;
                        
                        /* Se a linha acaba, não há mais anotações */
                        if(buffer[i] == '\n') break;
                        
                        /* 4.2: Guarda endereço e soma aos tokens e palavras. */
                        i += 6; word = &buffer[i]; total_tokens++; total_words++; 
                        
                        /* 4.3: Identifica se o token é mesmo palavra */
                        for(; buffer[i] != ' '; i++) 
                            if(buffer[i] < 'A' || (buffer[i] > 'Z' && buffer[i] < 'a')
                            || buffer[i] > 'z') { total_words--; break; }
                        
                        /* 4.4: Encontra e armazena o lema da palavra */
                        while(buffer[i] != 'L'
                        && strncmp(&buffer[i], "Lemma=", 6 * sizeof(char) != 0)) i++;
                        i += 6; lemma = &buffer[i];
                        
                        /* 4.5: Armazena palavra e lema nas tabelas */
                        word_table_insert(word, lemma, 
                                identifier, sentence, annotated);
                        lemma_table_insert(lemma, word);
                        
                        /* 4.6: Percorre até o final da anotação */
                        while(buffer[i] != ']') i++;
                    }
                    
                } /*if*/
            } /*for*/
            
            /* Fecha texto */
            fclose(file);
        }
        
    /** INTERFACE ITERATIVA *******************************************/
        printf("MAC0323-EP3: Localização de Palavras I\n\n");
        while(1)
        {
            verbosity = 0;
            printf("%s ", PROMPT); scanf(" %3s", option); 
            
            /* Caso a opção seja válida */
            if(option[0] != '-') 
            {
                fprintf(stderr, "Formato de opção inválido!\n");
                fprintf(stderr, "Use -<opcões> <palavra>\n");
                continue;
            }
            if(option[1] == 'F') break; /*1*/
            if(option[1] == 'e' || option[1] == 'a')
            {
                Word w;
                getchar(); query = getline(stdin, '\n'); 
                w = word_table_get(query); free(query);
                
                if(w != NULL) 
                {
                    verbosity = 0;
                    switch(option[2]) /* Verbosidade */
                    {
                        case 'v': verbosity = 1; break;
                        case 'V': verbosity = 2; break;
                        case ' ': case '\0': break;
                        default: fprintf(stderr, "Opção não reconhecida!\n");
                    }
                    
                    if(option[1] == 'e') /*2*/
                    {
                        word_print_sentences(w, verbosity);
                    }
                    if(option[1] == 'a') /*3*/
                    { 
                        query = word_lemma(w); 
                        lemma_list_words(query, visit); 
                    }
                }
                else { fprintf(stderr, "Palavra não encontrada.\n"); }
            }
            else switch(option[1]) 
            {
                case 't': /*4*/
                    word_print_tokens();
                    break;
                case 'd': /*5*/
                    word_print_words();
                    break;
                case 'l': /*6*/
                    lemma_print_lemmas();
                    break;
                case 'L': /*7*/
                    lemma_print_lemma_word();
                    break;
                case 's': /*8*/
                    printf("\n");
                    printf(" ESTATÍSTICAS DO TEXTO\n");
                    printf("\n");
                    printf(" * Total de tokens: %d;\n", total_tokens);
                    printf(" * Total de palavras: %d;\n", total_words);
                    printf(" * Total de sentenças: %d;\n", total_sentences);
                    printf(" * Total de tokens distintos: %d;\n", 
                          word_total_tokens());
                    printf(" * Total de palavras distintas: %d;\n", 
                          word_total_words());
                    printf(" * Total de lemas distintos: %d.\n", 
                          lemma_total_lemmas());
                    break;
                default:
                    fprintf(stderr, "Opção não reconhecida!\n");
                    continue;
                
                /* Sumário de opções:
                 * (1) -F: fim das instruções;
                 * (2) -e: todas as sentenças que contêm exatamente
                 *         a palavra
                 * (3) -a: todas as sentenças que contêm a palavra
                 *         e suas variantes;
                 * (4) -t: lista de todos os 'tokens' presentes no 
                 *         texto em ordem alfabética;
                 * (5) -d: lista de todas as palavras no texto, sem
                 *         repetição e em ordem alfabética;
                 * (6) -l: todas as palavras presentes no texto em 
                 *         sua forma lematizada (sem repetição e 
                 *         em ordem alfabética)
                 * (7) -L: lista de todos os lemas seguidos das 
                 *         palavras no texto com aquele lema;
                 * (8) -s: estatísticas do texto: total de tokens com
                 *         repetição, total de palavras, total de 
                 *         tokens distintos, total de palavras dis-
                 *         tintas, total de lemas distintos.
                 */
                    
            } /*switch*/
        } /*while(1)*/
        
    /** LIBERAÇÃO DE MEMÓRIA ******************************************/
        printf("Encerrando programa...\n");
        free(buffer); word_table_free(); lemma_table_free(); 
    
    return EXIT_SUCCESS;
}
