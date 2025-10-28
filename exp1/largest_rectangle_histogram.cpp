#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
#include <random>
#include <chrono>

class HistogramSolver {
public:
    static int largestRectangleArea(std::vector<int>& heights) {
        if (heights.empty()) return 0;
        
        std::stack<int> stk;
        int maxArea = 0;
        heights.push_back(0); // 哨兵
        
        for (int i = 0; i < heights.size(); ++i) {
            while (!stk.empty() && heights[i] < heights[stk.top()]) {
                int height = heights[stk.top()];
                stk.pop();
                int width = stk.empty() ? i : i - stk.top() - 1;
                maxArea = std::max(maxArea, height * width);
            }
            stk.push(i);
        }
        
        heights.pop_back(); // 恢复原数组
        return maxArea;
    }
};

class TestDataGenerator {
public:
    static std::vector<int> generateRandomHeights(int length, int maxHeight = 10000) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, maxHeight);
        
        std::vector<int> heights(length);
        for (int i = 0; i < length; ++i) {
            heights[i] = dis(gen);
        }
        return heights;
    }
};

void printVector(const std::vector<int>& vec) {
    std::cout << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        std::cout << vec[i];
        if (i < vec.size() - 1) std::cout << ", ";
    }
    std::cout << "]";
}

void runBasicTests() {
    std::cout << "=== 柱状图最大矩形面积测试 ===\n";
    
    std::vector<std::vector<int>> testCases = {
        {2, 1, 5, 6, 2, 3},
        {2, 4},
        {1, 1, 1, 1},
        {5},
        {0, 0, 0},
        {1, 2, 3, 4, 5}
    };
    
    for (size_t i = 0; i < testCases.size(); ++i) {
        std::vector<int> heights = testCases[i];
        int result = HistogramSolver::largestRectangleArea(heights);
        std::cout << "测试" << i + 1 << ": ";
        printVector(heights);
        std::cout << " -> 面积: " << result << std::endl;
    }
}

void runRandomTests() {
    std::cout << "\n=== 随机测试 ===\n";
    
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 1; i <= 10; ++i) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> lenDis(1, 1000);
        std::uniform_int_distribution<> heightDis(0, 1000);
        
        int length = lenDis(gen);
        auto heights = TestDataGenerator::generateRandomHeights(length, heightDis(gen));
        int result = HistogramSolver::largestRectangleArea(heights);
        
        std::cout << "随机测试" << i << ": 长度=" << length << " -> 最大面积: " << result << std::endl;
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "\n测试耗时: " << duration.count() << " ms\n";
}

int main() {
    runBasicTests();
    runRandomTests();
    return 0;
}