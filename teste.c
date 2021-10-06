#include <stdio.h>
#include "converte.h"

int main(void)
{
    //teste utf to var
    
    /*
    FILE* entrada1 = fopen("utf8_peq","r");
    FILE* saida1 = fopen("utf8_TO_var","w");
    utf_varint(entrada1, saida1);
    fclose(entrada1);
    fclose(saida1);
    

    //teste utf to var to utf
    
    FILE* entrada2 = fopen("utf8_TO_var","r");
    FILE* saida2 = fopen("utf8_TO_var_TO_utf8","w");
    varint_utf(entrada2, saida2);
    fclose(entrada2);
    fclose(saida2);
    */

    //teste var to utf
    
    FILE* entrada3 = fopen("var_peq","r");
    FILE* saida3 = fopen("var_TO_utf8","w");
    varint_utf(entrada3, saida3);
    fclose(entrada3);
    fclose(saida3);
      
    
    
    //teste var to utf to var
    
    FILE* entrada4 = fopen("var_TO_utf8","r");
    FILE* saida4 = fopen("var_TO_utf8_TO_var","w");
    utf_varint(entrada4, saida4);
    fclose(entrada4);
    fclose(saida4);
    
    

    
    return 0;
}