#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <pthread.h>
#include <cmath>
#include <cstdlib>

struct Matriz{
    int linhas;
    int colunas;
    std::vector<std::vector<int>> dados;
};

struct ThreadData{
    int thread_id;
    int elementos_por_thread;
    const Matriz* m1;
    const Matriz* m2;
    Matriz* resultado;
};

Matriz ler_matriz(const std::string& nome_arquivo){
    std::ifstream arquivo(nome_arquivo);
    Matriz mat;
    if(!arquivo.is_open()){
        std::cerr << "Erro ao abrir o arquivo " << nome_arquivo << std::endl;
        mat.linhas = 0;
        mat.colunas = 0;
        return mat;
    }
    arquivo >> mat.linhas >> mat.colunas;
    mat.dados.resize(mat.linhas, std::vector<int>(mat.colunas));
    for(int i = 0; i < mat.linhas; ++i){
        for(int j = 0; j < mat.colunas; ++j){
            arquivo >> mat.dados[i][j];
        }
    }
    arquivo.close();
    return mat;
}

void* multiplicar_parte(void* arg){
    ThreadData* data = (ThreadData*) arg;

    auto inicio = std::chrono::high_resolution_clock::now();

    int total_elementos = data->resultado->linhas * data->resultado->colunas;
    int start_element = data->thread_id * data->elementos_por_thread;
    int end_element = std::min(start_element + data->elementos_por_thread, total_elementos);

    for(int k = start_element; k < end_element; ++k){
        int i = k / data->resultado->colunas;
        int j = k % data->resultado->colunas;

        int soma = 0;
        for(int x = 0; x < data->m1->colunas; ++x){
            soma += data->m1->dados[i][x] * data->m2->dados[x][j];
        }
        data->resultado->dados[i][j] = soma;
    }

    auto fim = std::chrono::high_resolution_clock::now();
    long long duracao_us = std::chrono::duration_cast