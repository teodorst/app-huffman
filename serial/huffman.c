#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "huffman.h" 
#include "frequency.h"
#include "priority_queue.h"


char* read_bytes(FILE *fp, unsigned long long int nbits) {
    size_t nread = 0;
    unsigned long long int total_bytes = 0L;
    unsigned long long int nbytes = nbits % 8L == 0L ? nbits / 8L : nbits/8L + 1L; 
    char *buf = (char *) calloc (nbytes, sizeof(char));

    while((nread = fread(buf + total_bytes, 1, CHUNK, fp)) > 0) {
        total_bytes += CHUNK > nread ? nread : CHUNK;
    }

    if (total_bytes < nbytes)
        printf("less number of bytes %llu\n", total_bytes);

    return buf;
}

void write_decoded_ch(FILE* out_fp, char *resut) {
    
    fwrite(resut, strlen(resut), 1, out_fp);            

}

char* decode_bytes(node_t *root, char *buffer, unsigned long long int *nbits) {
    int i, j;
    node_t *node = root;
    unsigned long long int nbytes = *nbits % 8 == 0 ? *nbits/8 : *nbits/8 + 1;
    unsigned long long int count_bits = 0L;
    char *out_buffer = (char *) malloc (2 * nbytes * sizeof(char));
    unsigned long long int out_buffer_size = 0;
    int bit;

    for (i = 0; i < nbytes; i++) {
        for (j = 7 ; j >= 0 && count_bits < *nbits; j--) {
            
            /* get bit */
            bit = (buffer[i] >> j) & 1;
            
            /* go left in tree*/
            if (bit == 0) {
                node = node->left;
            }
            else    /* go right in tree */
            {
                node = node->right;
            }

            /* depth search from the root */
            if (node->left == NULL && node->right == NULL) {
                out_buffer[out_buffer_size++] = node->data;
                node = root;
            }

            count_bits ++;
        }
    }
    out_buffer[count_bits] = '\0';

    return out_buffer;
}

unsigned long long int write_codification_for_input_file(char **codification, FILE* input_fp, FILE* output_fp) {
    size_t nread = 0;
    int chunk_size = 0;
    char buf[CHUNK];
    char output_char = 0;
    int contor = 7;
    unsigned long long int bits = 0L;

    while((nread = fread(buf, 1, CHUNK, input_fp)) > 0) {
        chunk_size = CHUNK > nread ? nread : CHUNK;
        write_codification_for_chunk(buf, chunk_size, codification, output_fp, &output_char, &contor, &bits);
        memset(buf, '\0', CHUNK);
    }

    // if there are some bits to write in the last byte.
    if (contor != 7) {
        fwrite(&output_char, 1, 1, output_fp);
    }    
    return bits;
}


void write_codification_for_chunk(char *chunk, int chunk_size, char **codification, FILE* output_fp, char* output_char, int* contor,
    unsigned long long int* bits) {
    int i, j;
    for (i = 0; i < chunk_size; i ++) {
        char *codif = codification[(unsigned int)chunk[i]];
        for (j = 0; j < strlen(codif); j ++) {
            *output_char |= (codif[j] - 48) << *contor;
            *contor -= 1;
            *bits += 1;
            if (*contor == -1) {
                fwrite(output_char, 1, 1, output_fp);
                *contor = 7;
                *output_char = 0;
            }    
        }
    }
}


void write_codification(FILE* codification_fp, char **codification, unsigned long long int nbits) {
    int i;
    char *output_string = (char*) calloc(MAX_BITS_CODE + 5, sizeof(char));

    fprintf(codification_fp, "%llu\n", nbits);

    for (i = 0; i < MAX_BITS_CODE; i ++) {
        if (codification[i] != NULL) {
            sprintf(output_string, "%c : %s\n", i, codification[i]);
            fwrite(output_string, strlen(codification[i]) + 5, 1, codification_fp);            
            memset(output_string, 0, MAX_BITS_CODE + 5);
        }
    }
    free(output_string);
}


char** read_configuration(FILE *codification_fp, unsigned long long int* nbits) {
    char **codification = (char**) calloc(128, sizeof(char*));
    char *line = NULL;
    ssize_t nread;
    char index;
    size_t line_length;

    fscanf(codification_fp, "%llu", nbits);
    fgetc(codification_fp);
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


// Return a new node with given data
node_t* init_node(char data) {
    node_t* node = (node_t*) malloc(sizeof(node_t));
    node->data = data;
    node->left = node->right = NULL;
    node->priority = 0; // this data is unused in decompression phase
    return node;
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

