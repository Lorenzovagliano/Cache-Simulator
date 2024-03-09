# Cache Memory Simulator

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
