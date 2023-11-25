UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	CC=gcc-13
else
	CC=gcc
endif

FLAGS=-O3 -fopenmp
LIB_FLAGS=-lpthread -lomp -L/opt/homebrew/opt/libomp/lib
INCLUDE=-Iinclude -I/opt/homebrew/opt/libomp/include

BIN_DIR=bin
TEST_DIR=tests
BENCHMARK_DIR=benchmark

test: test_serial test_parallel

test_parallel:
	$(CC) $(FLAGS) -o $(BIN_DIR)/test_parallel $(TEST_DIR)/test_parallel.c $(INCLUDE) $(LIB_FLAGS)

test_serial:
	$(CC) $(FLAGS) -o $(BIN_DIR)/test_serial $(TEST_DIR)/test_serial.c $(INCLUDE) $(LIB_FLAGS)

benchmark: benchmark_serial benchmark_parallel

benchmark_serial:
	$(CC) $(FLAGS) -o $(BIN_DIR)/benchmark_serial  $(BENCHMARK_DIR)/benchmark_serial.c $(INCLUDE) $(LIB_FLAGS)

benchmark_parallel:
	$(CC) $(FLAGS) -o $(BIN_DIR)/benchmark_parallel  $(BENCHMARK_DIR)/benchmark_parallel.c $(INCLUDE) $(LIB_FLAGS)

clean:
	rm $(BIN_DIR)/*