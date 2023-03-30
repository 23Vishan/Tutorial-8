#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define TRUE 1
#define FALSE 0
#define BUFFER_SIZE 256

const char *DELIMITER = ", ";

/*
 * Represents data
 */
typedef struct Proc
{
    char parent[BUFFER_SIZE];
    char name[BUFFER_SIZE];
    int priority;
    int memory;
} Proc;

/*
 * Represents a node in the binary tree
 */
typedef struct Node
{
    Proc *data;
    struct Node *left;
    struct Node *right;
} Node;

/*
 * Create a node with a given set of data
 */
Node *create_node(Proc *data)
{
    Node *node = (Node*)malloc(sizeof(Node));
    node->data = data;
    node->left = NULL;
    node->right = NULL;

    return node;
}

/*
 *  Add a node to the binary tree
 */
void add_to_tree(Node *root, Proc *data)
{
    // parent is found
    if (strcmp(root->data->name, data->parent) == 0)
    {
        /* create node */
        Node *tmp = create_node(data);

        /* add as child node */
        if (root->left == NULL)
        {
            root->left = tmp;
            return;
        }
        if (root->right == NULL)
        {
            root->right = tmp;
            return;
        }
    }
    // parent is not found
    else
    {
        // call on left child
        if (root->left != NULL)
        {
            add_to_tree(root->left, data);
        }
        // call on right child
        if (root->right != NULL)
        {
            add_to_tree(root->right, data);
        }   
    }

    return;
}

/*
 * Prints the solution tree in depth first order
 */
void depth_first_print(Node *root)
{
    /* print node info */
    printf("==================================\n");
    printf("Name: %s, Parent: %s\n", root->data->name, root->data->parent);
    printf("Priority: %d, Memory: %d\n", root->data->priority, root->data->memory);

    /* call on left child */
    if (root->left != NULL)
    {
        depth_first_print(root->left);
    }

    /* call on right child */
    if (root->right != NULL)
    {
        depth_first_print(root->right);
    }

    return;
}

int main(void)
{
    Node *root = NULL;
    char buf[BUFFER_SIZE];

    /* open file*/
    FILE *fptr = fopen("processes_tree.txt", "r");

    /* get line from file */
    while (fgets(buf, BUFFER_SIZE, fptr) != NULL)
    {
        Proc *data = (Proc*)malloc(sizeof(Proc));

        /* parent */
        char *token = strtok(buf, DELIMITER);
        strcpy(data->parent, token);

        /* name */
        token = strtok(NULL, DELIMITER);
        strcpy(data->name, token);

        /* priority */
        token = strtok(NULL, DELIMITER);
        data->priority = atoi(token);

        /* memory*/
        token = strtok(NULL, DELIMITER);
        data->memory = atoi(token);

        /* add to tree */
        if (root == NULL)
        {
            root = create_node(data);
        }
        else
        {
            add_to_tree(root, data);
        }
    }

    /* print solution tree */
    depth_first_print(root);
}