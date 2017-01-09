#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#include "huffman.h" 
#include "frequency.h"
#include "priority_queue.h"

// Return a new node with given data
node_t* init_node(char data) {
    node_t* node = (node_t*) malloc(sizeof(node_t));
    node->data = data;
    node->left = node->right = NULL;
    node->priority = 0; // this data is unused in decompression phase
    return node;
}


int decode_bytes_for_chunk(node_t *root, char *buffer, unsigned long long int nbits, node_t **remainig_node, char* out_buffer) {
    int i, j;
    node_t *node = *remainig_node;
    unsigned long long int nbytes = nbits % 8 == 0 ? nbits/8 : nbits/8 + 1;
    unsigned long long int count_bits = 0L;
    int bit;
    int length = 0;

    for (i = 0; i < nbytes; i++) {
        for (j = 7 ; j >= 0 && count_bits < nbits ; j--) {
          
            /* get bit */
            bit = (buffer[i] >> j) & 1;
    
            if (bit == 0) {
                /* go left in tree*/
                node = node->left;
            } else {
                /* go right in tree */
                node = node->right;
            }

            /* depth search from the root */
            if (node->left == NULL && node->right == NULL) {
                out_buffer[length ++] = node->data;
                node = root;
            }

            count_bits ++;
        }
    }

    return length;
}

void decode_bytes(FILE *in_fp, FILE *out_fp, FILE *codification_fp, node_t *root, unsigned long long int *bits_per_chunk) {
    size_t nread = 0;
    node_t *remainig_node = root;
    int length = 0;

    int chunk_index = 0;
    unsigned long long int chunk_size = 0;
    if (bits_per_chunk[chunk_index] % 8 == 0)
        chunk_size = bits_per_chunk[chunk_index] / 8;
    else
        chunk_size = bits_per_chunk[chunk_index] / 8 + 1;

    char *aux_buf = (char *) calloc (chunk_size, sizeof(char));
    char *result = (char *) calloc (10000 * chunk_size, sizeof(char));


    while((nread = fread(aux_buf, 1, chunk_size, in_fp)) > 0) {
        remainig_node = root;

        if (nread == chunk_size){
            length = decode_bytes_for_chunk(root, aux_buf, bits_per_chunk[chunk_index], &remainig_node, result);
         }

        fwrite(result, 1, length, out_fp);

        /* reset buffers for the next chunk */
        memset(aux_buf, '\0', chunk_size);
        memset(result, '\0', 5 * chunk_size);
        
        chunk_index ++;
        if (bits_per_chunk[chunk_index] % 8 == 0)
            chunk_size = bits_per_chunk[chunk_index] / 8;
        else
            chunk_size = bits_per_chunk[chunk_index] / 8 + 1;
    }


    free(aux_buf);
    free(result);
}


void write_codification_for_input_file(char **codification, FILE* input_fp, FILE* output_fp, FILE *codification_fp, FILE *codification_fp2, int myrank, int nr_proc) {
    size_t nread = 0;
    char buf[CHUNK];
    long int input_file_size;
    MPI_Status status;

    memset(buf, '\0', CHUNK);

    // master task 
    if (myrank == 0){
        int i;

        // compute the file size
        fseek(input_fp, 0, SEEK_END);
        input_file_size = ftell(input_fp);
        fseek(input_fp, 0, SEEK_SET);

        long int nr_chunks = (input_file_size % CHUNK == 0) ? input_file_size/CHUNK : (input_file_size/CHUNK + 1);
        fprintf(codification_fp2, "%ld\n", nr_chunks);

        // compute number of chunk for each proc  
        long int *nr_of_chunks = (long int *) malloc (nr_proc * sizeof(long int));
        long int *nr_of_chunks_aux = (long int *) malloc (nr_proc * sizeof(long int));

        int standard_nr_per_chunk = nr_chunks / (nr_proc - 1);
        for (i = 1 ; i < nr_proc ; i++) {
            nr_of_chunks[i] = standard_nr_per_chunk;
            nr_of_chunks_aux[i] = nr_of_chunks[i];
        }

        if (nr_chunks % (nr_proc - 1) != 0) { 
            nr_of_chunks[nr_proc-1] += nr_chunks % (nr_proc - 1);
            nr_of_chunks_aux[nr_proc-1] = nr_of_chunks[nr_proc-1];
        }

        // send the number of chunks to each proc
        for (i = 1 ; i < nr_proc ; i ++){
            MPI_Send(&nr_of_chunks[i], 1, MPI_LONG_INT, i, MPI_TAG, MPI_COMM_WORLD);
        }

        i = 1;
        while((nread = fread(buf, sizeof(char), CHUNK, input_fp)) > 0) {
            
            //distribute the content of the input file
            if (nr_of_chunks[i] > 0){
                // send a chunk to proc i
                MPI_Send(&nread, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
                
                MPI_Send(buf, nread, MPI_CHAR, i, 0, MPI_COMM_WORLD);
                nr_of_chunks[i] --;

                if (nr_of_chunks[i] == 0)
                    i++;
            }
            
            memset(buf, '\0', CHUNK);
        }

        unsigned long long int output_buffer_bits;
        // gather data from all the others procs
        for (i = 1; i < nr_proc ; i++) {
        
            while(nr_of_chunks_aux[i] > 0){
                MPI_Recv(&output_buffer_bits, 1, MPI_UNSIGNED_LONG_LONG, i, 30, MPI_COMM_WORLD, &status);            
            
                unsigned long long int nbytes = output_buffer_bits % 8 == 0 ? output_buffer_bits/8 : output_buffer_bits/8 + 1;
                char *output_buffer = (char *) malloc (nbytes * sizeof(char));
                memset(output_buffer, '\0', nbytes);

                MPI_Recv(output_buffer, nbytes, MPI_CHAR, i, 30, MPI_COMM_WORLD, &status);

                // write in output file the content after codification
                fwrite(output_buffer, 1, nbytes, output_fp);
                fprintf(codification_fp2, "%llu\n", output_buffer_bits);
            
                free(output_buffer);

                nr_of_chunks_aux[i] --;
            }
        }
    }
    else
     {
        long int nr_of_chunks;
        int chunk_size;

        // receive the number of chunks from the zero proc 
        MPI_Recv(&nr_of_chunks, 1, MPI_LONG_INT, 0, MPI_TAG, MPI_COMM_WORLD, &status);

        // receive chunks write codification
        while(nr_of_chunks > 0){
            MPI_Recv(&chunk_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(buf, chunk_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &status);
            
            write_codification_for_chunk(buf, chunk_size, codification, output_fp, codification_fp2, myrank);
            
            memset(buf, '\0', CHUNK);
            nr_of_chunks --;
        }
    }
}

// in loc de fwrite se va face MPI_Send catre master
void write_codification_for_chunk(char *chunk, int chunk_size, char **codification, FILE* output_fp /*nu mai este necesar output_fp aici*/, FILE *codification_fp2, int myrank) {
    char* output_buffer = (char*) malloc(CHUNK);
    char output_char = 0;
    int output_buffer_contor = 0;
    int contor = 7;
    int i, j;
    unsigned long long int bits = 0;

    memset(output_buffer, '\0', CHUNK);

    
    for (i = 0; i < chunk_size; i ++) {
        char *codif = codification[(unsigned int)chunk[i]];
        for (j = 0; j < strlen(codif); j ++) {
            output_char |= (codif[j] - 48) << contor;
            contor --;
            bits += 1;

            if (contor == -1) {
                output_buffer[output_buffer_contor] = output_char;
                output_buffer_contor += 1;
                output_char = 0;
                contor = 7;
            }
        }   
    }

    if (contor > -1 && contor < 7) {
        output_buffer[output_buffer_contor] = output_char;
        output_buffer_contor += 1;
    }

    // sends the result back to proc 0
    MPI_Send(&bits, 1, MPI_UNSIGNED_LONG_LONG, 0, 30, MPI_COMM_WORLD);
    MPI_Send(output_buffer, output_buffer_contor, MPI_CHAR, 0, 30, MPI_COMM_WORLD);

    free(output_buffer);
}


void write_codification(FILE* codification_fp, char **codification) {
    int i;
    char *output_string = (char*) calloc(MAX_BITS_CODE + 5, sizeof(char));

    for (i = 0; i < MAX_BITS_CODE; i ++) {
        if (codification[i] != NULL) {
            sprintf(output_string, "%c : %s\n", i, codification[i]);
            fwrite(output_string, strlen(codification[i]) + 5, 1, codification_fp);            
            memset(output_string, 0, MAX_BITS_CODE + 5);
        }
    }
    free(output_string);
}


char** read_configuration(FILE *codification_fp) {
    char **codification = (char**) calloc(128, sizeof(char*));
    
    char *line = NULL;
    ssize_t nread;
    char index;
    size_t line_length;

    while ((nread = getline(&line, &line_length, codification_fp)) != -1) {
        if (nread == 1) {
            index = line[0];
            nread = getline(&line, &line_length, codification_fp);
            if (nread != -1) {
                line[nread-1] = '\0';
                codification[(unsigned char)index] = strdup(line+3);    
            }
        } else {
            line[nread-1] = '\0';
            sscanf(line, "%c", &index);
            codification[(unsigned char)index] = strdup(line+4);    
        }
    }
    

    free(line);
    return codification;
}


/* depth traversal of tree and build the code */
void find_codification(node_t *node, char *path, int level, char **codification) {
    if (node->left != NULL){
        path[level] = 48;
        find_codification(node->left, path, level + 1, codification);
    }
    
    if (node->right != NULL){
        path[level] = 49;
        find_codification(node->right, path, level + 1, codification);
    }
 
    /* if a leaf is found */
    if (!(node->left) && !(node->right)){
        path[level] = '\0';
        codification[(unsigned char)node->data] = strdup(path);
    }
}


/* this is for debugging */
/* depth traversal of tree and build the code*/
void print_codes(node_t *root, char *path, int level) {
    if (root->left != NULL){
        path[level] = 48;
        print_codes(root->left, path, level + 1);
    }
    
    if (root->right != NULL){
        path[level] = 49;
        print_codes(root->right, path, level + 1);
    }
 
    /* if a leaf is found */
    if (!(root->left) && !(root->right)){
        path[level] = '\0';
        /* print the codification for letter */
        printf("%c : %s\n", root->data, path);
    }
}


node_t* build_huffman_tree_from_codification(char **codification) {
    int i;
    int current_code_index = 0;
    int codification_length;

    // init the root node
    node_t* root = init_node('#');
    node_t* node = root;
    
    for (i = 0; i < 128; i ++) {
        if (codification[i]) {
            node = root;
            codification_length = strlen(codification[i]);
            current_code_index = 0;
            for (current_code_index = 0; current_code_index < codification_length; current_code_index ++) {
                if (codification[i][current_code_index] == '0') {
                    if (node->left == NULL) {
                        node->left = init_node('#');
                    }
                    node = node->left;
                }
                else {
                    if (node->right == NULL) {
                        node->right = init_node('#');
                    }
                    node = node->right;
                }
            }
            node->data = i;
        }
    }
    return root;
}


node_t* build_huffman_tree(unsigned long long int* frequecy){
    /* initialize and populate the priority queue */
    heap_t *h = init_priority_queue();
    insert_values_in_queue(frequecy, h);

    while (h->len > 1) {
        node_t* root = (struct node_t*) malloc (sizeof(struct node_t));
        node_t* left;
        node_t* right;

        /* extract two nodes from heap to form their root node */
        left = pop(h);
        right = pop(h);
        /* populate the root node */
        root->priority = left->priority + right->priority;
        root->data = '#';
        root->left = left;
        root->right = right;
        
        /* insert root node into the heap */
        push(h, root);
    }

    /* return the only node left in heap */
    return pop(h);
}