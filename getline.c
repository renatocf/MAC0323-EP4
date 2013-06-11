/***********************************************/
/**  MAC 0323  -  Estrutura de Dados          **/
/**  IME-USP   -  Primeiro  Semestre de 2013  **/
/**  Turma 45  -  Yoshiharu Kohayakawa        **/
/**                                           **/
/**  Terceiro  Exercício-Programa             **/
/**  Arquivo:  getline.c                      **/
/**                                           **/
/**  Renato Cordeiro Ferreira        7990933  **/
/***********************************************/ 

#include<getline.h>

char *getline(FILE *file, char separator)
{
    /** VARIÁVEIS *****************************************************/
        char *line;  /* Linha criada                                 */
        char *nline; /* Auxiliar para dobrar a linha se houver 
                        estouro no tamanho máximo                    */
        int n;       /* Número do caracter                           */
        int ch;      /* Caracter atual                               */
        int size;    /* Tamanho inicial da linha criada.             */
    
        n = 0; /* Inicia a posição do primeiro caracter */
    
        /* O tamanho da linha começa com o valor inicial,
         * mas se for maior, nós dobraremos o tamanho do
         * vetor. */
        size = INITIAL_BUF_SIZE;
    
        /* Cria a 'line' com size (tamanho inicial) + 1 
         * byte (para o \0) */
        line = (char *) malloc(size+1);
    
        /* Sai com -2 se o malloc abortou. Esta é apenas uma
         * convenção. */
        if(!line) out_of_memory(-2);
        
    /** CRIA AS LINHAS ************************************************/ 
        while((ch = getc(file)) != separator) 
        {
            if(n == size)
                /* DOBRA o tamanho, copia e aloca com o dobro
                 * de tamanho */
            {
                size *= 2; /* Dobraremos o tamanho */
                nline = (char *) malloc(size+1);
                if(!line) out_of_memory(-3); /* Checa malloc */
                strncpy(nline, line, n);
                free(line);
                line = nline;
            }
            line[n++] = ch;
        }
        
        if(n == 0 && ch == EOF) 
        {
            free(line); /* Caso a linha for vazia, retornamos NULL */
            return NULL;
        }
        
        line[n] = '\0';
        
        /* Criamos um vetor de caracteres, agora, com o tamanho
         * correto, usando o ponteiro newline */
        nline = (char *) malloc(n+1);
        if(!line) out_of_memory(-4);
        
        strcpy(nline, line); free(line);
        return nline;
}
