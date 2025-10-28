#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <ctime>
#include <cmath>
#include <iomanip>

class Complex {
private:
    double real;
    double imag;

public:
    Complex(double r = 0.0, double i = 0.0) : real(r), imag(i) {}
    
    double getReal() const { return real; }
    double getImag() const { return imag; }
    
    double magnitude() const {
        return std::sqrt(real * real + imag * imag);
    }
    
    bool operator==(const Complex& other) const {
        const double epsilon = 1e-9;
        return (std::abs(real - other.real) < epsilon) && 
               (std::abs(imag - other.imag) < epsilon);
    }
    
    bool operator<(const Complex& other) const {
        const double epsilon = 1e-9;
        double mag1 = magnitude();
        double mag2 = other.magnitude();
        
        if (std::abs(mag1 - mag2) < epsilon) {
            return real < other.real;
        }
        return mag1 < mag2;
    }
    
    friend std::ostream& operator<<(std::ostream& os, const Complex& c) {
        os << std::fixed << std::setprecision(2) 
           << c.real << (c.imag >= 0 ? "+" : "") << c.imag << "i";
        return os;
    }
};

class ComplexVectorOperations {
public:
    static std::vector<Complex> generateRandomVector(int size) {
        std::vector<Complex> vec;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dis(-10.0, 10.0);
        
        for (int i = 0; i < size; ++i) {
            if (i % 5 == 0) {
                vec.emplace_back(1.5, 2.5);
            } else {
                vec.emplace_back(dis(gen), dis(gen));
            }
        }
        return vec;
    }
    
    static void shuffle(std::vector<Complex>& vec) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::shuffle(vec.begin(), vec.end(), gen);
    }
    
    static int find(const std::vector<Complex>& vec, const Complex& target) {
        auto it = std::find(vec.begin(), vec.end(), target);
        return it != vec.end() ? std::distance(vec.begin(), it) : -1;
    }
    
    static bool insert(std::vector<Complex>& vec, int index, const Complex& c) {
        if (index >= 0 && index <= static_cast<int>(vec.size())) {
            vec.insert(vec.begin() + index, c);
            return true;
        }
        return false;
    }
    
    static bool removeAt(std::vector<Complex>& vec, int index) {
        if (index >= 0 && index < static_cast<int>(vec.size())) {
            vec.erase(vec.begin() + index);
            return true;
        }
        return false;
    }
    
    static void makeUnique(std::vector<Complex>& vec) {
        std::sort(vec.begin(), vec.end());
        auto last = std::unique(vec.begin(), vec.end());
        vec.erase(last, vec.end());
    }
};

class SortAlgorithms {
public:
    static void bubbleSort(std::vector<Complex>& vec) {
        int n = vec.size();
        for (int i = 0; i < n - 1; ++i) {
            for (int j = 0; j < n - i - 1; ++j) {
                if (vec[j + 1] < vec[j]) {
                    std::swap(vec[j], vec[j + 1]);
                }
            }
        }
    }
    
    static void mergeSort(std::vector<Complex>& vec, int left, int right) {
        if (left >= right) return;
        
        int mid = left + (right - left) / 2;
        mergeSort(vec, left, mid);
        mergeSort(vec, mid + 1, right);
        merge(vec, left, mid, right);
    }
    
private:
    static void merge(std::vector<Complex>& vec, int left, int mid, int right) {
        std::vector<Complex> leftVec(vec.begin() + left, vec.begin() + mid + 1);
        std::vector<Complex> rightVec(vec.begin() + mid + 1, vec.begin() + right + 1);
        
        int i = 0, j = 0, k = left;
        while (i < leftVec.size() && j < rightVec.size()) {
            if (leftVec[i] < rightVec[j]) {
                vec[k++] = leftVec[i++];
            } else {
                vec[k++] = rightVec[j++];
            }
        }
        
        while (i < leftVec.size()) vec[k++] = leftVec[i++];
        while (j < rightVec.size()) vec[k++] = rightVec[j++];
    }
};

class RangeSearch {
public:
    static std::vector<Complex> search(const std::vector<Complex>& vec, double minMag, double maxMag) {
        std::vector<Complex> result;
        std::copy_if(vec.begin(), vec.end(), std::back_inserter(result),
            [minMag, maxMag](const Complex& c) {
                double mag = c.magnitude();
                return mag >= minMag && mag < maxMag;
            });
        std::sort(result.begin(), result.end());
        return result;
    }
};

class PerformanceTimer {
public:
    static double measureSortTime(std::vector<Complex> vec, const std::string& sortType) {
        clock_t start = clock();
        
        if (sortType == "bubble") {
            SortAlgorithms::bubbleSort(vec);
        } else if (sortType == "merge") {
            SortAlgorithms::mergeSort(vec, 0, vec.size() - 1);
        }
        
        clock_t end = clock();
        return double(end - start) / CLOCKS_PER_SEC;
    }
};

void printVector(const std::vector<Complex>& vec, const std::string& title) {
    std::cout << title << " [" << vec.size() << "]: ";
    for (size_t i = 0; i < std::min(vec.size(), size_t(5)); ++i) {
        std::cout << vec[i] << " ";
    }
    if (vec.size() > 5) std::cout << "...";
    std::cout << std::endl;
}

int main() {
    std::cout << "=== 复数向量操作测试 ===" << std::endl;
    
    // 测试基本操作
    std::cout << "\n1. 基本操作测试:" << std::endl;
    auto vec = ComplexVectorOperations::generateRandomVector(8);
    printVector(vec, "原始向量");
    
    ComplexVectorOperations::shuffle(vec);
    printVector(vec, "置乱后");
    
    Complex target(1.5, 2.5);
    int index = ComplexVectorOperations::find(vec, target);
    std::cout << "查找 " << target << ": 索引=" << index << std::endl;
    
    ComplexVectorOperations::insert(vec, 2, Complex(3.0, 4.0));
    printVector(vec, "插入后");
    
    ComplexVectorOperations::removeAt(vec, 0);
    printVector(vec, "删除后");
    
    auto dupVec = ComplexVectorOperations::generateRandomVector(10);
    printVector(dupVec, "重复向量");
    ComplexVectorOperations::makeUnique(dupVec);
    printVector(dupVec, "唯一化后");
    
    // 性能测试
    std::cout << "\n2. 排序性能比较:" << std::endl;
    auto testVec = ComplexVectorOperations::generateRandomVector(500);
    
    std::vector<Complex> ordered = testVec;
    std::sort(ordered.begin(), ordered.end());
    
    std::vector<Complex> reversed = ordered;
    std::reverse(reversed.begin(), reversed.end());
    
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "起泡排序 - 顺序: " << PerformanceTimer::measureSortTime(ordered, "bubble") << "s\n";
    std::cout << "起泡排序 - 逆序: " << PerformanceTimer::measureSortTime(reversed, "bubble") << "s\n";
    std::cout << "起泡排序 - 随机: " << PerformanceTimer::measureSortTime(testVec, "bubble") << "s\n";
    
    std::cout << "归并排序 - 顺序: " << PerformanceTimer::measureSortTime(ordered, "merge") << "s\n";
    std::cout << "归并排序 - 逆序: " << PerformanceTimer::measureSortTime(reversed, "merge") << "s\n";
    std::cout << "归并排序 - 随机: " << PerformanceTimer::measureSortTime(testVec, "merge") << "s\n";
    
    // 区间查找测试
    std::cout << "\n3. 区间查找测试:" << std::endl;
    auto searchVec = ComplexVectorOperations::generateRandomVector(15);
    std::sort(searchVec.begin(), searchVec.end());
    printVector(searchVec, "排序向量");
    
    auto result = RangeSearch::search(searchVec, 2.0, 5.0);
    printVector(result, "模在[2,5)的元素");
    
    return 0;
}