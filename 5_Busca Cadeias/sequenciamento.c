#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define COD 10

typedef struct {
    char codigo[COD];
    double probabilidade;
    int ordem; // Para manter a ordem de leitura original
    int numGenes; // Para armazenar o número de genes
} Doenca;

void processarArquivo(const char *input, const char *output);
void quickSort(Doenca *doencas, int low, int high);
int partition(Doenca *doencas, int low, int high);
int kmp_search(const char *main_gene, const char *subchain, int subchain_len);
double round_custom(double value);

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Erro: Numero de argumentos invalido\n");
        return -1;
    }

    processarArquivo(argv[1], argv[2]);
    return 0;
}

void processarArquivo(const char *input, const char *output) {
    clock_t inicio = clock();

    FILE *arqEntrada = fopen(input, "r");
    FILE *arqSaida = fopen(output, "w");

    if (!arqEntrada || !arqSaida) {
        printf("Erro ao abrir arquivos\n");
        return;
    }

    int tamanhoSubcadeia;
    if (fscanf(arqEntrada, "%d\n", &tamanhoSubcadeia) != 1) {
        fprintf(arqSaida, "Erro ao ler tamanho da subcadeia\n");
        fclose(arqEntrada);
        fclose(arqSaida);
        return;
    }

    char *cadeiaGenes = NULL;
    size_t tamanhoCadeia = 0;
    ssize_t linhaLida = getline(&cadeiaGenes, &tamanhoCadeia, arqEntrada);

    if (linhaLida == -1) {
        fprintf(arqSaida, "Erro ao ler cadeia de genes\n");
        free(cadeiaGenes);
        fclose(arqEntrada);
        fclose(arqSaida);
        return;
    }
    cadeiaGenes[strcspn(cadeiaGenes, "\n")] = '\0';

    int numDoencas;
    if (fscanf(arqEntrada, "%d\n", &numDoencas) != 1) {
        fprintf(arqSaida, "Erro ao ler numero de doencas\n");
        free(cadeiaGenes);
        fclose(arqEntrada);
        fclose(arqSaida);
        return;
    }

    Doenca *doencas = malloc(numDoencas * sizeof(Doenca));
    int doencaIndex = 0;

    for (int i = 0; i < numDoencas; i++) {
        char *linhaGenes = NULL;
        size_t tamLinha = 0;

        if (getline(&linhaGenes, &tamLinha, arqEntrada) == -1) {
            fprintf(arqSaida, "Erro ao ler linha dos genes da doenca %d\n", i + 1);
            free(linhaGenes);
            continue;
        }
        linhaGenes[strcspn(linhaGenes, "\n")] = '\0';

        char codigoGene[COD];
        char *ptr = strchr(linhaGenes, ' ');
        if (!ptr) {
            fprintf(arqSaida, "Erro: formato inválido na linha %d\n", i + 1);
            free(linhaGenes);
            continue;
        }

        size_t tamCodigo = ptr - linhaGenes;
        memcpy(codigoGene, linhaGenes, tamCodigo);
        codigoGene[tamCodigo] = '\0';

        int numGenes = atoi(ptr + 1);
        ptr = strchr(ptr + 1, ' ');

        if (!ptr) {
            fprintf(arqSaida, "Erro: formato inválido na linha %d\n", i + 1);
            free(linhaGenes);
            continue;
        }
        ptr++;

        int geneCount = 0;
        int activeGeneCount = 0;

        for (int j = 0; j < numGenes; j++) {
            char *proxEspaco = strchr(ptr, ' ');
            size_t tamGene = proxEspaco ? (size_t)(proxEspaco - ptr) : strlen(ptr);

            geneCount++;

            int matchCount = 0;
            int totalSubchains = (tamGene + tamanhoSubcadeia - 1) / tamanhoSubcadeia;

            for (size_t k = 0; k < tamGene; k += tamanhoSubcadeia) {
                size_t subchainLen = (k + tamanhoSubcadeia <= tamGene) ? tamanhoSubcadeia : tamGene - k;
                char subchain[subchainLen + 1];
                strncpy(subchain, ptr + k, subchainLen);
                subchain[subchainLen] = '\0';

                if (kmp_search(cadeiaGenes, subchain, subchainLen)) {
                    matchCount += subchainLen;
                }
            }

            if ((double)matchCount / tamGene >= 0.9) {
                activeGeneCount++;
            }

            if (proxEspaco) ptr = proxEspaco + 1;
        }

        doencas[doencaIndex].probabilidade = (double)activeGeneCount / geneCount * 100;
        strcpy(doencas[doencaIndex].codigo, codigoGene);
        doencas[doencaIndex].ordem = doencaIndex;
        doencas[doencaIndex].numGenes = numGenes; // Store the number of genes
        doencaIndex++;

        free(linhaGenes);
    }

    quickSort(doencas, 0, doencaIndex - 1);

    for (int i = 0; i < doencaIndex; i++) {
        doencas[i].probabilidade = round_custom(doencas[i].probabilidade);
        fprintf(arqSaida, "%s->%.0f%%\n", doencas[i].codigo, doencas[i].probabilidade);
    }

    clock_t fim = clock();
    double tempo = (double)(fim - inicio) / CLOCKS_PER_SEC;
    fprintf(arqSaida, "Tempo de execucao: %.4f segundos\n", tempo);

    free(doencas);
    free(cadeiaGenes);
    fclose(arqEntrada);
    fclose(arqSaida);
}

void quickSort(Doenca *doencas, int low, int high) {
    if (low < high) {
        int pi = partition(doencas, low, high);
        quickSort(doencas, low, pi - 1);
        quickSort(doencas, pi + 1, high);
    }
}

int partition(Doenca *doencas, int low, int high) {
    double pivot = doencas[high].probabilidade;
    int ordem_pivot = doencas[high].ordem;
    int numGenes_pivot = doencas[high].numGenes;
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (doencas[j].probabilidade > pivot || 
            (doencas[j].probabilidade == pivot && doencas[j].numGenes > numGenes_pivot) || 
            (doencas[j].probabilidade == pivot && doencas[j].numGenes == numGenes_pivot && doencas[j].ordem < ordem_pivot)) {
            i++;
            Doenca temp = doencas[i];
            doencas[i] = doencas[j];
            doencas[j] = temp;
        }
    }

    Doenca temp = doencas[i + 1];
    doencas[i + 1] = doencas[high];
    doencas[high] = temp;

    return i + 1;
}

int kmp_search(const char *main_gene, const char *subchain, int subchain_len) {
    int m = strlen(main_gene);
    int n = subchain_len;
    int lps[n];

    int len = 0;
    lps[0] = 0;
    int i = 1;
    while (i < n) {
        if (subchain[i] == subchain[len]) {
            len++;
            lps[i] = len;
            i++;
        } else {
            if (len != 0) {
                len = lps[len - 1];
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }

    i = 0;
    int j = 0;
    while (i < m) {
        if (subchain[j] == main_gene[i]) {
            i++;
            j++;
        }

        if (j == n) {
            return 1;
        } else if (i < m && subchain[j] != main_gene[i]) {
            if (j != 0) {
                j = lps[j - 1];
            } else {
                i++;
            }
        }
    }

    return 0;
}

double round_custom(double value) {
    return (value >= 0) ? (int)(value + 0.5) : (int)(value - 0.5);
}