#include "sudoku.hpp"
#include <iostream>

int main() {
    SudokuGrid grid("004300209005009001070060043006002087190007400050083000600000105003508690042910300");
    grid.print();
    if (grid.solve()) {
        std::cout << "Solved" << std::endl;
    } else {
        std::cout << "Couldn't solve" << std::endl;
    }
    grid.print();
}
