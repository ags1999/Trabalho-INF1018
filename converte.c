/* Alexandre_Sanson 1711450 3WB */
/* Luana_Durand 1820807 */
#include <stdlib.h>
#include "converte.h"


union data
    {
        unsigned int valor_int;
        unsigned char vetor[4];
    };

int tamanho(char c1) //Retorna o tamanho em bytes do caractere utf
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

void utf_varint_aux(FILE *arq_saida,unsigned int palavra, int tm) // converte e insere o valor recebido
{
    

    int byte0, byte1, byte2, byte3;
    char grupo_7bits;
    union data temp;
    temp.valor_int = palavra;


    switch (tm)
    {
    case 2:
        byte1 = ((temp.vetor[1] - 0xC0));
        byte1 <<= 6;
        byte0 = (temp.vetor[0] - 0x80);
        temp.valor_int =  byte0 + byte1;
        break;
    
    case 3:
        byte2 = (temp.vetor[2] - 0xE0);
        byte2 <<= 12;
        byte1 = (temp.vetor[1] - 0x80);
        byte1 <<= 6;
        byte0 = (temp.vetor[0] - 0x80);
        temp.valor_int =  byte0 + byte1 + byte2;
        break;

    case 4:
        byte23 = temp.vetor[3] - 0xF0;
        byte3 <<= 18;
        byte2 = temp.vetor[2] - 0x80;
        byte2 <<= 12;
        byte1 = temp.vetor[1] - 0x80;
        byte1 <<= 6;
        byte0 = (temp.vetor[0] - 0x80);
        temp.valor_int = byte0 + byte1 + byte2 + byte3;
        break;

    default:
        break;
    }



    while (1)
    {
        grupo_7bits = temp.vetor[0];
        grupo_7bits &= 0x7f;


        if((temp.valor_int >> 7) == 0)
        {

            fputc(grupo_7bits, arq_saida);
            break;
        }
        else
        {
            grupo_7bits += 0x80;

            fputc(grupo_7bits, arq_saida);
        }
        temp.valor_int >>= 7;
    }
    
    return; 
    
}

int utf_varint(FILE *arq_entrada, FILE *arq_saida)
{
    int i = 0, tm = 0;
    unsigned int palavra;
    unsigned char byte1;
    //palavra
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
        palavra = 0;
        i = 0;
        byte1 = fgetc(arq_entrada);
        if(feof(arq_entrada))
            break;

        tm = tamanho(byte1);
        palavra += byte1;
        i++;
        while (i < tm)
        {
            palavra <<= 8;
            palavra += fgetc(arq_entrada);
            i++;
        }
        

        utf_varint_aux(arq_saida, palavra, tm); 
  
    }

    return 0;
}

unsigned int varint_utf_aux(unsigned char aux[],int tm, FILE *arq_saida) // converte e insere o valor recebido
{
        
    int i;
    union data temp, insercao;
    insercao.valor_int = 0;
    temp.valor_int = 0;
    for(i = 0; i < tm - 1; i++)
    {
        temp.valor_int += (aux[i] - 0x80) << 7*i;
    }
    temp.valor_int += aux[i] << 7*i;
        

    
    if(temp.valor_int < 0x80)
    {
        fputc(temp.vetor[0], arq_saida);
    }
    else if(temp.valor_int < 0x800)
    {
        insercao.valor_int = 0xC080;

        insercao.vetor[0] += (temp.vetor[0] & 0x3F);
        temp.valor_int >>= 6;
        insercao.vetor[1] += temp.vetor[0];

        fputc(insercao.vetor[1], arq_saida);
        fputc(insercao.vetor[0], arq_saida);
        
    }
    else if (temp.valor_int < 0x10000)
    {
        insercao.valor_int = 0xE08080;

        insercao.vetor[0] += (temp.vetor[0] & 0x3F);
        temp.valor_int >>= 6;

        insercao.vetor[1] += (temp.vetor[0] & 0x3F);
        temp.valor_int >>= 6;

        insercao.vetor[2] += temp.vetor[0];

        fputc(insercao.vetor[2], arq_saida);
        fputc(insercao.vetor[1], arq_saida);
        fputc(insercao.vetor[0], arq_saida);
        
    }
    else
    {
        insercao.valor_int = 0xF0808080;

        insercao.vetor[0] += (temp.vetor[0] & 0x3F);
        temp.valor_int >>= 6;
        i = temp.vetor[0] & 0x7F;

        insercao.vetor[1] += (temp.vetor[0] & 0x3F);
        temp.valor_int >>= 6;

        insercao.vetor[2] += (temp.vetor[0] & 0x3F);
        temp.valor_int >>= 6;

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
    unsigned char byte1;
    unsigned char palavra[5] = {0,0,0,0,0};
    
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
        byte1 = fgetc(arq_entrada);
        if(feof(arq_entrada))
            break;
        
        
        palavra[tm] = byte1;
        tm++;
        while (byte1 >> 7)
        {
            
            byte1 = fgetc(arq_entrada);
            
            
            palavra[tm] = byte1;
            tm++;
        }

        varint_utf_aux(palavra, tm, arq_saida);
       
        
    }

    return 0;
}

//gcc -Wall -g -o teste converte.c teste.c  && ./teste
