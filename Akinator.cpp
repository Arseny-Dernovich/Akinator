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

//
// // Графическое меню
// void DrawMenu() {
//     txSetFillColor(TX_WHITE);
//     txClear();
//     txSetColor(TX_BLACK);
//     txSetFillColor(TX_LIGHTBLUE);
//
//     const int buttonWidth = 200;
//     const int buttonHeight = 50;
//     const int startX = 100;
//     const int startY = 100;
//     const int spacing = 20;
//
//     const char* buttonLabels[] = {
//         "Отгадать",
//         "Сравнить объекты",
//         "Показать дерево",
//         "Определение объекта",
//         "Выход"
//     };
//
//     for (int i = 0; i < 5; i++) {
//         int x = startX;
//         int y = startY + i * (buttonHeight + spacing);
//         txRectangle(x, y, x + buttonWidth, y + buttonHeight);
//         txDrawText(x, y, x + buttonWidth, y + buttonHeight, buttonLabels[i], DT_CENTER | DT_VCENTER | DT_SINGLELINE);
//     }
// }
//
// // Определение, на какую кнопку нажали
// int GetButtonClick() {
//     const int buttonWidth = 200;
//     const int buttonHeight = 50;
//     const int startX = 100;
//     const int startY = 100;
//     const int spacing = 20;
//
//     if (txMouseButtons() & 1) {
//         for (int i = 0; i < 5; i++) {
//             int x = startX;
//             int y = startY + i * (buttonHeight + spacing);
//             if (txMouseX() > x && txMouseX() < x + buttonWidth &&
//                 txMouseY() > y && txMouseY() < y + buttonHeight) {
//                 return i + 1;
//             }
//         }
//     }
//     return 0; // Нажатий нет
// }

int main()
{
    FILE *file = fopen("akinator.txt", "r");
    if (!file)
    {
        printf("Не удалось открыть файл 'akinator.txt'.\n");
        return 1;
    }

    Tree_Node *root = Build_Tree(file, NULL);
    fclose(file);

    int choice = 0;
    while (1)
    {
        printf("Выберите действие:\n");
        printf("1) Отгадать\n");
        printf("2) Сравнить объекты\n");
        printf("3) Показать дерево\n");
        printf("4) Определение объекта\n");
        printf("5) Выход\n");
        printf("Ваш выбор: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            Play_Akinator(root);
            break;

        case 2:
            Compare_Objects(root);
            break;

        case 3:
            Display_Tree_With_Graphviz(root);
            break;

        case 4:
            Define_Object(root);
            break;

        case 5:
            Free_Tree(root);
            printf("Выход из программы.\n");
            return 0;

        default:
            printf("Некорректный ввод. Попробуйте снова.\n");
        }
    }
}

Tree_Node* Build_Tree (FILE* file, Tree_Node* parent)
{
    Skip_Spaces (file);

    int ch = fgetc (file);
    if (ch != '{') {
        ungetc (ch, file);
        return NULL;
    }

    char buffer[256] = "";
    if (!Read_Data (file, buffer, sizeof (buffer))) {
        return NULL;
    }

    Tree_Node* node = Create_Node (buffer, parent);

    node->left = Build_Tree (file, node);

    node->right = Build_Tree (file, node);

    Skip_Spaces (file);
    fgetc (file);

    return node;
}


Tree_Node* Create_Node (const char* data, Tree_Node* parent)
{
    Tree_Node* node = (Tree_Node*) calloc (1, sizeof (Tree_Node));
    if (node) {
        strncpy (node->data, data, sizeof (node->data) - 1);
        node->parent = parent;
    }
    return node;
}


void Skip_Spaces (FILE* file)
{
    int ch = 0;
    while ( (ch = fgetc (file)) != EOF) {

        if (!isspace (ch)) {
            ungetc (ch, file);
            break;
        }
    }
}


int Read_Data (FILE* file, char* buffer, int bufferSize)
{
    Skip_Spaces (file);

    int ch, i = 0;
    while ( (ch = fgetc (file)) != EOF && ch != '{' && ch != '}') {

        if (i < bufferSize - 1) {
            buffer[i++] = (char) ch;
        }
    }

    buffer[i] = '\0';

    if (ch == '{' || ch == '}') {

        ungetc (ch, file);
    }

    return i;
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

        Tree_Node *questionNode = (Tree_Node*) calloc (1, sizeof (Tree_Node));
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

        node = questionNode;

    } else {

        printf ("Некорректный ответ. Попробуйте снова.\n");
    }
}


Tree_Node* FindNode(Tree_Node* node, const char* name)
{
    if (!node) {
        return NULL;
    }

    printf("Проверяется узел: %s\n", node->data);

    if (strcmp(node->data, name) == 0) {

        printf("Найден узел: %s\n", node->data);
        return node;
    }

    Tree_Node* foundNode = FindNode(node->left, name);

    if (foundNode) {

        return foundNode;
    }

    return FindNode(node->right, name);
}


void RemoveNewline (char* str)
{
    char* newline = strchr(str, '\n');
    if (newline) {
        *newline = '\0';
    }
}


void GetObjectDefinition (Tree_Node* node, const char* name)
{
    if (!node) {
        printf("Объект не найден в дереве.\n");
        return;
    }

    Tree_Node* Name_Node = FindNode(node, name);
    if (!Name_Node) {
        printf("Объект '%s' не найден в дереве.\n", name);
        return;
    }

    Tree_Node* path[256] = {};
    int pathSize = 0;
    node = Name_Node->parent;
    while (node) {

        path[pathSize++] = node;
        node = node->parent;
    }

    printf("Определение объекта: ");
    printf ("Это -");
    for (int i = pathSize - 1; i >= 0; i--) {

        RemoveNewline (path[i]->data);
        printf("%s, ", path[i]->data);


        if (i > 0) {

            if (path[i - 1] == path[i]->left) {

                printf("");

            } else if (path[i - 1] == path[i]->right) {

                printf("не ");
            }
        }
    }
    printf ("\n");

}



void Define_Object (Tree_Node* root)
{
    char objectName[256] = "";
    printf ("Введите имя объекта для определения: ");
    scanf ("%s", objectName);

    Tree_Node* node = FindNode (root, objectName);
    printf ("%s " , node->data);
    GetObjectDefinition (node , objectName);
}


Tree_Node* FindCommonAncestor (Tree_Node *node1, Tree_Node *node2)
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


void Compare_Objects (Tree_Node *root)
{
    char obj1[256] = "", obj2[256] = "";
    printf ("Введите первый объект: ");
    scanf ("%s", obj1);
    printf ("Введите второй объект: ");
    scanf ("%s", obj2);

    Tree_Node *node1 = FindNode (root, obj1);
    Tree_Node *node2 = FindNode (root, obj2);

    if (node1 && node2) {

        Tree_Node *commonAncestor = FindCommonAncestor (node1, node2);
        if (commonAncestor) {

            printf ("Общий признак для %s и %s: %s\n", obj1, obj2, commonAncestor->data);

        } else {

            printf ("Общий предок не найден.\n");
        }

    } else {
        printf ("Один или оба объекта не найдены в дереве.\n");
    }
}


void GenerateDot (Tree_Node *node, FILE *file)
{
    if (node == NULL) {
        return;
    }

    if (node->parent == NULL)
        fprintf (file, "    \"%s\" [label=\"%s\" , fillcolor = yellow];\n", node->data, node->data);

    else
        fprintf (file, "    \"%s\" [label=\"%s\"];\n", node->data, node->data);

    if (node->left) {
        fprintf (file, "    \"%s\" -> \"%s\" [label=\"Да\" , color = green];\n", node->data, node->left->data);
        GenerateDot (node->left, file);
    }

    if (node->right) {
        fprintf (file, "    \"%s\" -> \"%s\" [label=\"Нет\" , color = red];\n", node->data, node->right->data);
        GenerateDot (node->right, file);
    }
}

void CreateDotFile (Tree_Node *root, const char *filename)
{
    FILE *file = fopen (filename, "w");
    if (!file) {
        fprintf (stderr, "PARTIA NE RAZRESHILA OPEN FILE.\n");
        return;
    }

    fprintf (file, "digraph Tree {\n");
    fprintf (file, "node [shape=record, style=filled, fillcolor=lightblue fontname=\"Arial\"];\n");
    GenerateDot (root, file);
    fprintf (file, "}\n");

    fclose (file);
}

void Display_Tree_With_Graphviz (Tree_Node *root)
{
    const char* dotFilename = "akinator.dot";
    const char* imageFilename = "akinator.bmp";

    CreateDotFile (root, dotFilename);
    system ("iconv -f CP1251 -t UTF-8 < akinator.dot > akinatoru.dot");

    char command[512] = "";
    snprintf (command, sizeof (command), "dot -Tpng akinatoru.dot  -o %s", imageFilename);
    system (command);
}

void Free_Tree (Tree_Node *node)
{
    if (!node) return;
    Free_Tree (node->left);
    Free_Tree (node->right);
    free (node);
}
