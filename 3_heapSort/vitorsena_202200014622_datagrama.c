#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definições e limites
#define MAX_BYTES 1600

// Estrutura para representar um pacote
typedef struct Pacote {
    int numeroPrioridade;   // Número do pacote
    char dados[MAX_BYTES];  // Dados do pacote (em formato hexadecimal)
} Pacote;

// Protótipos das funções
void inserirPacoteNoHeap(Pacote p);
Pacote removerPacoteDoHeap();
int estaHeapVazia();
void heapificarParaCima(int indice);
void heapificarParaBaixo(int indice);
void trocarPacotes(Pacote* a, Pacote* b);
void processarArquivoEntrada(char* nomeArquivoEntrada, char* nomeArquivoSaida);

// Variáveis globais
Pacote *heapDePacotes;
int tamanhoHeap = 0;
int capacidadeHeap;

// Funções para manipulação do heap
void inserirPacoteNoHeap(Pacote p) {
    if (tamanhoHeap >= capacidadeHeap) {
        capacidadeHeap *= 2;
        heapDePacotes = realloc(heapDePacotes, capacidadeHeap * sizeof(Pacote));
    }
    heapDePacotes[tamanhoHeap] = p;
    heapificarParaCima(tamanhoHeap);
    tamanhoHeap++;
}

Pacote removerPacoteDoHeap() {
    if (tamanhoHeap == 0) {
        fprintf(stderr, "Heap está vazio\n");
        exit(1);
    }
    Pacote pacoteRaiz = heapDePacotes[0];
    heapDePacotes[0] = heapDePacotes[--tamanhoHeap];
    heapificarParaBaixo(0);
    return pacoteRaiz;
}

int estaHeapVazia() {
    return tamanhoHeap == 0;
}

void heapificarParaCima(int indice) {
    while (indice > 0 && heapDePacotes[indice].numeroPrioridade < heapDePacotes[(indice - 1) / 2].numeroPrioridade) {
        trocarPacotes(&heapDePacotes[indice], &heapDePacotes[(indice - 1) / 2]);
        indice = (indice - 1) / 2;
    }
}

void heapificarParaBaixo(int indice) {
    int menor = indice;
    int filhoEsquerdo = 2 * indice + 1;
    int filhoDireito = 2 * indice + 2;

    if (filhoEsquerdo < tamanhoHeap && heapDePacotes[filhoEsquerdo].numeroPrioridade < heapDePacotes[menor].numeroPrioridade) {
        menor = filhoEsquerdo;
    }
    if (filhoDireito < tamanhoHeap && heapDePacotes[filhoDireito].numeroPrioridade < heapDePacotes[menor].numeroPrioridade) {
        menor = filhoDireito;
    }
    if (menor != indice) {
        trocarPacotes(&heapDePacotes[indice], &heapDePacotes[menor]);
        heapificarParaBaixo(menor);
    }
}

void trocarPacotes(Pacote* a, Pacote* b) {
    Pacote temp = *a;
    *a = *b;
    *b = temp;
}

// Função para processar a entrada e saída de dados
void processarArquivoEntrada(char* nomeArquivoEntrada, char* nomeArquivoSaida) {
    FILE* arquivoEntrada = fopen(nomeArquivoEntrada, "r");
    FILE* arquivoSaida = fopen(nomeArquivoSaida, "w");

    // Inicialização
    if (!arquivoEntrada || !arquivoSaida) {
        perror("Erro ao abrir arquivo");
        exit(1);
    }

    char buffer[MAX_BYTES];
    int quantidadePacotes, quantidadePacotesBloco;

    if (fscanf(arquivoEntrada, "%d %d", &quantidadePacotes, &quantidadePacotesBloco) != 2) {
        perror("Erro ao ler quantidade de pacotes e pacotes por bloco");
        exit(1);
    }

    capacidadeHeap = quantidadePacotesBloco;
    heapDePacotes = malloc(sizeof(Pacote) * capacidadeHeap);
    int posicaoSequencia = 0;

    for (int i = 0; i < (quantidadePacotes + quantidadePacotesBloco - 1) / quantidadePacotesBloco; i++) {
        for (int j = 0; j < quantidadePacotesBloco; j++) {
            int numeroPacote, tamanhoPacote;
            if (fscanf(arquivoEntrada, "%d %d %[^\n]", &numeroPacote, &tamanhoPacote, buffer) != 3) {
                break;
            }

            // Adicionar vírgulas entre os bytes
            char dadosFormatados[MAX_BYTES];
            int k = 0;
            for (int m = 0; buffer[m] != '\0'; m++) {
                if (buffer[m] == ' ') {
                    dadosFormatados[k++] = ',';
                } else {
                    dadosFormatados[k++] = buffer[m];
                }
            }
            dadosFormatados[k] = '\0';

            Pacote p;
            p.numeroPrioridade = numeroPacote;
            strcpy(p.dados, dadosFormatados);

            inserirPacoteNoHeap(p);
        }

        int primeiraImpressao = 1;
        while (!estaHeapVazia() && heapDePacotes[0].numeroPrioridade == posicaoSequencia) {
            Pacote p = removerPacoteDoHeap();
            if (primeiraImpressao) {
                fprintf(arquivoSaida, "|%s", p.dados);
                primeiraImpressao = 0;
            } else {
                fprintf(arquivoSaida, "|%s", p.dados);
            }
            posicaoSequencia++;
        }
        if (!primeiraImpressao) {
            fprintf(arquivoSaida, "|\n");
        }
    }

    fclose(arquivoEntrada);
    fclose(arquivoSaida);
    free(heapDePacotes);
}

// Função principal
int main(int argc, char *argv[]) {
    // Verifica se a quantidade de argumentos está correta
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <arquivo_entrada> <arquivo_saida>\n", argv[0]);
        return 1;
    }

    // Processamento de entrada
    processarArquivoEntrada(argv[1], argv[2]);

    return 0;
}