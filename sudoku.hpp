#include <array>
#include <bitset>
#include <cassert>
#include <iostream>
#include <string_view>
#include <sstream>

class SudokuGrid {
    int grid[9][9];

    std::array<std::array<std::bitset<9>, 3>, 3> found_grid;
    std::array<std::bitset<9>, 9> found_row;
    std::array<std::bitset<9>, 9> found_col;
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

    // To be called once every value is updated
    void update_possible_values(int x, int y) {
        auto grid_x = x / 3;
        auto grid_y = y / 3;
        auto grid = found_grid[grid_x][grid_y];
        auto row = found_row[y];
        auto col = found_col[x];
        possible_values[x][y] = (grid | row | col).flip();
    }

    void update_grid_found(int x, int y) {
        auto x_start = x * 3;
        auto y_start = y * 3;
        auto found = &found_grid[x][y];
        found->reset();
        for (auto y_cell = y_start; y_cell < y_start + 3; y_cell++) {
            for (auto x_cell = x_start; x_cell < x_start + 3; x_cell++) {
                // std::cout << "grid pos: " << x_cell << ", " << y_cell << "\n";
                auto idx = grid[x_cell][y_cell];
                if (idx == -1) continue;
                found->set(idx);
            }
        }
    }

    void update_row_found(int y) {
        auto found = &found_row[y];
        found->reset();
        for (auto x = 0; x < 9; x++) {
            auto idx = grid[x][y];
            if (idx == -1) continue;
            found->set(idx);
        }
    }

    void update_col_found(int x) {
        auto found = &found_col[x];
        found->reset();
        for (auto y = 0; y < 9; y++) {
            auto idx = grid[x][y];
            if (idx == -1) continue;
            found->set(idx);
        }
    }
            
    public:
        SudokuGrid(std::string_view s) {
            assert(s.size() == 3 * 3 * 3 * 3);
            for (auto y = 0; y < 9; y++) {
                for (auto x = 0; x < 9; x++) {
                    auto c = s[9*y + x];
                    grid[x][y] = c - '1';
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

        void update_all() {
            for (auto x = 0; x < 9; x++) {
                update_col_found(x);
                for (auto y = 0; y < 9; y++) {
                    if (x == 0) update_row_found(y);
                    if (x % 3 == 0 && y % 3 == 0) {
                        auto grid_x = x / 3;
                        auto grid_y = y / 3;
                        update_grid_found(grid_x, grid_y);
                    }
                    auto value = grid[x][y];
                    if (value == -1) update_possible_values(x, y);
                }
            }
        }

        void update(int x, int y) {
            update_col_found(x);
            update_row_found(y);
            auto grid_x = x / 3;
            auto grid_y = y / 3;
            update_grid_found(grid_x, grid_y);
            for (auto xx = 0; xx < 9; xx++) {
                update_possible_values(xx, y);
            }
            for (auto yy = 0; yy < 9; yy++) {
                update_possible_values(x, yy);
            }

            auto closest_grid_x = x - (x % 3);
            auto closest_grid_y = y - (y % 3);
            for (auto yy = closest_grid_y; yy < closest_grid_y + 3; yy++) {
                for (auto xx = closest_grid_x; xx < closest_grid_x + 3; xx++) {
                    update_possible_values(xx, yy);
                }
            }
        }

        void print_possibles() {
            for (auto y = 0; y < 9; y++) {
                for (auto x = 0; x < 9; x++) {
                    auto value = grid[x][y];
                    if (value != -1) continue;
                    auto grid_x = x / 3;
                    auto grid_y = y / 3;
                    auto grid = found_grid[grid_x][grid_y];
                    std::cout << "Position: " << x << ", " << y << ":\n";
                    std::cout << "  Grid found: " << missing_string(grid) << "\n";
                    auto row = found_row[y];
                    std::cout << "  Row found: " << missing_string(row) << "\n";
                    auto col = found_col[x];
                    std::cout << "  Col found: " << missing_string(col) << "\n";
                    auto all = possible_values[x][y];
                    std::cout << "  Possibilities: " << missing_string(all) << std::endl;
                }
            }
        }

        bool solve() {
            update_all();
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
                    }
                }
                if (min_count == 10) return true;
                auto possible = possible_values[min_x][min_y];
                if (min_count == 1) {
                    auto index = possible._Find_first();
                    grid[min_x][min_y] = index;
                    update(min_x, min_y);
                } else {
                    for (auto i = 0; i < 9; i++) {
                        if (!possible.test(i)) continue;
                        auto clone = *this;
                        clone.grid[min_x][min_y] = i;
                        clone.update(min_x, min_y);
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
