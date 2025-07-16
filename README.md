# Parallel Algorithms in C - OpenMP and MPI

Este repositório contém três algoritmos clássicos implementados em C, com versões sequenciais, paralelizadas com OpenMP e paralelizadas com MPI. O projeto faz parte de um estudo de caso sobre programação paralela, comparando desempenho, speedup, eficiência e escalabilidade.

## Algoritmos implementados

- **Bucket Sort** – Ordenação baseada em distribuição
- **Friendly Numbers** – Verificação de números amigos em um intervalo
- **Histogram** – Construção de histograma a partir de uma lista de inteiros

## Modelos de paralelização utilizados

- [x] OpenMP (paralelismo por threads)
- [x] MPI (paralelismo por processos)
- [x] Versões sequenciais para comparação

## Métricas analisadas

- Tempo de execução (em segundos)
- Speedup
- Eficiência
- Escalabilidade (com diferentes números de threads e tamanhos de entrada)
