#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>

void gerar_e_salvar_matriz(int linhas, int colunas, const std::string& nome_arquivo){
    std::ofstream arquivo(nome_arquivo);
    if(!arquivo.is_open()){
        std::cerr << "Erro ao abrir o arquivo " << nome_arquivo << std::endl;
        return;
    }

    arquivo << linhas << " " << colunas << std::endl;

    for(int i = 0; i < linhas; ++i){
        for(int j = 0; j < colunas; ++j){
            arquivo << (rand() % 10) << (j == colunas - 1 ? "" : " ");
        }
        arquivo << std::endl;
    }

    arquivo.close();
    std::cout << "Matriz " << linhas << "x" << colunas << " gerada e salva em " << nome_arquivo << std::endl;
}

int main(int argc, char* argv[]){
    if(argc != 5){
        std::cerr << "Uso: " << argv[0] << " <n1> <m1> <n2> <m2>" << std::endl;
        return 1;
    }

    int n1 = std::atoi(argv[1]);
    int m1 = std::atoi(argv[2]);
    int n2 = std::atoi(argv[3]);
    int m2 = std::atoi(argv[4]);

    if(m1 != n2){
        std::cerr << "Erro: O número de colunas da primeira matriz (m1) deve ser igual ao número de linhas da segunda (n2)." << std::endl;
        return 1;
    }

    srand(time(nullptr));

    gerar_e_salvar_matriz(n1, m1, "matriz1.txt");
    gerar_e_salvar_matriz(n2, m2, "matriz2.txt");

    return 0;
}