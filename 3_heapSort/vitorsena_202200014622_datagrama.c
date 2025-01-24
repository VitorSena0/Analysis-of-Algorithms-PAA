#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//-------------[ Definições ]-------------
#define MAX_BYTES 1600

//----------------[ Estruturas ]----------------
typedef struct Pacote {
    int numeroPrioridade;   // Número do pacote, usado para definir a prioridade
    char dados[MAX_BYTES];  // Dados do pacote (em formato hexadecimal)
} Pacote;

//-------------[ Protótipos ]-------------
void inserirPacoteNoHeap(Pacote p);
Pacote removerPacoteDoHeap();
int estaHeapVazia();
void heapificarParaCima(int indice);
void heapificarParaBaixo(int indice);
void trocarPacotes(Pacote* a, Pacote* b);
void processarArquivoEntrada(char* nomeArquivoEntrada, char* nomeArquivoSaida);

//-------------[ Variáveis Globais ]-------------
Pacote *heapDePacotes;  // Ponteiro para o heap de pacotes
int tamanhoHeap = 0;    // Tamanho atual do heap
int capacidadeHeap;     // Capacidade máxima do heap

//--------------[ Manipulação do Heap ]--------------
void inserirPacoteNoHeap(Pacote p) {
    // Verifica se o heap está cheio e realoca memória se necessário
    if (tamanhoHeap >= capacidadeHeap) {
        capacidadeHeap *= 2;
        heapDePacotes = realloc(heapDePacotes, capacidadeHeap * sizeof(Pacote));
    }
    // Insere o novo pacote no final do heap
    heapDePacotes[tamanhoHeap] = p;
    // Ajusta a posição do novo pacote para manter a propriedade do heap
    heapificarParaCima(tamanhoHeap);
    tamanhoHeap++;
}

//---------------[Remove o pacote de maior prioridade do heap]---------------
Pacote removerPacoteDoHeap() {
    // Verifica se o heap está vazio
    if (tamanhoHeap == 0) {
        fprintf(stderr, "Heap está vazio\n");
        exit(1);
    }
    // Remove o pacote raiz (de maior prioridade)
    Pacote pacoteRaiz = heapDePacotes[0];
    heapDePacotes[0] = heapDePacotes[--tamanhoHeap];
    // Ajusta a posição do novo pacote raiz para manter a propriedade do heap
    heapificarParaBaixo(0);
    return pacoteRaiz;
}

//---------------[Verifica se o heap está vazio]---------------
int estaHeapVazia() {
    // Retorna verdadeiro se o heap estiver vazio
    return tamanhoHeap == 0;
}

//---------------[Heapificação UP]---------------
void heapificarParaCima(int indice) {
    // Ajusta a posição do pacote subindo no heap enquanto necessário
    while (indice > 0 && heapDePacotes[indice].numeroPrioridade < heapDePacotes[(indice - 1) / 2].numeroPrioridade) {
        trocarPacotes(&heapDePacotes[indice], &heapDePacotes[(indice - 1) / 2]);
        indice = (indice - 1) / 2;
    }
}

//---------------[Heapificação Down]---------------
void heapificarParaBaixo(int indice) {
    int menor = indice;
    int filhoEsquerdo = 2 * indice + 1;
    int filhoDireito = 2 * indice + 2;

    // Ajusta a posição do pacote descendo no heap enquanto necessário
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

//---------------[Troca dois pacotes de posição no heap]---------------
void trocarPacotes(Pacote* a, Pacote* b) {
    // Troca dois pacotes de posição no heap
    Pacote temp = *a;
    *a = *b;
    *b = temp;
}

//---------------[Processa o arquivo de entrada]---------------
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

    // Lê a quantidade de pacotes e a quantidade de pacotes por bloco do arquivo de entrada
    if (fscanf(arquivoEntrada, "%d %d", &quantidadePacotes, &quantidadePacotesBloco) != 2) {
        perror("Erro ao ler quantidade de pacotes e pacotes por bloco");
        exit(1);
    }

    capacidadeHeap = quantidadePacotesBloco;
    heapDePacotes = malloc(sizeof(Pacote) * capacidadeHeap);
    int posicaoSequencia = 0;

    // Processa os pacotes em blocos
    for (int i = 0; i < (quantidadePacotes + quantidadePacotesBloco - 1) / quantidadePacotesBloco; i++) {
        // Lê os pacotes do bloco atual
        for (int j = 0; j < quantidadePacotesBloco; j++) {
            int numeroPacote, tamanhoPacote;
            // Lê o número do pacote, o tamanho do pacote e os dados do pacote
            if (fscanf(arquivoEntrada, "%d %d %[^\n]", &numeroPacote, &tamanhoPacote, buffer) != 3) {
                break;
            }

            // Adiciona vírgulas entre os bytes dos dados
            char dadosFormatados[MAX_BYTES];
            int k = 0;
            // Formata os dados do pacote
            for (int m = 0; buffer[m] != '\0'; m++) {
                if (buffer[m] == ' ') {
                    dadosFormatados[k++] = ',';
                } else {
                    dadosFormatados[k++] = buffer[m];
                }
            }
            dadosFormatados[k] = '\0';

            // Cria um pacote com os dados lidos
            Pacote p;
            p.numeroPrioridade = numeroPacote;
            strcpy(p.dados, dadosFormatados);

            // Insere o pacote no heap
            inserirPacoteNoHeap(p);
        }

        int primeiraImpressao = 1;
        // Remove e imprime os pacotes do heap na ordem correta
        while (!estaHeapVazia() && heapDePacotes[0].numeroPrioridade == posicaoSequencia) {
            // Remove o pacote de maior prioridade do heap
            Pacote p = removerPacoteDoHeap();
            if (primeiraImpressao) {
                fprintf(arquivoSaida, "|%s", p.dados);
                primeiraImpressao = 0;
            } else {
                fprintf(arquivoSaida, "|%s", p.dados);
            }
            posicaoSequencia++;
        }
        // Imprime uma nova linha após cada bloco de pacotes
        if (!primeiraImpressao) {
            fprintf(arquivoSaida, "|\n");
        }
    }

    // Fecha os arquivos e libera a memória
    fclose(arquivoEntrada);
    fclose(arquivoSaida);
    free(heapDePacotes);
}

//---------------[Função Principal]---------------
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