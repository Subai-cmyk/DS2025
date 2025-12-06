#include <iostream>
#include <string>
#include <cstring>
#include <queue>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <fstream>

using namespace std;

// 位图类 Bitmap
class Bitmap {
private:
    unsigned char* M;
    int N, _sz;

protected:
    void init(int n) {
        M = new unsigned char[N = (n + 7) / 8];
        memset(M, 0, N);
        _sz = 0;
    }

public:
    Bitmap(int n = 8) { init(n); }
    
    Bitmap(const char* file, int n = 8) {
        init(n);
        FILE* fp = fopen(file, "r");
        if (fp) {
            fread(M, sizeof(char), N, fp);
            fclose(fp);
        }
        for (int k = 0; k < n; k++) _sz += test(k);
    }
    
    ~Bitmap() {
        delete[] M;
        M = NULL;
        _sz = 0;
    }
    
    int size() { return _sz; }
    
    void set(int k) {
        expand(k);
        if (!test(k)) {
            _sz++;
            M[k >> 3] |= (0x80 >> (k & 0x07));
        }
    }
    
    void clear(int k) {
        expand(k);
        if (test(k)) {
            _sz--;
            M[k >> 3] &= ~(0x80 >> (k & 0x07));
        }
    }
    
    bool test(int k) {
        expand(k);
        return M[k >> 3] & (0x80 >> (k & 0x07));
    }
    
    void dump(const char* file) {
        FILE* fp = fopen(file, "w");
        if (fp) {
            fwrite(M, sizeof(char), N, fp);
            fclose(fp);
        }
    }
    
    char* bits2string(int n) {
        expand(n - 1);
        char* s = new char[n + 1];
        s[n] = '\0';
        for (int i = 0; i < n; i++) {
            s[i] = test(i) ? '1' : '0';
        }
        return s;
    }
    
    void expand(int k) {
        if (k < 8 * N) return;
        
        int oldN = N;
        unsigned char* oldM = M;
        init(2 * k);
        memcpy(M, oldM, oldN);
        delete[] oldM;
    }
};

// 二叉树节点类
template<typename T>
struct BinNode {
    T data;
    int weight;
    BinNode<T>* parent;
    BinNode<T>* lc;
    BinNode<T>* rc;
    
    BinNode() : parent(NULL), lc(NULL), rc(NULL), weight(0) {}
    BinNode(T e, int w, BinNode<T>* p = NULL, BinNode<T>* l = NULL, BinNode<T>* r = NULL)
        : data(e), weight(w), parent(p), lc(l), rc(r) {}
    
    bool isLeaf() { return !lc && !rc; }
};

// 二叉树类
template<typename T>
class BinTree {
protected:
    BinNode<T>* _root;
    int _size;
    
    // 修改：removeAt现在返回删除的节点数
    int removeAt(BinNode<T>* x) {
        if (!x) return 0;
        int count = 1; // 当前节点
        count += removeAt(x->lc);
        count += removeAt(x->rc);
        delete x;
        return count;
    }
    
public:
    BinTree() : _root(NULL), _size(0) {}
    ~BinTree() { 
        if (_root) {
            removeAt(_root);
        }
    }
    
    int size() const { return _size; }
    bool empty() const { return !_root; }
    BinNode<T>* root() const { return _root; }
    
    BinNode<T>* insertAsRoot(const T& e, int w) {
        _size = 1;
        return _root = new BinNode<T>(e, w);
    }
    
    BinNode<T>* insertAsLC(BinNode<T>* x, const T& e, int w) {
        _size++;
        x->lc = new BinNode<T>(e, w, x);
        return x->lc;
    }
    
    BinNode<T>* insertAsRC(BinNode<T>* x, const T& e, int w) {
        _size++;
        x->rc = new BinNode<T>(e, w, x);
        return x->rc;
    }
    
    // 修改：简化remove方法
    int remove(BinNode<T>* x) {
        if (!x) return 0;
        
        if (x->parent) {
            if (x->parent->lc == x) 
                x->parent->lc = NULL;
            else 
                x->parent->rc = NULL;
        }
        
        if (x == _root) {
            _root = NULL;
        }
        
        int n = removeAt(x);
        _size -= n;
        return n;
    }
};

// Huffman编码树类
class HuffTree {
private:
    BinTree<char>* _tree;
    unordered_map<char, string> _codeMap;
    
    struct NodeCompare {
        bool operator()(BinNode<char>* a, BinNode<char>* b) {
            return a->weight > b->weight;
        }
    };
    
    void generateCodes(BinNode<char>* node, string code) {
        if (!node) return;
        
        if (node->isLeaf()) {
            _codeMap[node->data] = code;
            return;
        }
        
        generateCodes(node->lc, code + "0");
        generateCodes(node->rc, code + "1");
    }
    
    // 辅助函数：释放Huffman树构建过程中创建的节点
    void cleanupPriorityQueue(priority_queue<BinNode<char>*, vector<BinNode<char>*>, NodeCompare>& pq) {
        while (!pq.empty()) {
            BinNode<char>* node = pq.top();
            pq.pop();
            // 注意：这里不能直接delete，因为节点可能已经被转移到树结构中
        }
    }
    
public:
    HuffTree() : _tree(new BinTree<char>()) {}
    ~HuffTree() { 
        delete _tree; 
    }
    
    void build(const unordered_map<char, int>& freqMap) {
        if (freqMap.empty()) return;
        
        // 清理之前的树
        delete _tree;
        _tree = new BinTree<char>();
        _codeMap.clear();
        
        if (freqMap.size() == 1) {
            // 只有一个字符的特殊情况
            auto it = freqMap.begin();
            _tree->insertAsRoot(it->first, it->second);
            _codeMap[it->first] = "0";
            return;
        }
        
        priority_queue<BinNode<char>*, vector<BinNode<char>*>, NodeCompare> pq;
        
        // 为每个字符创建单节点树
        for (const auto& pair : freqMap) {
            BinNode<char>* node = new BinNode<char>(pair.first, pair.second);
            pq.push(node);
        }
        
        // 构建Huffman树
        while (pq.size() > 1) {
            BinNode<char>* x = pq.top(); pq.pop();
            BinNode<char>* y = pq.top(); pq.pop();
            
            BinNode<char>* parent = new BinNode<char>('\0', x->weight + y->weight);
            parent->lc = x;
            parent->rc = y;
            x->parent = parent;
            y->parent = parent;
            
            pq.push(parent);
        }
        
        // 设置根节点
        BinNode<char>* huffmanRoot = pq.top();
        _tree->insertAsRoot('\0', huffmanRoot->weight);
        _tree->root()->lc = huffmanRoot->lc;
        _tree->root()->rc = huffmanRoot->rc;
        
        if (_tree->root()->lc) _tree->root()->lc->parent = _tree->root();
        if (_tree->root()->rc) _tree->root()->rc->parent = _tree->root();
        
        // 释放临时根节点
        delete huffmanRoot;
        
        // 生成编码
        generateCodes(_tree->root(), "");
    }
    
    string encode(const string& text) {
        string result = "";
        for (char c : text) {
            char lowerC = tolower(c);
            if (_codeMap.find(lowerC) != _codeMap.end()) {
                result += _codeMap[lowerC];
            }
        }
        return result;
    }
    
    string decode(const string& code) {
        string result = "";
        BinNode<char>* current = _tree->root();
        
        for (char bit : code) {
            if (bit == '0') {
                current = current->lc;
            } else {
                current = current->rc;
            }
            
            if (current && current->isLeaf()) {
                result += current->data;
                current = _tree->root();
            }
        }
        
        return result;
    }
    
    const unordered_map<char, string>& getCodeMap() const {
        return _codeMap;
    }
    
    void printCodes() {
        cout << "Huffman Codes:" << endl;
        vector<pair<char, string>> codes(_codeMap.begin(), _codeMap.end());
        sort(codes.begin(), codes.end(), [](const auto& a, const auto& b) {
            return a.second.length() < b.second.length() || 
                  (a.second.length() == b.second.length() && a.first < b.first);
        });
        
        for (const auto& pair : codes) {
            cout << "'" << pair.first << "': " << pair.second << endl;
        }
    }
};

// 文本处理函数
unordered_map<char, int> calculateFrequency(const string& text) {
    unordered_map<char, int> freqMap;
    
    for (char c : text) {
        if (isalpha(c)) {
            char lowerC = tolower(c);
            freqMap[lowerC]++;
        }
    }
    
    return freqMap;
}

// 主函数
int main() {
    // 《I Have a Dream》演讲片段
    string speech = 
        "I have a dream that one day this nation will rise up and live out the true meaning of its creed. "
        "I have a dream that one day on the red hills of Georgia the sons of former slaves and the sons of "
        "former slave owners will be able to sit down together at the table of brotherhood. "
        "I have a dream that one day even the state of Mississippi a state sweltering with the heat of "
        "injustice sweltering with the heat of oppression will be transformed into an oasis of freedom and justice. "
        "I have a dream that my four little children will one day live in a nation where they will not be "
        "judged by the color of their skin but by the content of their character. I have a dream today.";
    
    // 计算字母频率
    unordered_map<char, int> freqMap = calculateFrequency(speech);
    
    cout << "Character Frequencies in 'I Have a Dream':" << endl;
    int totalChars = 0;
    vector<pair<char, int>> freqVec(freqMap.begin(), freqMap.end());
    sort(freqVec.begin(), freqVec.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });
    
    for (const auto& pair : freqVec) {
        cout << "'" << pair.first << "': " << pair.second << endl;
        totalChars += pair.second;
    }
    cout << "Total alphabetic characters: " << totalChars << endl << endl;
    
    // 构建Huffman树
    HuffTree huffTree;
    huffTree.build(freqMap);
    
    // 显示编码表
    huffTree.printCodes();
    cout << endl;
    
    // 测试编码
    vector<string> testWords = {"dream", "freedom", "justice", "brotherhood", "nation"};
    
    for (const string& word : testWords) {
        string encoded = huffTree.encode(word);
        string decoded = huffTree.decode(encoded);
        
        cout << "Word: " << word << endl;
        cout << "Encoded: " << encoded << endl;
        cout << "Decoded: " << decoded << endl;
        cout << "Length: " << encoded.length() << " bits" << endl;
        cout << "Original would be: " << word.length() * 8 << " bits (ASCII)" << endl;
        cout << "Compression ratio: " << (double)encoded.length() / (word.length() * 8) * 100 << "%" << endl << endl;
    }
    
    // 使用Bitmap存储编码
    string dreamEncoded = huffTree.encode("dream");
    Bitmap bitmap(dreamEncoded.length());
    
    for (int i = 0; i < dreamEncoded.length(); i++) {
        if (dreamEncoded[i] == '1') {
            bitmap.set(i);
        }
    }
    
    cout << "Bitmap representation of 'dream': ";
    char* bitmapStr = bitmap.bits2string(dreamEncoded.length());
    cout << bitmapStr << endl;
    delete[] bitmapStr;
    
    // 测试解码
    cout << "\nTesting encoding and decoding full sentences:" << endl;
    string testSentence = "ihaveadream";
    string encodedSentence = huffTree.encode(testSentence);
    string decodedSentence = huffTree.decode(encodedSentence);
    
    cout << "Original: " << testSentence << endl;
    cout << "Encoded: " << encodedSentence << endl;
    cout << "Decoded: " << decodedSentence << endl;
    
    return 0;
}