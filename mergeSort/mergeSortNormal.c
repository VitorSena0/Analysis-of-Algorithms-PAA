#include <stdio.h>
#include <stdlib.h>
#include <time.h>
// Compilar - gcc ./mergeSortNormal.c -o ./mergeSortNormal.exe
// Executar - ./mergeSortNormal.exe
// Verificar tempo terminal - $tempoExecucao = Measure-Command { $saida = ./mergeSortNormal.exe }
// $saida  # Imprime a saída do programa
// $tempoExecucao  # Imprime o tempo de execução
#define TAMANHO_ARRAY 100  // Tamanho do array para ordenar

// Função de intercalação para combinar dois subarrays ordenados
void intercalar(int *array, int inicio, int meio, int fim, int *temp) {
    int i = inicio, j = meio + 1, k = 0; // O k é o índice do array temporário
    // Intercala os dois subarrays ordenados
    // Complexidade O(n)
    while (i <= meio && j <= fim) {
        if (array[i] <= array[j])// Complexidade O(1)
            temp[k++] = array[i++];// Complexidade O(1)
        else
            temp[k++] = array[j++]; // Complexidade O(1)
    }
    /*
        Este dois whiles abaixo são para adicionar os elementos restantes dos subarrays
        Assim garantindo que todos os elementos sejam adicionados ao array temporário 
        e depois copiados de volta para o array original
        complexidade O(n)
    */
    while (i <= meio) temp[k++] = array[i++];
    while (j <= fim) temp[k++] = array[j++];
    
    // Copia os elementos ordenados de volta para o array original
    // Complexidade O(n)
    for (i = inicio, k = 0; i <= fim; i++, k++) {
        array[i] = temp[k]; // Complexidade O(1)
    }
    // Complexidade total: O(n + n + n) = O(3n) = O(n)
}

// Função auxiliar de ordenação com Merge Sort recursivo sem usar threads
void merge_sort_aux(int *array, int inicio, int fim, int *temp) {
    if (inicio < fim) {
        int meio = (inicio + fim) / 2;

        // Chama recursivamente para dividir e ordenar as duas metades
        merge_sort_aux(array, inicio, meio, temp);
        merge_sort_aux(array, meio + 1, fim, temp);

        // Intercala as duas metades ordenadas
        intercalar(array, inicio, meio, fim, temp);
    }
}

// Função principal de ordenação com Merge Sort
void merge_sort(int *array, int tamanho) {
    int *temp = malloc(tamanho * sizeof(int));
    if (temp == NULL) {
        fprintf(stderr, "Erro ao alocar memória\n");
        exit(EXIT_FAILURE);
    }
    merge_sort_aux(array, 0, tamanho - 1, temp);
    for(int i = 0; i < tamanho; i++){
        printf("%d ", temp[i]);
    }
    printf("\n");
    printf("\n");
    free(temp);
}

int main() {
    int array[TAMANHO_ARRAY];
    for (int i = 0; i < TAMANHO_ARRAY; i++) {
        array[i] = rand() % 10000;  // Gera um array aleatório, limite do rand() é 32767
    }

    // Inicia a medição do tempo
    clock_t inicio = clock();

    // Executa o Merge Sort sequencial
    merge_sort(array, TAMANHO_ARRAY);

    // Finaliza a medição do tempo
    clock_t fim = clock();
    double tempo_gasto = (double)(fim - inicio) / CLOCKS_PER_SEC;

    // Imprime o array ordenado (opcional)
    printf("Array ordenado:\n");
    for (int i = 0; i < TAMANHO_ARRAY; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");

    // Exibe o tempo de execução
    printf("Tempo de execução (sequencial): %.4f segundos\n", tempo_gasto);

    return 0;
}