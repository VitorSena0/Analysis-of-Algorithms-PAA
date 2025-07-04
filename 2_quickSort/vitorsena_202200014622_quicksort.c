#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>  // Para usar ceil()

int contagemTrocas = 0;  // Contador de trocas
int contagemChamadas = 0; // Contador de chamadas

typedef struct EstatisticasAlgoritmo {
    int operacoesTotais;
    char siglaAlgoritmo[3];
} EstatisticasAlgoritmo;

// Função para incrementar o contador de chamadas
void incrementarChamadas() {
    contagemChamadas++;
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
void ordenarEstatisticas(EstatisticasAlgoritmo arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            // Primeiro critério: número de trocas e chamadas (crescente)
            if (arr[j].operacoesTotais > arr[j + 1].operacoesTotais ||
                // Segundo critério: desempate pela prioridade da sigla (crescente)
                (arr[j].operacoesTotais == arr[j + 1].operacoesTotais &&
                 obterPrioridade(arr[j].siglaAlgoritmo) > obterPrioridade(arr[j + 1].siglaAlgoritmo))) {
                // Troca os elementos
                EstatisticasAlgoritmo temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

// Função para trocar os valores de dois ponteiros e contar a troca
void realizarTroca(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
    contagemTrocas++;  // Incrementa o contador de trocas
}

// Função para calcular a mediana estável de três elementos
int encontrarMedianaEstavel(int* array, int idx1, int idx2, int idx3) {
    int a = array[idx1], b = array[idx2], c = array[idx3];

    if ((a < b && b < c) || (c < b && b < a)) return idx2; // Mediana é b
    if ((b < a && a < c) || (c < a && a < b)) return idx1; // Mediana é a
    if ((b < c && c < a) || (a < c && c < b)) return idx3; // Mediana é c

    // Empates (valores iguais): garante estabilidade pelo índice
    if (a == b && b == c) return idx1; // Todos iguais, retorna o menor índice
    if (a == b) return idx1 < idx2 ? idx1 : idx2;
    if (b == c) return idx2 < idx3 ? idx2 : idx3;
    if (a == c) return idx1 < idx3 ? idx1 : idx3;

    return idx1; // Caso não tratado (não deve ocorrer)
}

// Função para obter a mediana de três elementos usando a mediana estável
int obterMedianaDeTres(int arr[], int baixo, int alto, int ehLomuto) {
    int n = alto - baixo + 1;  // Ajusta o tamanho do array
    int idx1 = baixo + n / 4;           // V1 = V[n/4]
    int idx2 = baixo + n / 2;           // V2 = V[n/2]
    int idx3 = baixo + 3 * n / 4;       // V3 = V[3n/4]

    // Usa a função encontrarMedianaEstavel para determinar a mediana
    return encontrarMedianaEstavel(arr, idx1, idx2, idx3);
}

//----------------[ Função de particionamento Lomuto ]----------------

int particionarLomuto(int arr[], int baixo, int alto) {
    int pivo = arr[alto];
    int i = baixo - 1;

    for (int j = baixo; j < alto; j++) {
        if (arr[j] <= pivo)
            realizarTroca(&arr[++i], &arr[j]);
    }
    realizarTroca(&arr[++i], &arr[alto]);
    return i;
}

void quickSortLomuto(int arr[], int baixo, int alto) {
    incrementarChamadas();  // Incrementa a chamada da função

    if (baixo < alto) {
        int pivo = particionarLomuto(arr, baixo, alto);
        quickSortLomuto(arr, baixo, pivo - 1);
        quickSortLomuto(arr, pivo + 1, alto);
    }
}

//----------------[ Função de particionamento Lomuto com mediana de três ]----------------

// Função para particionar o array usando a mediana de três
int particionarMedianaDeTres(int arr[], int baixo, int alto) {
    int pivo = obterMedianaDeTres(arr, baixo, alto, 1);
    realizarTroca(&arr[pivo], &arr[alto]);
    return particionarLomuto(arr, baixo, alto);
}

// Função Recursiva de QuickSort com mediana de três
void quickSortMedianaDeTres(int arr[], int baixo, int alto) {
    incrementarChamadas();  // Incrementa a chamada da função
    if (baixo < alto) {
        int pivo = particionarMedianaDeTres(arr, baixo, alto);
        quickSortMedianaDeTres(arr, baixo, pivo - 1);
        quickSortMedianaDeTres(arr, pivo + 1, alto);
    }
}

//----------------[ Função de particionamento Lomuto com pivô aleatório ]----------------

int particionarLomutoAleatorio(int arr[], int baixo, int alto) {
    int n = alto - baixo + 1;
    int aleatorio = baixo + abs(arr[baixo]) % n;
    realizarTroca(&arr[aleatorio], &arr[alto]);
    return particionarLomuto(arr, baixo, alto);
}

void quickSortLomutoAleatorio(int arr[], int baixo, int alto) {
    incrementarChamadas();  // Incrementa a chamada da função

    if (baixo < alto) {
        int pivo = particionarLomutoAleatorio(arr, baixo, alto);
        quickSortLomutoAleatorio(arr, baixo, pivo - 1);
        quickSortLomutoAleatorio(arr, pivo + 1, alto);
    }
}

//----------------[ Função de particionamento Hoare ]----------------

int particionarHoare(int arr[], int baixo, int alto) {
    int pivo = arr[baixo];  // Assume que o pivô está na posição inicial
    int i = baixo - 1;
    int j = alto + 1;

    while (1) {
        while (arr[--j] > pivo);  // Encontra o elemento menor que o pivô
        while (arr[++i] < pivo);  // Encontra o elemento maior que o pivô

        if (i < j) {
            realizarTroca(&arr[i], &arr[j]);
        } else {
            return j;
        }
    }
}

void quickSortHoare(int arr[], int baixo, int alto) {
    incrementarChamadas();  // Incrementa a chamada da função

    if (baixo < alto) {
        int pivo = particionarHoare(arr, baixo, alto);
        quickSortHoare(arr, baixo, pivo);
        quickSortHoare(arr, pivo + 1, alto);
    }
}

//----------------[ Função de particionamento Hoare com mediana de três ]----------------

int particionarHoareV2(int arr[], int baixo, int alto) {
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
        realizarTroca(&arr[i], &arr[j]);
    }
}

int particionarHoareMedianaDeTres(int arr[], int baixo, int alto) {
    int pivo = obterMedianaDeTres(arr, baixo, alto, 0);
    realizarTroca(&arr[pivo], &arr[baixo]);
    return particionarHoareV2(arr, baixo, alto);
}

void quickSortHoareMediana(int arr[], int baixo, int alto) {
    incrementarChamadas();
    if (baixo < alto) {
        int pivo = particionarHoareMedianaDeTres(arr, baixo, alto);
        quickSortHoareMediana(arr, baixo, pivo);
        quickSortHoareMediana(arr, pivo + 1, alto);
    }
}

//----------------[ Função de particionamento Hoare com pivô aleatório ]----------------

int particionarHoareAleatorio(int arr[], int baixo, int alto) {
    int n = alto - baixo + 1;
    int aleatorio = baixo + abs(arr[baixo]) % n;
    realizarTroca(&arr[baixo], &arr[aleatorio]);
    return particionarHoare(arr, baixo, alto);
}

void quickSortHoareAleatorio(int arr[], int baixo, int alto) {
    incrementarChamadas();  // Incrementa a chamada da função

    if (baixo < alto) {
        int pivo = particionarHoareAleatorio(arr, baixo, alto);
        quickSortHoareAleatorio(arr, baixo, pivo);
        quickSortHoareAleatorio(arr, pivo + 1, alto);
    }
}

// Função auxiliar para executar e imprimir resultados
void executarOrdenacao(void (*algoritmo)(int*, int, int), int arr[], int n, char *sigla, FILE *outputArq, EstatisticasAlgoritmo estatisticas[], int posicao) {
    contagemChamadas = 0;  // Reinicia os contadores
    contagemTrocas = 0;

    // Executa o algoritmo de ordenação
    algoritmo(arr, 0, n - 1);

    // Salva o resultado no arquivo de saída
    estatisticas[posicao].operacoesTotais = contagemChamadas + contagemTrocas;
    strcpy(estatisticas[posicao].siglaAlgoritmo, sigla);
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
    EstatisticasAlgoritmo estatisticas[6] = {0};

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
        fprintf(outputArq, "%d:N(%d),", i, tamanhoLista);
        // O memcpy é usado para copiar o array original para o array temporário
        memcpy(tempArray, arrayOriginal, tamanhoLista * sizeof(int));
        executarOrdenacao(quickSortLomuto, tempArray, tamanhoLista, "LP", outputArq, estatisticas, 0);

        memcpy(tempArray, arrayOriginal, tamanhoLista * sizeof(int));
        executarOrdenacao(quickSortMedianaDeTres, tempArray, tamanhoLista, "LM", outputArq, estatisticas, 1);

        memcpy(tempArray, arrayOriginal, tamanhoLista * sizeof(int));
        executarOrdenacao(quickSortLomutoAleatorio, tempArray, tamanhoLista, "LA", outputArq, estatisticas, 2);

        memcpy(tempArray, arrayOriginal, tamanhoLista * sizeof(int));
        executarOrdenacao(quickSortHoare, tempArray, tamanhoLista, "HP", outputArq, estatisticas, 3);

        memcpy(tempArray, arrayOriginal, tamanhoLista * sizeof(int));
        executarOrdenacao(quickSortHoareMediana, tempArray, tamanhoLista, "HM", outputArq, estatisticas, 4);

        memcpy(tempArray, arrayOriginal, tamanhoLista * sizeof(int));
        executarOrdenacao(quickSortHoareAleatorio, tempArray, tamanhoLista, "HA", outputArq, estatisticas, 5);

        /*
         critérios de ordenação são:
         1 - Número total de operações (trocas + chamadas): Ordenar em ordem crescente (menos operações vem primeiro).
         2 - Desempate pela sigla: Se o número de operações for igual, usa-se a seguinte ordem de prioridade:
         LP > LM > LA > HP > HM > HA (mais à esquerda tem mais prioridade).
        */
        // Ordena o array de acordo com o número de trocas
        ordenarEstatisticas(estatisticas, 6);

        // Imprime o array ordenado
        for (int i = 0; i < 6; i++) {
            if (i == 5) {
                fprintf(outputArq, "%s(%d)", estatisticas[i].siglaAlgoritmo, estatisticas[i].operacoesTotais);
                break;
            }
            fprintf(outputArq, "%s(%d),", estatisticas[i].siglaAlgoritmo, estatisticas[i].operacoesTotais);
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
    if (argc < 3) {
        printf("Uso: %s <arquivo de entrada> <arquivo de saída>\n", argv[0]);
        return 1;
    }

    processarDocumentos(argv[1], argv[2]);

    return 0;
}