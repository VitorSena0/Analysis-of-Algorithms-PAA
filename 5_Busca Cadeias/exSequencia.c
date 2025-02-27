#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define TAMANHO_CODIGO 10 // Tamanho máximo do código de identificação da doença

// Estrutura para representar uma doença com seus dados associados
typedef struct {
    char codigo[TAMANHO_CODIGO]; // Código único de identificação
    double probabilidade;        // Probabilidade calculada
    int ordem;                   // Ordem original para desempate
    int numGenes;                // Número total de genes analisados
} Doenca;

// Variáveis globais para uso nas funções de comparação
static const char *cadeiaGenesGlobal;    // Cadeia principal de genes
static int tamanhoSubcadeiaGlobal;       // Comprimento alvo das subcadeias

// Função de comparação para ordenar índices de subcadeias
int compararSubcadeias(const void *a, const void *b) {
    int indiceA = *(const int*)a;
    int indiceB = *(const int*)b;
    return strncmp(cadeiaGenesGlobal + indiceA, 
                   cadeiaGenesGlobal + indiceB, 
                   tamanhoSubcadeiaGlobal);
}

/**
 * Implementação customizada do algoritmo QuickSort para manter a estabilidade
 * @param base Ponteiro para o array a ser ordenado
 * @param numElementos Número de elementos no array
 * @param tamanhoElemento Tamanho de cada elemento em bytes
 * @param comparar Função de comparação personalizada
 */
void quickSortCustomizado(void *base, size_t numElementos, size_t tamanhoElemento, 
                         int (*comparar)(const void *, const void *)) {
    if (numElementos <= 1) return;

    char *array = (char *)base;
    size_t indicePivo = numElementos - 1;
    char *pivo = array + indicePivo * tamanhoElemento;

    size_t indiceParticao = 0; // Índice de partição

    for (size_t j = 0; j < numElementos - 1; j++) {
        char *atual = array + j * tamanhoElemento;
        if (comparar(atual, pivo) <= 0) {
            if (indiceParticao != j) {
                // Troca elementos usando memcpy para tipos genéricos
                char temp[tamanhoElemento];
                memcpy(temp, array + indiceParticao * tamanhoElemento, tamanhoElemento);
                memcpy(array + indiceParticao * tamanhoElemento, atual, tamanhoElemento);
                memcpy(atual, temp, tamanhoElemento);
            }
            indiceParticao++;
        }
    }

    // Coloca o pivô na posição correta
    if (indiceParticao != indicePivo) {
        char temp[tamanhoElemento];
        memcpy(temp, array + indiceParticao * tamanhoElemento, tamanhoElemento);
        memcpy(array + indiceParticao * tamanhoElemento, pivo, tamanhoElemento);
        memcpy(pivo, temp, tamanhoElemento);
    }

    // Chamadas recursivas para as partições
    quickSortCustomizado(array, indiceParticao, tamanhoElemento, comparar);
    quickSortCustomizado(array + (indiceParticao + 1) * tamanhoElemento, 
                       numElementos - indiceParticao - 1, tamanhoElemento, comparar);
}

/**
 * Busca binária em subcadeias ordenadas
 * @param subcadeiaAlvo Subcadeia alvo para busca
 * @param indices Array de índices ordenados
 * @param numSubcadeias Número de subcadeias
 * @return 1 se encontrou, 0 caso contrário
 */
int buscaBinariaSubcadeia(const char *subcadeiaAlvo, int *indices, int numSubcadeias) {
    int esquerda = 0, direita = numSubcadeias - 1;
    while (esquerda <= direita) {
        int meio = (esquerda + direita) / 2;
        int resultadoComparacao = strncmp(subcadeiaAlvo, cadeiaGenesGlobal + indices[meio], 
                                       tamanhoSubcadeiaGlobal);
        if (resultadoComparacao == 0) return 1;
        if (resultadoComparacao < 0) direita = meio - 1;
        else esquerda = meio + 1;
    }
    return 0;
}

/**
 * Implementação do algoritmo KMP para busca de padrões
 * @param genePrincipal Cadeia principal de genes
 * @param subcadeia Subcadeia a ser procurada
 * @param tamanhoSubcadeia Comprimento da subcadeia
 * @return 1 se encontrou, 0 caso contrário
 */
int buscaKMP(const char *genePrincipal, const char *subcadeia, int tamanhoSubcadeia) {
    int tamanhoPrincipal = strlen(genePrincipal);
    int tamanhoSub = tamanhoSubcadeia;
    
    if (tamanhoSub == 0) return 0;
    
    // Tabela de falha (maior prefixo que também é sufixo)
    int lps[tamanhoSub];
    lps[0] = 0;
    
    for (int i = 1, len = 0; i < tamanhoSub;) {
        if (subcadeia[i] == subcadeia[len]) {
            lps[i++] = ++len;
        } else {
            if (len != 0) len = lps[len - 1];
            else lps[i++] = 0;
        }
    }

    // Busca principal
    for (int i = 0, j = 0; i < tamanhoPrincipal;) {
        if (subcadeia[j] == genePrincipal[i]) {
            i++;
            j++;
        }
        if (j == tamanhoSub) return 1;
        else if (i < tamanhoPrincipal && subcadeia[j] != genePrincipal[i]) {
            j ? (j = lps[j - 1]) : i++;
        }
    }
    return 0;
}

/**
 * Arredonda números para o inteiro mais próximo
 * @param valor Valor a ser arredondado
 * @return Valor arredondado
 */
double arredondarParaInteiro(double valor) {
    return (valor >= 0) ? (int)(valor + 0.5) : (int)(valor - 0.5);
}

/**
 * Merge para o MergeSort - combina duas partições ordenadas
 * @param doencas Array de doenças
 * @param esquerda Índice esquerdo
 * @param meio Índice do meio
 * @param direita Índice direito
 */
void merge(Doenca *doencas, int esquerda, int meio, int direita) {
    int tamanhoEsquerda = meio - esquerda + 1;
    int tamanhoDireita = direita - meio;
    Doenca *esquerdaArray = malloc(tamanhoEsquerda * sizeof(Doenca));
    Doenca *direitaArray = malloc(tamanhoDireita * sizeof(Doenca));
    
    // Copia dados para arrays temporários
    for (int i = 0; i < tamanhoEsquerda; i++)
        esquerdaArray[i] = doencas[esquerda + i];
    for (int j = 0; j < tamanhoDireita; j++)
        direitaArray[j] = doencas[meio + 1 + j];
    
    // Combina os arrays mantendo a ordem
    int i = 0, j = 0, k = esquerda;
    while (i < tamanhoEsquerda && j < tamanhoDireita) {
        if (esquerdaArray[i].probabilidade > direitaArray[j].probabilidade) {
            doencas[k++] = esquerdaArray[i++];
        } else if (esquerdaArray[i].probabilidade < direitaArray[j].probabilidade) {
            doencas[k++] = direitaArray[j++];
        } else {
            // Mantém ordem original para valores iguais (estabilidade)
            doencas[k++] = esquerdaArray[i++];
        }
    }
    
    // Copia elementos restantes
    while (i < tamanhoEsquerda)
        doencas[k++] = esquerdaArray[i++];
    while (j < tamanhoDireita)
        doencas[k++] = direitaArray[j++];
    
    free(esquerdaArray);
    free(direitaArray);
}

/**
 * Implementação do MergeSort para ordenação estável
 * @param doencas Array de doenças
 * @param esquerda Índice inicial
 * @param direita Índice final
 */
void mergeSort(Doenca *doencas, int esquerda, int direita) {
    if (esquerda < direita) {
        int meio = esquerda + (direita - esquerda) / 2;
        mergeSort(doencas, esquerda, meio);
        mergeSort(doencas, meio + 1, direita);
        merge(doencas, esquerda, meio, direita);
    }
}

/**
 * Imprime as doenças formatadas no arquivo de saída
 * @param doencas Array de doenças
 * @param numDoencas Número de doenças
 * @param arquivoSaida Arquivo de saída
 */
void imprimirDoencas(Doenca *doencas, int numDoencas, FILE *arquivoSaida) {
    for (int i = 0; i < numDoencas; i++) {
        fprintf(arquivoSaida, "%s->%.0f%%\n", 
               doencas[i].codigo, doencas[i].probabilidade);
    }
}

/**
 * Processa o arquivo de entrada e gera o arquivo de saída
 * @param arquivoEntrada Nome do arquivo de entrada
 * @param arquivoSaida Nome do arquivo de saída
 */
void processarArquivo(const char *arquivoEntrada, const char *arquivoSaida) {
    FILE *entrada = fopen(arquivoEntrada, "r");
    FILE *saida = fopen(arquivoSaida, "w");

    if (!entrada || !saida) {
        printf("Erro ao abrir arquivos\n");
        return;
    }

    // Lê comprimento das subcadeias
    int tamanhoSubcadeia;
    fscanf(entrada, "%d\n", &tamanhoSubcadeia);

    // Lê cadeia principal de genes
    char *cadeiaGenes = NULL;
    size_t tamanhoBuffer = 0;
    int tamanhoCadeia = getline(&cadeiaGenes, &tamanhoBuffer, entrada);
    cadeiaGenes[strcspn(cadeiaGenes, "\n")] = '\0';

    // Prepara índices para ordenação
    int numSubcadeias = tamanhoCadeia >= tamanhoSubcadeia ? tamanhoCadeia - tamanhoSubcadeia + 1 : 0;
    int *indices = NULL;
    if (numSubcadeias > 0) {
        indices = malloc(numSubcadeias * sizeof(int));
        for (int i = 0; i < numSubcadeias; i++) indices[i] = i;
        
        // Configura globais para uso na comparação
        cadeiaGenesGlobal = cadeiaGenes;
        tamanhoSubcadeiaGlobal = tamanhoSubcadeia;
        quickSortCustomizado(indices, numSubcadeias, sizeof(int), compararSubcadeias);
    }

    // Lê número de doenças
    int numDoencas;
    fscanf(entrada, "%d\n", &numDoencas);

    Doenca *doencas = malloc(numDoencas * sizeof(Doenca));
    
    // Processa cada doença
    for (int i = 0; i < numDoencas; i++) {
        char *linhaGenes = NULL;
        size_t tamanhoLinha = 0;
        getline(&linhaGenes, &tamanhoLinha, entrada);
        if (!linhaGenes) continue;
        linhaGenes[strcspn(linhaGenes, "\n")] = '\0';

        // Extrai código e número de genes
        char codigo[TAMANHO_CODIGO];
        char *ptrEspaco = strchr(linhaGenes, ' ');
        size_t tamanhoCodigo = ptrEspaco - linhaGenes;
        memcpy(codigo, linhaGenes, tamanhoCodigo);
        codigo[tamanhoCodigo] = '\0';

        int totalGenes = atoi(ptrEspaco + 1);
        ptrEspaco = strchr(ptrEspaco + 1, ' ');
        if (!ptrEspaco) {
            free(linhaGenes);
            continue;
        }
        ptrEspaco++;

        int genesAtivos = 0;
        int genesProcessados = 0;

        // Analisa cada gene
        for (int j = 0; j < totalGenes; j++) {
            char *proxEspaco = strchr(ptrEspaco, ' ');
            size_t tamanhoGene = proxEspaco ? (size_t)(proxEspaco - ptrEspaco) : strlen(ptrEspaco);

            int correspondencias = 0;
            for (size_t k = 0; k < tamanhoGene; k += tamanhoSubcadeia) {
                size_t restante = tamanhoGene - k;
                size_t tamanhoAtual = (restante >= tamanhoSubcadeia) ? tamanhoSubcadeia : restante;

                if (tamanhoAtual == tamanhoSubcadeia && numSubcadeias > 0) {
                    // Usa busca binária para subcadeias completas
                    if (buscaBinariaSubcadeia(ptrEspaco + k, indices, numSubcadeias)) {
                        correspondencias += tamanhoSubcadeia;
                    }
                } else {
                    // Usa KMP para subcadeias menores
                    char subcadeia[tamanhoAtual + 1];
                    strncpy(subcadeia, ptrEspaco + k, tamanhoAtual);
                    subcadeia[tamanhoAtual] = '\0';
                    if (buscaKMP(cadeiaGenes, subcadeia, tamanhoAtual)) {
                        correspondencias += tamanhoAtual;
                    }
                }
            }

            // Verifica se o gene está ativo (>=90% de correspondências)
            if ((double)correspondencias / tamanhoGene >= 0.9) genesAtivos++;
            genesProcessados++;

            ptrEspaco = proxEspaco ? proxEspaco + 1 : ptrEspaco + tamanhoGene;
        }

        // Armazena resultados
        doencas[i].probabilidade = (double)genesAtivos / genesProcessados * 100;
        strcpy(doencas[i].codigo, codigo);
        doencas[i].ordem = i;
        doencas[i].numGenes = totalGenes;

        free(linhaGenes);
    }

    // Arredonda probabilidades para inteiros
    for (int i = 0; i < numDoencas; i++) {
        doencas[i].probabilidade = arredondarParaInteiro(doencas[i].probabilidade);
    }

    // Ordena doenças por probabilidade (decrescente)
    mergeSort(doencas, 0, numDoencas - 1);

    // Gera saída
    imprimirDoencas(doencas, numDoencas, saida);

    // Libera recursos
    free(indices);
    free(cadeiaGenes);
    free(doencas);
    
    fclose(entrada);
    fclose(saida);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Uso: %s arquivo_entrada arquivo_saida\n", argv[0]);
        return 1;
    }
    processarArquivo(argv[1], argv[2]);
    return 0;
}