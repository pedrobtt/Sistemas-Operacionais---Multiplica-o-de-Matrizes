#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>

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

void multiplicar_e_salvar_parte(int processo_id, int elementos_por_processo, const Matriz& m1, const Matriz& m2, const Matriz& resultado_modelo) {
    auto inicio = std::chrono::high_resolution_clock::now();
    int total_elementos = resultado_modelo.linhas * resultado_modelo.colunas;
    int start_element = processo_id * elementos_por_processo;
    int end_element = std::min(start_element + elementos_por_processo, total_elementos);
    std::vector<int> resultados_parciais;
    resultados_parciais.reserve(end_element - start_element);

    for(int k = start_element; k < end_element; ++k){
        int i = k / resultado_modelo.colunas;
        int j = k % resultado_modelo.colunas;
        int soma = 0;
        for(int x = 0; x < m1.colunas; ++x) {
            soma += m1.dados[i][x] * m2.dados[x][j];
        }
        resultados_parciais.push_back(soma);
    }

    auto fim = std::chrono::high_resolution_clock::now();
    long long duracao_us = std::chrono::duration_cast<std::chrono::microseconds>(fim - inicio).count();

    std::string nome_arquivo = "resultado_processo_" + std::to_string(processo_id) + ".txt";
    std::ofstream arquivo_saida(nome_arquivo);
    
    arquivo_saida << resultado_modelo.linhas << " " << resultado_modelo.colunas << std::endl;
    
    int idx_parcial = 0;
    for(int k = start_element; k < end_element; ++k){
        int i = k / resultado_modelo.colunas;
        int j = k % resultado_modelo.colunas;
        arquivo_saida << "c" << i << "," << j << " " << resultados_parciais[idx_parcial++] << std::endl;
    }
    
    arquivo_saida << duracao_us << std::endl;
    arquivo_saida.close();
}

int main(int argc, char* argv[]){
    if(argc != 4){
        std::cerr << "Uso: " << argv[0] << " <arquivo_matriz1> <arquivo_matriz2> <P_elementos_por_processo>" << std::endl;
        return 1;
    }

    Matriz m1 = ler_matriz(argv[1]);
    Matriz m2 = ler_matriz(argv[2]);
    int P = std::atoi(argv[3]);

    if(m1.colunas != m2.linhas){
        std::cerr << "As matrizes não podem ser multiplicadas." << std::endl;
        return 1;
    }

    Matriz resultado_modelo;
    resultado_modelo.linhas = m1.linhas;
    resultado_modelo.colunas = m2.colunas;

    int total_elementos_resultado = resultado_modelo.linhas * resultado_modelo.colunas;
    int num_processos = ceil((double)total_elementos_resultado / P);

    std::cout << "Calculando com " << num_processos << " processos..." << std::endl;

    pid_t pid;

    for(int i = 0; i < num_processos; ++i){
        pid = fork();
        if(pid < 0){
            std::cerr << "Erro no fork()" << std::endl;
            return 1;
        }else if(pid == 0){
            multiplicar_e_salvar_parte(i, P, m1, m2, resultado_modelo);
            exit(0);
        }
    }

    for(int i = 0; i < num_processos; ++i){
        wait(NULL);
    }

    std::cout << "Todos os processos filhos terminaram. Os resultados parciais foram salvos." << std::endl;

    return 0;
}