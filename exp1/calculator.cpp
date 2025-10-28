#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <cctype>
#include <stdexcept>
#include <map>
#include <iomanip>

class Calculator {
private:
    enum Operator { ADD, SUB, MUL, DIV, POW, FAC, L_P, R_P, EOE };
    static constexpr int NUM_OPERATORS = 9;
    static constexpr char OPERATOR_CHARS[NUM_OPERATORS] = {'+', '-', '*', '/', '^', '!', '(', ')', '\0'};
    
    // 运算符优先级表 [栈顶][当前]
    static constexpr char PRIORITY_TABLE[NUM_OPERATORS][NUM_OPERATORS] = {
        {'>', '>', '<', '<', '<', '<', '<', '>', '>'}, // +
        {'>', '>', '<', '<', '<', '<', '<', '>', '>'}, // -
        {'>', '>', '>', '>', '<', '<', '<', '>', '>'}, // *
        {'>', '>', '>', '>', '<', '<', '<', '>', '>'}, // /
        {'>', '>', '>', '>', '>', '<', '<', '>', '>'}, // ^
        {'>', '>', '>', '>', '>', '>', ' ', '>', '>'}, // !
        {'<', '<', '<', '<', '<', '<', '<', '=', ' '}, // (
        {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}, // )
        {'<', '<', '<', '<', '<', '<', '<', ' ', '='}  // \0
    };

    template<typename T>
    class Stack {
    private:
        std::vector<T> data;
    public:
        bool empty() const { return data.empty(); }
        int size() const { return data.size(); }
        T& top() { 
            if (empty()) throw std::runtime_error("栈空");
            return data.back();
        }
        void push(const T& item) { data.push_back(item); }
        void pop() { 
            if (empty()) throw std::runtime_error("栈空");
            data.pop_back();
        }
    };

    static Operator charToOperator(char c) {
        for (int i = 0; i < NUM_OPERATORS; ++i) {
            if (c == OPERATOR_CHARS[i]) return static_cast<Operator>(i);
        }
        return EOE;
    }

    static char getPriority(Operator op1, Operator op2) {
        return PRIORITY_TABLE[op1][op2];
    }

    static double factorial(double n) {
        if (n < 0 || n != static_cast<int>(n)) {
            throw std::runtime_error("阶乘需要非负整数");
        }
        double result = 1;
        for (int i = 2; i <= static_cast<int>(n); ++i) result *= i;
        return result;
    }

    static double calculateBinary(double a, Operator op, double b) {
        switch (op) {
            case ADD: return a + b;
            case SUB: return a - b;
            case MUL: return a * b;
            case DIV: 
                if (b == 0) throw std::runtime_error("除零错误");
                return a / b;
            case POW: return std::pow(a, b);
            default: throw std::runtime_error("无效二元运算");
        }
    }

    static double calculateUnary(Operator op, double a) {
        if (op == FAC) return factorial(a);
        throw std::runtime_error("无效一元运算");
    }

    static bool isDigit(char c) {
        return std::isdigit(c) || c == '.';
    }

    static std::pair<double, int> extractNumber(const std::string& expr, int start) {
        std::string numStr;
        int i = start;
        
        // 处理负号
        if (i < expr.length() && expr[i] == '-' && 
            (i == 0 || expr[i-1] == '(' || std::string("+-*/^").find(expr[i-1]) != std::string::npos)) {
            numStr += expr[i++];
        }
        
        while (i < expr.length() && (isDigit(expr[i]))) {
            numStr += expr[i++];
        }
        
        if (numStr.empty()) throw std::runtime_error("数字格式错误");
        return {std::stod(numStr), i};
    }

public:
    static double evaluate(const std::string& expression) {
        if (expression.empty()) return 0.0;
        
        std::string expr = "\0" + expression + "\0";
        Stack<double> operands;
        Stack<Operator> operators;
        operators.push(charToOperator('\0'));
        
        int i = 1;
        while (i < expr.length()) {
            if (isDigit(expr[i]) || 
                (expr[i] == '-' && (i == 1 || std::string("\0(+-*/^").find(expr[i-1]) != std::string::npos))) {
                
                auto [num, nextPos] = extractNumber(expr, i);
                operands.push(num);
                i = nextPos;
            } else {
                Operator currOp = charToOperator(expr[i]);
                if (currOp == EOE) break;
                
                char priority = getPriority(operators.top(), currOp);
                
                if (priority == '<') {
                    operators.push(currOp);
                    i++;
                } else if (priority == '=') {
                    operators.pop();
                    i++;
                } else if (priority == '>') {
                    processOperation(operands, operators);
                } else {
                    throw std::runtime_error("优先级错误");
                }
            }
        }
        
        while (operators.top() != charToOperator('\0')) {
            processOperation(operands, operators);
        }
        
        if (operands.size() != 1) throw std::runtime_error("表达式无效");
        return operands.top();
    }

private:
    static void processOperation(Stack<double>& operands, Stack<Operator>& operators) {
        Operator op = operators.top();
        operators.pop();
        
        if (op == FAC) {
            if (operands.empty()) throw std::runtime_error("阶乘操作数不足");
            double a = operands.top(); operands.pop();
            operands.push(calculateUnary(op, a));
        } else {
            if (operands.size() < 2) throw std::runtime_error("操作数不足");
            double b = operands.top(); operands.pop();
            double a = operands.top(); operands.pop();
            operands.push(calculateBinary(a, op, b));
        }
    }
};

class FunctionEvaluator {
public:
    static double evaluateFunction(const std::string& func, double arg) {
        static const std::map<std::string, std::function<double(double)>> functions = {
            {"sin", [](double x) { return std::sin(x * M_PI / 180); }},
            {"cos", [](double x) { return std::cos(x * M_PI / 180); }},
            {"tan", [](double x) { return std::tan(x * M_PI / 180); }},
            {"log", [](double x) { 
                if (x <= 0) throw std::runtime_error("对数参数需为正数");
                return std::log10(x);
            }},
            {"ln", [](double x) { 
                if (x <= 0) throw std::runtime_error("自然对数参数需为正数");
                return std::log(x);
            }},
            {"sqrt", [](double x) { 
                if (x < 0) throw std::runtime_error("平方根参数需为非负数");
                return std::sqrt(x);
            }},
            {"abs", [](double x) { return std::abs(x); }}
        };
        
        auto it = functions.find(func);
        if (it == functions.end()) throw std::runtime_error("未知函数: " + func);
        return it->second(arg);
    }
    
    static std::string replaceFunctions(const std::string& expr) {
        std::string result = expr;
        std::vector<std::string> funcs = {"sin", "cos", "tan", "log", "ln", "sqrt", "abs"};
        
        for (const auto& func : funcs) {
            size_t pos = 0;
            while ((pos = result.find(func, pos)) != std::string::npos) {
                size_t end = pos + func.length();
                if (end < result.length() && result[end] == '(') {
                    size_t start = pos;
                    int parenCount = 0;
                    size_t i = end;
                    
                    while (i < result.length()) {
                        if (result[i] == '(') parenCount++;
                        else if (result[i] == ')') {
                            if (--parenCount == 0) {
                                std::string arg = result.substr(end + 1, i - end - 1);
                                try {
                                    double value = evaluateExtended(arg);
                                    result.replace(start, i - start + 1, std::to_string(value));
                                    break;
                                } catch (...) {
                                    pos = i + 1;
                                    break;
                                }
                            }
                        }
                        i++;
                    }
                } else {
                    pos++;
                }
            }
        }
        return result;
    }
    
    static double evaluateExtended(const std::string& expr) {
        return Calculator::evaluate(replaceFunctions(expr));
    }
};

void runCalculatorTests() {
    std::cout << "=== 计算器测试 ===\n";
    
    std::vector<std::string> tests = {
        "2+3", "2*3+5", "(2+3)*5", "2^3", "5!", 
        "2+3*4", "sin(30)", "cos(60)", "sqrt(16)", "2+sin(30)"
    };
    
    for (const auto& test : tests) {
        try {
            double result = FunctionEvaluator::evaluateExtended(test);
            std::cout << test << " = " << std::fixed << std::setprecision(6) << result << std::endl;
        } catch (const std::exception& e) {
            std::cout << test << " -> 错误: " << e.what() << std::endl;
        }
    }
}

int main() {
    runCalculatorTests();
    
    std::cout << "\n=== 交互式计算器 ===\n输入表达式 (quit退出):\n";
    std::string input;
    while (std::cout << "> " && std::getline(std::cin, input)) {
        if (input == "quit" || input == "exit") break;
        try {
            double result = FunctionEvaluator::evaluateExtended(input);
            std::cout << "= " << std::fixed << std::setprecision(6) << result << std::endl;
        } catch (const std::exception& e) {
            std::cout << "错误: " << e.what() << std::endl;
        }
    }
    
    return 0;
}