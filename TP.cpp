#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <queue>
#include <algorithm>
#include <cmath>

// Função que verifica se um endereço está presente na cache
bool isHit(const std::vector<unsigned int> &cache, unsigned int address) {
    return std::find(cache.begin(), cache.end(), address) != cache.end();
}

// Estrutura para armazenar informações de cada linha da cache
struct CacheEntry {
    int index; // Índice da linha na cache
    bool valid; // Flag para indicar se a linha está válida ou não
    unsigned int address; // Endereço armazenado na linha
};

// Função principal
int main(int argc, char* argv[]) {
    // Verifica se o número correto de argumentos foi fornecido na linha de comando
    if (argc != 5) {
        std::cerr << "Uso correto: ./nome_do_programa <tamanho_total_cache> <tamanho_linha> <tamanho_grupo> <arquivo_acessos>" << std::endl;
        return 1;
    }

    // Extrai os argumentos passados na linha de comando
    int cacheSize = std::stoi(argv[1]); // Tamanho total da cache, em bytes
    int lineSize = std::stoi(argv[2]); // Tamanho de cada linha, em bytes
    int groupSize = std::stoi(argv[3]); // Tamanho de cada grupo (associação) em linhas
    unsigned int desc = std::log2(lineSize); // Deslocamento a ser ignorado
    unsigned int quantGroup = (cacheSize/lineSize)/groupSize; // Quantidade de grupos
    unsigned int quantLine = cacheSize/lineSize; // Quantidade de linhas
    std::string fileName = argv[4]; // Nome do arquivo com os acessos à memória

    // Abre o arquivo de entrada
    std::ifstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo." << std::endl;
        return 1;
    }

    // Vetor para armazenar os endereços lidos do arquivo
    std::vector<std::string> addresses;
    std::string line;
    // Lê os endereços do arquivo e os armazena no vetor addresses
    while (std::getline(file, line)) {
        addresses.push_back(line);
    }
    file.close();

    // Vetor para armazenar os endereços da cache
    std::vector<unsigned int> cache;
    // Vetor para armazenar o histórico de estados da cache (para a saída)
    std::vector<std::vector<CacheEntry>> cacheHistory;
    int hits = 0; // Contador de hits
    int misses = 0; // Contador de misses

    // Fila para manter o controle da substituição de linhas na cache
    std::queue<int> cacheQueue;

    std::vector<std::queue<int>> cacheQueue2(quantGroup);

    std::vector<unsigned int> cache2(quantLine,-1);

    //Verifica se é associatividade direta
    if(groupSize == cacheSize/lineSize){
        // Loop principal para cada endereço lido do arquivo
        for (const auto &address : addresses) {
            unsigned int addr = stoul(address, nullptr, 16); // Converte o endereço hexadecimal para inteiro
            unsigned int transformedAddr = (addr >> desc) << desc; // Remove os bits menos significativos para obter o bloco
            unsigned int blockIdentifier = transformedAddr >> desc; // Obtém o identificador de bloco

            bool hit = isHit(cache, blockIdentifier); // Verifica se o bloco está presente na cache

            CacheEntry entry;
            entry.valid = true;
            entry.address = blockIdentifier;
            
            // Se o bloco não estiver na cache
            if (!hit) {
                // Se a cache ainda não estiver cheia
                if (cache.size() < static_cast<size_t>(cacheSize / lineSize)) {
                    cache.push_back(blockIdentifier); // Adiciona o bloco na cache
                    cacheQueue.push(static_cast<int>(cache.size() - 1)); // Adiciona o índice à fila de controle
                } else {
                    // Se a cache estiver cheia, substitui o bloco mais antigo
                    int oldestIndex = cacheQueue.front();
                    cache[oldestIndex] = blockIdentifier;
                    cacheQueue.pop();
                    cacheQueue.push(oldestIndex);
                }
                misses++; // Incrementa o contador de misses
            } else {
                hits++; // Incrementa o contador de hits
            }

            // Cria um snapshot do estado atual da cache
            std::vector<CacheEntry> cacheSnapshot;
            for (int i = 0; i < cache.size(); ++i) {
                CacheEntry cacheLine;
                cacheLine.index = i;
                cacheLine.valid = true;
                cacheLine.address = cache[i];
                cacheSnapshot.push_back(cacheLine);
            }

            // Preenche o restante do snapshot com linhas vazias, se necessário
            while (cacheSnapshot.size() < static_cast<size_t>(cacheSize / lineSize)) {
                CacheEntry emptyLine;
                emptyLine.valid = false;
                cacheSnapshot.push_back(emptyLine);
            }

            // Adiciona o snapshot ao histórico da cache
            cacheHistory.push_back(cacheSnapshot);
        }
    }
    //Verifica se é associatividade por conjunto
    else if(groupSize < cacheSize/lineSize){
        //Implementar aqui
        for (const auto &address : addresses) {
            unsigned int addr = stoul(address, nullptr, 16); // Converte o endereço hexadecimal para inteiro
            unsigned int transformedAddr = (addr >> desc) << desc; // Remove os bits menos significativos para obter o bloco
            unsigned int blockIdentifier = transformedAddr >> desc; // Obtém o identificador de bloco

            bool hit = isHit(cache2, blockIdentifier); // Verifica se o bloco está presente na cache2
            CacheEntry entry;
            entry.valid = true;
            entry.address = blockIdentifier;
           
            unsigned int group_adrr = blockIdentifier%quantGroup;

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
            std::vector<CacheEntry> cacheSnapshot;
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

    // Abre o arquivo de saída
    std::ofstream output("saida.txt");
    if (!output.is_open()) {
        std::cerr << "Erro ao criar o arquivo de saída." << std::endl;
        return 1;
    }

    // Escreve os snapshots da cache2 no arquivo de saída
    for (const auto &snapshot : cacheHistory) {
        output << "================" << std::endl;
        output << "IDX V ** ADDR **" << std::endl;
        
        int lineCount = 0;
        for (const auto &entry : snapshot) {
            if (entry.valid) {
                std::stringstream ss;
                ss << std::setw(8) << std::setfill('0') << std::hex << entry.address;
                std::string addressStr = ss.str();
                std::transform(addressStr.begin(), addressStr.end(), addressStr.begin(), ::toupper); // Convert to uppercase
                if (addressStr != "FFFFFFFF")
                    output << std::setw(3) << std::setfill('0') << lineCount << " " << entry.valid << " 0x" << addressStr << std::endl;
                else
                    output << std::setw(3) << std::setfill('0') << lineCount << " " << '0' << " 0" << std::endl;
            } else {
                output << std::setw(3) << std::setfill('0') << lineCount << " " << entry.valid << " 0" << std::endl;
            }
            lineCount++;
        }
    }

    // Escreve o número de hits e misses no arquivo de saída
    output << std::endl;
    output << "#hits: " << hits << std::endl;
    output << "#miss: " << misses << std::endl;

    output.close(); // Fecha o arquivo de saída

    return 0; // Retorna 0 para indicar sucesso
}
