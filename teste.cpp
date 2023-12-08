#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <queue>
#include <cmath>
#include <algorithm>

using namespace std;

// Função que verifica se um endereço está presente na cache2
bool isHit(const vector<unsigned int> &cache2, unsigned int address) {
    return find(cache2.begin(), cache2.end(), address) != cache2.end();
}

// Estrutura para armazenar informações de cada linha da cache2
struct CacheEntry {
    int index; // Índice da linha na cache2
    bool valid; // Flag para indicar se a linha está válida ou não
    unsigned int address; // Endereço armazenado na linha
};

// Função principal
int main(int argc, char* argv[]) {
    // Verifica se o número correto de argumentos foi fornecido na linha de comando
    if (argc != 5) {
        cerr << "Uso correto: ./nome_do_programa <tamanho_total_cache> <tamanho_linha> <tamanho_grupo> <arquivo_acessos>" << endl;
        return 1;
    }

    // Extrai os argumentos passados na linha de comando
    int cacheSize = stoi(argv[1]); // Tamanho total da cache2, em bytes
    int lineSize = stoi(argv[2]); // Tamanho de cada linha, em bytes
    int groupSize = stoi(argv[3]); // Tamanho de cada grupo (associação) em linhas
    unsigned int desc = log2(lineSize); //Deslocamento a ser ignorado
    unsigned int quantGroup = (cacheSize/lineSize)/groupSize; //Quantidade de grupos
    unsigned int quantLine = cacheSize/lineSize; // Quantidade de linhas  
    string fileName = argv[4]; // Nome do arquivo com os acessos à memória

    // Abre o arquivo de entrada
    ifstream file(fileName);
    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo." << endl;
        return 1;
    }

    // Vetor para armazenar os endereços lidos do arquivo
    vector<string> addresses;
    string line;
    // Lê os endereços do arquivo e os armazena no vetor addresses
    while (getline(file, line)) {
        addresses.push_back(line);
    }
    file.close();

    // Vetor para armazenar os endereços da cache2
    // Vetor para armazenar o histórico de estados da cache2 (para a saída)
    vector<vector<CacheEntry>> cacheHistory;
    int hits = 0; // Contador de hits
    int misses = 0; // Contador de misses

    // Fila para manter o controle da substituição de linhas na cache2

    vector<queue<int>> cacheQueue2(quantGroup);

    vector<unsigned int> cache2(quantLine,-1);

    //Verifica se é associatividade por conjunto
    if(groupSize == 2 || groupSize == 3){
        //Implementar aqui
        for (const auto &address : addresses) {
            unsigned int addr = stoul(address, nullptr, 16); // Converte o endereço hexadecimal para inteiro
            unsigned int transformedAddr = (addr >> desc) << desc; // Remove os bits menos significativos para obter o bloco
            unsigned int blockIdentifier = transformedAddr >> desc; // Obtém o identificador de bloco

            bool hit = isHit(cache2, blockIdentifier); // Verifica se o bloco está presente na cache2
            CacheEntry entry;
            entry.valid = true;
            entry.address = blockIdentifier;
           
            unsigned int group_adrr = quantGroup - (blockIdentifier%quantGroup) -1;

            // Se o bloco não estiver na cache2
            if (!hit) {
                // Se a o grupo correspondente não tiver cheio
                int inicio = group_adrr * groupSize; // qual  grupo x quantidade de grupos para encontrar indice da cache2 inicial daquele grupo
                int fim = inicio + groupSize - 1;
                int index=-1;
                bool encontrado = false;
                for (int i = inicio; i <= fim; ++i) {
                    if (cache2[i] == -1) {
                    encontrado = true;
                    index = i;
                    break; // Se encontrou, pode sair do loop
                    }
                }
                if (encontrado) {
                    cache2[index]=(blockIdentifier); // Adiciona o bloco na cache2
                    cacheQueue2[group_adrr].push(index); // Adiciona o índice à fila de controle
                } else {
                    // Se a cache2 estiver cheia, substitui o bloco mais antigo
                    int oldestIndex = cacheQueue2[group_adrr].front();
                    cache2[oldestIndex] = blockIdentifier;
                    cacheQueue2[group_adrr].pop();
                    cacheQueue2[group_adrr].push(oldestIndex);
                }
                misses++; // Incrementa o contador de misses
            } else {
                hits++; // Incrementa o contador de hits
            }
                 // Cria um snapshot do estado atual da cache2
            vector<CacheEntry> cacheSnapshot;
            for (int i = 0; i < cache2.size(); ++i) {
                CacheEntry cacheLine;
                cacheLine.index = i;
                cacheLine.valid = true;
                cacheLine.address = cache2[i];
                cacheSnapshot.push_back(cacheLine);
            }

            // Preenche o restante do snapshot com linhas vazias, se necessário
            while (cacheSnapshot.size() < static_cast<size_t>(cacheSize / lineSize)) {
                CacheEntry emptyLine;
                emptyLine.valid = false;
                cacheSnapshot.push_back(emptyLine);
            }

            // Adiciona o snapshot ao histórico da cache2
            cacheHistory.push_back(cacheSnapshot);
            }

        }
            

    //Verifica se é mapeamento direto
    else if(groupSize == 4){
        //Implementar aqui
    }

    for(int i = 0; i < cache2.size(); i++){
        std::cout << cache2[i] << '\n';
    }

    // Abre o arquivo de saída
    ofstream output("saida.txt");
    if (!output.is_open()) {
        cerr << "Erro ao criar o arquivo de saída." << endl;
        return 1;
    }

    // Escreve os snapshots da cache2 no arquivo de saída
    for (const auto &snapshot : cacheHistory) {
        output << "================" << endl;
        output << "IDX V ** ADDR **" << endl;
        
        int lineCount = 0;
        for (const auto &entry : snapshot) {
            if (entry.valid) {
                std::stringstream ss;
                ss << std::setw(8) << std::setfill('0') << hex << entry.address;
                if(ss.str() != "ffffffff")
                output << setw(3) << setfill('0') << lineCount << " " << entry.valid << " 0x" << setw(8) << setfill('0') << hex << entry.address << endl;
                else 
                output << setw(3) << setfill('0') << lineCount << " " << '0' << " 0" << endl;
            } else {
                output << setw(3) << setfill('0') << lineCount << " " << entry.valid << " 0" << endl;
            }
            lineCount++;
        }
    }

    // Escreve o número de hits e misses no arquivo de saída
    output << "#hits: " << hits << endl;
    output << "#miss: " << misses << endl;

    output.close(); // Fecha o arquivo de saída

    return 0; // Retorna 0 para indicar sucesso
}