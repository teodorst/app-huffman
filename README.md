
# Huffman Compress and Decompress

Authors: `Teodor Stefu` `Bogdan Nadolu` `Daniela Dragan`

[![N|Solid](https://upload.wikimedia.org/wikipedia/commons/thumb/c/c6/Huffman_(To_be_or_not_to_be).svg/709px-Huffman_(To_be_or_not_to_be).svg.png)](https://en.wikipedia.org/wiki/Huffman_coding)

The Huffman compression one of the many methods used to reduce the dimension of files. The decompression is the opposite operation which recreates the initial document. These two - the compression and decompression - are made using a (binary) tree structure similar to the picture below. The tree is constructed using a bottom-up approach. Finding the codification of a character is done by traversing the tree from the root to the corresponding leaf.

Steps for compression:
1. parse the input file (read it and find the frequency of each character)
2. build the corresponding Huffman tree (a binary tree where 0 is left and 1 is right)
3. compress the input (save the character-codification mapping in a structure and encode the input string using it).

Steps for decompression:
1. get the Huffman tree (read the codification from a metadata file and build the Huffman tree)
2. decompress the file (traverse the tree until you reach a leaf then write it).

### Purpose
This programme has as a purpose to optimeze the serial code of the algorithm using threads(OpenMP, Pthreads) and passing messaging (MPI). These three separated methods, two parallel and one distributed improve time complexity.

### Test and Measurements of time
The testing and measurements of time have been done on a cluster (domain: fep.grid.pub.ro) on a few queues. To see the the commands used check the README-run&measurments.

### Resources
For the programs to compile, load the following modules `compilers/gnu-5.4.0` and `libraries/openmpi-2.0.1-gcc-5.4.0` and for profiling `utilities/intel_parallel_studio_xe_2016`.

The test queues are :
> ibm-nehalem.q
> ibm-nehalem4.q
> ibm-nehalem12.q
> campus-haswell.q

### Setup
To load into memory the input file (size can be up to 2GB) first you have to run the following command:

```sh
$ ulimit -s unlimited
```