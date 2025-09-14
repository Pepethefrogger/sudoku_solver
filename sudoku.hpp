#include <array>
#include <bitset>
#include <cassert>
#include <iostream>
#include <string_view>
#include <sstream>

class SudokuGrid {
    alignas(64)
    std::array<std::array<int, 9>, 9> grid;

    alignas(64)
    std::array<std::array<std::bitset<9>, 9>, 9> possible_values;

    std::string missing_string(std::bitset<9> b) {
        std::string s;
        for (auto i = 0; i < 9; i++) {
            if (b.test(i)) {
                s += std::to_string(i+1);
                s += ", ";
            }
        }
        return s;
    }

    void set(int x, int y, int v) {
        grid[x][y] = v;
        for (auto xx = 0; xx < 9; xx++) {
            possible_values[xx][y].set(v, false);
        }
        for (auto yy = 0; yy < 9; yy++) {
            possible_values[x][yy].set(v, false);
        }

        auto grid_x = x - (x % 3);
        auto grid_y = y - (y % 3);
        for (auto xx = grid_x; xx < grid_x + 3; xx++) {
            for (auto yy = grid_y; yy < grid_y + 3; yy++) {
                possible_values[xx][yy].set(v, false);
            }
        }
    }
            
    public:
        SudokuGrid(std::string_view s) {
            assert(s.size() == 3 * 3 * 3 * 3);
            for (auto& r : possible_values) {
                for (auto& v : r) {
                    v.set();
                }
            }
            for (auto y = 0; y < 9; y++) {
                for (auto x = 0; x < 9; x++) {
                    auto c = s[9*y + x];
                    auto v = c - '1';
                    if (v == -1) {
                        grid[x][y] = -1;
                    } else {
                        set(x, y, c - '1');
                    }
                }
            }
        }

        std::string to_string() {
            std::stringstream str;
            for (auto y = 0; y < 9; y++) {
                for (auto x = 0; x < 9; x++) {
                    auto v = grid[x][y];
                    str << v + 1;
                }
            }
            return str.str();
        }

        void print() {
            for (auto y = 0; y < 9; y++) {
                for (auto x = 0; x < 9; x++) {
                    std::cout << grid[x][y] + 1 << " ";
                }
                std::cout << "\n";
            }
            std::cout << std::endl;
        }

        void print_possibles() {
            for (auto y = 0; y < 9; y++) {
                for (auto x = 0; x < 9; x++) {
                    auto value = grid[x][y];
                    if (value != -1) continue;
                    auto all = possible_values[x][y];
                    std::cout << "  Possibilities: " << missing_string(all) << std::endl;
                }
            }
        }

        bool solve() {
            while (true) {
                int min_x;
                int min_y;
                int min_count = 10;
                for (auto x = 0; x < 9; x++) {
                    for (auto y = 0; y < 9; y++) {
                        auto value = grid[x][y];
                        if (value != -1) continue;
                        auto possible = possible_values[x][y];
                        int count = possible.count();
                        if (count < min_count) {
                            min_x = x;
                            min_y = y;
                            min_count = count;
                        }
                        if (count == 1) goto end_loop;
                    }
                }
                end_loop:
                if (min_count == 10) return true;
                auto possible = possible_values[min_x][min_y];
                if (min_count == 1) {
                    auto index = possible._Find_first();
                    set(min_x, min_y, index);
                } else {
                    auto clone = *this;
                    for (auto i = 0; i < 9; i++) {
                        if (!possible.test(i)) continue;
                        clone = *this;
                        clone.set(min_x, min_y, i);
                        if (clone.solve()) {
                            *this = clone;
                            return true;
                        }
                    }
                    return false;
                }
            }
        }
};
