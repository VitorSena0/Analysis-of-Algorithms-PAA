#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

typedef struct Pacote {
    int numero_pacote;
    int tamanho;
    char **conteudo;
} Pacote;

void trocar_pacote(Pacote* pacotes, uint32_t a, uint32_t b);
uint32_t esquerdo(uint32_t i);
uint32_t direito(uint32_t i);
void construir_min_heap(Pacote* pacotes, uint32_t n);
void heapify_min(Pacote* pacotes, uint32_t T, uint32_t i);
void heapsort(Pacote* pacotes, uint32_t n);

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <arquivo_entrada> <arquivo_saida>\n", argv[0]);
        return 1;
    }

    FILE* input = fopen(argv[1], "r");
    FILE* output = fopen(argv[2], "w");

    if (!input || !output) {
        perror("Erro ao abrir os arquivos");
        return 1;
    }

    int total_pacotes, quantidade_ordenar;
    if (fscanf(input, "%d %d", &total_pacotes, &quantidade_ordenar) != 2) {
        fprintf(stderr, "Erro ao ler total_pacotes e quantidade_ordenar\n");
        fclose(input);
        fclose(output);
        return 1;
    }

    Pacote *pacotes = malloc((total_pacotes + quantidade_ordenar) * sizeof(Pacote));
    if (!pacotes) {
        perror("Erro ao alocar memória para pacotes");
        fclose(input);
        fclose(output);
        return 1;
    }

    int pacote_esperado = 0;
    int lidos = 0;

    for (int inicio = 0; inicio < total_pacotes; inicio += quantidade_ordenar) {
        int fim = inicio + quantidade_ordenar;
        if (fim > total_pacotes) {
            fim = total_pacotes;
        }

        for (int i = inicio; i < fim; i++) {
            lidos++;
            if (fscanf(input, "%d %d", &pacotes[i].numero_pacote, &pacotes[i].tamanho) != 2) {
                fprintf(stderr, "Erro ao ler numero_pacote e tamanho do pacote %d\n", i);
                for (int k = inicio; k < i; k++) {
                    if (pacotes[k].conteudo) {
                        for (int j = 0; j < pacotes[k].tamanho; j++) {
                            free(pacotes[k].conteudo[j]);
                        }
                        free(pacotes[k].conteudo);
                    }
                }
                free(pacotes);
                fclose(input);
                fclose(output);
                return 1;
            }

            pacotes[i].conteudo = malloc(pacotes[i].tamanho * sizeof(char*));
            if (!pacotes[i].conteudo) {
                perror("Erro ao alocar memória para conteudo");
                for (int k = inicio; k <= i; k++) {
                    if (pacotes[k].conteudo) {
                        for (int j = 0; j < pacotes[k].tamanho; j++) {
                            free(pacotes[k].conteudo[j]);
                        }
                        free(pacotes[k].conteudo);
                    }
                }
                free(pacotes);
                fclose(input);
                fclose(output);
                return 1;
            }

            for (int j = 0; j < pacotes[i].tamanho; j++) {
                pacotes[i].conteudo[j] = malloc(1612 * sizeof(char));
                if (!pacotes[i].conteudo[j]) {
                    perror("Erro ao alocar memória para conteudo[j]");
                    for (int k = inicio; k <= i; k++) {
                        if (pacotes[k].conteudo) {
                            for (int l = 0; l < pacotes[k].tamanho; l++) {
                                free(pacotes[k].conteudo[l]);
                            }
                            free(pacotes[k].conteudo);
                        }
                    }
                    free(pacotes);
                    fclose(input);
                    fclose(output);
                    return 1;
                }

                if (fscanf(input, "%s", pacotes[i].conteudo[j]) != 1) {
                    fprintf(stderr, "Erro ao ler conteudo[j] do pacote %d\n", i);
                    for (int k = inicio; k <= i; k++) {
                        if (pacotes[k].conteudo) {
                            for (int l = 0; l < pacotes[k].tamanho; l++) {
                                free(pacotes[k].conteudo[l]);
                            }
                            free(pacotes[k].conteudo);
                        }
                    }
                    free(pacotes);
                    fclose(input);
                    fclose(output);
                    return 1;
                }
            }
        }

        heapsort(pacotes, lidos);
        while (lidos > 0 && pacotes[0].numero_pacote == pacote_esperado) {
            for (int l = 0; l < pacotes[0].tamanho; l++) {
                fprintf(output, "%s", pacotes[0].conteudo[l]);
                if (l < pacotes[0].tamanho - 1) {
                    fprintf(output, ",");
                }
            }
            fprintf(output, "|");
            pacote_esperado++;
            trocar_pacote(pacotes, 0, --lidos);
            heapify_min(pacotes, lidos, 0);
        }
    }

    for (int i = 0; i < total_pacotes; i++) {
        if (pacotes[i].conteudo) {
            for (int j = 0; j < pacotes[i].tamanho; j++) {
                free(pacotes[i].conteudo[j]);
            }
            free(pacotes[i].conteudo);
        }
    }
    free(pacotes);

    fclose(input);
    fclose(output);
    return 0;
}

void trocar_pacote(Pacote* pacotes, uint32_t a, uint32_t b) {
    Pacote temp = pacotes[a];
    pacotes[a] = pacotes[b];
    pacotes[b] = temp;
}

uint32_t esquerdo(uint32_t i) {
    return 2 * i + 1;
}

uint32_t direito(uint32_t i) {
    return 2 * i + 2;
}

void construir_min_heap(Pacote* pacotes, uint32_t n) {
    for (int32_t i = n / 2 - 1; i >= 0; i--) {
        heapify_min(pacotes, n, i);
    }
}

void heapify_min(Pacote* pacotes, uint32_t T, uint32_t i) {
    uint32_t P = i, E = esquerdo(i), D = direito(i);

    if (E < T && pacotes[E].numero_pacote < pacotes[P].numero_pacote) {
        P = E;
    }

    if (D < T && pacotes[D].numero_pacote < pacotes[P].numero_pacote) {
        P = D;
    }

    if (P != i) {
        trocar_pacote(pacotes, P, i);
        heapify_min(pacotes, T, P);
    }
}

void heapsort(Pacote* pacotes, uint32_t n) {
    construir_min_heap(pacotes, n);

    for (uint32_t i = n - 1; i > 0; i--) {
        trocar_pacote(pacotes, 0, i); 
        heapify_min(pacotes, i, 0);    
    }
}