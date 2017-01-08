# app-huffman

1. Run generate_test_files.sh : ./generate_test_files.sh
2. If everything goes right there will be a folder named 'tests' with the random generated files of different dimensions.
3. Run test_compression_OpenMP.sh : ./test_compression_OpenMP.sh
4. There will be 'timeOpenMP.txt' with the compression durations.

Recommended: Before commit use: "rm -rf tests" and "rm timeOpenMP.txt"