#include <stdio.h>   // Para operações de entrada/saída (fopen, fscanf, fprintf, etc.)
#include <stdlib.h>  // Para funções de alocação de memória (não usado diretamente aqui)
#include <stdbool.h> // Para usar o tipo de dado 'bool' (true/false)

#define MAX_SIZE 100 // Tamanho máximo definido para o labirinto

// Estrutura para armazenar coordenadas (x,y) no labirinto
typedef struct {
    int x; // Coordenada vertical (linha)
    int y; // Coordenada horizontal (coluna)
} Posicao;

// Protótipos das funções
bool isSaida(Posicao pos, int linhas, int colunas, Posicao inicio, char labirinto[][MAX_SIZE]);
bool encontrarSaida(char labirinto[][MAX_SIZE], bool visitados[][MAX_SIZE], bool dead[][MAX_SIZE],
                   int linhas, int colunas, Posicao atual, Posicao inicio, 
                   FILE *output, Posicao *saida);

int main(int argc, char *argv[]) {
    // Verificação dos argumentos de linha de comando
    if(argc < 3) {
        printf("Uso: %s <arquivo_entrada> <arquivo_saida>\n", argv[0]);
        return 1; // Sai com código de erro se não tiver argumentos suficientes
    }

    // Abre os arquivos de entrada e saída
    FILE *input = fopen(argv[1], "r");  // Arquivo de entrada (labirintos)
    FILE *output = fopen(argv[2], "w"); // Arquivo de saída (solução)
    
    // Verifica se os arquivos foram abertos corretamente
    if(!input || !output) {
        fprintf(stderr, "Erro ao abrir arquivos.\n");
        // Fecha os arquivos que foram abertos (se algum foi)
        if(input) fclose(input);
        if(output) fclose(output);
        return 1; // Sai com código de erro
    }

    // Lê o número total de labirintos a serem processados
    int numLabirintos;
    fscanf(input, "%d", &numLabirintos);

    // Processa cada labirinto individualmente
    for(int lab = 0; lab < numLabirintos; lab++) {
        int colunas, linhas;
        // Lê as dimensões do labirinto atual (colunas x linhas)
        fscanf(input, "%d %d", &colunas, &linhas);

        // Declaração das estruturas de dados:
        char labirinto[MAX_SIZE][MAX_SIZE];        // Armazena o labirinto
        bool visitados[MAX_SIZE][MAX_SIZE] = {false}; // Rastreia células visitadas
        bool dead[MAX_SIZE][MAX_SIZE] = {false};   // Rastreia dead-ends (caminhos sem saída)
        Posicao inicio = {-1, -1}; // Inicializa posição inicial como inválida

        // Leitura do labirinto do arquivo
        for(int i = 0; i < linhas; i++) {
            for(int j = 0; j < colunas; j++) {
                fscanf(input, " %c", &labirinto[i][j]);
                // Verifica se é a posição inicial 'X'
                if(labirinto[i][j] == 'X') {
                    inicio.x = i; // Armazena coordenada x (linha)
                    inicio.y = j; // Armazena coordenada y (coluna)
                }
            }
        }

        // Escreve no arquivo de saída o cabeçalho com a posição inicial
        fprintf(output, "L%d:INI@%d,%d", lab, inicio.x, inicio.y);
        
        Posicao saida = {-1, -1}; // Armazenará a posição da saída encontrada
        visitados[inicio.x][inicio.y] = true; // Marca a posição inicial como visitada
        
        // Define a ordem de prioridade dos movimentos:
        // Direita (D), Frente (F), Esquerda (E), Trás (T)
        const Posicao movimentos[] = {{0, 1}, {-1, 0}, {0, -1}, {1, 0}};
        const char* dirs[] = {"D", "F", "E", "T"};
        bool encontrou = false; // Flag para indicar se encontrou saída
        
        // Tenta cada direção na ordem de prioridade definida
        for(int i = 0; i < 4 && !encontrou; i++) {
            // Calcula a próxima posição baseada no movimento atual
            Posicao prox = {inicio.x + movimentos[i].x, inicio.y + movimentos[i].y};
            
            // Verifica se o movimento é válido:
            // 1. Está dentro dos limites do labirinto
            // 2. Não é parede ('1')
            // 3. Não foi visitado ainda
            // 4. Não é um dead-end conhecido
            if(prox.x >= 0 && prox.x < linhas && prox.y >= 0 && prox.y < colunas &&
               labirinto[prox.x][prox.y] != '1' && !visitados[prox.x][prox.y] && !dead[prox.x][prox.y]) {
                
                // Registra o movimento no arquivo de saída
                fprintf(output, "|%s->%d,%d", dirs[i], prox.x, prox.y);
                visitados[prox.x][prox.y] = true; // Marca como visitado
                
                // Chama a função recursiva para explorar a partir da nova posição
                if(encontrarSaida(labirinto, visitados, dead, linhas, colunas, prox, inicio, output, &saida)) {
                    encontrou = true; // Se encontrou saída, seta flag
                } else {
                    // Se não encontrou saída, faz backtracking:
                    // 1. Registra o backtracking no arquivo de saída
                    fprintf(output, "|BT@%d,%d->%d,%d", prox.x, prox.y, inicio.x, inicio.y);
                    // 2. Desmarca como visitado (para outros caminhos)
                    visitados[prox.x][prox.y] = false;
                    // 3. Marca como dead-end (não precisa explorar novamente)
                    dead[prox.x][prox.y] = true;
                }
            }
        }

        // Escreve o resultado final no arquivo de saída
        if(encontrou) {
            fprintf(output, "|FIM@%d,%d\n", saida.x, saida.y);
        } else {
            fprintf(output, "|FIM@-,-\n");
        }
    }

    // Fecha os arquivos ao final do processamento
    fclose(input);
    fclose(output);
    return 0; // Retorna 0 indicando sucesso
}

/**
 * Verifica se uma posição é uma saída válida do labirinto
 * 
 * @param pos Posição a ser verificada
 * @param linhas Número total de linhas do labirinto
 * @param colunas Número total de colunas do labirinto
 * @param inicio Posição inicial (para verificar se não é a saída)
 * @param labirinto Matriz representando o labirinto
 * @return true se é uma saída válida, false caso contrário
 */
bool isSaida(Posicao pos, int linhas, int colunas, Posicao inicio, char labirinto[][MAX_SIZE]) {
    // Para ser saída, deve:
    // 1. Estar na borda do labirinto (linha 0, última linha, coluna 0 ou última coluna)
    // 2. Ser um caminho livre ('0')
    // 3. Não ser a posição inicial
    return (pos.x == 0 || pos.x == linhas-1 || pos.y == 0 || pos.y == colunas-1) &&
           labirinto[pos.x][pos.y] == '0' &&
           !(pos.x == inicio.x && pos.y == inicio.y);
}

/**
 * Função recursiva que implementa o algoritmo de backtracking para encontrar a saída
 * 
 * @param labirinto Matriz representando o labirinto
 * @param visitados Matriz que rastreia células já visitadas no caminho atual
 * @param dead Matriz que rastreia dead-ends (caminhos sem saída já explorados)
 * @param linhas Número total de linhas do labirinto
 * @param colunas Número total de colunas do labirinto
 * @param atual Posição atual no labirinto
 * @param inicio Posição inicial (para verificação de saída)
 * @param output Arquivo onde será escrita a solução
 * @param saida Ponteiro para armazenar a posição da saída encontrada
 * @return true se encontrou saída a partir desta posição, false caso contrário
 */
bool encontrarSaida(char labirinto[][MAX_SIZE], bool visitados[][MAX_SIZE], bool dead[][MAX_SIZE],
                   int linhas, int colunas, Posicao atual, Posicao inicio, 
                   FILE *output, Posicao *saida) {
    
    // Se esta posição já foi marcada como dead-end, retorna false
    if(dead[atual.x][atual.y]) return false;
    
    // Verifica se a posição atual é uma saída válida
    if(isSaida(atual, linhas, colunas, inicio, labirinto)) {
        *saida = atual; // Armazena as coordenadas da saída
        return true;    // Retorna true indicando que encontrou saída
    }

    // Define a ordem de prioridade dos movimentos:
    // Direita (D), Frente (F), Esquerda (E), Trás (T)
    const Posicao movimentos[] = {{0, 1}, {-1, 0}, {0, -1}, {1, 0}};
    const char* dirs[] = {"D", "F", "E", "T"};
    bool found = false; // Flag para indicar se encontrou saída

    // Tenta cada direção na ordem de prioridade definida
    for(int i = 0; i < 4 && !found; i++) {
        // Calcula a próxima posição baseada no movimento atual
        Posicao prox = {atual.x + movimentos[i].x, atual.y + movimentos[i].y};
        
        // Verifica se o movimento é válido:
        // 1. Está dentro dos limites do labirinto
        // 2. Não é parede ('1')
        // 3. Não foi visitado no caminho atual
        // 4. Não é um dead-end conhecido
        if(prox.x >= 0 && prox.x < linhas && prox.y >= 0 && prox.y < colunas &&
           labirinto[prox.x][prox.y] != '1' && !visitados[prox.x][prox.y] && !dead[prox.x][prox.y]) {
            
            // Registra o movimento no arquivo de saída
            fprintf(output, "|%s->%d,%d", dirs[i], prox.x, prox.y);
            visitados[prox.x][prox.y] = true; // Marca como visitado
            
            // Chamada recursiva para explorar a partir da nova posição
            if(encontrarSaida(labirinto, visitados, dead, linhas, colunas, prox, inicio, output, saida)) {
                found = true; // Se encontrou saída, seta flag
            } else {
                // Se não encontrou saída, faz backtracking:
                // 1. Registra o backtracking no arquivo de saída
                fprintf(output, "|BT@%d,%d->%d,%d", prox.x, prox.y, atual.x, atual.y);
                // 2. Desmarca como visitado (para outros caminhos)
                visitados[prox.x][prox.y] = false;
                // 3. Marca como dead-end (não precisa explorar novamente)
                dead[prox.x][prox.y] = true;
            }
        }
    }

    // Se não encontrou saída por nenhum caminho a partir desta posição,
    // marca como dead-end para evitar reexploração no futuro
    if(!found) dead[atual.x][atual.y] = true;
    return found; // Retorna se encontrou saída ou não
}