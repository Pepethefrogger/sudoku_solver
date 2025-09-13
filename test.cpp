#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>
#include "sudoku.hpp"

using TestPair = std::tuple<std::string, std::string>;
std::vector<TestPair> read_sudoku_csv(std::string_view filename) {
    std::ifstream file(filename.data());
    if (!file) {
        std::cout << "Failed to read file" << std::endl;
        std::exit(1);
    }
    std::vector<TestPair> result;
    std::string line;
    file >> line;
    while (file >> line) {
        auto index = line.find(',');
        auto first = line.substr(0, index);
        auto second = line.substr(index+1);
        result.emplace_back(first, second);
    }
    return result;
}

int main() {
    auto problems = read_sudoku_csv("test/sudoku.csv");
    problems.resize(100000);
    auto i = 0;
    for (auto problem : problems) {
        auto p = std::get<0>(problem);
        auto s = std::get<1>(problem);
        SudokuGrid g(p);
        if (!g.solve()) {
            std::cout << "[ERR] Failed to solve sudoku: " << p << std::endl;
        }
        auto str = g.to_string();
        if (str != s) {
            std::cout << "[ERR] Sudoku: " << p << ", " << str << " doesn't match the solution: " << s << std::endl;
            g.print();
        } else {
            // std::cout << "[OK] Sudoku: " << p << std::endl;
        }
        if (i % 10000 == 0) {
            std::cout << i << std::endl;
        }
        i++;
    }
}
