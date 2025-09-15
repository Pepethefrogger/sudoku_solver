#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <thread>
#include <tuple>
#include <vector>
#include "sudoku.hpp"

std::tuple<std::vector<std::string>, std::vector<std::string>> read_sudoku_csv(std::string_view filename) {
    std::ifstream file(filename.data());
    if (!file) {
        std::cout << "Failed to read file" << std::endl;
        std::exit(1);
    }
    std::vector<std::string> problems;
    std::vector<std::string> solutions;
    std::string line;
    file >> line;
    while (file >> line) {
        auto index = line.find(',');
        auto first = line.substr(0, index);
        auto second = line.substr(index+1);
        problems.emplace_back(first);
        solutions.emplace_back(second);
    }
    return std::tuple(problems, solutions);
}

int main() {
    auto challenge = read_sudoku_csv("test/sudoku.csv");
    auto problems = std::get<0>(challenge);
    // problems.resize(100000);
    auto solutions = std::get<1>(challenge);
    // solutions.resize(100000);
    int n_problems = problems.size();
    int n_threads = std::thread::hardware_concurrency();
    auto solvers_per_thread = n_problems / n_threads;

    std::vector<std::thread> threads;
    threads.reserve(n_threads);
    auto start = std::chrono::high_resolution_clock::now();
    for (auto i = 0; i < n_threads; i++) {
        auto start_i = i * solvers_per_thread;
        auto end_i = std::min(n_problems, (i + 1) * solvers_per_thread);
        threads.emplace_back([&problems, &solutions, start_i, end_i](){
            for (auto j = start_i; j != end_i; j++) {
                auto& p = problems[j];
                SudokuGrid g(p);
                if (!g.solve()) {
                    std::cout << "[ERR] Failed to solve sudoku: " << problems[j] << std::endl;
                }
                if (!g.check(solutions[j])) {
                    std::cout << "[ERR] Sudoku: " << problems[j] << ", " << g.to_string() << " doesn't match the solution: " << solutions[j] << std::endl;
                    g.print();
                }
            }
        });
    }
    for (auto& t : threads) t.join();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Solve time: " << elapsed.count() << "s\n";
}
