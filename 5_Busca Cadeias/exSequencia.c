#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define COD 10
#define MAX_GENE 1000

void processarArquivo(const char *input, const char *output);

/*
 * Função: computarLPS
 * Calcula o vetor LPS (Longest Prefix Suffix) para o padrão.
 */
int *computarLPS(const char *padrao, int M) {
    int *lps = malloc(M * sizeof(int));
    if (!lps) {
        fprintf(stderr, "Erro na alocacao de memoria para LPS.\n");
        exit(1);
    }
    int len = 0;
    lps[0] = 0;
    int i = 1;
    while (i < M) {
        if (padrao[i] == padrao[len]) {
            len++;
            lps[i] = len;
            i++;
        } else {
            if (len != 0)
                len = lps[len - 1];
            else {
                lps[i] = 0;
                i++;
            }
        }
    }
    return lps;
}

/*
 * Função: kmpSearch
 * Realiza a busca do padrão no texto utilizando o algoritmo KMP.
 * Retorna 1 se encontrar o padrão, ou 0 caso contrário.
 */
int kmpSearch(const char *texto, const char *padrao) {
    int N = (int)strlen(texto);
    int M = (int)strlen(padrao);
    if (M == 0)
        return 1;
    int *lps = computarLPS(padrao, M);
    int i = 0, j = 0;
    while (i < N) {
        if (texto[i] == padrao[j]) {
            i++;
            j++;
            if (j == M) {
                free(lps);
                return 1;
            }
        } else {
            if (j != 0)
                j = lps[j - 1];
            else
                i++;
        }
    }
    free(lps);
    return 0;
}

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

    // Lendo a cadeia de genes (tamanho variável)
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
    cadeiaGenes[strcspn(cadeiaGenes, "\n")] = '\0';  // Removendo o '\n'

    int numDoencas;
    if (fscanf(arqEntrada, "%d\n", &numDoencas) != 1) {
        fprintf(arqSaida, "Erro ao ler numero de doencas\n");
        free(cadeiaGenes);
        fclose(arqEntrada);
        fclose(arqSaida);
        return;
    }

    // Processamento das doenças
    for (int i = 0; i < numDoencas; i++) {
        char *linhaGenes = NULL;
        size_t tamLinha = 0;

        if (getline(&linhaGenes, &tamLinha, arqEntrada) == -1) {
            fprintf(arqSaida, "Erro ao ler linha dos genes da doenca %d\n", i + 1);
            free(linhaGenes);
            continue;
        }
        linhaGenes[strcspn(linhaGenes, "\n")] = '\0';  // Removendo o '\n'

        // Pegando código da doença
        char codigoGene[COD];
        char *ptr = strchr(linhaGenes, ' ');  // Localiza o primeiro espaço
        if (!ptr) {
            fprintf(arqSaida, "Erro: formato inválido na linha %d\n", i + 1);
            free(linhaGenes);
            continue;
        }

        size_t tamCodigo = ptr - linhaGenes;
        memcpy(codigoGene, linhaGenes, tamCodigo);
        codigoGene[tamCodigo] = '\0';

        // Pegando número de genes
        int numGenes = atoi(ptr + 1);
        ptr = strchr(ptr + 1, ' ');  // Avança para o primeiro gene

        if (!ptr) {
            fprintf(arqSaida, "Erro: formato inválido na linha %d\n", i + 1);
            free(linhaGenes);
            continue;
        }
        ptr++;  // Avança para o primeiro caractere do primeiro gene

        // **Alocação fixa para genes (evita múltiplos mallocs)**
        char genes[numGenes][MAX_GENE];

        // Extraindo os genes manualmente (mais eficiente que strchr)
        for (int j = 0; j < numGenes; j++) {
            char *proxEspaco = strchr(ptr, ' ');

            size_t tamGene = proxEspaco ? (size_t)(proxEspaco - ptr) : strlen(ptr);
            memcpy(genes[j], ptr, tamGene);
            genes[j][tamGene] = '\0';

            if (proxEspaco) ptr = proxEspaco + 1;  // Avança para o próximo gene
        }

        // Exemplo de uso: imprimir no arquivo de saída
        fprintf(arqSaida, "Codigo da doenca: %s\n", codigoGene);
        fprintf(arqSaida, "Numero de genes: %d\n", numGenes);
        for (int j = 0; j < numGenes; j++) {
            fprintf(arqSaida, "Gene %d: %s\n", j, genes[j]);
        }

        free(linhaGenes);
    }

    clock_t fim = clock();
    double tempo = (double)(fim - inicio) / CLOCKS_PER_SEC;
    fprintf(arqSaida, "Tempo de execucao: %.4f segundos\n", tempo);

    free(cadeiaGenes);
    fclose(arqEntrada);
    fclose(arqSaida);
}