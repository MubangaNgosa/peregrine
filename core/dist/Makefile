# Compiler settings
CC=gcc-10
CXX=g++-10

# Compiler flags
CFLAGS=-std=c++20 -Wall -Wextra -Wpedantic -fPIC -I../ -Itbb2020/include -I/usr/include/oneapi

# Linker flags
LDFLAGS=-L/home/peregrine/tbb2020/lib/intel64/gcc4.8 -ltbb -ltbbmalloc -lpthread -latomic -lzmq

# Object files
JOBPACKET_OBJ=jobPacket.o
CREATEJOBPACKETS_OBJ=createJobPackets.o
DATAGRAPH_OBJ=../DataGraph.o
JOBPOOL_OBJ=jobPool.o

# Executable names
WORKER_EXEC=worker
JOBPOOL_EXEC=jobPool

# Default target
all: $(WORKER_EXEC) $(JOBPOOL_EXEC)

# Worker executable
$(WORKER_EXEC): worker.cpp $(JOBPACKET_OBJ) $(CREATEJOBPACKETS_OBJ) $(DATAGRAPH_OBJ)
	$(CXX) $(CFLAGS) -o $(WORKER_EXEC) worker.cpp $(JOBPACKET_OBJ) $(CREATEJOBPACKETS_OBJ) $(DATAGRAPH_OBJ) $(LDFLAGS)

# JobPool executable
$(JOBPOOL_EXEC): jobPool.cpp $(JOBPACKET_OBJ) $(CREATEJOBPACKETS_OBJ) $(DATAGRAPH_OBJ)
	$(CXX) $(CFLAGS) -o $(JOBPOOL_EXEC) jobPool.cpp $(JOBPACKET_OBJ) $(CREATEJOBPACKETS_OBJ) $(DATAGRAPH_OBJ) $(LDFLAGS)

# Compile jobPacket.cpp to jobPacket.o
$(JOBPACKET_OBJ): jobPacket.cpp jobPacket.h
	$(CXX) $(CFLAGS) -c jobPacket.cpp -o $(JOBPACKET_OBJ)

# Compile createJobPackets.cpp to createJobPackets.o
$(CREATEJOBPACKETS_OBJ): createJobPackets.cpp createJobPackets.h jobPacket.h
	$(CXX) $(CFLAGS) -c createJobPackets.cpp -o $(CREATEJOBPACKETS_OBJ)

# Compile jobPool.cpp to jobPool.o
$(JOBPOOL_OBJ): jobPool.cpp
	$(CXX) $(CFLAGS) -c jobPool.cpp -o $(JOBPOOL_OBJ)

# Clean target
clean:
	rm -f $(JOBPOOL_EXEC) $(WORKER_EXEC) $(JOBPACKET_OBJ) $(CREATEJOBPACKETS_OBJ) $(JOBPOOL_OBJ)

.PHONY: all clean
