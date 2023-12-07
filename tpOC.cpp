#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>
#include <queue>
#include <algorithm>

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
    int groupSize = std::stoi(argv[3]); // Tamanho de cada grupo (associação)
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

    // Loop principal para cada endereço lido do arquivo
    for (const auto &address : addresses) {
        unsigned int addr = std::stoul(address, nullptr, 16); // Converte o endereço hexadecimal para inteiro
        unsigned int transformedAddr = (addr >> 10) << 10; // Remove os bits menos significativos para obter o bloco
        unsigned int blockIdentifier = transformedAddr >> 10; // Obtém o identificador de bloco

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

    // Abre o arquivo de saída
    std::ofstream output("saida.txt");
    if (!output.is_open()) {
        std::cerr << "Erro ao criar o arquivo de saída." << std::endl;
        return 1;
    }

    // Escreve os snapshots da cache no arquivo de saída
    for (const auto &snapshot : cacheHistory) {
        output << "================" << std::endl;
        output << "IDX V ** ADDR **" << std::endl;
        
        for (const auto &entry : snapshot) {
            if (entry.valid) {
                output << std::setw(3) << std::setfill('0') << entry.index << " " << entry.valid << " 0x" << std::setw(8) << std::setfill('0') << std::hex << entry.address << std::endl;
            } else {
                output << std::setw(3) << std::setfill('0') << entry.index << " " << entry.valid << " 0" << std::endl;
            }
        }
    }

    // Escreve o número de hits e misses no arquivo de saída
    output << "#hits: " << hits << std::endl;
    output << "#miss: " << misses << std::endl;

    output.close(); // Fecha o arquivo de saída

    return 0; // Retorna 0 para indicar sucesso
}
