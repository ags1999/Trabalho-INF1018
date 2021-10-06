/* Alexandre_Sanson 1711450 3WB */
#include <stdlib.h>
#include "converte.h"


union data
    {
        unsigned int i1;
        unsigned char vetor[4];
    };

int tamanho(char c1)
{
    int i = 0;
    unsigned char aux = c1;
    //printf("aux = %hhx\n", aux);
    while(aux >> 7)
    {
        aux <<= 1;
        i++;
    }
    
    if(!i)
        i++;
    return i;
}

void utf_varint_aux(FILE *arq_saida, int aux, int tm)
{
    

    //int i;
    char grupo_7bits;
    union data temp;
    temp.i1 = aux;
    //printf("aux = %X\n",temp.vetor[0]);

    while (1)
    {
        grupo_7bits = temp.vetor[0];
        grupo_7bits &= 0x7f;
        //printf("atual %hhx\n", grupo_7bits);
        //printf("next %hhx\n", temp.i1 >> 7);
        if((temp.i1 >> 7) == 0)
        {
            //printf("end %hhx\n", grupo_7bits);
            fputc(grupo_7bits, arq_saida);
            break;
        }
        else
        {
            grupo_7bits += 0x80;
            //printf("cont %hhx\n", grupo_7bits);
            fputc(grupo_7bits, arq_saida);
        }
        temp.i1 >>= 7;
    }
    
    return; 
    
}

int utf_varint(FILE *arq_entrada, FILE *arq_saida)
{
    int i = 0, tm = 0;
    unsigned int aux;
    unsigned char c1;

    while(1)
    {
        aux = 0;
        i = 0;
        c1 = fgetc(arq_entrada);
        if(feof(arq_entrada))
            break;
        //printf("%hhx\n", c1);
        tm = tamanho(c1);
        //printf ("tm = %d\n", tm);
        aux += c1;
        i++;
        while (i < tm)
        {
            aux <<= 8;
            aux += fgetc(arq_entrada);
            i++;
        }
        
        printf("main = %08X\n", aux);
        utf_varint_aux(arq_saida, aux, tm); 
  
    }

    return 0;
}

unsigned int varint_utf_aux(unsigned char aux[],int tm, FILE *arq_saida)
{
        
    int i;
    union data temp, insercao;
    insercao.i1 = 0;
    temp.i1 = 0;
    for(i = 0; i < tm - 1; i++)
    {
        temp.i1 += (aux[i] - 0x80) << 7*i;
    }
    temp.i1 += aux[i] << 7*i;
        
    printf("original : %08X\n", temp.i1);
    printf("%hhx->", temp.vetor[0]);
    printf("%hhx->", temp.vetor[1]);
    printf("%hhx->", temp.vetor[2]);
    printf("%hhx->", temp.vetor[3]);
    printf("\n");

    printf("%d\n", tm);
    
    
    if(temp.i1 < 0x80)
    {
        printf("Impresso: %hhx\n", temp.vetor[0]);
        //fputc(temp.vetor[0], arq_saida);
    }
    else if(temp.i1 < 0x800)
    {
        insercao.i1 = 0xC080;
        printf("hmm: %hhx\n", temp.vetor[0] & 0x7F);
        insercao.vetor[0] += (temp.vetor[0] & 0x7F);
        temp.i1 >>= 7;
        insercao.vetor[1] += temp.vetor[0];
        //printf("Impresso: %hhx\n", temp.vetor[0]);
        //printf("Impresso: %hhx\n", temp.vetor[1]);
        
    }
    else if (temp.i1 < 0x10000)
    {
        insercao.i1 = 0xE08080;
        printf("hmm: %hhx\n", temp.vetor[0] & 0x7F);
        insercao.vetor[0] += (temp.vetor[0] & 0x7F);
        temp.i1 >>= 7;
        printf("hmm: %hhx\n", temp.vetor[0] & 0x7F);
        insercao.vetor[1] += (temp.vetor[0] & 0x7F);
        temp.i1 >>= 7;
        printf("hmm: %hhx\n", temp.vetor[0] & 0x7F);
        insercao.vetor[2] += temp.vetor[0];
        //printf("Impresso: %hhx\n", temp.vetor[0]);
        //printf("Impresso: %hhx\n", temp.vetor[1]);
        //printf("Impresso: %hhx\n", temp.vetor[2]);
        
    }
    else
    {
        insercao.i1 = 0xF0808080;
        printf("hmm: %hhx\n", temp.vetor[0] & 0x7F);
        insercao.vetor[0] += (temp.vetor[0] & 0x7F);
        temp.i1 >>= 7;
        i = temp.vetor[0] && 0x7F;
        printf("hmm: %hhx\n", i);
        insercao.vetor[1] += (temp.vetor[0] & 0x7F);
        temp.i1 >>= 7;
        printf("hmm: %hhx\n", temp.vetor[0] & 0x7F);
        insercao.vetor[2] += (temp.vetor[0] & 0x7F);
        temp.i1 >>= 7;
        insercao.vetor[3] += temp.vetor[0];
        //printf("Impresso: %hhx\n", temp.vetor[0]);
        //printf("Impresso: %hhx\n", temp.vetor[1]);
        //printf("Impresso: %hhx\n", temp.vetor[2]);
        //printf("Impresso: %hhx\n", temp.vetor[3]);
    }
    
    /*
    for(i = 3; i > -1; i--)
    {
        if(temp.vetor[i])
        {
            printf("Impresso: %hhx\n", temp.vetor[i]);
            fputc(temp.vetor[i], arq_saida);
        }
    }
    */
    
    printf("utf: %X\n", temp.i1);
    return 0;
}

int varint_utf(FILE *arq_entrada, FILE *arq_saida)
{
    int tm, i;
    unsigned char c1;
    unsigned char aux[5] = {0,0,0,0,0};
    
    while (1)
    {
        tm = 0;
        c1 = fgetc(arq_entrada);
        if(feof(arq_entrada))
            break;
        //fputc(c1, arq_saida);
        
        aux[tm] = c1;
        tm++;
        while (c1 >> 7)
        {
            
            c1 = fgetc(arq_entrada);
            
            //fputc(c1, arq_saida);
            aux[tm] = c1;
            tm++;
        }
        //printf("%d\n",tm);
        printf("valor varint: \n");
        for(i = 0 ;i < tm; i++)
        {
            printf("%02hhx.", aux[i]);
        }
        printf("\n");
        
        varint_utf_aux(aux, tm, arq_saida);
       
        
    }

    return 0;
}

//gcc -Wall -g -o teste converte.c teste.c  && ./teste
