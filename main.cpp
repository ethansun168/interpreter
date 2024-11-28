#include <math.h>
#include <exception>
#include <queue>
#include <stack>
#include <iostream>
#include <string>
#include <unordered_map>

class Interpreter {
private:
    std::vector<char> specialStrings = {'=', '(', ')', '#', '+', '-', '*', '/', '^', '<', '>', '!'};
    std::unordered_map<char, int> precedence = {
        {'+', 1},
        {'-', 1},
        {'*', 2},
        {'/', 2},
        {'^', 3}
    };
    std::unordered_map<std::string, double> vars;

    bool isOperator(char ch) const {
        return ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^';
    }

    bool isSpecial(char ch) const {
        for (const auto& elt : specialStrings) {
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

    double applyOp(double a, double b, char op) {
        switch (op) {
            case '+': return a + b;
            case '-': return a - b;
            case '*': return a * b;
            case '/': return a / b;
            case '^': return std::pow(a, b);
            default: throw std::runtime_error("Invalid operator at line " + std::to_string(pc));
        }
    }

    void processTopOperator(std::stack<double>& values, std::stack<char>& ops) {
        if (values.size() < 2) {
            throw std::runtime_error("Invalid math expression at line " + std::to_string(pc + 1));
        }
        double b = values.top();
        values.pop();
        double a = values.top();
        values.pop();
        char op = ops.top();
        ops.pop();
        values.push(applyOp(a, b, op));
    }

    // 0 means false, 1 means true
    double boolEvaluate(const std::vector<std::string>& tokens, size_t& begin, size_t& end) {
        size_t idx = 0;
        for (size_t i = begin; i < end; ++i) {
            if (tokens[i] == ">" || tokens[i] == "<") {
                // < or > are at the beginning / end of expression
                if (i == begin || i == end) {
                    throw std::runtime_error("Could not evaluate boolean expression at line " + std::to_string(pc + 1));
                }
                idx = i;
                break;
            }
        }

        // Base case
        if (idx == 0) {
            // No bool operator found
            return mathEvaluate(tokens, begin, end);
        }

        // Recurse
        size_t lhs_begin = begin;
        size_t lhs_end = idx;
        size_t rhs_begin = idx + 1;
        size_t rhs_end = end;

        double lhs = boolEvaluate(tokens, lhs_begin, lhs_end);
        double rhs = boolEvaluate(tokens, rhs_begin, rhs_end);

        if (tokens[idx] == "<") {
            return lhs < rhs;
        }
        else {
            return lhs > rhs;
        }
    }

    // For math expression only
    double mathEvaluate(const std::vector<std::string>& tokens, size_t& begin, size_t& end) {
        std::stack<double> values;
        std::stack<char> ops;

        while (begin < end) {
            std::string token = tokens[begin++];
            if (isNumber(token)) {
                values.push(std::stod(token));
            }
            else if (token == "(") {
                // Recursively calculate expressions in parentheses
                values.push(mathEvaluate(tokens, begin, end));
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
            else if(vars.find(token) != vars.end()) {
                values.push(vars[token]);
            }
            else {
                throw std::runtime_error("Invalid math expression at line " + std::to_string(pc + 1));
            }
        }

        // Apply remaining operators
        while (!ops.empty()) {
            processTopOperator(values, ops);
        }
        if(values.size() != 1) {
            throw std::runtime_error("Invalid math expression at line " + std::to_string(pc + 1));
        }
        return values.top();
    }

public:
    std::vector<std::vector<std::string>> instr;
    int pc = 0;

    Interpreter() {}

    void parse(const std::vector<std::string>& tokens) {
        for (const auto& token : tokens) {
            std::cout << token << ' ';
        }
        std::cout << std::endl;

        size_t begin = 0;
        size_t end = tokens.size();

        if (tokens[0] == "#" || tokens[0] == "") {
            ++pc;
            return;
        }
        else if(tokens[0] == "end") {
            // Go back to closest while or if
            int old_pc = pc;
            while (pc >= 0) {
                if(instr[pc][0] == "while") {
                    return;
                } 
                if (instr[pc][0] == "if") {
                    pc = old_pc + 1;
                    return;
                }
                --pc;
            }
            throw std::runtime_error("End doesn't have a matching statement: line " + std::to_string(old_pc + 1));
        }
        
        // if (tokens.size() < 2 || (isNumber(tokens[0]) && tokens[1] == "=")) {
        //     throw std::runtime_error("Could not parse line " + std::to_string(pc + 1));
        // }

        // if(tokens[1] == "=") {
        //     begin = 2;
        //     vars[tokens[0]] = mathEvaluate(tokens, begin, end);
        //     std::cout << vars[tokens[0]] << std::endl;
        // }
        else if (tokens[0] == "while") {
            begin = 1;
            if (!boolEvaluate(tokens, begin, end)) {
                // jump to corresponding end - account for nested if / while
                std::stack<bool> keywords;

            }
        }
        else if(tokens[0] == "if") {
            begin = 1;
            // bool cond = boolEvaluate(tokens, begin, end);
        }
        else if (tokens.size() == 1) {
            // Print the value of the token
            if(vars.find(tokens[0]) != vars.end()) {
                std::cout << vars[tokens[0]] << std::endl;
            }
            else {
                throw std::runtime_error("Variable " + tokens[0] + " not found");
            }
        }
        // tokens.size() > 1
        else if (tokens[1] == "=") {
            if (!isNumber(tokens[0])) {
                begin = 2;
                vars[tokens[0]] = mathEvaluate(tokens, begin, end);
            }
            else {
                throw std::runtime_error("Cannot assign to a number");
            }
        }
        else {
            std::cout << mathEvaluate(tokens, begin, end) << std::endl;
        }
        ++pc;
    }

    std::vector<std::string> splitTokens(const std::string& line) {
        std::vector<std::string> tokens;
        std::string token = "";
        for (const auto& ch : line) {
            if (ch == ' ') {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
            }
            else if (isSpecial(ch)) {
                if (!token.empty()) {
                    tokens.push_back(token);
                    token.clear();
                }
                tokens.push_back(std::string(1, ch));
            }
            else {
                token += ch;
            }
        }

        if (!token.empty()) {
            tokens.push_back(token);
        }
        else if (tokens.empty()) {
            tokens.push_back("");
        }
       
        return tokens;
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
    // Tokenize the whole program
    while(!lines.empty()) {
        i.instr.emplace_back(i.splitTokens(lines.front()));
        lines.pop();
    }
    while (i.pc < (int) i.instr.size()) {
        try {
            i.parse(i.instr[i.pc]);
        }
        catch (std::runtime_error& e) {
            std::cout << e.what() << std::endl;
            exit(1);
        }

    }
}