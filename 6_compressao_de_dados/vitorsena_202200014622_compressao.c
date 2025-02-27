#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Tabela de lookup para conversão de hexadecimal (apenas caracteres válidos)
static const unsigned char hexLookup[128] = {
    ['0'] = 0,  ['1'] = 1,  ['2'] = 2,  ['3'] = 3,
    ['4'] = 4,  ['5'] = 5,  ['6'] = 6,  ['7'] = 7,
    ['8'] = 8,  ['9'] = 9,  
    ['A'] = 10, ['B'] = 11, ['C'] = 12, ['D'] = 13, ['E'] = 14, ['F'] = 15,
    ['a'] = 10, ['b'] = 11, ['c'] = 12, ['d'] = 13, ['e'] = 14, ['f'] = 15
};

// Estrutura do nó da árvore de Huffman
typedef struct No {
    unsigned char simbolo;  // Símbolo (byte)
    int freq;               // Frequência do símbolo
    struct No *esq, *dir;
} No;

// Estrutura para o min-heap
typedef struct {
    No **array;     // Array de ponteiros para nós
    int tamanho;    // Número atual de nós
    int capacidade; // Capacidade máxima do heap
} MinHeap;

// Estrutura para a pilha usada na geração iterativa dos códigos de Huffman
typedef struct {
    No *no;
    char codigo[256]; // Armazena o código em "0" e "1"
    int profundidade;
} PilhaItem;

// Buffer global para frequências (evita alocações dinâmicas frequentes)
static int freqBuffer[256];

// Protótipos das funções
void processarArquivo(const char *arquivoEntrada, const char *arquivoSaida);
static unsigned char *hexParaBytes(const char *sequenciaHex, int qtdBytes);
static void zerarFrequencias(void);
static int *contarFrequencia(const unsigned char *dados, int n);
static MinHeap *criarMinHeap(int capacidade);
static void inserirMinHeap(MinHeap *heap, No *novoNo);
static No *extrairMin(MinHeap *heap);
static void troca(No **a, No **b);
static void heapify(MinHeap *heap, int idx);
static No *construirArvoreHuffman(const int *frequencia);
static void gerarCodigosHuffmanIterativo(No *raiz, char codigos[256][256]);
static void liberarArvore(No *raiz);

static char *compactarHuffman(const unsigned char *dados, int n, int *tamanhoCompactado);
static char *compactarRLE(const unsigned char *dados, int n, int *tamanhoCompactado);
static double calcularTaxaCompressao(int tamanhoOriginal, int tamanhoComprimido);
static void imprimirResultado(FILE *saida, int indiceSequencia, const char *metodo, double taxa, const char *conteudoHex);

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
        if (saida) fclose(saida);
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
        buffer[strcspn(buffer, "\n")] = '\0';

        // Remove os espaços e monta a string contínua
        char sequenciaHex[qtdBytesNosDados * 2 + 1];
        int idx = 0;
        for (int j = 0; buffer[j] != '\0'; j++) {
            if (buffer[j] != ' ')
                sequenciaHex[idx++] = buffer[j];
        }
        sequenciaHex[idx] = '\0';

        // Converte a sequência hex em bytes
        unsigned char *dados = hexParaBytes(sequenciaHex, qtdBytesNosDados);
        if (!dados) {
            fprintf(stderr, "Erro na conversão de hex para bytes\n");
            continue;
        }

        // Compactação Huffman
        int tamanhoHuff = 0;
        char *huffComp  = compactarHuffman(dados, qtdBytesNosDados, &tamanhoHuff);
        double taxaHuff = calcularTaxaCompressao(qtdBytesNosDados, tamanhoHuff);

        // Compactação RLE
        int tamanhoRLE = 0;
        char *rleComp  = compactarRLE(dados, qtdBytesNosDados, &tamanhoRLE);
        double taxaRLE = calcularTaxaCompressao(qtdBytesNosDados, tamanhoRLE);

        // Seleciona o método com menor tamanho; em caso de empate, imprime ambos
        if (tamanhoHuff < tamanhoRLE) {
            imprimirResultado(saida, i, "HUF", taxaHuff, huffComp);
        } else if (tamanhoRLE < tamanhoHuff) {
            imprimirResultado(saida, i, "RLE", taxaRLE, rleComp);
        } else {
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

//----------------[ Converte string hex para array de bytes (usando lookup) ]----------------
static unsigned char *hexParaBytes(const char *sequenciaHex, int qtdBytes) {
    unsigned char *saida = malloc(qtdBytes);
    if (!saida) return NULL;
    for (int i = 0; i < qtdBytes; i++) {
        unsigned char high = hexLookup[(unsigned char)sequenciaHex[2*i]];
        unsigned char low  = hexLookup[(unsigned char)sequenciaHex[2*i+1]];
        saida[i] = (high << 4) | low;
    }
    return saida;
}

//----------------[ Zera o buffer global de frequências ]----------------
static void zerarFrequencias(void) {
    memset(freqBuffer, 0, sizeof(freqBuffer));
}

//----------------[ Conta frequência dos bytes (0..255) usando buffer global ]----------------
static int *contarFrequencia(const unsigned char *dados, int n) {
    zerarFrequencias();
    for (int i = 0; i < n; i++) {
        freqBuffer[dados[i]]++;
    }
    return freqBuffer;
}

//----------------[ Cria min-heap ]----------------
static MinHeap *criarMinHeap(int capacidade) {
    MinHeap *heap = malloc(sizeof(MinHeap));
    heap->array = malloc(capacidade * sizeof(No*));
    heap->tamanho = 0;
    heap->capacidade = capacidade;
    return heap;
}

//----------------[ Insere nó no min-heap ]----------------
static void inserirMinHeap(MinHeap *heap, No *novoNo) {
    if (heap->tamanho >= heap->capacidade) return;
    heap->array[heap->tamanho] = novoNo;
    int i = heap->tamanho;
    heap->tamanho++;
    while (i && heap->array[i]->freq < heap->array[(i - 1) / 2]->freq) {
        troca(&heap->array[i], &heap->array[(i - 1) / 2]);
        i = (i - 1) / 2;
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

//----------------[ Mantém propriedade do min-heap ]----------------
static void heapify(MinHeap *heap, int idx) {
    int menor = idx;
    int esq = 2 * idx + 1;
    int dir = 2 * idx + 2;
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
            novo->freq = frequencia[i];
            novo->esq = novo->dir = NULL;
            inserirMinHeap(heap, novo);
        }
    }

    while (heap->tamanho > 1) {
        No *min1 = extrairMin(heap);
        No *min2 = extrairMin(heap);
        No *interno = malloc(sizeof(No));
        interno->simbolo = 0;
        interno->freq = min1->freq + min2->freq;
        interno->esq = min1;
        interno->dir = min2;
        inserirMinHeap(heap, interno);
    }

    No *raiz = extrairMin(heap);
    free(heap->array);
    free(heap);
    return raiz;
}

//----------------[ Gera códigos de Huffman de forma iterativa ]----------------
static void gerarCodigosHuffmanIterativo(No *raiz, char codigos[256][256]) {
    if (!raiz) return;
    // Usamos uma pilha fixa; em casos extremos, ajuste o tamanho se necessário
    PilhaItem pilha[512];
    int topo = 0;
    pilha[topo].no = raiz;
    pilha[topo].profundidade = 0;
    pilha[topo].codigo[0] = '\0';
    topo++;

    while (topo > 0) {
        PilhaItem item = pilha[--topo];
        No *node = item.no;
        int prof = item.profundidade;
        if (!node->esq && !node->dir) {
            strcpy(codigos[node->simbolo], item.codigo);
        } else {
            if (node->dir) {
                PilhaItem novo;
                novo.no = node->dir;
                novo.profundidade = prof + 1;
                strcpy(novo.codigo, item.codigo);
                novo.codigo[prof] = '1';
                novo.codigo[prof + 1] = '\0';
                pilha[topo++] = novo;
            }
            if (node->esq) {
                PilhaItem novo;
                novo.no = node->esq;
                novo.profundidade = prof + 1;
                strcpy(novo.codigo, item.codigo);
                novo.codigo[prof] = '0';
                novo.codigo[prof + 1] = '\0';
                pilha[topo++] = novo;
            }
        }
    }
}

//----------------[ Libera a árvore de Huffman ]----------------
static void liberarArvore(No *raiz) {
    if (raiz) {
        liberarArvore(raiz->esq);
        liberarArvore(raiz->dir);
        free(raiz);
    }
}

//----------------[ Compacta dados usando Huffman (com inserção direta de bits) ]----------------
static char *compactarHuffman(const unsigned char *dados, int n, int *tamanhoCompactado) {
    int *freq = contarFrequencia(dados, n);
    // Como usamos buffer global, não precisamos liberar freq
    No *raiz = construirArvoreHuffman(freq);
    if (!raiz) {
        *tamanhoCompactado = n;
        return NULL;
    }

    char codigos[256][256] = {0};
    gerarCodigosHuffmanIterativo(raiz, codigos);

    // Aloca buffer de saída de bits (pior caso: n * 256 bits)
    int maxBytes = (n * 256) / 8 + 1;
    unsigned char *outBuffer = malloc(maxBytes);
    memset(outBuffer, 0, maxBytes);
    int bitPos = 0; // posição atual em bits

    // Insere os bits diretamente no buffer de saída
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
    int numBytes = (bitPos + 7) / 8;

    // Converte o buffer de saída para uma string hexadecimal
    char *saidaHex = malloc(numBytes * 2 + 1);
    for (int i = 0; i < numBytes; i++) {
        sprintf(&saidaHex[2 * i], "%02X", outBuffer[i]);
    }
    saidaHex[numBytes * 2] = '\0';

    free(outBuffer);
    liberarArvore(raiz);
    *tamanhoCompactado = numBytes;
    return saidaHex;
}

//----------------[ Compacta dados usando RLE (com buffer local) ]----------------
static char *compactarRLE(const unsigned char *dados, int n, int *tamanhoCompactado) {
    if (n == 0) {
        *tamanhoCompactado = 0;
        char *vazio = malloc(1);
        vazio[0] = '\0';
        return vazio;
    }
    unsigned char rleBytes[2 * n];
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
        sprintf(&saidaHex[2 * i], "%02X", rleBytes[i]);
    }
    saidaHex[idx * 2] = '\0';

    *tamanhoCompactado = idx;
    return saidaHex;
}

//----------------[ Calcula a taxa de compressão ]----------------
static double calcularTaxaCompressao(int tamanhoOriginal, int tamanhoComprimido) {
    if (tamanhoOriginal == 0) return 0.0;
    return 100.0 * ((double)tamanhoComprimido / (double)tamanhoOriginal);
}

//----------------[ Imprime o resultado no formato desejado ]----------------
static void imprimirResultado(FILE *saida, int indiceSequencia, const char *metodo, double taxa, const char *conteudoHex) {
    // Exemplo: "0->HUF(22.22%)=0000"
    fprintf(saida, "%d->%s(%.2f%%)=%s\n", indiceSequencia, metodo, taxa, (conteudoHex ? conteudoHex : ""));
}
