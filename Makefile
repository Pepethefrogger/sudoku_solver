DEBUG ?= 1

FLAGS = -Wall -Wextra
ifeq ($(DEBUG), 1)
	BUILD_DIR = debug
	FLAGS += -g -pg -O0
else
	BUILD_DIR = release
	FLAGS += -Ofast -flto -march=native
endif

LIB = sudoku.hpp
EXE = $(BUILD_DIR)/main
SRC = main.cpp

TEST_DIR = test
TEST_SRC = test.cpp
TEST_EXE = $(BUILD_DIR)/test

PROFILES_DIR = profiles
GPROF_OUT = gmon.out
TEST_PROFILE = $(PROFILES_DIR)/test.out

.PHONY: all

all: $(EXE) $(TEST_EXE)

run: $(EXE) $(FLAGS)
	./$(EXE)

profile: $(GPROF_OUT) $(TEST_EXE)
	gprof $(TEST_EXE) > $(TEST_PROFILE)

test: $(TEST_EXE)
	./$(TEST_EXE)

$(EXE): $(SRC) $(LIB) | $(BUILD_DIR)
	g++ $(SRC) -o $(EXE) $(FLAGS)

$(TEST_EXE): $(TEST_SRC) $(LIB) | $(BUILD_DIR)
	g++ $(TEST_SRC) -o $(TEST_EXE) $(FLAGS)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)
