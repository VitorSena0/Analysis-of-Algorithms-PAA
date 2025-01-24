#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
// Compilar - gcc -o .\mergeSortThread .\mergeSortThread.c -pthread
// Executar - .\mergeSortThread.exe
// Verificar tempo terminal - $tempoExecucao = Measure-Command { $saida = ./mergeSortThread.exe }
// $saida  # Imprime a saída do programa
// $tempoExecucao  # Imprime o tempo de execução
#define TAMANHO_ARRAY 100000  // Tamanho do array para ordenar
#define LIMITE_THREADS 10    // Limite de threads para evitar overhead

typedef struct {
    int *array;
    int inicio;
    int fim;
} Args;

// Função de intercalação para combinar dois subarrays ordenados
void intercalar(int *array, int inicio, int meio, int fim) {
    int i = inicio, j = meio + 1, k = 0; // O k é o índice do array temporário
    int *temp = malloc((fim - inicio + 1) * sizeof(int)); // Array temporário para armazenar a intercalação
    
    while (i <= meio && j <= fim) { // Enquanto houver elementos em ambos os subarrays ele vai comparar e adicionar ao array temporário
        if (array[i] <= array[j])
            temp[k++] = array[i++];
        else
            temp[k++] = array[j++];
    }
    while (i <= meio) temp[k++] = array[i++]; // Adiciona os elementos restantes do subarray esquerdo
    while (j <= fim) temp[k++] = array[j++]; // Adiciona os elementos restantes do subarray direito
    
    for (i = inicio, k = 0; i <= fim; i++, k++) { // Copia os elementos ordenados de volta para o array original
        array[i] = temp[k];
    }
    free(temp);
}

// Função de ordenação com Merge Sort recursivo usando threads
void *merge_sort(void *arg) {
    Args *args = (Args *)arg;
    int inicio = args->inicio;
    int fim = args->fim;
    int meio;

    if (inicio < fim) {
        meio = (inicio + fim) / 2;
        
        // Dividir as chamadas recursivas em threads se o limite permitir
        if ((fim - inicio) > TAMANHO_ARRAY / LIMITE_THREADS) {
            pthread_t thread_esquerda, thread_direita;
            Args args_esquerda = {args->array, inicio, meio};
            Args args_direita = {args->array, meio + 1, fim};
            
            pthread_create(&thread_esquerda, NULL, merge_sort, &args_esquerda);
            pthread_create(&thread_direita, NULL, merge_sort, &args_direita);

            pthread_join(thread_esquerda, NULL);
            pthread_join(thread_direita, NULL);
        } else {
            // Se já há threads suficientes, chamamos recursivamente sem threads
            Args args_esquerda = {args->array, inicio, meio};
            Args args_direita = {args->array, meio + 1, fim};
            
            merge_sort(&args_esquerda);
            merge_sort(&args_direita);
        }
        
        // Intercala as duas metades ordenadas
        intercalar(args->array, inicio, meio, fim);
    }
    return NULL;
}

int main() {
    int array[TAMANHO_ARRAY];
    for (int i = 0; i < TAMANHO_ARRAY; i++) {
        array[i] = rand() % 1000;  // Gera um array aleatório
    }

    // Argumentos para a thread principal do Merge Sort
    Args args = {array, 0, TAMANHO_ARRAY - 1};

    // Inicia o Merge Sort paralelo
    pthread_t thread_principal;
    pthread_create(&thread_principal, NULL, merge_sort, &args);
    pthread_join(thread_principal, NULL);

    // Imprime o array ordenado (opcional)
    printf("Array ordenado:\n");
    for (int i = 0; i < TAMANHO_ARRAY; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");

    return 0;
}
