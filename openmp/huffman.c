#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "huffman.h" 
#include "frequency.h"
#include "priority_queue.h"

#define NUM_THREADS 8

// Return a new node with given data
node_t* init_node(char data) {
    node_t* node = (node_t*) malloc(sizeof(node_t));
    node->data = data;
    node->left = node->right = NULL;
    node->priority = 0; // this data is unused in decompression phase
    return node;
}

void write_decoded_ch(FILE* out_fp, char *result, int length) {
    
    fwrite(result, length, 1, out_fp);
    
}

int decode_bytes_for_chunk(node_t *root, char *buffer, unsigned long long int nbits, node_t **remainig_node, char* out_buffer) {
    
    int i, j;
    node_t *node = *remainig_node;
    unsigned long long int nbytes = nbits % 8 == 0 ? nbits/8 : nbits/8 + 1;
    unsigned long long int count_bits = 0L;
    unsigned long long int out_buffer_size = 0;
    int bit;
    int length = 0;

    //#pragma omp parallel for private(i, j) shared(nbits, nbytes, buffer, node, root, out_buffer, count_bits, out_buffer_size, bit, length) schedule(static, 500)
    for (i = 0; i < nbytes; i++) {
        for (j = 7 ; j >= 0 && count_bits < nbits; j--) {
            
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
                out_buffer[out_buffer_size++] = node->data;
                node = root;
                length ++;
            }

            count_bits ++;
        }
    }

    *remainig_node = node;
    return length;
}

void decode_bytes(FILE *in_fp, FILE *out_fp, node_t *root, unsigned long long int nbits) {
    size_t nread = 0;
    unsigned long long int total_bytes = 0L;
    unsigned long long int nbytes = nbits % 8L == 0L ? nbits / 8L : nbits/8L + 1L; 
    char *aux_buf = (char *) calloc (CHUNK, sizeof(char));
    char *result = (char *) calloc (nbytes, sizeof(char));
    node_t *remainig_node;
    remainig_node = root;

    int length = 0;
    int minimum_size = CHUNK * 8;
    while((nread = fread(aux_buf, 1, CHUNK, in_fp)) > 0) {
        if (nbits > minimum_size) {
            length = decode_bytes_for_chunk(root, aux_buf, (CHUNK * 8), &remainig_node, result);
            nbits -= minimum_size;
        } else {
            length = decode_bytes_for_chunk(root, aux_buf, nbits, &remainig_node, result);
        }
        
        /* write in out file the result for that CHUNK*/
        write_decoded_ch(out_fp, result, length);

        total_bytes += CHUNK > nread ? nread : CHUNK;

        memset(aux_buf, '\0', CHUNK);
        memset(result, '\0', nbytes);

    }

    free(aux_buf);
    free(result);

    if (total_bytes < nbytes)
        printf("less number of bytes %llu\n", total_bytes);
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


void write_codification_metadata(FILE* codification_fp, int file_size, char **codification, int codification_size, 
    int *output_buffer_contors, long *nbits_buffer, int task_num) {

    int i;
    char *output_string = (char*) calloc(MAX_BITS_CODE + 5, sizeof(char));
    long nbits = 0;
    
    for (i = 0; i < task_num; i ++) {
        nbits += nbits_buffer[i];
    }
    fprintf(codification_fp, "%d\n", file_size);
    fprintf(codification_fp, "%ld\n", nbits);
    fprintf(codification_fp, "%d\n", codification_size);

    for (i = 0; i < MAX_BITS_CODE; i ++) {
        if (codification[i] != NULL) {
            fprintf(codification_fp, "%c : %s\n", i, codification[i]);
        }
    }
    for (i = 0; i < task_num; i ++) {
        fprintf(codification_fp, "%d %ld\n", output_buffer_contors[i], nbits_buffer[i]);
    }
    free(output_string);
}

char **read_configuration_metadata(FILE* codification_fp, int *file_size, int **input_buffer_contors, 
    long **nbits_buffer, int *tasks_num, long *nbits, int *compressed_file_size) {
    int LINESZ = 100;
    char **codification = (char**) calloc(128, sizeof(char*));
    char line[LINESZ];
    char index;
    int codification_length;
    int line_index = 0;
    int i;

    // read number of lines from codification
    fscanf(codification_fp, "%d\n", file_size);
    fscanf(codification_fp, "%ld\n", nbits);
    fscanf(codification_fp, "%d\n", &codification_length);
    // read codification
    for (i = 0; i < codification_length; i ++ ) {
        memset(line, '\0', LINESZ);
        fgets(line, LINESZ, codification_fp);
        line[strlen(line)-1] = '\0';
        line_index = 4;
        index = line[0];

        if (line[0] == ':') {
            line_index = 2;
            index = '\n';
        }
        codification[(unsigned char)index] = strdup(line+line_index);
    }

    *tasks_num = *file_size % CHUNK == 0 ? *file_size / CHUNK : *file_size / CHUNK + 1;
    *input_buffer_contors = (int*) calloc(*tasks_num, sizeof(int));
    *nbits_buffer = (long*) calloc(*tasks_num, sizeof(long));

    for (i = 0; i < *tasks_num; i ++) {
        fscanf(codification_fp, "%d %ld", *input_buffer_contors + i, *nbits_buffer + i);
    }

    for (i = 0; i < *tasks_num; i ++) {
        *compressed_file_size += (*input_buffer_contors)[i];
    }

    for (i = *tasks_num - 1; i > 0; i --) {
        (*input_buffer_contors)[i] = (*input_buffer_contors)[i-1];
    }
    (*input_buffer_contors)[0] = 0;

    for (i = 1; i < *tasks_num; i ++) {
        (*input_buffer_contors)[i] += (*input_buffer_contors)[i-1];
    }

    return codification;

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


node_t* build_huffman_tree_from_codification(char **codification) {
    int i;
    int current_code_index = 0;
    int codification_length;

    // init the root node
    node_t* root = init_node('#');
    node_t* node = root;
    
    //static schedule static -> chunck-uri de dimensiunea 500 sunt calculate si asignate thread-urilor in etapa de compilare
    // si nu in etapa de run-time ca la dynamic ceea ce ar fi presupus existenta mai multor "lock"-uri
    omp_set_dynamic(0);
    omp_set_num_threads(NUM_THREADS);
    #pragma omp parallel for private(i, current_code_index, node, codification_length) shared(codification, root) schedule(static, 500)
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


node_t* build_huffman_tree(int* frequecy){
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

long get_file_length(FILE *fp) {
    long sz = 0;
    fseek(fp, 0L, SEEK_END);
    sz = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    return sz;
}

void write_codification_for_chunk_tasks(char *chunk, int index, int upper_limit, char **codification,
	char *output_buffer, int *output_buffer_contor, long *bits) {
    
    *output_buffer_contor = index;
    *bits = 0;
    int output_buffer_contor_static = index;
    int contor = 7;
    int i, j;
    char output_char = 0;
    char *codif = NULL;
    int codif_size = 0;

    for (i = index; i < upper_limit; i ++) {
        codif = codification[(unsigned char)chunk[i]];
        codif_size = strlen(codif);
        for (j = 0; j < codif_size; j ++) {
            output_char |= (codif[j] - 48) << contor;
            contor --;
            *bits += 1;

            if (contor == -1) {
                output_buffer[output_buffer_contor_static++] = output_char;
                output_char = 0;
                contor = 7;
            }
        }
    }

    if (contor > -1 && contor < 7) {
        output_buffer[output_buffer_contor_static] = output_char;
        output_buffer_contor_static += 1;
    }

    *output_buffer_contor = output_buffer_contor_static - index;

}

int decode_bytes_tasks(node_t *root, char *buffer, unsigned long long int nbits, char* out_buffer) {

    int i, j;
    node_t *node = root;
    unsigned long long int nbytes = nbits % 8 == 0 ? nbits/8 : nbits/8 + 1;
    unsigned long long int count_bits = 0L;
    int bit;
    int length = 0;
    for (i = 0; i < nbytes; i++) {
        for (j = 7 ; j >= 0 && count_bits < nbits; j--) {

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

