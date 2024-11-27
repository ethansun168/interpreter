#include <math.h>
#include <exception>
#include <queue>
#include <stack>
#include <iostream>
#include <string>
#include <unordered_map>

class Interpreter {
private:
    std::vector<char> supportedChars;
    std::unordered_map<std::string, int> vars;

    bool isOperator(char ch) const {
        return ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^';
    }

    std::unordered_map<char, int> precedence = {
        {'+', 1},
        {'-', 1},
        {'*', 2},
        {'/', 2},
        {'^', 3}
    };

    bool supported(char ch) const {
        for (const auto& elt : supportedChars) {
            if (ch == elt) {
                return true;
            }
        }
        return false;
    }

    bool isNumber(const std::string& str) const {
        for (const auto& ch : str) {
            if (!isdigit(ch)) {
                return false;
            }
        }
        return true;
    }

    std::vector<std::string> splitTokens(const std::string& line) {
        std::vector<std::string> ret;
        std::string var = "";
        for (const auto& ch : line) {
            if (ch == ' ') continue;
            if (isdigit(ch)) {
                if (!var.empty() && !isNumber(var)) {
                    ret.push_back(var);
                    vars[var] = 0;
                    var = "";
                }
                var += ch;
            }
            else if (isalpha(ch)) {
                if(!var.empty() && isNumber(var)) {
                    ret.push_back(var);
                    var = "";
                }
                var += ch;
            }
            else {
                if(!var.empty()) {
                    ret.push_back(var);
                    var.clear();
                }
                ret.push_back(std::string(1, ch));
            }
        }
        if (!var.empty()) {
            ret.push_back(var);
        }
       
        return ret;
    }

    int applyOp(int a, int b, char op) {
        switch (op) {
            case '+': return a + b;
            case '-': return a - b;
            case '*': return a * b;
            case '/': return a / b;
            case '^': return std::pow(a, b);
            default: throw std::runtime_error("Invalid operator");
        }
    }

    void processTopOperator(std::stack<int>& values, std::stack<char>& ops) {
        double b = values.top();
        values.pop();
        double a = values.top();
        values.pop();
        char op = ops.top();
        ops.pop();
        values.push(applyOp(a, b, op));
    }

    // For math expression only
    int combine(const std::vector<std::string>& tokens, size_t& pos) {
        std::stack<int> values;
        std::stack<char> ops;

        while (pos < tokens.size()) {
            std::string token = tokens[pos++];
            if (isNumber(token)) {
                values.push(std::stoi(token));
            }
            else if (token == "(") {
                // Recursively calculate expressions in parentheses
                values.push(combine(tokens, pos));
            }
            else if (token == ")") {
                break;
            }
            else if (isOperator(token[0])) {
                while (!ops.empty() && precedence[ops.top()] >= precedence[token[0]]) {
                    processTopOperator(values, ops);
                }
                ops.push(token[0]);
            }
            else {
                throw std::runtime_error("Could not parse mathematical expression");
            }
        }

        // Apply remaining operators
        while (!ops.empty()) {
            processTopOperator(values, ops);
        }
        return values.top();
    }

public:
    Interpreter() {
        supportedChars = {'=', '(', ')', '#'};
    }

    void evaluate(std::string line) {
        std::cout << "Evaluating: " << line << std::endl;
        std::vector<std::string> tokens = splitTokens(line);
        // for (const auto& token : tokens) {
        //     std::cout << token << ' ';
        // }
        // std::cout << std::endl;
        if (tokens.empty() || tokens[0] == "#") return;
        size_t pos = 0;
        std::cout << combine(tokens, pos) << std::endl;
    }
};

int main() {
    Interpreter i;

    // Load the whole input
    std::string line;
    std::queue<std::string> lines;
    while(getline(std::cin, line)) {
        lines.push(line);
    }
    while(!lines.empty()) {
        // TODO: more processing here
        try {
            i.evaluate(lines.front());
        }
        catch (std::runtime_error& e) {
            std::cout << e.what() << std::endl;
            exit(1);
        }
        lines.pop();
    }
}