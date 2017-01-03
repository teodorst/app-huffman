#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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


void decode_bytes_for_chunk(node_t *root, FILE *output_fp, char *buffer, int buffer_size, node_t **remainig_node, char* output_buffer, int *output_buffer_size) {
    
    int i;    

    for (i = 0; i < buffer_size; i++) {
        if (buffer[i] == '0') {
            /* go left in tree*/
            *remainig_node = (*remainig_node)->left;
        } else {
            /* go right in tree */
            *remainig_node = (*remainig_node)->right;
        }
        
        /* depth search from the root */
        if ((*remainig_node)->left == NULL && (*remainig_node)->right == NULL) {
            output_buffer[*output_buffer_size] = (*remainig_node)->data;
            *output_buffer_size += 1;
            *remainig_node = root;
            if (*output_buffer_size == CHUNK) {
                fwrite(output_buffer, sizeof(char), CHUNK, output_fp);
                memset(output_buffer, '\0', CHUNK);
                *output_buffer_size = 0;
            }
        }   
    }

}

void decode_bytes(FILE *in_fp, FILE *out_fp, node_t *root) {
    size_t nread = 0;
    char *input_buffer = (char *) calloc (CHUNK, sizeof(char));
    char *decoded_result = (char *) calloc(CHUNK, sizeof(char));
    int decoded_result_size = 0;
    node_t *remainig_node = root;

    while((nread = fread(input_buffer, sizeof(char), CHUNK, in_fp)) > 0) {
        decode_bytes_for_chunk(root, out_fp, input_buffer, nread, &remainig_node, decoded_result, &decoded_result_size);
        
        /* reset buffers for the next chunk */
        memset(input_buffer, '\0', CHUNK);
        
    }

    /* write remaining decoded data */
    if (decoded_result_size > 0) {
        fwrite(decoded_result, sizeof(char), decoded_result_size, out_fp);
    }
    
    /* free used memory */
    free(input_buffer);
    free(decoded_result);

}


void write_codification_for_input_file(char **codification, FILE* input_fp, FILE* output_fp) {
    size_t nread = 0;
    char buf[CHUNK];

        
    // daca sunt master distribui 
    if (myrank == 0){
    // int rank = 0;

    // calcul dimensiun fisier - fseek

    // creare vector in care sa se retina cate chunk-uri va prelucra un anumit procesor + calcul pt fiecare procesor  
        int *nr_of_chunks = (int *) malloc (nr_proc * sizeof(int));

    // trimit catre celelalte procesoare cate chunk-uri vor prelucra
        for (i = 1 ; i < nr_proc ; i ++)
            MPI_Send(nr_of_chunks[i], ..);

        while((nread = fread(buf, sizeof(char), CHUNK, input_fp)) > 0) {

            //cat timp mai am chank-uri le trimit unui anumit procesor
            while(){
                // trimite fiecare chunk catre un anumit procesor.
                MPI_Send(buf_size, ..);
                MPI_Send(buf, ..);
            }
        
            memset(buf, '\0', CHUNK);
        }

        //primeste datele de la celelalte procesoare
        for (i = 1; i < nr_proc ; i++) //pt fiecare procesor
            // cat timp mai am chunk-uri de primit
            while(nr_of_chunks[i] > 0){
                MPI_Recv(output_buffer_size, ..);     // marimea outputului 
                
                //alocare memorie buffer 


                MPI_Recv(output_buffer, ..);     // outputul in urma encodarii

                // scriere in fisier a outputului
                
                // eliberare memorie buffer de output
     
            }
    }
    else
    {
        // primeste numarul de chunk-uri de la master
        MPI_Recv(nr_of_chunks, ..);

        // primeste chunk-urile si codifica
        while(nr_of_chunks > 0){
            MPI_Recv(chunk_size, ..);
            MPI_Recv(chunk_content, ..);
            write_codification_for_chunk(buf/*chunk content*/, nread/*chunk size*/, codification, output_fp);
        }
    }
   
}

// in loc de fwrite se va face MPI_Send catre master
void write_codification_for_chunk(char *chunk, int chunk_size, char **codification, FILE* output_fp /*nu mai este necesar output_fp aici*/) {
    int i;
    char* output_buffer = (char*) malloc(CHUNK);
    int current_size = 0;
    char *codif = NULL;
    int codif_size = 0;
    int remaining_size  = 0;

    for (i = 0; i < chunk_size; i ++) {
        codif = codification[(unsigned int)chunk[i]];
        codif_size = strlen(codif);
        
	if (current_size + codif_size >= CHUNK) {
            remaining_size = current_size + codif_size  - CHUNK;
            strncpy(output_buffer + current_size, codif, codif_size - remaining_size);
            
	    fwrite(output_buffer, sizeof(char), CHUNK, output_fp);
            memset(output_buffer, '\0', CHUNK);
            current_size = remaining_size;
            
	    if (remaining_size > 0) {
                strncpy(output_buffer, codif + codif_size - remaining_size, remaining_size);
            }
        }
        else {
            strcpy(output_buffer + current_size, codif);
            current_size += codif_size;
        }   
    }
    fwrite(output_buffer, sizeof(char), current_size, output_fp);

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

