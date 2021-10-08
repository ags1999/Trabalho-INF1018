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
    

    int v0, v1, v2, v3;
    char grupo_7bits;
    union data temp;
    temp.i1 = aux;


    switch (tm)
    {
    case 2:
        v1 = ((temp.vetor[1] - 0xC0));
        v1 <<= 6;
        v0 = (temp.vetor[0] - 0x80);
        temp.i1 =  v0 + v1;
        break;
    
    case 3:
        v2 = (temp.vetor[2] - 0xE0);
        v2 <<= 12;
        v1 = (temp.vetor[1] - 0x80);
        v1 <<= 6;
        v0 = (temp.vetor[0] - 0x80);
        temp.i1 =  v0 + v1 + v2;
        break;

    case 4:
        v3 = temp.vetor[3] - 0xF0;
        v3 <<= 18;
        v2 = temp.vetor[2] - 0x80;
        v2 <<= 12;
        v1 = temp.vetor[1] - 0x80;
        v1 <<= 6;
        v0 = (temp.vetor[0] - 0x80);
        temp.i1 = v0 + v1 + v2 + v3;
        break;

    default:
        break;
    }



    while (1)
    {
        grupo_7bits = temp.vetor[0];
        grupo_7bits &= 0x7f;


        if((temp.i1 >> 7) == 0)
        {

            fputc(grupo_7bits, arq_saida);
            break;
        }
        else
        {
            grupo_7bits += 0x80;

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
        if(ferror(arq_entrada))
        {
            fprintf(stderr,"Erro de Leitura\n");
            return -1;
        }

        if(ferror(arq_saida))
        {
            fprintf(stderr,"Erro de Escrita\n");
            return -1;
        }
        aux = 0;
        i = 0;
        c1 = fgetc(arq_entrada);
        if(feof(arq_entrada))
            break;

        tm = tamanho(c1);
        aux += c1;
        i++;
        while (i < tm)
        {
            aux <<= 8;
            aux += fgetc(arq_entrada);
            i++;
        }
        

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
        

    
    if(temp.i1 < 0x80)
    {
        fputc(temp.vetor[0], arq_saida);
    }
    else if(temp.i1 < 0x800)
    {
        insercao.i1 = 0xC080;

        insercao.vetor[0] += (temp.vetor[0] & 0x3F);
        temp.i1 >>= 6;
        insercao.vetor[1] += temp.vetor[0];

        fputc(insercao.vetor[1], arq_saida);
        fputc(insercao.vetor[0], arq_saida);
        
    }
    else if (temp.i1 < 0x10000)
    {
        insercao.i1 = 0xE08080;

        insercao.vetor[0] += (temp.vetor[0] & 0x3F);
        temp.i1 >>= 6;

        insercao.vetor[1] += (temp.vetor[0] & 0x3F);
        temp.i1 >>= 6;

        insercao.vetor[2] += temp.vetor[0];

        fputc(insercao.vetor[2], arq_saida);
        fputc(insercao.vetor[1], arq_saida);
        fputc(insercao.vetor[0], arq_saida);
        
    }
    else
    {
        insercao.i1 = 0xF0808080;

        insercao.vetor[0] += (temp.vetor[0] & 0x3F);
        temp.i1 >>= 6;
        i = temp.vetor[0] & 0x7F;

        insercao.vetor[1] += (temp.vetor[0] & 0x3F);
        temp.i1 >>= 6;

        insercao.vetor[2] += (temp.vetor[0] & 0x3F);
        temp.i1 >>= 6;

        insercao.vetor[3] += temp.vetor[0];

        fputc(insercao.vetor[3], arq_saida);
        fputc(insercao.vetor[2], arq_saida);
        fputc(insercao.vetor[1], arq_saida);
        fputc(insercao.vetor[0], arq_saida);
    }
    
    
    

    return 0;
}

int varint_utf(FILE *arq_entrada, FILE *arq_saida)
{
    int tm;
    unsigned char c1;
    unsigned char aux[5] = {0,0,0,0,0};
    
    while (1)
    {
        if(ferror(arq_entrada))
        {
            fprintf(stderr,"Erro de Leitura\n");
            return -1;
        }

        if(ferror(arq_saida))
        {
            fprintf(stderr,"Erro de Escrita\n");
            return -1;
        }
        
        tm = 0;
        c1 = fgetc(arq_entrada);
        if(feof(arq_entrada))
            break;
        
        
        aux[tm] = c1;
        tm++;
        while (c1 >> 7)
        {
            
            c1 = fgetc(arq_entrada);
            
            
            aux[tm] = c1;
            tm++;
        }

        varint_utf_aux(aux, tm, arq_saida);
       
        
    }

    return 0;
}

//gcc -Wall -g -o teste converte.c teste.c  && ./teste
