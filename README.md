# Cache Memory Simulator

This project is a cache memory simulator implemented in C++. It takes inputs such as the total cache size, line size, group size, and memory access file to simulate different cache memory configurations. The simulator supports three types of cache mappings: Fully Associative, Set Associative, and Direct Mapping.

The program reads memory access patterns from a file and simulates cache hits and misses accordingly. It maintains cache history and outputs the cache states along with hit and miss counts to an output file.

To run the simulator, compile the provided code and execute it with the appropriate command-line arguments specifying the cache configuration and input file.

For detailed instructions on how to use the simulator and interpret the output, refer to the sections below.

## Compilation

Compiler: GCC

Command: `g++ TP.cpp -o TP`

## Execution

Command: `./TP <total_cache_size> <line_size> <group_size> <input_file>`

Note: The group size defines the implemented logic:
- If `group_size = total_cache_size / line_size` -> Fully Associative
- If `group_size < total_cache_size / line_size` -> Set Associative
- If `group_size = 1` -> Direct Mapping

A text file with the input must be created, as indicated in the execution. 
Another file with the output (`saida.txt`) will be generated.

Examples:
Fully Associative: `./TP 4096 1024 4 input.txt`
Set Associative: `./TP 4096 1024 2 input.txt`
Direct Mapping: `./TP 4096 1024 1 input.txt`
