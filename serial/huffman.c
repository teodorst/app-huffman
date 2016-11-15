#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "huffman.h" 
#include "frequency.h"
#include "priority_queue.h"


void write_codification_for_input_file(char **codification, FILE* input_fp, FILE* output_fp) {
    size_t nread = 0;
    int chunk_size = 0;
    char buf[CHUNK];
    char output_char = 0;
    int contor = 7;
    unsigned long long int bits;

    while((nread = fread(buf, 1, CHUNK, input_fp)) > 0) {
        chunk_size = CHUNK > nread ? nread : CHUNK;
        write_codification_for_chunk(buf, chunk_size, codification, output_fp, &output_char, &contor, &bits);
        memset(buf, '\0', CHUNK);
    }
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


void write_codification(FILE* codification_fp, char **codification) {
    int i;

    for (i = 0; i < MAX_BITS_CODE; i ++) {
        if (codification[i] != NULL) {
            char *output_string = (char*) calloc(MAX_BITS_CODE + 6, sizeof(char));
            sprintf(output_string, "%c : %s\n", i, codification[i]);
            fwrite(output_string, i + 6, 1, codification_fp);            
            free(output_string);
        }
    }
}


/*depth travversal of tree and build the code*/
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


// note_t* build_huffman_tree_from_codification() {
    
// }


node_t* build_huffman_tree(unsigned long long int* frequecy){
    /* initialize and populate the priority queue */
    heap_t *h = init_priority_queue();
    insert_values_in_queue(frequecy, h);

    while (h->len > 1){
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

