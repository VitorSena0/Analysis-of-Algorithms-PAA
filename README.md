# Analysis of Algorithms (PAA)

Resumo
-------
Repositório com implementações em C de problemas e técnicas comuns em Análise de Algoritmos: ordenação, heaps, programação dinâmica, busca em cadeias, compressão, backtracking, etc.

O que cada código faz
----------------------------------------
- 1_mergeSort/_porto.c  
  Implementa Merge Sort para ordenar containers e processa inspeções; gera duas filas (inconsistência de CNPJ e diferença de peso) e escreve saída formatada.

- 1_mergeSort/mergeSortThread.c  
  Merge Sort paralelo usando pthreads para dividir trabalho entre threads (exemplo com array aleatório).

- 2_quickSort/_quicksort.c  
  Várias versões de QuickSort (Lomuto/Hoare) com pivôs (mediana de três, aleatório) e conta operações para comparar desempenho entre variantes.

- 2_quickSort/quickSortTeste.c  
  Variante / arquivo de teste para as implementações de QuickSort; mede trocas e chamadas e imprime resultados.

- 3_heapSort/_datagrama.c  
  Usa um min-heap para ordenar/gerenciar pacotes (datagramas) por prioridade e monta saída no formato solicitado.

- 3_heapSort/rafaelDatagrama.c  
  Outra implementação (C) para processar datagramas usando heap/min-heap e imprimir pacotes na ordem correta.

- 3_heapSort/heapsort.py  
  Versão em Python da solução de heap para datagramas (mesma lógica do caso em C).

- 4_programacao Dinamica/transportadora.c  
  Resolve um problema tipo mochila com duas restrições (peso e volume) por programação dinâmica e aloca pacotes a veículos para maximizar valor transportado.

- 5_Busca Cadeias/_sequenciamento.c  
  Busca de padrões em sequências genéticas: indexa subcadeias, usa busca binária e KMP; calcula probabilidade de “doenças” e ordena os resultados.

- 6_compressao_de_dados/compressao.c  
  Implementa compressão por Huffman e RLE, compara os tamanhos e imprime a melhor taxa/formato (hexadecimal da saída comprimida).

- 6_compressao_de_dados/_compressao.c  
  Variante otimizada/alternativa da compressão (Huffman + RLE) com pequenas melhorias de implementação.

- 7_Backtrackingebranch-and-bound/_labirinto.c  
  Resolve labirintos por backtracking, marca dead-ends e registra movimentos e backtracks no arquivo de saída.

- 7_Backtrackingebranch-and-bound/backtracking.c  
  Variante do algoritmo de backtracking para labirintos com mesma lógica de movimentos, backtracking e saída formatada.

Como compilar / rodar (rápido)
------------------------------
- Compilar: gcc -O2 -Wall -std=c11 -o programa arquivo.c  
- Programas que leem/geram arquivos geralmente usam: ./programa entrada.txt saida.txt  
- Para o mergeSortThread use a flag -pthread: gcc -pthread ...

Autor
-----
Vitor Sena — https://github.com/VitorSena0
