#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//-------------[ Definições e macros ]----------------

#define TAMANHO_PLACA 8
#define TAMANHO_CODIGO_PACOTE 14
#define TOLERANCIA 1e-6

// Macro para calcular o valor absoluto
#define valorAbsoluto(numero) ((numero) < 0 ? -(numero) : (numero))
// Macro para arredondar um número para o inteiro mais próximo
#define arredondar(valor) ((int)((valor) < 0 ? ((valor) - 0.5) : ((valor) + 0.5)))


//-------------[ Estruturas ]---------------- 

// Estrutura que representa um pacote.
// O campo "disponivel": 1 = disponível para transporte; 0 = já transportado.
typedef struct {
    char codigo[TAMANHO_CODIGO_PACOTE];
    int peso;
    int volume;
    float valor;
    int ordem;
    int disponivel; // 1 se disponível; 0 se já foi transportado
} Pacote;

// Estrutura que representa um veículo.
typedef struct {
    char placa[TAMANHO_PLACA];
    int pesoLimite;
    int volumeLimite;
    int pesoAtual;
    int volumeAtual;
    float valorTransportado;
    char **codigos;    // Vetor com os códigos dos pacotes carregados
    int quantidadeCargas;
} Veiculo;


//-------------[ Prototipação de funções ]----------------

Pacote **resolverMochilaParaVeiculo(Veiculo *veiculo, Pacote **pacotesDisponiveis, int quantidadePacotesDisponiveis, int *quantidadePacotesEscolhidos);
void liberarMemoria(Veiculo *vetorVeiculos, Pacote *todosPacotes, int numeroVeiculos, int quantidadeTotalPacotes);
void processarArquivo(char *entrada, char *saida);
void iniciarDadosVeiculo(Veiculo *veiculo, int pesoLimite, int volumeLimite);


/*
   Função que resolve o problema da mochila (knapsack) com duas restrições
   (peso e volume) para um determinado veículo, usando programação dinâmica.
   Parâmetros:
     - veiculo: ponteiro para o veículo que está sendo analisado.
     - pacotesDisponiveis: vetor (filtrado) de ponteiros para os pacotes disponíveis.
     - quantidadePacotesDisponiveis: número de pacotes disponíveis (tamanho do vetor).
     - quantidadePacotesEscolhidos: endereço de variável que receberá a quantidade de pacotes escolhidos.
   Retorna:
     - vetor (dinâmico) contendo os ponteiros para os pacotes escolhidos.
*/


//--------------[ Função principal ]----------------

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s <entrada> <saida>\n", argv[0]);
        return 1;
    }
    processarArquivo(argv[1], argv[2]);
    return 0;
}


//--------------[ Função gera a tabela 3D e resolve o problema da mochila ]----------------

Pacote **resolverMochilaParaVeiculo(Veiculo *veiculo, Pacote **pacotesDisponiveis, int quantidadePacotesDisponiveis, int *quantidadePacotesEscolhidos) {
    int capacidadePesoMaxima  = veiculo->pesoLimite;
    int capacidadeVolumeMaxima = veiculo->volumeLimite;

    // Dimensões para a tabela DP:
    int dimensaoItens  = quantidadePacotesDisponiveis + 1;       // Camadas: 0 (nenhum item) até n
    int dimensaoPeso   = capacidadePesoMaxima + 1;                // 0 ... capacidadePesoMaxima
    int dimensaoVolume = capacidadeVolumeMaxima + 1;              // 0 ... capacidadeVolumeMaxima
    int tamanhoPorItem = dimensaoPeso * dimensaoVolume;           // Tamanho de cada camada (DP[i])
    int tamanhoTotal   = dimensaoItens * tamanhoPorItem;           // Tabela inteira

    // Aloca a tabela DP em um vetor unidimensional (inicialmente zerado)
    float *tabelaDP = calloc(tamanhoTotal, sizeof(float));
    if (tabelaDP == NULL) {
        perror("Erro na alocacao da tabela DP");
        exit(EXIT_FAILURE);
    }

    // Cria um vetor auxiliar 1-indexado para os pacotes (facilitando o acesso)
    Pacote **vetorPacotesAux = malloc((quantidadePacotesDisponiveis + 1) * sizeof(Pacote *));
    if (vetorPacotesAux == NULL) {
        perror("Erro na alocacao do vetor auxiliar de pacotes");
        exit(EXIT_FAILURE);
    }
    for (int i = 1; i <= quantidadePacotesDisponiveis; i++) {
        vetorPacotesAux[i] = pacotesDisponiveis[i - 1];
    }

    // Macro para acessar a tabela DP: DP(i, p, v)
    // Índice calculado como: (i * (dimensaoPeso * dimensaoVolume)) + (p * dimensaoVolume) + (v)
    #define DP(i, p, v) tabelaDP[(i) * tamanhoPorItem + (p) * dimensaoVolume + (v)]

    // A camada 0 já está zerada (calloc). Processa cada item (camada de 1 a n)
    for (int i = 1; i <= quantidadePacotesDisponiveis; i++) {
        // Copia toda a camada anterior (i-1) para a camada atual (i)
        memcpy(&DP(i, 0, 0), &DP(i - 1, 0, 0), sizeof(float) * tamanhoPorItem);

        // Obtém as propriedades do item atual em variáveis locais
        Pacote *pacoteAtual = vetorPacotesAux[i];
        int pesoItem   = pacoteAtual->peso;
        int volumeItem = pacoteAtual->volume;
        float valorItem = pacoteAtual->valor;

        // Atualiza somente os estados onde o item cabe (p >= pesoItem e v >= volumeItem)
        for (int p = pesoItem; p <= capacidadePesoMaxima; p++) {
            for (int v = volumeItem; v <= capacidadeVolumeMaxima; v++) {
                float candidato = DP(i - 1, p - pesoItem, v - volumeItem) + valorItem;
                if (candidato > DP(i, p, v) + TOLERANCIA) {
                    DP(i, p, v) = candidato;
                }
            }
        }
    }

    // Reconstrução da solução ótima (backtracking)
    int pesoRestante = capacidadePesoMaxima;
    int volRestante  = capacidadeVolumeMaxima;
    Pacote **vetorPacotesEscolhidos = malloc(quantidadePacotesDisponiveis * sizeof(Pacote *));
    if (vetorPacotesEscolhidos == NULL) {
        perror("Erro na alocacao do vetor de pacotes escolhidos");
        exit(EXIT_FAILURE);
    }
    int contador = 0;

    // Percorre as camadas de trás para frente para identificar os itens incluídos
    for (int i = quantidadePacotesDisponiveis; i >= 1; i--) {
        if (valorAbsoluto(DP(i, pesoRestante, volRestante) - DP(i - 1, pesoRestante, volRestante)) > TOLERANCIA) {
            vetorPacotesEscolhidos[contador++] = vetorPacotesAux[i];
            pesoRestante -= vetorPacotesAux[i]->peso;
            volRestante  -= vetorPacotesAux[i]->volume;
        }
    }

    // Inverte a ordem dos itens para manter a sequência original
    for (int i = 0; i < contador / 2; i++) {
        Pacote *temp = vetorPacotesEscolhidos[i];
        vetorPacotesEscolhidos[i] = vetorPacotesEscolhidos[contador - 1 - i];
        vetorPacotesEscolhidos[contador - 1 - i] = temp;
    }

    free(tabelaDP);
    free(vetorPacotesAux);

    *quantidadePacotesEscolhidos = contador;
    return vetorPacotesEscolhidos;

    #undef DP
}


//-----------------[ Inicializa os dados do veículo ]-------------------

void iniciarDadosVeiculo(Veiculo *veiculo, int pesoLimite, int volumeLimite) {
    veiculo->pesoAtual = 0;
    veiculo->volumeAtual = 0;
    veiculo->valorTransportado = 0.0f;
    veiculo->codigos = NULL;
    veiculo->quantidadeCargas = 0;
    veiculo->pesoLimite = pesoLimite;
    veiculo->volumeLimite = volumeLimite;
}

//--------------[ Função que libera a memória alocada ]----------------

void liberarMemoria(Veiculo *vetorVeiculos, Pacote *todosPacotes, int numeroVeiculos, int quantidadeTotalPacotes) {
    for (int i = 0; i < numeroVeiculos; i++) {
        if (vetorVeiculos[i].codigos != NULL) {
            for (int j = 0; j < vetorVeiculos[i].quantidadeCargas; j++) {
                free(vetorVeiculos[i].codigos[j]);
            }
            free(vetorVeiculos[i].codigos);
        }
    }
    free(vetorVeiculos);
    free(todosPacotes);
}


//----------------[ Função que processa o arquivo de entrada e gera o arquivo de saída ]----------------

void processarArquivo(char *entrada, char *saida){

    FILE *arquivoEntrada = fopen(entrada, "r");
    FILE *arquivoSaida   = fopen(saida, "w");
    if (arquivoEntrada == NULL || arquivoSaida == NULL) {
        fprintf(stderr, "Erro ao abrir os arquivos.\n");
        if (arquivoEntrada) fclose(arquivoEntrada);
        if (arquivoSaida) fclose(arquivoSaida);
        return;
    }

    // Leitura dos veículos
    int numeroVeiculos;
    fscanf(arquivoEntrada, "%d", &numeroVeiculos);
    Veiculo *vetorVeiculos = malloc(numeroVeiculos * sizeof(Veiculo));
    if (vetorVeiculos == NULL) {
        perror("Erro ao alocar os veiculos");
        exit(EXIT_FAILURE);
    }

    printf("Numero de veiculos: %d\n", numeroVeiculos);
    
    for (int i = 0; i < numeroVeiculos; i++) {
        int pesoLim, volLim;
        fscanf(arquivoEntrada, "%s %d %d", vetorVeiculos[i].placa, &pesoLim, &volLim);
        iniciarDadosVeiculo(&vetorVeiculos[i], pesoLim, volLim);
    }

    // Leitura dos pacotes
    int quantidadeTotalPacotes;
    fscanf(arquivoEntrada, "%d", &quantidadeTotalPacotes);
    Pacote *todosPacotes = malloc(quantidadeTotalPacotes * sizeof(Pacote));
    if (todosPacotes == NULL) {
        perror("Erro ao alocar os pacotes");
        exit(EXIT_FAILURE);
    }

    printf("Numero de pacotes: %d\n", quantidadeTotalPacotes);
    
    for (int i = 0; i < quantidadeTotalPacotes; i++) {
        fscanf(arquivoEntrada, "%s %f %d %d", 
               todosPacotes[i].codigo, &todosPacotes[i].valor,
               &todosPacotes[i].peso, &todosPacotes[i].volume);
        todosPacotes[i].ordem = i;       // Define a ordem de chegada
        todosPacotes[i].disponivel = 1;    // Inicialmente, todos estão disponíveis
    }

    // Para cada veículo, constrói um vetor de pacotes disponíveis (filtrando os que ainda não foram transportados)
    for (int i = 0; i < numeroVeiculos; i++) {
        Pacote **pacotesDisponiveis = malloc(quantidadeTotalPacotes * sizeof(Pacote *));
        if (pacotesDisponiveis == NULL) {
            perror("Erro ao alocar pacotesDisponiveis");
            exit(EXIT_FAILURE);
        }
        int qtdDisponiveis = 0;
        for (int j = 0; j < quantidadeTotalPacotes; j++) {
            if (todosPacotes[j].disponivel == 1) {
                pacotesDisponiveis[qtdDisponiveis++] = &todosPacotes[j];
            }
        }

        int quantidadePacotesEscolhidos = 0;
        if (qtdDisponiveis > 0) {
            Pacote **pacotesEscolhidos = resolverMochilaParaVeiculo(&vetorVeiculos[i],
                                                                     pacotesDisponiveis,
                                                                     qtdDisponiveis,
                                                                     &quantidadePacotesEscolhidos);
            // Atualiza os totais do veículo e marca os pacotes escolhidos como utilizados
            for (int k = 0; k < quantidadePacotesEscolhidos; k++) {
                vetorVeiculos[i].pesoAtual += pacotesEscolhidos[k]->peso;
                vetorVeiculos[i].volumeAtual += pacotesEscolhidos[k]->volume;
                vetorVeiculos[i].valorTransportado += pacotesEscolhidos[k]->valor;
                pacotesEscolhidos[k]->disponivel = 0; // Marca como já transportado
            }
            // Armazena os códigos dos pacotes carregados no veículo
            if (quantidadePacotesEscolhidos > 0) {
                vetorVeiculos[i].codigos = malloc(quantidadePacotesEscolhidos * sizeof(char *));
                if (vetorVeiculos[i].codigos == NULL) {
                    perror("Erro ao alocar codigos do veiculo");
                    exit(EXIT_FAILURE);
                }
                for (int k = 0; k < quantidadePacotesEscolhidos; k++) {
                    vetorVeiculos[i].codigos[k] = strdup(pacotesEscolhidos[k]->codigo);
                }
            }
            vetorVeiculos[i].quantidadeCargas = quantidadePacotesEscolhidos;
            free(pacotesEscolhidos);
        }
        free(pacotesDisponiveis);
    }

    // Imprime as informações dos veículos no arquivo de saída
    for (int i = 0; i < numeroVeiculos; i++) {
        int percentualPeso = (vetorVeiculos[i].pesoLimite > 0) ?
                             arredondar((double)vetorVeiculos[i].pesoAtual * 100 / vetorVeiculos[i].pesoLimite) : 0;
        int percentualVolume = (vetorVeiculos[i].volumeLimite > 0) ?
                               arredondar((double)vetorVeiculos[i].volumeAtual * 100 / vetorVeiculos[i].volumeLimite) : 0;
        fprintf(arquivoSaida, "[%s]R$%.2f,%dKG(%d%%),%dL(%d%%)->",
                vetorVeiculos[i].placa,
                vetorVeiculos[i].valorTransportado,
                vetorVeiculos[i].pesoAtual, percentualPeso,
                vetorVeiculos[i].volumeAtual, percentualVolume);
        if (vetorVeiculos[i].quantidadeCargas == 0)
            fprintf(arquivoSaida, "Nenhum pacote carregado");
        else {
            for (int j = 0; j < vetorVeiculos[i].quantidadeCargas; j++) {
                if (j > 0)
                    fprintf(arquivoSaida, ",");
                fprintf(arquivoSaida, "%s", vetorVeiculos[i].codigos[j]);
            }
        }
        fprintf(arquivoSaida, "\n");
    }

    // Imprime os pacotes pendentes (aqueles que ainda estão disponíveis) e a soma total de seus atributos
    float somaValorPendentes = 0.0f;
    int somaPesoPendentes = 0;
    int somaVolumePendentes = 0;
    for (int i = 0; i < quantidadeTotalPacotes; i++) {
        if (todosPacotes[i].disponivel == 1) {
            somaValorPendentes += todosPacotes[i].valor;
            somaPesoPendentes += todosPacotes[i].peso;
            somaVolumePendentes += todosPacotes[i].volume;
        }
    }
    fprintf(arquivoSaida, "PENDENTE:R$%.2f,%dKG,%dL->", somaValorPendentes, somaPesoPendentes, somaVolumePendentes);
    int pendentesImpressos = 0;
    for (int i = 0; i < quantidadeTotalPacotes; i++) {
        if (todosPacotes[i].disponivel == 1) {
            if (pendentesImpressos > 0)
                fprintf(arquivoSaida, ",");
            fprintf(arquivoSaida, "%s", todosPacotes[i].codigo);
            pendentesImpressos++;
        }
    }
    if (pendentesImpressos == 0)
        fprintf(arquivoSaida, "Nenhum pacote pendente");
    fprintf(arquivoSaida, "\n");

    // Libera a memória alocada para os veículos
    liberarMemoria(vetorVeiculos, todosPacotes, numeroVeiculos, quantidadeTotalPacotes);

    fclose(arquivoEntrada);
    fclose(arquivoSaida);

}
