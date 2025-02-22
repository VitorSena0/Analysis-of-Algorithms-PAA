# Definições e limites
MAX_BYTES = 1600

# Estrutura para representar um pacote
class Pacote:
    def __init__(self, numeroPrioridade, dados):
        self.numeroPrioridade = numeroPrioridade
        self.dados = dados

# Funções para manipulação do heap
class HeapDePacotes:
    def __init__(self, capacidade):
        self.heap = []
        self.tamanhoHeap = 0
        self.capacidadeHeap = capacidade

    def inserirPacoteNoHeap(self, pacote):
        if self.tamanhoHeap >= self.capacidadeHeap:
            self.capacidadeHeap *= 2
        self.heap.append(pacote)
        self.tamanhoHeap += 1
        self.heapificarParaCima(self.tamanhoHeap - 1)

    def removerPacoteDoHeap(self):
        if self.tamanhoHeap == 0:
            raise IndexError("Heap está vazio")
        pacoteRaiz = self.heap[0]
        self.heap[0] = self.heap[self.tamanhoHeap - 1]
        self.tamanhoHeap -= 1
        self.heap.pop()
        self.heapificarParaBaixo(0)
        return pacoteRaiz

    def estaHeapVazia(self):
        return self.tamanhoHeap == 0

    def heapificarParaCima(self, indice):
        while indice > 0 and self.heap[indice].numeroPrioridade < self.heap[(indice - 1) // 2].numeroPrioridade:
            self.trocarPacotes(indice, (indice - 1) // 2)
            indice = (indice - 1) // 2

    def heapificarParaBaixo(self, indice):
        menor = indice
        filhoEsquerdo = 2 * indice + 1
        filhoDireito = 2 * indice + 2

        if filhoEsquerdo < self.tamanhoHeap and self.heap[filhoEsquerdo].numeroPrioridade < self.heap[menor].numeroPrioridade:
            menor = filhoEsquerdo
        if filhoDireito < self.tamanhoHeap and self.heap[filhoDireito].numeroPrioridade < self.heap[menor].numeroPrioridade:
            menor = filhoDireito
        if menor != indice:
            self.trocarPacotes(indice, menor)
            self.heapificarParaBaixo(menor)

    def trocarPacotes(self, i, j):
        self.heap[i], self.heap[j] = self.heap[j], self.heap[i]

# Função para processar a entrada e saída de dados
def processarArquivoEntrada(nomeArquivoEntrada, nomeArquivoSaida):
    with open(nomeArquivoEntrada, 'r') as arquivoEntrada, open(nomeArquivoSaida, 'w') as arquivoSaida:
        quantidadePacotes, quantidadePacotesBloco = map(int, arquivoEntrada.readline().strip().split())

        heapDePacotes = HeapDePacotes(quantidadePacotesBloco)
        posicaoSequencia = 0

        for _ in range((quantidadePacotes + quantidadePacotesBloco - 1) // quantidadePacotesBloco):
            for _ in range(quantidadePacotesBloco):
                linha = arquivoEntrada.readline().strip()
                if not linha:
                    break
                numeroPacote, tamanhoPacote, *dados = linha.split()
                dadosFormatados = ','.join(dados)

                pacote = Pacote(int(numeroPacote), dadosFormatados)
                heapDePacotes.inserirPacoteNoHeap(pacote)

            primeiraImpressao = True
            while not heapDePacotes.estaHeapVazia() and heapDePacotes.heap[0].numeroPrioridade == posicaoSequencia:
                pacote = heapDePacotes.removerPacoteDoHeap()
                if primeiraImpressao:
                    arquivoSaida.write(f"|{pacote.dados}")
                    primeiraImpressao = False
                else:
                    arquivoSaida.write(f"|{pacote.dados}")
                posicaoSequencia += 1

            if not primeiraImpressao:
                arquivoSaida.write("|\n")

# Função principal
if __name__ == "__main__":
    import sys
    if len(sys.argv) != 3:
        print(f"Uso: {sys.argv[0]} <arquivo_entrada> <arquivo_saida>")
        sys.exit(1)

    processarArquivoEntrada(sys.argv[1], sys.argv[2])