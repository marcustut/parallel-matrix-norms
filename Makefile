CC=gcc

FLAGS=-O3
BLAS_FLAGS=-L/opt/homebrew/opt/openblas/lib -I/opt/homebrew/opt/openblas/include -lblas -lm -lpthread
INCLUDE=-Iinclude

BIN_DIR=bin
TEST_DIR=tests
BENCHMARK_DIR=benchmark

test: test_serial test_parallel

test_parallel:
	$(CC) $(FLAGS) -o $(BIN_DIR)/test_parallel $(TEST_DIR)/test_parallel.c $(INCLUDE) $(BLAS_FLAGS)

test_serial:
	$(CC) $(FLAGS) -o $(BIN_DIR)/test_serial $(TEST_DIR)/test_serial.c $(INCLUDE) $(BLAS_FLAGS)

benchmark: benchmark_serial benchmark_parallel

benchmark_serial:
	$(CC) $(FLAGS) -o $(BIN_DIR)/benchmark_serial  $(BENCHMARK_DIR)/benchmark_serial.c $(INCLUDE) $(BLAS_FLAGS)

benchmark_parallel:
	$(CC) $(FLAGS) -o $(BIN_DIR)/benchmark_parallel  $(BENCHMARK_DIR)/benchmark_parallel.c $(INCLUDE) $(BLAS_FLAGS)

clean:
	rm $(BIN_DIR)/*