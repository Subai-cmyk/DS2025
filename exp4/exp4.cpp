#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <random>
#include <iomanip>

using namespace std;

// --- 1. Basic Structures ---

struct BoundingBox {
    int id;
    float x, y, w, h;
    float score;
};

// --- 2. Helper Functions ---

// Intersection over Union (IoU)
float calculateIoU(const BoundingBox& a, const BoundingBox& b) {
    float xA = max(a.x, b.x);
    float yA = max(a.y, b.y);
    float xB = min(a.x + a.w, b.x + b.w);
    float yB = min(a.y + a.h, b.y + b.h);

    float interW = max(0.0f, xB - xA);
    float interH = max(0.0f, yB - yA);
    float interArea = interW * interH;

    float areaA = a.w * a.h;
    float areaB = b.w * b.h;

    float unionArea = areaA + areaB - interArea;
    if (unionArea <= 0) return 0.0f;
    return interArea / unionArea;
}

// --- 3. Sorting Algorithms (Sort Descending by Score) ---

// A. Quick Sort (O(n log n))
int partition(vector<BoundingBox>& arr, int low, int high) {
    float pivot = arr[high].score;
    int i = (low - 1);
    for (int j = low; j <= high - 1; j++) {
        // We want Descending order, so check if current > pivot
        if (arr[j].score > pivot) { 
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return (i + 1);
}

void quickSort(vector<BoundingBox>& arr, int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quickSort(arr, low, pi - 1);
        quickSort(arr, pi + 1, high);
    }
}

// B. Merge Sort (O(n log n))
void merge(vector<BoundingBox>& arr, int left, int mid, int right) {
    int n1 = mid - left + 1;
    int n2 = right - mid;
    vector<BoundingBox> L(n1), R(n2);

    for (int i = 0; i < n1; i++) L[i] = arr[left + i];
    for (int j = 0; j < n2; j++) R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < n1 && j < n2) {
        if (L[i].score >= R[j].score) { // Descending
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

void mergeSort(vector<BoundingBox>& arr, int left, int right) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    mergeSort(arr, left, mid);
    mergeSort(arr, mid + 1, right);
    merge(arr, left, mid, right);
}

// C. Heap Sort (O(n log n))
// Standard HeapSort builds a Max-Heap to get Ascending order.
// To get Descending order [Max...Min], we can build a Min-Heap.
void heapify(vector<BoundingBox>& arr, int n, int i) {
    int smallest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;

    if (l < n && arr[l].score < arr[smallest].score)
        smallest = l;

    if (r < n && arr[r].score < arr[smallest].score)
        smallest = r;

    if (smallest != i) {
        swap(arr[i], arr[smallest]);
        heapify(arr, n, smallest);
    }
}

void heapSort(vector<BoundingBox>& arr) {
    int n = arr.size();
    // Build Min Heap
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    // One by one extract an element from heap
    for (int i = n - 1; i > 0; i--) {
        // Move current root (minimum) to end
        swap(arr[0], arr[i]);
        // call min heapify on the reduced heap
        heapify(arr, i, 0);
    }
}

// D. Bubble Sort (O(n^2)) - The baseline
void bubbleSort(vector<BoundingBox>& arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; i++) {
        bool swapped = false;
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j].score < arr[j + 1].score) { // Descending
                swap(arr[j], arr[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) break;
    }
}

// --- 4. Non-Maximum Suppression ---

vector<BoundingBox> runNMS(vector<BoundingBox>& boxes, float iouThreshold) {
    vector<BoundingBox> kept;
    // Note: 'boxes' is assumed to be sorted already
    
    // Use a flag vector to mark suppressed boxes (more efficient than erasing)
    vector<bool> suppressed(boxes.size(), false);

    for (size_t i = 0; i < boxes.size(); ++i) {
        if (suppressed[i]) continue;

        kept.push_back(boxes[i]);

        for (size_t j = i + 1; j < boxes.size(); ++j) {
            if (suppressed[j]) continue;
            
            if (calculateIoU(boxes[i], boxes[j]) > iouThreshold) {
                suppressed[j] = true;
            }
        }
    }
    return kept;
}

// --- 5. Data Generation Utilities ---

vector<BoundingBox> generateRandom(int count) {
    vector<BoundingBox> boxes;
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> disLoc(0, 800);
    uniform_real_distribution<> disSize(20, 100);
    uniform_real_distribution<> disScore(0, 1);

    for (int i = 0; i < count; ++i) {
        boxes.push_back({
            i,
            (float)disLoc(gen), (float)disLoc(gen),
            (float)disSize(gen), (float)disSize(gen),
            (float)disScore(gen)
        });
    }
    return boxes;
}

vector<BoundingBox> generateClustered(int count) {
    vector<BoundingBox> boxes;
    random_device rd;
    mt19937 gen(rd());
    
    // Create random cluster centers
    int numClusters = max(3, count / 100);
    vector<pair<float, float>> clusters;
    uniform_real_distribution<> disC(100, 700);
    for(int i=0; i<numClusters; ++i) 
        clusters.push_back({(float)disC(gen), (float)disC(gen)});

    uniform_real_distribution<> disOffset(-100, 100);
    uniform_real_distribution<> disSize(20, 80);
    uniform_real_distribution<> disScoreBase(0.5, 1.0);

    for (int i = 0; i < count; ++i) {
        auto center = clusters[i % numClusters];
        float offX = disOffset(gen);
        float offY = disOffset(gen);
        // Scores are often higher near cluster centers
        float score = max(0.0f, min(1.0f, (float)(disScoreBase(gen) - (abs(offX)/500.0f))));

        boxes.push_back({
            i,
            center.first + offX, center.second + offY,
            (float)disSize(gen), (float)disSize(gen),
            score
        });
    }
    return boxes;
}

// --- 6. Benchmarking Logic ---

void runBenchmark(int count, string distName, vector<BoundingBox>(*genFunc)(int)) {
    cout << "\n=================================================" << endl;
    cout << "  Dataset: " << count << " boxes (" << distName << ")" << endl;
    cout << "=================================================" << endl;
    
    auto boxes = genFunc(count);
    float threshold = 0.5f;

    vector<string> algoNames = {"Quick", "Merge", "Heap", "Bubble"};
    
    // Print Table Header
    cout << left << setw(10) << "Algo" 
         << setw(12) << "Sort(ms)" 
         << setw(12) << "NMS(ms)" 
         << setw(12) << "Total(ms)" 
         << "Kept" << endl;
    cout << string(55, '-') << endl;

    for (const auto& name : algoNames) {
        // Deep copy for fair testing
        vector<BoundingBox> copy = boxes;
        
        // 1. Measure Sort
        auto startSort = chrono::high_resolution_clock::now();
        if (name == "Quick") quickSort(copy, 0, copy.size() - 1);
        else if (name == "Merge") mergeSort(copy, 0, copy.size() - 1);
        else if (name == "Heap") heapSort(copy);
        else if (name == "Bubble") bubbleSort(copy);
        auto endSort = chrono::high_resolution_clock::now();
        
        // 2. Measure NMS
        auto startNMS = chrono::high_resolution_clock::now();
        auto result = runNMS(copy, threshold);
        auto endNMS = chrono::high_resolution_clock::now();

        chrono::duration<double, milli> sortDur = endSort - startSort;
        chrono::duration<double, milli> nmsDur = endNMS - startNMS;
        double total = sortDur.count() + nmsDur.count();

        cout << left << setw(10) << name 
             << fixed << setprecision(3)
             << setw(12) << sortDur.count() 
             << setw(12) << nmsDur.count() 
             << setw(12) << total 
             << result.size() << endl;
    }
}

int main() {
    cout << "NMS Algorithm Performance Analysis" << endl;
    cout << "Comparing Quick, Merge, Heap, and Bubble Sort impact on NMS." << endl;
    
    // Define test scales
    // Note: Bubble sort will be very slow on 10,000!
    vector<int> counts = {100, 1000, 5000};
    
    for (int n : counts) {
        runBenchmark(n, "Random Dist", generateRandom);
        runBenchmark(n, "Clustered Dist", generateClustered);
    }
    
    return 0;
}
