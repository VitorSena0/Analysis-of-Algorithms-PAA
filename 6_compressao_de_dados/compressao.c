#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//----------------[ Estrutura de nó para Huffman ]----------------
typedef struct No {
    unsigned char simbolo;  // Armazena o byte (0..255)
    int freq;               // Frequência do símbolo
    struct No *esq, *dir;
} No;

//----------------[ Estrutura de nó para Heap ]----------------
typedef struct {
    No **array;    // Array de ponteiros para nós
    int tamanho;   // Quantidade atual de nós no heap
    int capacidade;// Tamanho máximo do heap
} MinHeap;

//----------------[ Protótipos ]----------------
void processarArquivo(const char *arquivoEntrada, const char *arquivoSaida);
static unsigned char *hexParaBytes(const char *sequenciaHex, int qtdBytes);
static int *contarFrequencia(const unsigned char *dados, int n);
static MinHeap *criarMinHeap(int capacidade);
static void inserirMinHeap(MinHeap *heap, No *novoNo);
static No *extrairMin(MinHeap *heap);
static void troca(No **a, No **b);
static void heapify(MinHeap *heap, int idx);
static No *construirArvoreHuffman(const int *frequencia);
static void gerarCodigosHuffman(No *raiz, char (*codigos)[256], char *atual, int profundidade);
static void liberarArvore(No *raiz);

static char *compactarHuffman(const unsigned char *dados, int n, int *tamanhoCompactado);
static char *compactarRLE(const unsigned char *dados, int n, int *tamanhoCompactado);

static double calcularTaxaCompressao(int tamanhoOriginal, int tamanhoComprimido);
static void imprimirResultado(FILE *saida, int indiceSequencia, 
                              const char *metodo, double taxa, const char *conteudoHex);

// Função auxiliar: converte um caractere hexadecimal em inteiro
static int hexVal(char c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    else if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    else if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    return 0;
}

//----------------[ Função principal ]----------------
int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <arquivo_entrada> <arquivo_saida>\n", argv[0]);
        return 1;
    }
    processarArquivo(argv[1], argv[2]);
    return 0;
}

//----------------[ Processa o arquivo de entrada/saída ]----------------
void processarArquivo(const char *arquivoEntrada, const char *arquivoSaida) {
    FILE *entrada = fopen(arquivoEntrada, "r");
    FILE *saida   = fopen(arquivoSaida, "w");

    if (!entrada || !saida) {
        fprintf(stderr, "Erro ao abrir arquivos\n");
        if (entrada) fclose(entrada);
        if (saida)   fclose(saida);
        return;
    }

    int qtdSequenciasDados;
    fscanf(entrada, "%d", &qtdSequenciasDados);

    for (int i = 0; i < qtdSequenciasDados; i++) {
        int qtdBytesNosDados;
        fscanf(entrada, "%d", &qtdBytesNosDados);
        fgetc(entrada); // Consome o '\n' ou espaço após o número

        // Lê a linha com possíveis espaços (2 hex + 1 espaço) * qtdBytes + sobra
        char buffer[3 * qtdBytesNosDados + 2];
        if (!fgets(buffer, sizeof(buffer), entrada)) {
            fprintf(stderr, "Erro na leitura da sequência\n");
            break;
        }
        // Remove possível '\n'
        buffer[strcspn(buffer, "\n")] = '\0';

        // Monta a string sem espaços
        char sequenciaHex[qtdBytesNosDados * 2 + 1];
        int idx = 0;
        for (int j = 0; buffer[j] != '\0'; j++) {
            if (buffer[j] != ' ') {
                sequenciaHex[idx++] = buffer[j];
            }
        }
        sequenciaHex[idx] = '\0';

        // Converte a sequência hex em bytes
        unsigned char *dados = hexParaBytes(sequenciaHex, qtdBytesNosDados);
        if (!dados) {
            fprintf(stderr, "Erro na conversão de hex para bytes\n");
            continue;
        }

        // ========== Compressão Huffman ==========
        int tamanhoHuff = 0;
        char *huffComp  = compactarHuffman(dados, qtdBytesNosDados, &tamanhoHuff);
        double taxaHuff = calcularTaxaCompressao(qtdBytesNosDados, tamanhoHuff);

        // ========== Compressão RLE ==========
        int tamanhoRLE = 0;
        char *rleComp  = compactarRLE(dados, qtdBytesNosDados, &tamanhoRLE);
        double taxaRLE = calcularTaxaCompressao(qtdBytesNosDados, tamanhoRLE);

        // Decide qual método imprimir (ou ambos, em caso de empate)
        if (tamanhoHuff < tamanhoRLE) {
            imprimirResultado(saida, i, "HUF", taxaHuff, huffComp);
        } else if (tamanhoRLE < tamanhoHuff) {
            imprimirResultado(saida, i, "RLE", taxaRLE, rleComp);
        } else {
            // Empate: imprime primeiro Huffman, depois RLE
            imprimirResultado(saida, i, "HUF", taxaHuff, huffComp);
            imprimirResultado(saida, i, "RLE", taxaRLE, rleComp);
        }

        free(dados);
        free(huffComp);
        free(rleComp);
    }

    fclose(entrada);
    fclose(saida);
}

//----------------[ Converte string hex para array de bytes (versão otimizada) ]----------------
static unsigned char *hexParaBytes(const char *sequenciaHex, int qtdBytes) {
    unsigned char *saida = malloc(qtdBytes);
    if (!saida) return NULL;
    for (int i = 0; i < qtdBytes; i++) {
        int high = hexVal(sequenciaHex[2*i]);
        int low  = hexVal(sequenciaHex[2*i+1]);
        saida[i] = (unsigned char)((high << 4) | low);
    }
    return saida;
}

//----------------[ Conta frequência dos bytes (0..255) ]----------------
static int *contarFrequencia(const unsigned char *dados, int n) {
    int *freq = calloc(256, sizeof(int));
    if (!freq) return NULL;
    for (int i = 0; i < n; i++) {
        freq[dados[i]]++;
    }
    return freq;
}

//----------------[ Cria min-heap ]----------------
static MinHeap *criarMinHeap(int capacidade) {
    MinHeap *heap = malloc(sizeof(MinHeap));
    heap->array     = malloc(capacidade * sizeof(No*));
    heap->tamanho   = 0;
    heap->capacidade= capacidade;
    return heap;
}

//----------------[ Insere nó no min-heap ]----------------
static void inserirMinHeap(MinHeap *heap, No *novoNo) {
    if (heap->tamanho >= heap->capacidade) return;
    heap->array[heap->tamanho] = novoNo;
    int i = heap->tamanho;
    heap->tamanho++;
    while (i && heap->array[i]->freq < heap->array[(i-1)/2]->freq) {
        troca(&heap->array[i], &heap->array[(i-1)/2]);
        i = (i-1)/2;
    }
}

//----------------[ Extrai o menor nó do heap ]----------------
static No *extrairMin(MinHeap *heap) {
    if (!heap->tamanho) return NULL;
    No *temp = heap->array[0];
    heap->tamanho--;
    heap->array[0] = heap->array[heap->tamanho];
    heapify(heap, 0);
    return temp;
}


//----------------[ Troca ponteiros de nós ]----------------
static void troca(No **a, No **b) {
    No *t = *a;
    *a = *b;
    *b = t;
}

//----------------[ Ajusta o heap para manter propriedade de mínimo ]----------------
static void heapify(MinHeap *heap, int idx) {
    int menor = idx;
    int esq   = 2*idx + 1;
    int dir   = 2*idx + 2;
    if (esq < heap->tamanho && heap->array[esq]->freq < heap->array[menor]->freq)
        menor = esq;
    if (dir < heap->tamanho && heap->array[dir]->freq < heap->array[menor]->freq)
        menor = dir;
    if (menor != idx) {
        troca(&heap->array[menor], &heap->array[idx]);
        heapify(heap, menor);
    }
}

//----------------[ Constrói árvore de Huffman a partir das frequências ]----------------
static No *construirArvoreHuffman(const int *frequencia) {
    int count = 0;
    for (int i = 0; i < 256; i++) {
        if (frequencia[i] > 0)
            count++;
    }
    if (count == 0)
        return NULL;
    MinHeap *heap = criarMinHeap(count);
    for (int i = 0; i < 256; i++) {
        if (frequencia[i] > 0) {
            No *novo = malloc(sizeof(No));
            novo->simbolo = (unsigned char)i;
            novo->freq    = frequencia[i];
            novo->esq     = NULL;
            novo->dir     = NULL;
            inserirMinHeap(heap, novo);
        }
    }
    while (heap->tamanho > 1) {
        No *min1 = extrairMin(heap);
        No *min2 = extrairMin(heap);
        No *interno = malloc(sizeof(No));
        interno->simbolo = 0;
        interno->freq    = min1->freq + min2->freq;
        interno->esq     = min1;
        interno->dir     = min2;
        inserirMinHeap(heap, interno);
    }
    // Retorna a raiz da árvore
    No *raiz = extrairMin(heap);
    free(heap->array);
    free(heap);
    return raiz;
}

//----------------[ Gera códigos binários para cada símbolo ]----------------
static void gerarCodigosHuffman(No *raiz, char (*codigos)[256], char *atual, int profundidade) {
    if (!raiz) return;
    if (!raiz->esq && !raiz->dir) {
        atual[profundidade] = '\0';
        strcpy(codigos[raiz->simbolo], atual);
        return;
    }
    atual[profundidade] = '0';
    gerarCodigosHuffman(raiz->esq, codigos, atual, profundidade+1);
    atual[profundidade] = '1';
    gerarCodigosHuffman(raiz->dir, codigos, atual, profundidade+1);
}

//----------------[ Compacta dados usando Huffman (versão otimizada) ]----------------
static char *compactarHuffman(const unsigned char *dados, int n, int *tamanhoCompactado) {
    int *freq = contarFrequencia(dados, n);
    if (!freq) {
        *tamanhoCompactado = n;
        return NULL;
    }
    No *raiz = construirArvoreHuffman(freq);
    free(freq);
    if (!raiz) {
        *tamanhoCompactado = n;
        return NULL;
    }
    char codigos[256][256] = {0};
    char buffer[256];
    gerarCodigosHuffman(raiz, codigos, buffer, 0);

    // Calcula tamanho máximo necessário (pior caso)
    int maxBytes = n * 256 / 8 + 1;
    unsigned char *outBuffer = malloc(maxBytes);
    memset(outBuffer, 0, maxBytes);
    int bitPos = 0; // posição atual em bits do outBuffer

    // Percorre os dados e insere os bits diretamente no buffer de saída
    for (int i = 0; i < n; i++) {
        char *codigo = codigos[dados[i]];
        int len = strlen(codigo);
        for (int j = 0; j < len; j++) {
            if (codigo[j] == '1') {
                outBuffer[bitPos / 8] |= (1 << (7 - (bitPos % 8)));
            }
            bitPos++;
        }
    }
    int numBytes = (bitPos + 7) / 8; // arredonda para cima

    char *saidaHex = malloc(numBytes * 2 + 1);
    for (int i = 0; i < numBytes; i++) {
        sprintf(&saidaHex[2*i], "%02X", outBuffer[i]);
    }
    saidaHex[numBytes * 2] = '\0';

    free(outBuffer);
    liberarArvore(raiz);
    *tamanhoCompactado = numBytes;
    return saidaHex;
}

//----------------[ Compacta dados usando RLE ]----------------
static char *compactarRLE(const unsigned char *dados, int n, int *tamanhoCompactado) {
    if (n == 0) {
        *tamanhoCompactado = 0;
        char *vazio = malloc(1);
        vazio[0] = '\0';
        return vazio;
    }
    unsigned char *rleBytes = malloc(2 * n);
    int idx = 0;
    int count = 1;
    unsigned char atual = dados[0];
    for (int i = 1; i < n; i++) {
        if (dados[i] == atual && count < 255) {
            count++;
        } else {
            rleBytes[idx++] = (unsigned char)count;
            rleBytes[idx++] = atual;
            atual = dados[i];
            count = 1;
        }
    }
    rleBytes[idx++] = (unsigned char)count;
    rleBytes[idx++] = atual;
    
    char *saidaHex = malloc(idx * 2 + 1);
    for (int i = 0; i < idx; i++) {
        sprintf(&saidaHex[2*i], "%02X", rleBytes[i]);
    }
    saidaHex[idx * 2] = '\0';
    
    free(rleBytes);
    *tamanhoCompactado = idx;
    return saidaHex;
}

//----------------[ Calcula a taxa de compressão ]----------------
static double calcularTaxaCompressao(int tamanhoOriginal, int tamanhoComprimido) {
    if (tamanhoOriginal == 0) return 0.0;
    return 100.0 * ((double)tamanhoComprimido / (double)tamanhoOriginal);
}

//----------------[ Imprime resultado no formato desejado ]----------------
static void imprimirResultado(FILE *saida, int indiceSequencia, 
                              const char *metodo, double taxa, const char *conteudoHex) {
    // Exemplo de formato: "0->HUF(22.22%)=0000"
    fprintf(saida, "%d->%s(%.2f%%)=%s\n", indiceSequencia, metodo, taxa, (conteudoHex ? conteudoHex : ""));
}

//----------------[ Libera árvore de Huffman ]----------------
static void liberarArvore(No *raiz) {
    if (raiz) {
        liberarArvore(raiz->esq);
        liberarArvore(raiz->dir);
        free(raiz);
    }
}
