#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define tam 8

void processarDocumento(char *inputArq, char *outputArq);

int main(int argc,char **argv){
    if(argc > 3){
        printf("Argumentos Insuficientes!\n");
        return 1;
    }
    
    processarDocumento(argv[1], argv[2]);
    
    return 0;
}


void processarDocumento(char *inputArq, char *outputArq){
    FILE *input = fopen(inputArq, "r");
    FILE *output = fopen(outputArq, "w");

    if(!input || !output){
        printf("Erro ao abrir arquivos\n");
        fclose(input);
        fclose(output);
        return;
    }

    
    int numPlacas, numPacotesRastreamento,  peso, volume;
    int placaVeiculo[tam];
    

    if(fscanf(input, "%d", &numPlacas) != 1){
        printf("Erro ao ler numero de placas\n");
        fclose(input);
        fclose(output);
        return;
    }

    for(int i = 0; i < numPlacas; i++){
        if(fscanf(input, "%s %d %d", placaVeiculo, &peso, &volume) != 2){
            printf("Erro ao ler peso e volume\n");
            fclose(input);
            fclose(output);
            return;
        }
    }

    if(fscanf(input, "%d", &numPacotesRastreamento) != 1){
        printf("Erro ao ler numero de pacotes\n");
        fclose(input);
        fclose(output);
        return;
    }

    for(int i = 0; i < numPacotesRastreamento; i++){
        if(fscanf(input, "%d %d", &peso, &volume) != 2){
            printf("Erro ao ler peso e volume\n");
            fclose(input);
            fclose(output);
            return;
        }
    }



}