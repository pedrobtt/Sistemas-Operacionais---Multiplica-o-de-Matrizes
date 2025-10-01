#include <iostream>
#include <vector>
#include <string>
#include <numeric>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <algorithm>
#include <glob.h>

long long ler_tempo_resultado(const std::string& nome_arquivo){
    std::ifstream arquivo(nome_arquivo);
    if(!arquivo.is_open()) return -1;

    long long tempo = -1;
    std::string linha_final;
    std::string linha_atual;
    
    while(std::getline(arquivo, linha_atual)){
        if(!linha_atual.empty()){
            linha_final = linha_atual;
        }
    }
    
    if(!linha_final.empty()){
        try{
            tempo = std::stoll(linha_final);
        } catch (...){
            return -1;
        }
    }
    return tempo;
}

long long ler_maior_tempo_parcial(const std::string& padrao_arquivo){
    glob_t glob_result;
    glob(padrao_arquivo.c_str(), GLOB_TILDE, NULL, &glob_result);

    long long max_tempo = -1;

    for(unsigned int i = 0; i < glob_result.gl_pathc; ++i){
        long long tempo_atual = ler_tempo_resultado(std::string(glob_result.gl_pathv[i]));
        if(tempo_atual > max_tempo){
            max_tempo = tempo_atual;
        }
    }

    globfree(&glob_result);
    return max_tempo;
}

int main(){
    const int NUM_EXECUCOES = 10;
    std::vector<int> tamanhos_matriz = {100, 200, 400, 800, 1600, 2270};

    std::cout << std::left << std::setw(15) << "Tamanho"
              << std::setw(20) << "Sequencial (us)"
              << std::setw(20) << "Threads (us)"
              << std::setw(20) << "Processos (us)" << std::endl;
    std::cout << "------------------------------------------------------------------" << std::endl;

    for(int size : tamanhos_matriz){
        std::vector<long long> tempos_seq, tempos_threads, tempos_processos;

        std::cout << std::left << std::setw(15) << (std::to_string(size) + "x" + std::to_string(size));
        std::cout.flush();

        for(int i = 0; i < NUM_EXECUCOES; ++i){
            system(("./auxiliar " + std::to_string(size) + " " + std::to_string(size) + " " + std::to_string(size) + " " + std::to_string(size) + " > /dev/null 2>&1").c_str());
            system("./sequencial matriz1.txt matriz2.txt > /dev/null");
            tempos_seq.push_back(ler_tempo_resultado("resultado_seq.txt"));
        }
        long long media_seq = std::accumulate(tempos_seq.begin(), tempos_seq.end(), 0LL) / NUM_EXECUCOES;
        std::cout << std::setw(20) << media_seq;
        std::cout.flush();

        int P = (size * size + 7) / 8;
        for(int i = 0; i < NUM_EXECUCOES; ++i){
            system(("./auxiliar " + std::to_string(size) + " " + std::to_string(size) + " " + std::to_string(size) + " " + std::to_string(size) + " > /dev/null 2>&1").c_str());
            system(("./paralelo_threads matriz1.txt matriz2.txt " + std::to_string(P) + " > /dev/null").c_str());
            tempos_threads.push_back(ler_maior_tempo_parcial("resultado_thread_*.txt"));
            system("rm resultado_thread_*.txt");
        }
        long long media_threads = std::accumulate(tempos_threads.begin(), tempos_threads.end(), 0LL) / NUM_EXECUCOES;
        std::cout << std::setw(20) << media_threads;
        std::cout.flush();

        for(int i = 0; i < NUM_EXECUCOES; ++i){
            system(("./auxiliar " + std::to_string(size) + " " + std::to_string(size) + " " + std::to_string(size) + " " + std::to_string(size) + " > /dev/null 2>&1").c_str());
            system(("./paralelo_processos matriz1.txt matriz2.txt " + std::to_string(P) + " > /dev/null").c_str());
            tempos_processos.push_back(ler_maior_tempo_parcial("resultado_processo_*.txt"));
            system("rm resultado_processo_*.txt");
        }
        long long media_processos = std::accumulate(tempos_processos.begin(), tempos_processos.end(), 0LL) / NUM_EXECUCOES;
        std::cout << std::setw(20) << media_processos << std::endl;
    }
    
    system("rm matriz1.txt matriz2.txt resultado_seq.txt");
    return 0;
}