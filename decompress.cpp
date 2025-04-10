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
    
    // Método para comprobar si es un nodo hoja
    bool isLeaf() const {
        return !m_L && !m_R;
    }
};

// Comparador para la cola de prioridad
struct Compare {
    bool operator()(const HuffBinTree* a, const HuffBinTree* b) {
        return a->m_V.freq > b->m_V.freq;
    }
};

// Función para imprimir el árbol (para depuración)
void printTree(HuffBinTree* node, string prefix = "") {
    if (node) {
        if (node->isLeaf()) {
            cout << prefix << "Leaf: '" << node->m_V.ch << "' (" << (int)node->m_V.ch << ") freq: " << node->m_V.freq << endl;
        } else {
            cout << prefix << "Internal: freq=" << node->m_V.freq << endl;
            printTree(static_cast<HuffBinTree*>(node->m_L), prefix + "  L: ");
            printTree(static_cast<HuffBinTree*>(node->m_R), prefix + "  R: ");
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Uso: decompress <input.huff> <output.txt>" << endl;
        return 1;
    }

    ifstream input(argv[1], ios::binary);
    if (!input.is_open()) {
        cerr << "No se pudo abrir el archivo comprimido." << endl;
        return 1;
    }

    // Leer tabla de frecuencias
    size_t size;
    input.read(reinterpret_cast<char*>(&size), sizeof(size));
    
    unordered_map<char, long long> freq;
    for (size_t i = 0; i < size; i++) {
        char c;
        long long f;
        input.read(&c, sizeof(char));
        input.read(reinterpret_cast<char*>(&f), sizeof(long long));
        freq[c] = f;
    }

    // Caso especial: si solo hay un carácter
    if (freq.size() == 1) {
        auto it = freq.begin();
        ofstream output(argv[2], ios::binary);
        for (long long i = 0; i < it->second; i++) {
            output.put(it->first);
        }
        output.close();
        cout << "Archivo descomprimido correctamente (un solo carácter)." << endl;
        return 0;
    }

    // Reconstruir árbol de Huffman exactamente como en la compresión
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
    
    // Debug: Imprimir el árbol para verificar la estructura
    // cout << "Árbol de Huffman:" << endl;
    // printTree(root);

    // Calcular la longitud total del texto original
    long long total_chars = 0;
    for (auto& pair : freq) {
        total_chars += pair.second;
    }
    
    // Leer los datos comprimidos
    vector<unsigned char> compressed_data;
    unsigned char byte;
    while (input.read(reinterpret_cast<char*>(&byte), 1)) {
        compressed_data.push_back(byte);
    }
    
    // Descomprimir
    ofstream output(argv[2], ios::binary);
    if (!output.is_open()) {
        cerr << "No se pudo abrir el archivo de salida." << endl;
        delete root;
        return 1;
    }

    // Decodificar bit a bit
    HuffBinTree* current = root;
    long long chars_decoded = 0;
    
    for (size_t i = 0; i < compressed_data.size() && chars_decoded < total_chars; i++) {
        unsigned char byte_val = compressed_data[i];
        
        for (int bit = 7; bit >= 0 && chars_decoded < total_chars; bit--) {
            bool bit_val = (byte_val >> bit) & 1;
            
            if (bit_val == 0) {
                current = static_cast<HuffBinTree*>(current->m_L);
            } else {
                current = static_cast<HuffBinTree*>(current->m_R);
            }
            
            // Si llegamos a una hoja
            if (current && current->isLeaf()) {
                output.put(current->m_V.ch);
                chars_decoded++;
                current = root; // Volver a la raíz
            }
        }
    }

    cout << "Caracteres decodificados: " << chars_decoded << " de " << total_chars << " esperados." << endl;

    input.close();
    output.close();
    
    // Liberar memoria
    delete root;
    
    cout << "Archivo descomprimido correctamente." << endl;
    return 0;
}