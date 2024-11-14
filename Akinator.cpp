#include  "My_features.h"

struct Tree_Node {
    char data[256];
    struct Tree_Node* left;
    struct Tree_Node* right;
    struct Tree_Node* parent;
};

Tree_Node* Build_Tree (FILE *file, Tree_Node *parent);
Tree_Node* findNode (Tree_Node *node, const char *name);
Tree_Node* findCommonAncestor (Tree_Node *node1, Tree_Node *node2);
void Play_Akinator (Tree_Node *root);
void compareObjects (Tree_Node *root);
void displayTree (Tree_Node *node, int level);
void DisplayTreeWithGraphviz(Tree_Node *root);
void freeTree (Tree_Node *node);
Tree_Node* CreateNode(const char* data, Tree_Node* parent);
void SkipSpaces(FILE* file);
int ReadData(FILE* file, char* buffer, int bufferSize);


int main ()
{
    FILE *file = fopen ("akinator.txt", "r");

    Tree_Node* root = Build_Tree (file , NULL);
    fclose (file);

    int choice = 0;
    while (1) {

        printf ("Выберите действие:\n");
        printf ("1) Отгадать\n");
        printf ("2) Сравнить объекты\n");
        printf ("3) Показать дерево\n");
        printf ("4) Выход\n");
        printf ("Ваш выбор: ");
        scanf ("%d", &choice);

        switch (choice) {

            case 1:
                Play_Akinator (root);
                break;

            case 2:
                compareObjects (root);
                break;

            case 3:
                DisplayTreeWithGraphviz (root);
                break;

            case 4:
                freeTree (root);
                printf ("Выход из программы.\n");
                return 0;

            default:
                printf ("Некорректный ввод. Попробуйте снова.\n");
        }
    }
}



Tree_Node* Build_Tree (FILE* file, Tree_Node* parent)
{
    SkipSpaces(file);

    int ch = fgetc(file);
    if (ch != '{') {
        ungetc(ch, file);
        return NULL;
    }

    char buffer[256] = "";
    if (!ReadData(file, buffer, sizeof(buffer))) {
        return NULL;
    }

    Tree_Node* node = CreateNode (buffer, parent);

    node->left = Build_Tree(file, node);

    node->right = Build_Tree(file, node);

    SkipSpaces(file);
    fgetc(file);

    return node;
}

Tree_Node* CreateNode(const char* data, Tree_Node* parent)
{
    Tree_Node* node = (Tree_Node*) calloc(1, sizeof(Tree_Node));
    if (node) {
        strncpy(node->data, data, sizeof(node->data) - 1);
        node->parent = parent;
    }
    return node;
}

void SkipSpaces(FILE* file)
{
    int ch = 0;
    while ((ch = fgetc(file)) != EOF) {
        if (!isspace(ch)) {
            ungetc(ch, file);
            break;
        }
    }
}

int ReadData(FILE* file, char* buffer, int bufferSize)
{
    SkipSpaces(file);

    int ch, i = 0;
    while ((ch = fgetc(file)) != EOF && ch != '{' && ch != '}') {
        if (i < bufferSize - 1) {
            buffer[i++] = (char) ch;
        }
    }
    buffer[i] = '\0';

    if (ch == '{' || ch == '}') {
        ungetc(ch, file);
    }

    return i > 0;
}


void Play_Akinator (Tree_Node *node)
{
    while (node->left && node->right) {
        printf ("Это %s? (да/нет): ", node->data);
        char answer[10] = "";
        scanf ("%s", answer);

        if (stricmp (answer, "да") == 0) {
            node = node->left;
        } else if (stricmp (answer, "нет") == 0) {
            node = node->right;
        } else {
            printf ("Некорректный ответ. Введите 'да' или 'нет'.\n");
        }
    }

    printf ("Ваш объект: %s? (да/нет): ", node->data);
    char answer[10] = "";
    scanf ("%s", answer);

    if (stricmp (answer, "да") == 0) {

        printf ("Угадал!\n");

    } else if (stricmp (answer, "нет") == 0) {

        char newObject[256] = "";
        printf ("Какой объект вы имели в виду? ");
        scanf ("%s", newObject);

        char question[256] = "";
        printf ("Что можно спросить, чтобы отличить %s от %s? ", newObject, node->data);
        scanf (" %[^\n]", question);

        Tree_Node *newNode = (Tree_Node*) calloc (1, sizeof (Tree_Node));
        strcpy (newNode->data, newObject);

        Tree_Node *oldNode = node;

        Tree_Node *questionNode = (Tree_Node*) calloc(1, sizeof (Tree_Node));
        strcpy (questionNode->data, question);

        questionNode->left = newNode;
        questionNode->right = oldNode;


        questionNode->parent = node->parent;
        if (node->parent) {
            if (node->parent->left == node) {

                node->parent->left = questionNode;

            } else {

                node->parent->right = questionNode;
            }
        }

        // free (oldNode);
        node = questionNode;

        printf("База данных обновлена.\n");
    } else {
        printf("Некорректный ответ. Попробуйте снова.\n");
    }
}


Tree_Node* findNode (Tree_Node *node, const char *name)
{
    if (!node)
        return NULL;

    if (stricmp (node->data, name) == 0) return node;

    Tree_Node *foundNode = findNode (node->left, name);
    if (foundNode)
        return foundNode;

    return findNode (node->right, name);
}

Tree_Node* findCommonAncestor (Tree_Node *node1, Tree_Node *node2)
{
    Tree_Node *ancestor1 = node1;
    while (ancestor1) {
        Tree_Node *ancestor2 = node2;
        while (ancestor2) {
            if (ancestor1 == ancestor2) {
                return ancestor1;
            }
            ancestor2 = ancestor2->parent;
        }
        ancestor1 = ancestor1->parent;
    }
    return NULL;
}

void compareObjects (Tree_Node *root)
{
    char obj1[256] = "", obj2[256] = "";
    printf ("Введите первый объект: ");
    scanf ("%s", obj1);
    printf ("Введите второй объект: ");
    scanf ("%s", obj2);

    Tree_Node *node1 = findNode (root, obj1);
    Tree_Node *node2 = findNode (root, obj2);

    if (node1 && node2) {
        Tree_Node *commonAncestor = findCommonAncestor (node1, node2);
        if (commonAncestor) {
            printf ("Общий признак для %s и %s: %s\n", obj1, obj2, commonAncestor->data);
        } else {
            printf("Общий предок не найден.\n");
        }
    } else {
        printf("Один или оба объекта не найдены в дереве.\n");
    }
}

void displayTree (Tree_Node *node, int level)
{
    if (!node) return;
    for (int i = 0; i < level; i++) printf("  ");
    printf ("%s\n", node->data);
    displayTree (node->left, level + 1);
    displayTree (node->right, level + 1);
}

void GenerateDot (Tree_Node *node, FILE *file)
{
    if (node == NULL) {
        return;
    }

    fprintf (file, "    \"%s\" [label=\"%s\"];\n", node->data, node->data);

    if (node->left) {
        fprintf (file, "    \"%s\" -> \"%s\" [label=\"Да\"];\n", node->data, node->left->data);
        GenerateDot (node->left, file);
    }

    if (node->right) {
        fprintf (file, "    \"%s\" -> \"%s\" [label=\"Нет\"];\n", node->data, node->right->data);
        GenerateDot (node->right, file);
    }
}

void CreateDotFile (Tree_Node *root, const char *filename)
{
    FILE *file = fopen (filename, "w");
    if (!file) {
        fprintf (stderr, "Не удалось создать файл.\n");
        return;
    }

    fprintf (file, "digraph Tree {\n");
    GenerateDot (root, file);
    fprintf (file, "}\n");

    fclose (file);
}

void DisplayTreeWithGraphviz (Tree_Node *root)
{
    const char *dotFilename = "akinator.dot";
    const char *imageFilename = "akinator.png";

    CreateDotFile (root, dotFilename);

    char command[512] = "";
    snprintf (command, sizeof(command), "dot -Tpng %s -o %s", dotFilename, imageFilename);
    system (command);

    printf("Дерево сохранено в файл %s\n", imageFilename);
}

void freeTree (Tree_Node *node)
{
    if (!node) return;
    freeTree (node->left);
    freeTree (node->right);
    free (node);
}
