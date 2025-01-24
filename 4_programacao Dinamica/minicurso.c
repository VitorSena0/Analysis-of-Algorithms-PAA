#include <stdio.h>
#include <string.h>

#define tam 1
char armazenaNomes[tam][20];
char armazenaData[tam][11]; // Alterado para acomodar datas maiores
char armazenaMatricula[tam][13];

void imprimirDados(){
    for(int i = 0; i < tam; i++){
        printf("\nNome: %s, Data: %s, Matricula: %s\n", armazenaNomes[i], armazenaData[i], armazenaMatricula[i]);
    }
}

int main(){
    char nomes[tam][20];
    char datas[tam][11]; // Alterado para acomodar datas maiores
    char matriculas[tam][13];
    
    for(int i = 0; i < tam; i++){
        printf("Digite o %dº nome: \n", i + 1);
        scanf("%19s", nomes[i]); // Alterado para evitar buffer overflow
        getchar();
        printf("Digite a data de nascimento (dd/mm/aaaa) do %dº nome: \n", i + 1);
        scanf("%10s", datas[i]); // Alterado para acomodar datas maiores
        getchar();
        printf("Digite a matricula do %dº nome: \n", i + 1);
        scanf("%12s", matriculas[i]);

        // Copiando para as variáveis globais
        strcpy(armazenaNomes[i], nomes[i]);
        strcpy(armazenaData[i], datas[i]);
        strcpy(armazenaMatricula[i], matriculas[i]);
    }

    imprimirDados();

    return 0;
}
