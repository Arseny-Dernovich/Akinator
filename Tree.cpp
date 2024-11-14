#include "My_features.h"


struct Node_t {
    int data;
    Node_t* left;
    Node_t* right;
    Node_t* parent;
};

struct Binary_Tree {
    Node_t* root;
};

Node_t* New_Node(int data, Node_t* parent)
{
    Node_t* node = (Node_t*) calloc(1, sizeof(*node));
    if (node) {
        node->data = data;
        node->left = NULL;
        node->right = NULL;
        node->parent = parent;
    }
    return node;
}

void Insert(Node_t** root, int value)
{
    if (*root == NULL) {

        *root = New_Node(value, NULL);
        return;
    }

    Node_t* tmp = *root;

    while (tmp) {

        if (value > tmp->data) {

            if (tmp->right) {
                tmp = tmp->right;

            } else {

                tmp->right = New_Node(value, tmp);
                return;
            }

        } else if (value < tmp->data) {

            if (tmp->left) {
                tmp = tmp->left;

            } else {

                tmp->left = New_Node(value, tmp);
                return;
            }

        } else {
            return;
        }
    }
}

Node_t* Get_Node_By_Value (Node_t* node, int value)
{
    while (node) {

        if (node->data > value) {
            node = node->left;

        } else if (node->data < value) {

            node = node->right;

        } else {

            return node;
        }
    }
    return NULL;
}

void Dump_Node(FILE* file, Node_t* node, bool isRoot)
{
    if (!node) return;

    const char* fillColor = isRoot ? "lightgreen" : "lightblue";

    fprintf(file, "    node%d [label=<<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\">\n", node->data);
    fprintf(file, "        <TR><TD COLSPAN=\"2\">%d</TD></TR>\n", node->data);

    if (node->left || node->right) {
        fprintf(file, "        <TR><TD PORT=\"left\">Left</TD><TD PORT=\"right\">Right</TD></TR>\n");
    } else {
        fprintf(file, "        <TR><TD>NULL</TD><TD>NULL</TD></TR>\n");
    }

    fprintf(file, "    </TABLE>>, shape=none, fillcolor=%s, style=filled];\n", fillColor);

    if (node->left) {
        fprintf(file, "    node%d:left -> node%d;\n", node->data, node->left->data);
        Dump_Node(file, node->left, false);
    }
    if (node->right) {
        fprintf(file, "    node%d:right -> node%d;\n", node->data, node->right->data);
        Dump_Node(file, node->right, false);
    }
}

void Dump_Tree(Binary_Tree* tree, const char* filename)
{
    FILE* file = fopen(filename, "w");
    if (!file) return;

    fprintf(file, "digraph BinaryTree {\n");
    fprintf(file, "    rankdir=TB;\n");
    fprintf(file, "    ordering=out;\n");
    fprintf(file, "    splines=true;\n");
    fprintf(file, "    nodesep=0.5; ranksep=0.7;\n");
    fprintf(file, "    node [shape=none];\n");

    Dump_Node(file, tree->root, true);

    fprintf(file, "}\n");
    fclose(file);
}


void Dump_Inorder(Node_t* node)
{
    if (!node)
        return;
    Dump_Inorder(node->left);
    printf("%d ", node->data);
    Dump_Inorder(node->right);
}

void Free_Tree(Node_t* node)
{
    if (!node)
        return;
    Free_Tree(node->left);
    Free_Tree(node->right);
    free(node);
}

int main()
{
    Binary_Tree tree = {NULL};

    Insert(&tree.root, 50);
    Insert(&tree.root, 5);
    Insert(&tree.root, 12);
    Insert(&tree.root, 15);
    Insert(&tree.root, 70);
    Insert(&tree.root, 60);
    Insert(&tree.root, 17);

    Dump_Tree(&tree, "tree.dot");
    Dump_Inorder(tree.root);

    system("dot -Tpng tree.dot -o tree.png");

    Free_Tree(tree.root);

    return 0;
}
