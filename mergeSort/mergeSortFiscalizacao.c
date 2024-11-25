#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//-------[ Estruturas ]-------

typedef struct FilaContainerCNPJ{// Estrutura da fila de containers, serve para armazenar os containers que estão com os dados divergentes
    struct Container *container;
    struct FilaContainerCNPJ *ProxContainer;
    char cnpj[19];
}FilaContainerCNPJ;

typedef struct FilaContainerPeso{// Estrutura da fila de containers, serve para armazenar os containers que estão com os dados divergentes
    struct Container *container;
    int diferencaPeso, porcentagemPeso;
    struct FilaContainerPeso *ProxContainer;
}FilaContainerPeso;

typedef struct Container{
    char codigo[12], cnpj[19];
    int pesoKG;
    int prioridadeChegada;
}Container;

//-------[ Prototipos das funcoes ]-------

void processarDocumentos(char *input, char *output);
void mergeSort(Container *array, int tamanho);
void merge_sort_aux(Container *array, int inicio, int fim, Container *temp);
void intercalar(Container *array, int inicio, int meio, int fim, Container *temp);
int buscaBinaria(Container *array, int inicio, int fim, char *codigo);
int inspesionaContainer(Container *container, char *cnpj, int peso, int porcentagemDiff);
void adicionarFilaCNPJ(FilaContainerCNPJ **fila, Container *novoContainer,char *cnpj);
void adicionarFilaPeso(FilaContainerPeso **fila, Container *novoContainer, int diferencaPeso, int porcentagemPeso);
void liberarFilaCNPJ(FilaContainerCNPJ *fila);
void liberarFilaPeso(FilaContainerPeso *fila);

//--------------------[ Funcao principal ]--------------------
int main(int argc, char **argv){
    if(argc != 3){
        printf("Numero de argumentos invalidos\n");
        return -1;
    }
    processarDocumentos(argv[1], argv[2]);
    return 0;
}

//--------------------[ Funcao de processamento do input ]--------------------
void processarDocumentos(char *input, char *output){
    FILE *arqInput = fopen(input, "r");
    FILE *arqOutput = fopen(output, "w");
    if(!arqInput || !arqOutput){
        printf("Erro ao abrir os arquivos\n");
        if(arqInput) fclose(arqInput);
        if(arqOutput) fclose(arqOutput);
        return;
    }

    Container *containers = NULL;
   /*
    FilaContainerCNPJ *filaCNPJ = (FilaContainerCNPJ *)malloc(sizeof(FilaContainerCNPJ));
    filaCNPJ->ProxContainer = NULL;
    Porém, o ponteiro filaCNPJ inicializa com um nó vazio, o que causa problemas ao tratá-lo como uma fila inicialmente vazia. Isso porque:

    A função adicionarFilaCNPJ espera que *fila seja NULL para uma fila vazia.
    Você inicia a fila com um nó válido alocado, mas sem dados relevantes.

    Isto estava causando erro de segmentação de memória, pois você estava tentando acessar um ponteiro nulo.
   */
    FilaContainerCNPJ *filaCNPJ = NULL;
    FilaContainerPeso *filaPeso = NULL;

    int qtdContaineresCadastrados,qtdInspecaoConteineres, pesoContainer;
    char codigo[12], cnpj[19];

    if(fscanf(arqInput, "%d", &qtdContaineresCadastrados) != 1){
        printf("Erro ao ler a quantidade de containers cadastrados\n");
        fclose(arqInput);
        fclose(arqOutput);
    }

    containers = (Container *)malloc(qtdContaineresCadastrados * sizeof(Container));

    for(int i = 0; i < qtdContaineresCadastrados; i++){
        if(fscanf(arqInput, "%s %s %d", codigo, cnpj, &pesoContainer) != 3){
            printf("Erro ao ler os dados do container %d\n", i+1);
            fclose(arqInput);
            fclose(arqOutput);
        }
        strcpy(containers[i].codigo, codigo);
        strcpy(containers[i].cnpj, cnpj);
        containers[i].pesoKG = pesoContainer;
        containers[i].prioridadeChegada = i;
    }

    mergeSort(containers, qtdContaineresCadastrados);

    int posicaoContainer, resultadoInspecao;
    if(fscanf(arqInput, "%d", &qtdInspecaoConteineres) != 1){
        printf("Erro ao ler a quantidade de inspecões de containers\n");
        fclose(arqInput);
        fclose(arqOutput);
    }

    for(int i = 0; i < qtdInspecaoConteineres; i++){
        if(fscanf(arqInput, "%s %s %d", codigo, cnpj, &pesoContainer) != 3){
            printf("Erro ao ler os dados da inspecão do container %d\n", i+1);
            fclose(arqInput);
            fclose(arqOutput);
        }
        posicaoContainer = buscaBinaria(containers, 0, qtdContaineresCadastrados - 1, codigo);
        if(posicaoContainer == -1){
            printf("Container %s nao encontrado\n", codigo);
        }else{
            int diffPeso = pesoContainer < containers[posicaoContainer].pesoKG ? containers[posicaoContainer].pesoKG - pesoContainer : pesoContainer - containers[posicaoContainer].pesoKG;
            // Arredondar pra cima
            int porcentagemDiff = ((diffPeso * 100) + containers[posicaoContainer].pesoKG / 2) / containers[posicaoContainer].pesoKG;

            resultadoInspecao = inspesionaContainer(&containers[posicaoContainer], cnpj, pesoContainer, porcentagemDiff); 
            if(resultadoInspecao == 0){
                adicionarFilaCNPJ(&filaCNPJ, &containers[posicaoContainer], cnpj);
            }
            if(resultadoInspecao == 1){
                adicionarFilaPeso(&filaPeso, &containers[posicaoContainer], diffPeso, porcentagemDiff);
            }
            if(resultadoInspecao == 2){
                continue;
            }
        }
    }
    FilaContainerCNPJ *auxCNPJ = filaCNPJ;
  while (auxCNPJ != NULL) {
      if (auxCNPJ->container != NULL) {
          fprintf(arqOutput, "%s: %s<->%s\n", auxCNPJ->container->codigo, auxCNPJ->container->cnpj, auxCNPJ->cnpj);
      }
      auxCNPJ = auxCNPJ->ProxContainer;
  }


    FilaContainerPeso *auxPeso = filaPeso;
    while (auxPeso != NULL) {
        fprintf(arqOutput, "%s: %dkg (%d%%)\n", auxPeso->container->codigo, auxPeso->diferencaPeso, auxPeso->porcentagemPeso);
        auxPeso = auxPeso->ProxContainer;
    }

    liberarFilaCNPJ(filaCNPJ);
    liberarFilaPeso(filaPeso);

    fclose(arqInput);
    fclose(arqOutput);
    free(containers);
}


//----------------[ Intercalar os subarrays ordenados ]----------------
void intercalar(Container *array, int inicio, int meio, int fim, Container *temp) {
    int i = inicio, j = meio + 1, k = 0; // O k é o índice do array temporário
    // Intercala os dois subarrays ordenados
    // Complexidade O(n)
    while (i <= meio && j <= fim) {
        if (strcmp(array[i].codigo, array[j].codigo) <= 0)// Complexidade O(1)
            temp[k++] = array[i++];// Complexidade O(1)
        else
            temp[k++] = array[j++]; // Complexidade O(1)
    }
    /*
        Este dois whiles abaixo são para adicionar os elementos restantes dos subarrays
        Assim garantindo que todos os elementos sejam adicionados ao array temporário 
        e depois copiados de volta para o array original
        complexidade O(n)
    */
    while (i <= meio) temp[k++] = array[i++];
    while (j <= fim) temp[k++] = array[j++];

    // Copia os elementos ordenados de volta para o array original
    // Complexidade O(n)
    for (i = inicio, k = 0; i <= fim; i++, k++) {
        array[i] = temp[k]; // Complexidade O(1)
    }
    // Complexidade total: O(n + n + n) = O(3n) = O(n)
}

//----------------[ Função auxiliar de ordenação com Merge Sort recursivo ]----------------
void merge_sort_aux(Container *array, int inicio, int fim, Container *temp) {
    if (inicio < fim) {
        int meio = (inicio + fim) / 2;

        // Chama recursivamente para dividir e ordenar as duas metades
        merge_sort_aux(array, inicio, meio, temp);
        merge_sort_aux(array, meio + 1, fim, temp);

        // Intercala as duas metades ordenadas
        intercalar(array, inicio, meio, fim, temp);
    }
}

//----------------[ Função principal de ordenação com Merge Sort ]----------------
// Função principal de ordenação com Merge Sort
void mergeSort(Container *array, int tamanho) {
    Container *temp = (Container *) malloc(tamanho * sizeof(Container));
    if (temp == NULL) {
        fprintf(stderr, "Erro ao alocar memória\n");
        exit(EXIT_FAILURE);
    }
    merge_sort_aux(array, 0, tamanho - 1, temp);

    free(temp);
}

//----------------[ Função de busca binária para encontrar um container ]----------------
int buscaBinaria(Container *array, int inicio, int fim, char *codigo){
    if(inicio > fim){
        return -1;
    }
    int meio = (inicio + fim) / 2;
    if(strcmp(array[meio].codigo, codigo) == 0){
        return meio;
    }else if(strcmp(array[meio].codigo, codigo) < 0){
        return buscaBinaria(array, meio + 1, fim, codigo);
    }else{
        return buscaBinaria(array, inicio, meio - 1, codigo);
    }
}

//----------------[ Função de inspeção de container ]----------------
int inspesionaContainer(Container *container, char *cnpj, int peso, int porcentagemDiff){
    if(strcmp(container->cnpj, cnpj) != 0){// Se strcmp for diferente de 0, os CNPJs são diferentes
        return 0;
    }
    if(porcentagemDiff > 10){
        return 1;
    }

    return 2;
}

//----------------[ Função de adicionar container na fila de CNPJ ]----------------
void adicionarFilaCNPJ(FilaContainerCNPJ **fila, Container *novoContainer, char *cnpj) {
    FilaContainerCNPJ *novoNo = (FilaContainerCNPJ *)malloc(sizeof(FilaContainerCNPJ));
    novoNo->container = novoContainer;
    strcpy(novoNo->cnpj, cnpj);
    novoNo->ProxContainer = NULL;


  if (*fila == NULL || (*fila)->container->prioridadeChegada > novoContainer->prioridadeChegada) {
    novoNo->ProxContainer = *fila; // Pode ser NULL, o que está correto
    *fila = novoNo;
  } else {
    FilaContainerCNPJ *atual = *fila;
    while (atual->ProxContainer != NULL &&
           atual->ProxContainer->container->prioridadeChegada <= novoContainer->prioridadeChegada) {
        atual = atual->ProxContainer;
    }

    // Insere o novo nó no meio ou no final
    novoNo->ProxContainer = atual->ProxContainer;
    atual->ProxContainer = novoNo;
  }

}

//----------------[ Função de adicionar container na fila de peso ]----------------
void adicionarFilaPeso(FilaContainerPeso **fila, Container *novoContainer, int diferencaPeso, int porcentagemPeso) {
    FilaContainerPeso *novoNo = (FilaContainerPeso *)malloc(sizeof(FilaContainerPeso));
    novoNo->container = novoContainer;
    novoNo->diferencaPeso = diferencaPeso;
    novoNo->porcentagemPeso = porcentagemPeso;
    novoNo->ProxContainer = NULL;

    if (*fila == NULL || 
        (*fila)->porcentagemPeso < porcentagemPeso || 
        ((*fila)->porcentagemPeso == porcentagemPeso && (*fila)->container->prioridadeChegada > novoContainer->prioridadeChegada)) {
        // Adiciona no início se a fila estiver vazia ou se o novo container tiver maior prioridade
        novoNo->ProxContainer = *fila;
        *fila = novoNo;
    } else {
        // Percorre a fila até encontrar o ponto correto de inserção
        FilaContainerPeso *atual = *fila;
        while (atual->ProxContainer != NULL &&
               (atual->ProxContainer->porcentagemPeso > porcentagemPeso || 
               (atual->ProxContainer->porcentagemPeso == porcentagemPeso && 
                atual->ProxContainer->container->prioridadeChegada <= novoContainer->prioridadeChegada))) {
            atual = atual->ProxContainer;
        }

        // Insere o novo nó no meio ou no final
        novoNo->ProxContainer = atual->ProxContainer;
        atual->ProxContainer = novoNo;
    }
}


//---------------[ Funções para liberar memória das filas ]----------------
void liberarFilaCNPJ(FilaContainerCNPJ *fila) {
    FilaContainerCNPJ *atual = fila;
    while (atual != NULL) {
        FilaContainerCNPJ *proximo = atual->ProxContainer;
        free(atual);
        atual = proximo;
    }
}

void liberarFilaPeso(FilaContainerPeso *fila) {
    FilaContainerPeso *atual = fila;
    while (atual != NULL) {
        FilaContainerPeso *proximo = atual->ProxContainer;
        free(atual);
        atual = proximo;
    }
}

