#include "TXLib.h"
#include "My_features.h"

struct Tree_Node {
    char data[256];
    Tree_Node* left;
    Tree_Node* right;
    Tree_Node* parent;
};

Tree_Node* Build_Tree(FILE* file, Tree_Node* parent);
Tree_Node* FindNode(Tree_Node* node, const char* name);
Tree_Node* FindCommonAncestor(Tree_Node* node1, Tree_Node* node2);
void Play_Akinator(Tree_Node* root);
void Define_Object(Tree_Node* root);
void Compare_Objects(Tree_Node* root);
void Display_Tree_With_Graphviz(Tree_Node* root);
void Free_Tree(Tree_Node* node);
Tree_Node* Create_Node(const char* data, Tree_Node* parent);
void Skip_Spaces(FILE* file);
int Read_Data(FILE* file, char* buffer, int bufferSize);
void Save_Tree_To_File (Tree_Node *node, FILE *file);
