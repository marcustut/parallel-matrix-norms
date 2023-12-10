UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	CC=gcc-13
else
	CC=gcc
endif

FLAGS=-O3 -fopenmp
LIB_FLAGS=-lpthread -L/opt/homebrew/opt/libomp/lib
INCLUDE=-Iinclude -I/opt/homebrew/opt/libomp/include

BIN_DIR=bin
TEST_DIR=tests
BENCHMARK_DIR=benchmark

test: test_serial test_parallel_pthread test_parallel_openmp

test_serial:
	$(CC) $(FLAGS) -o $(BIN_DIR)/test_serial $(TEST_DIR)/test_serial.c $(INCLUDE) $(LIB_FLAGS)

test_parallel_pthread:
	$(CC) $(FLAGS) -o $(BIN_DIR)/test_parallel_pthread $(TEST_DIR)/test_parallel_pthread.c $(INCLUDE) $(LIB_FLAGS)

test_parallel_openmp:
	$(CC) $(FLAGS) -o $(BIN_DIR)/test_parallel_openmp $(TEST_DIR)/test_parallel_openmp.c $(INCLUDE) $(LIB_FLAGS)

benchmark: benchmark_serial benchmark_parallel_pthread benchmark_parallel_openmp

benchmark_serial:
	$(CC) $(FLAGS) -o $(BIN_DIR)/benchmark_serial  $(BENCHMARK_DIR)/benchmark_serial.c $(INCLUDE) $(LIB_FLAGS)

benchmark_serial_multiplication:
	$(CC) $(FLAGS) -o $(BIN_DIR)/benchmark_serial_multiplication  $(BENCHMARK_DIR)/benchmark_serial_multiplication.c $(INCLUDE) $(LIB_FLAGS)

benchmark_parallel_pthread:
	$(CC) $(FLAGS) -o $(BIN_DIR)/benchmark_parallel_pthread  $(BENCHMARK_DIR)/benchmark_parallel_pthread.c $(INCLUDE) $(LIB_FLAGS)

benchmark_parallel_openmp:
	$(CC) $(FLAGS) -o $(BIN_DIR)/benchmark_parallel_openmp  $(BENCHMARK_DIR)/benchmark_parallel_openmp.c $(INCLUDE) $(LIB_FLAGS)

benchmark_parallel_mpi:
	mpicc -o $(BIN_DIR)/benchmark_parallel_mpi  $(BENCHMARK_DIR)/benchmark_parallel_mpi.c $(INCLUDE) $(LIB_FLAGS)

clean:
	rm $(BIN_DIR)/*