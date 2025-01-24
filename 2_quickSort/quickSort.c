#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>  // Para usar ceil()

int contadorTrocas = 0;  // Contador de trocas
int contadorChamadas = 0; // Contador de chamadas

typedef struct SortTrocaMaisChamada{
    int trocasEsomas;
    char siglaAlgoritmo[3];
}SortTrocaMaisChamada;

// Função para incrementar o contador de chamadas
void incrementaContadorChamadas() {
    contadorChamadas++;  
}

// Função para determinar a prioridade de uma sigla
int obterPrioridade(char *sigla) {
    if (strcmp(sigla, "LP") == 0) return 1;
    if (strcmp(sigla, "LM") == 0) return 2;
    if (strcmp(sigla, "LA") == 0) return 3;
    if (strcmp(sigla, "HP") == 0) return 4;
    if (strcmp(sigla, "HM") == 0) return 5;
    if (strcmp(sigla, "HA") == 0) return 6;
    return 7; // Sigla desconhecida
}

// Função para ordenar o array
void ordenarAlgoritmos(SortTrocaMaisChamada arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            // Primeiro critério: número de trocas e chamadas (crescente)
            if (arr[j].trocasEsomas > arr[j + 1].trocasEsomas ||
                // Segundo critério: desempate pela prioridade da sigla (crescente)
                (arr[j].trocasEsomas == arr[j + 1].trocasEsomas &&
                 obterPrioridade(arr[j].siglaAlgoritmo) > obterPrioridade(arr[j + 1].siglaAlgoritmo))) {
                // Troca os elementos
                SortTrocaMaisChamada temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}


// Função para trocar os valores de dois ponteiros e contar a troca
void troca(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
    contadorTrocas++;  // Incrementa o contador de trocas
}


int medianaDeTres(int arr[], int baixo, int alto, int ehLomuto) {
    int n =  alto - baixo + 1;  // Ajusta o tamanho do array, pois Hoare não inclui o último elemento no cálculo
    int n1 = baixo + n / 4;           // V1 = V[n/4]
    int n2 = baixo + n / 2;           // V2 = V[n/2]
    int n3 = baixo + 3 * n / 4;       // V3 = V[3n/4]

    // Verifica qual elemento é a mediana sem realizar trocas
    if ((arr[n1] < arr[n2] && arr[n2] < arr[n3]) || (arr[n3] < arr[n2] && arr[n2] < arr[n1])) {
        return n2; // arr[n2] é a mediana
    } else if ((arr[n2] < arr[n1] && arr[n1] < arr[n3]) || (arr[n3] < arr[n1] && arr[n1] < arr[n2])) {
        return n1; // arr[n1] é a mediana
    } else {
        return n3; // arr[n3] é a mediana
    }
}


//----------------[ Função de particionamento Lomuto ]----------------

int particionaLomuto(int arr[], int baixo, int alto) {
    int pivo = arr[alto];
    int i = baixo - 1;

    for (int j = baixo; j < alto; j++) {
        if (arr[j] <= pivo)
            troca(&arr[++i], &arr[j]);
    }
    troca(&arr[++i], &arr[alto]);
    return i;
}

void quickSortLomuto(int arr[], int baixo, int alto) {
    incrementaContadorChamadas();  // Incrementa a chamada da função

    if (baixo < alto) {
        int pivo = particionaLomuto(arr, baixo, alto);
        quickSortLomuto(arr, baixo, pivo - 1);
        quickSortLomuto(arr, pivo + 1, alto);
    }
}

//----------------[ Função de particionamento Lomuto com mediana de três ]----------------

// Função para particionar o array usando a mediana de três
int particionaMedianaDeTres(int arr[], int baixo, int alto) {
    int pivo = medianaDeTres(arr, baixo, alto, 1);
    troca(&arr[pivo], &arr[alto]);
    return particionaLomuto(arr, baixo, alto);
}

// Função Recursiva de QuickSort com mediana de três
void quickSortMedianaDeTres(int arr[], int baixo, int alto) {
    incrementaContadorChamadas();  // Incrementa a chamada da função
    if (baixo < alto) {
        int pivo = particionaMedianaDeTres(arr, baixo, alto);
        quickSortMedianaDeTres(arr, baixo, pivo - 1);
        quickSortMedianaDeTres(arr, pivo + 1, alto);
    }
}

//----------------[ Função de particionamento Lomuto com pivô aleatório ]----------------

int particionaLomutoAleatorio(int arr[], int baixo, int alto) {
    int n = alto - baixo + 1;
    int aleatorio = baixo + abs(arr[baixo]) % n;
    troca(&arr[aleatorio], &arr[alto]);
    return particionaLomuto(arr, baixo, alto);
}

void quickSortLomutoAleatorio(int arr[], int baixo, int alto) {
    incrementaContadorChamadas();  // Incrementa a chamada da função

    if (baixo < alto) {
        int pivo = particionaLomutoAleatorio(arr, baixo, alto);
        quickSortLomutoAleatorio(arr, baixo, pivo - 1);
        quickSortLomutoAleatorio(arr, pivo + 1, alto);
    }
}

//----------------[ Função de particionamento Hoare ]----------------

int particionaHoare(int arr[], int baixo, int alto) {
    int pivo = arr[baixo];  // Assume que o pivô está na posição inicial
    int i = baixo - 1;
    int j = alto + 1;

    while (1) {
        while(arr[--j] > pivo);  // Encontra o elemento menor que o pivô
        while(arr[++i] < pivo);  // Encontra o elemento maior que o pivô

        if (i < j) {
            troca(&arr[i], &arr[j]);
        } else {
            return j;
        }
    }
}

void quickSortHoare(int arr[], int baixo, int alto) {
    incrementaContadorChamadas();  // Incrementa a chamada da função

    if (baixo < alto) {
        int pivo = particionaHoare(arr, baixo, alto);
        quickSortHoare(arr, baixo, pivo);
        quickSortHoare(arr, pivo + 1, alto);
    }
}

//----------------[ Função de particionamento Hoare com mediana de três ]----------------

int particionaHoareV2(int arr[], int baixo, int alto) {
    int pivo = arr[baixo];  // Assume que o pivô está na posição inicial
    int i = baixo - 1;
    int j = alto + 1;

    while (1) {
        do {
            i++;
        } while (arr[i] < pivo);

        do {
            j--;
        } while (arr[j] > pivo);

        if (i >= j) {
            return j;
        }
        troca(&arr[i], &arr[j]);
    }
}

int particionaHoareMedianaDeTres(int arr[], int baixo, int alto) {
    int pivo = medianaDeTres(arr, baixo, alto, 0);
    troca(&arr[pivo], &arr[baixo]);
    return particionaHoareV2(arr, baixo, alto);
}

void quickSortHoareMediana(int arr[], int baixo, int alto) {
    incrementaContadorChamadas();
    if (baixo < alto) {
        int pivo = particionaHoareMedianaDeTres(arr, baixo, alto);
        quickSortHoareMediana(arr, baixo, pivo);
        quickSortHoareMediana(arr, pivo + 1, alto);
    }
}

//----------------[ Função de particionamento Hoare com pivô aleatório ]----------------

int particionaHoareAleatorio(int arr[], int baixo, int alto) {
    int n = alto - baixo + 1;
    int aleatorio = baixo + abs(arr[baixo]) % n;
    troca(&arr[baixo], &arr[aleatorio]);
    return particionaHoare(arr, baixo, alto);
}

void quickSortHoareAleatorio(int arr[], int baixo, int alto) {
    incrementaContadorChamadas();  // Incrementa a chamada da função

    if (baixo < alto) {
        int pivo = particionaHoareAleatorio(arr, baixo, alto);
        quickSortHoareAleatorio(arr, baixo, pivo);
        quickSortHoareAleatorio(arr, pivo + 1, alto);
    }
}

// Função auxiliar para executar e imprimir resultados
void executarOrdenacao(void (*algoritmo)(int*, int, int), int arr[], int n, char *sigla, FILE *outputArq, SortTrocaMaisChamada SortTrocaMaisChamada[], int posicao) {
    contadorChamadas = 0;  // Reinicia os contadores
    contadorTrocas = 0;

    // Executa o algoritmo de ordenação
    algoritmo(arr, 0, n - 1);

    // Salva o resultado no arquivo de saída
    SortTrocaMaisChamada[posicao].trocasEsomas = contadorChamadas + contadorTrocas;
    strcpy(SortTrocaMaisChamada[posicao].siglaAlgoritmo, sigla);

}

// Processa os documentos
void processarDocumentos(char *arqInput, char *arqOutput) {
    FILE *inputArq = fopen(arqInput, "r");
    FILE *outputArq = fopen(arqOutput, "w");

    if (!inputArq || !outputArq) {
        printf("Erro ao abrir os arquivos\n");
        fclose(inputArq);
        fclose(outputArq);
        return;
    }

    int qtdListas, tamanhoLista;
    SortTrocaMaisChamada SortTrocaMaisChamada[6] = {0};
    

    if (fscanf(inputArq, "%d", &qtdListas) != 1) {
        printf("Erro ao ler a linha\n");
        fclose(inputArq);
        fclose(outputArq);
        return;
    }

    for (int i = 0; i < qtdListas; i++) {
        if (fscanf(inputArq, "%d", &tamanhoLista) != 1) {
            printf("Erro ao ler a linha\n");
            fclose(inputArq);
            fclose(outputArq);
            return;
        }

        // Aloca o array original
        int *arrayOriginal = malloc(tamanhoLista * sizeof(int));
        if (!arrayOriginal) {
            printf("Erro ao alocar memória\n");
            fclose(inputArq);
            fclose(outputArq);
            return;
        }

        // Lê os elementos para o array original
        for (int j = 0; j < tamanhoLista; j++) {
            if (fscanf(inputArq, "%d", &arrayOriginal[j]) != 1) {
                printf("Erro ao ler a linha\n");
                free(arrayOriginal);
                fclose(inputArq);
                fclose(outputArq);
                return;
            }
        }

        // Aloca um buffer temporário
        int *tempArray = malloc(tamanhoLista * sizeof(int));
        if (!tempArray) {
            printf("Erro ao alocar memória para o buffer temporário\n");
            free(arrayOriginal);
            fclose(inputArq);
            fclose(outputArq);
            return;
        }

        // Executa os algoritmos de ordenação
        fprintf(outputArq, "%d:N(%d)", i, tamanhoLista);
        // O memcpy é usado para copiar o array original para o array temporário
        memcpy(tempArray, arrayOriginal, tamanhoLista * sizeof(int));
        executarOrdenacao(quickSortLomuto, tempArray, tamanhoLista, "LP", outputArq, SortTrocaMaisChamada, 0);

        memcpy(tempArray, arrayOriginal, tamanhoLista * sizeof(int));
        executarOrdenacao(quickSortMedianaDeTres, tempArray, tamanhoLista, "LM", outputArq, SortTrocaMaisChamada, 1);

        memcpy(tempArray, arrayOriginal, tamanhoLista * sizeof(int));
        executarOrdenacao(quickSortLomutoAleatorio, tempArray, tamanhoLista, "LA", outputArq, SortTrocaMaisChamada, 2);

        memcpy(tempArray, arrayOriginal, tamanhoLista * sizeof(int));
        executarOrdenacao(quickSortHoare, tempArray, tamanhoLista, "HP", outputArq, SortTrocaMaisChamada, 3);

        memcpy(tempArray, arrayOriginal, tamanhoLista * sizeof(int));
        executarOrdenacao(quickSortHoareMediana, tempArray, tamanhoLista, "HM", outputArq, SortTrocaMaisChamada, 4);

        memcpy(tempArray, arrayOriginal, tamanhoLista * sizeof(int));
        executarOrdenacao(quickSortHoareAleatorio, tempArray, tamanhoLista, "HA", outputArq, SortTrocaMaisChamada, 5);

       /*
        critérios de ordenação são:
        1 - Número total de operações (trocas + chamadas): Ordenar em ordem crescente (menos operações vem primeiro).
        2 - Desempate pela sigla: Se o número de operações for igual, usa-se a seguinte ordem de prioridade:
        LP > LM > LA > HP > HM > HA (mais à esquerda tem mais prioridade).
       */
        // Ordena o array de acordo com o número de trocas
        ordenarAlgoritmos(SortTrocaMaisChamada, 6);
        
        // Imprime o array ordenado
        for (int i = 0; i < 6; i++) {
            if(i == 5) {
                fprintf(outputArq, "%s(%d)", SortTrocaMaisChamada[i].siglaAlgoritmo, SortTrocaMaisChamada[i].trocasEsomas);
                break;
            }
            fprintf(outputArq, "%s(%d),", SortTrocaMaisChamada[i].siglaAlgoritmo, SortTrocaMaisChamada[i].trocasEsomas);
        }

        fprintf(outputArq, "\n");

        // Libera a memória alocada
        free(arrayOriginal);
        free(tempArray);
    }

    fclose(inputArq);
    fclose(outputArq);
}

int main(int argc, char **argv) {
   if(argc < 3) {
        printf("Uso: %s <tamanho do array>\n", argv[0]);
        return 1;
    }

    processarDocumentos(argv[1], argv[2]);

    return 0;

}
