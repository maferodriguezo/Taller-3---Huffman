#include <iostream>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <bitset>
#include <vector>
#include <string>
#include "BinTree.h"

using namespace std;

// Estructura de datos para un nodo Huffman
struct HuffNode {
    char ch;
    long long freq;
    HuffNode(char c, long long f) : ch(c), freq(f) {}
    HuffNode(long long f) : ch(0), freq(f) {}
};

// Subclase para acceder a los atributos protegidos de BinTree
class HuffBinTree : public BinTree<HuffNode> {
public:
    using Super = BinTree<HuffNode>;
    using Super::m_V;
    using Super::m_L;
    using Super::m_R;

    HuffBinTree(const HuffNode& node) : Super(node) {}
};

// Comparador para la cola de prioridad
struct Compare {
    bool operator()(const HuffBinTree* a, const HuffBinTree* b) {
        return a->m_V.freq > b->m_V.freq;
    }
};

// Genera códigos binarios recursivamente
void generate_codes(const HuffBinTree* node, const string& path, unordered_map<char, string>& code_map) {
    if (!node->m_L && !node->m_R) {
        code_map[node->m_V.ch] = path;
        return;
    }
    if (node->m_L) generate_codes(static_cast<HuffBinTree*>(node->m_L), path + "0", code_map);
    if (node->m_R) generate_codes(static_cast<HuffBinTree*>(node->m_R), path + "1", code_map);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Uso: compress <input.txt> <output.huff>" << endl;
        return 1;
    }

    ifstream input(argv[1], ios::binary);
    if (!input.is_open()) {
        cerr << "No se pudo abrir el archivo de entrada." << endl;
        return 1;
    }

    // Leer archivo y contar frecuencias
    unordered_map<char, long long> freq;
    string content((istreambuf_iterator<char>(input)), istreambuf_iterator<char>());
    for (char c : content) freq[c]++;

    // Construir árbol de Huffman
    priority_queue<HuffBinTree*, vector<HuffBinTree*>, Compare> pq;
    for (auto& pair : freq) {
        pq.push(new HuffBinTree(HuffNode(pair.first, pair.second)));
    }

    while (pq.size() > 1) {
        auto left = pq.top(); pq.pop();
        auto right = pq.top(); pq.pop();
        long long total = left->m_V.freq + right->m_V.freq;
        auto parent = new HuffBinTree(HuffNode(total));
        parent->set_left(left);
        parent->set_right(right);
        pq.push(parent);
    }

    HuffBinTree* root = pq.top();
    unordered_map<char, string> code_map;
    generate_codes(root, "", code_map);

    // Escribir archivo comprimido
    ofstream output(argv[2], ios::binary);

    // Guardar tabla de frecuencias
    size_t size = freq.size();
    output.write(reinterpret_cast<const char*>(&size), sizeof(size));
    for (auto& pair : freq) {
        output.write(&pair.first, sizeof(char));
        output.write(reinterpret_cast<const char*>(&pair.second), sizeof(long long));
    }

    // Codificar texto
    string encoded;
    for (char c : content) {
        encoded += code_map[c];
    }

    // Rellenar hasta múltiplo de 8 bits
    while (encoded.size() % 8 != 0) encoded += '0';

    // Guardar texto codificado como bytes
    for (size_t i = 0; i < encoded.size(); i += 8) {
        bitset<8> byte(encoded.substr(i, 8));
        unsigned char val = static_cast<unsigned char>(byte.to_ulong());
        output.write(reinterpret_cast<const char*>(&val), 1);
    }

    input.close();
    output.close();
    cout << "Archivo comprimido correctamente." << endl;

    return 0;
}
