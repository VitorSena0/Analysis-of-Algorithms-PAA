#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_SIZE 100

typedef struct {
    int x;
    int y;
} Posicao;

bool isSaida(Posicao pos, int linhas, int colunas, Posicao inicio, char labirinto[][MAX_SIZE]);
bool encontrarSaida(char labirinto[][MAX_SIZE], bool visitados[][MAX_SIZE], bool dead[][MAX_SIZE],
                   int linhas, int colunas, Posicao atual, Posicao inicio, 
                   FILE *output, Posicao *saida);

int main(int argc, char *argv[]) {
    if(argc < 3) {
        printf("Uso: %s <arquivo_entrada> <arquivo_saida>\n", argv[0]);
        return 1;
    }

    FILE *input = fopen(argv[1], "r");
    FILE *output = fopen(argv[2], "w");
    
    if(!input || !output) {
        fprintf(stderr, "Erro ao abrir arquivos.\n");
        if(input) fclose(input);
        if(output) fclose(output);
        return 1;
    }

    int numLabirintos;
    fscanf(input, "%d", &numLabirintos);

    for(int lab = 0; lab < numLabirintos; lab++) {
        int colunas, linhas;
        fscanf(input, "%d %d", &colunas, &linhas);

        char labirinto[MAX_SIZE][MAX_SIZE];
        bool visitados[MAX_SIZE][MAX_SIZE] = {false};
        bool dead[MAX_SIZE][MAX_SIZE] = {false}; // Nova matriz de dead-ends
        Posicao inicio = {-1, -1};

        // Ler labirinto e encontrar posição inicial
        for(int i = 0; i < linhas; i++) {
            for(int j = 0; j < colunas; j++) {
                fscanf(input, " %c", &labirinto[i][j]);
                if(labirinto[i][j] == 'X') {
                    inicio.x = i;
                    inicio.y = j;
                }
            }
        }

        fprintf(output, "L%d:INI@%d,%d", lab, inicio.x, inicio.y);
        
        Posicao saida = {-1, -1};
        visitados[inicio.x][inicio.y] = true;
        
        // Tentar cada direção na ordem de prioridade
        const Posicao movimentos[] = {{0, 1}, {-1, 0}, {0, -1}, {1, 0}}; // D, F, E, T
        const char* dirs[] = {"D", "F", "E", "T"};
        bool encontrou = false;
        
        for(int i = 0; i < 4 && !encontrou; i++) {
            Posicao prox = {inicio.x + movimentos[i].x, inicio.y + movimentos[i].y};
            
            if(prox.x >= 0 && prox.x < linhas && prox.y >= 0 && prox.y < colunas &&
               labirinto[prox.x][prox.y] != '1' && !visitados[prox.x][prox.y] && !dead[prox.x][prox.y]) {
                
                fprintf(output, "|%s->%d,%d", dirs[i], prox.x, prox.y);
                visitados[prox.x][prox.y] = true;
                
                if(encontrarSaida(labirinto, visitados, dead, linhas, colunas, prox, inicio, output, &saida)) {
                    encontrou = true;
                } else {
                    fprintf(output, "|BT@%d,%d->%d,%d", prox.x, prox.y, inicio.x, inicio.y);
                    visitados[prox.x][prox.y] = false;
                    dead[prox.x][prox.y] = true; // Marca como dead-end
                }
            }
        }

        if(encontrou) {
            fprintf(output, "|FIM@%d,%d\n", saida.x, saida.y);
        } else {
            fprintf(output, "|FIM@-,-\n");
        }
    }

    fclose(input);
    fclose(output);
    return 0;
}

bool isSaida(Posicao pos, int linhas, int colunas, Posicao inicio, char labirinto[][MAX_SIZE]) {
    return (pos.x == 0 || pos.x == linhas-1 || pos.y == 0 || pos.y == colunas-1) &&
           labirinto[pos.x][pos.y] == '0' &&
           !(pos.x == inicio.x && pos.y == inicio.y);
}

bool encontrarSaida(char labirinto[][MAX_SIZE], bool visitados[][MAX_SIZE], bool dead[][MAX_SIZE],
                   int linhas, int colunas, Posicao atual, Posicao inicio, 
                   FILE *output, Posicao *saida) {
    
    if(dead[atual.x][atual.y]) return false; // Pula células mortas
    
    if(isSaida(atual, linhas, colunas, inicio, labirinto)) {
        *saida = atual;
        return true;
    }

    const Posicao movimentos[] = {{0, 1}, {-1, 0}, {0, -1}, {1, 0}}; // D, F, E, T
    const char* dirs[] = {"D", "F", "E", "T"};
    bool found = false;

    for(int i = 0; i < 4 && !found; i++) {
        Posicao prox = {atual.x + movimentos[i].x, atual.y + movimentos[i].y};
        
        if(prox.x >= 0 && prox.x < linhas && prox.y >= 0 && prox.y < colunas &&
           labirinto[prox.x][prox.y] != '1' && !visitados[prox.x][prox.y] && !dead[prox.x][prox.y]) {
            
            fprintf(output, "|%s->%d,%d", dirs[i], prox.x, prox.y);
            visitados[prox.x][prox.y] = true;
            
            if(encontrarSaida(labirinto, visitados, dead, linhas, colunas, prox, inicio, output, saida)) {
                found = true;
            } else {
                fprintf(output, "|BT@%d,%d->%d,%d", prox.x, prox.y, atual.x, atual.y);
                visitados[prox.x][prox.y] = false;
                dead[prox.x][prox.y] = true; // Marca como dead-end
            }
        }
    }

    if(!found) dead[atual.x][atual.y] = true; // Marca célula atual como morta
    return found;
}