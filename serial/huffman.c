#include <stdio.h>
#include <stdlib.h>

#include "huffman.h" 
#include "frequency.h"
#include "priority_queue.h"


/*depth travversal of tree and build the code*/
void write_codification(FILE* codification_fp, node_t *root, int *vect, int top) {
    int i;
    if (root->left != NULL){
        vect[top] = 0;
        write_codification(codification_fp, root->left, vect, top + 1);
    }
    
    if (root->right != NULL){
        vect[top] = 1;
        write_codification(codification_fp, root->right, vect, top + 1);
    }
 
    /* if a leaf is found */
    if (!(root->left) && !(root->right)){
        
        // format output string
        char *output_string = (char*) calloc(MAX_BITS_CODE + 5, sizeof(char));
        output_string[0] =  root->data;
        output_string[1] = output_string[3] = ' ';
        output_string[2] = ':';
        for (i = 0; i < top; i++) {
            output_string[4 + i] = (char)(48 + vect[i]);
        }
        output_string[4 + i] = '\n';
        
        
        fwrite(output_string, i + 6, 1, codification_fp);
        
        free(output_string);
    }
}

/* this is for debugging */
/* depth traversal of tree and build the code*/
void print_codes(node_t *root, int *vect, int top) {
	int i;
    if (root->left != NULL){
        vect[top] = 0;
        print_codes(root->left, vect, top + 1);
    }
 	
    if (root->right != NULL){
        vect[top] = 1;
        print_codes(root->right, vect, top + 1);
    }
 
 	/* if a leaf is found */
    if (!(root->left) && !(root->right)){

    	/* print the letter */
        printf("%c : ", root->data);

        /* print the code */
    	for (i = 0; i < top; i++)
        	printf("%d", vect[i]);
    	printf("\n");
    }
}


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

