#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>  // Para usar ceil()

int contadorTrocas = 0;  // Contador de trocas
int contadorChamadas = 0; // Contador de chamadas

// Função para incrementar o contador de chamadas
void incrementaContadorChamadas() {
    contadorChamadas++;  
}

// Função para trocar os valores de dois ponteiros e contar a troca
void troca(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
    contadorTrocas++;  // Incrementa o contador de trocas
}


int medianaDeTres(int arr[], int baixo, int alto, int ehLomuto) {
    int n = ehLomuto == 1 || ehLomuto == 0? alto - baixo + 1 : alto - baixo;  // Ajusta o tamanho do array, pois Hoare não inclui o último elemento no cálculo
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
void executarOrdenacao(void (*algoritmo)(int*, int, int), int arr[], int n, char *sigla, FILE *outputArq) {
    contadorChamadas = 0;  // Reinicia os contadores
    contadorTrocas = 0;

    // Executa o algoritmo de ordenação
    algoritmo(arr, 0, n - 1);

    // Salva o resultado no arquivo de saída
    fprintf(outputArq, "%s(%d) ", sigla, contadorChamadas + contadorTrocas);
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
        fprintf(outputArq, "%d: N(%d) ", i, tamanhoLista);
        // O memcpy é usado para copiar o array original para o array temporário
        memcpy(tempArray, arrayOriginal, tamanhoLista * sizeof(int));
        executarOrdenacao(quickSortLomuto, tempArray, tamanhoLista, "LP", outputArq);

        memcpy(tempArray, arrayOriginal, tamanhoLista * sizeof(int));
        executarOrdenacao(quickSortMedianaDeTres, tempArray, tamanhoLista, "LM", outputArq);

        memcpy(tempArray, arrayOriginal, tamanhoLista * sizeof(int));
        executarOrdenacao(quickSortLomutoAleatorio, tempArray, tamanhoLista, "LA", outputArq);

        memcpy(tempArray, arrayOriginal, tamanhoLista * sizeof(int));
        executarOrdenacao(quickSortHoare, tempArray, tamanhoLista, "HP", outputArq);

        memcpy(tempArray, arrayOriginal, tamanhoLista * sizeof(int));
        executarOrdenacao(quickSortHoareMediana, tempArray, tamanhoLista, "HM", outputArq);

        memcpy(tempArray, arrayOriginal, tamanhoLista * sizeof(int));
        executarOrdenacao(quickSortHoareAleatorio, tempArray, tamanhoLista, "HA", outputArq);

        fprintf(outputArq, "\n");

        // Libera a memória alocada
        free(arrayOriginal);
        free(tempArray);
    }

    fclose(inputArq);
    fclose(outputArq);
}
//-23, 10, 7, -34, 432, 3
//955, -32, 1, 9
//834, 27, 39, 19, 3, -1, -33
//847, 38, -183, -13, 94, -2, -42, 54, 28, 100
int main(int argc, char **argv) {
   if(argc < 3) {
        printf("Uso: %s <tamanho do array>\n", argv[0]);
        return 1;
    }

    processarDocumentos(argv[1], argv[2]);

    return 0;
    
}