#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>

struct Matriz{
    int linhas;
    int colunas;
    std::vector<std::vector<int>> dados;
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

void salvar_resultado(const Matriz& mat, long long tempo_us, const std::string& nome_arquivo){
    std::ofstream arquivo(nome_arquivo);
    if(!arquivo.is_open()){
        std::cerr << "Erro ao salvar o resultado em " << nome_arquivo << std::endl;
        return;
    }

    arquivo << mat.linhas << " " << mat.colunas << std::endl;
    for(int i = 0; i < mat.linhas; ++i){
        for(int j = 0; j < mat.colunas; ++j){
            arquivo << mat.dados[i][j] << (j == mat.colunas - 1 ? "" : " ");
        }
        arquivo << std::endl;
    }
    
    arquivo << tempo_us << std::endl;
    arquivo.close();
}

int main(int argc, char* argv[]){
    if(argc != 3){
        std::cerr << "Uso: " << argv[0] << " <arquivo_matriz1> <arquivo_matriz2>" << std::endl;
        return 1;
    }

    Matriz m1 = ler_matriz(argv[1]);
    Matriz m2 = ler_matriz(argv[2]);

    if(m1.colunas != m2.linhas){
        std::cerr << "As matrizes não podem ser multiplicadas." << std::endl;
        return 1;
    }

    Matriz resultado;
    resultado.linhas = m1.linhas;
    resultado.colunas = m2.colunas;
    resultado.dados.resize(resultado.linhas, std::vector<int>(resultado.colunas, 0));

    auto inicio = std::chrono::high_resolution_clock::now();

    for(int i = 0; i < m1.linhas; ++i){
        for(int j = 0; j < m2.colunas; ++j){
            for(int k = 0; k < m1.colunas; ++k){
                resultado.dados[i][j] += m1.dados[i][k] * m2.dados[k][j];
            }
        }
    }

    auto fim = std::chrono::high_resolution_clock::now();

    long long duracao_us = std::chrono::duration_cast<std::chrono::microseconds>(fim - inicio).count();
    
    std::cout << "Multiplicação sequencial concluída em " << duracao_us << " us." << std::endl;

    salvar_resultado(resultado, duracao_us, "resultado_seq.txt");

    return 0;
}