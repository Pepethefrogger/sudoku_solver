DEBUG ?= 1

FLAGS = -g -Wall -Wextra -march=native -Ofast -ffast-math -funroll-loops -ftree-vectorize -fopt-info-vec-optimized
ifeq ($(DEBUG), 1)
	BUILD_DIR = debug
else
	BUILD_DIR = release
	FLAGS += -flto
endif

LIB = sudoku.hpp

TEST_DIR = test
TEST_SRC = test.cpp
TEST_EXE = $(BUILD_DIR)/test

PROFILES_DIR = profiles
GPROF_OUT = gmon.out
TEST_PROFILE = $(PROFILES_DIR)/simd.data

.PHONY: all run test profile

all: $(EXE) $(TEST_EXE)

profile: $(TEST_EXE)
	sudo perf record -e cpu-cycles,cache-references,cache-misses,branch-instructions,branch-misses -g -o $(TEST_PROFILE) ./$(TEST_EXE)

test: $(TEST_EXE)
	./$(TEST_EXE) test/sudoku.csv

$(TEST_EXE): $(TEST_SRC) $(LIB) | $(BUILD_DIR)
	g++ $(TEST_SRC) -o $(TEST_EXE) $(FLAGS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(GPROF_OUT)
