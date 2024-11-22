#include "Akinator.h"





int main ()
{
    const char* filename = "akinator.txt";

    FILE *file = fopen (filename, "r");
    if (!file)
    {
        printf ("Не удалось открыть файл 'akinator.txt'.\n");
        return 1;
    }

    Tree_Node *root = Build_Tree (file, NULL);
    fclose (file);

    int choice = 0;
    while (1)
    {
        printf ("Выберите действие:\n");
        printf ("1) Отгадать\n");
        printf ("2) Сравнить объекты\n");
        printf ("3) Показать дерево\n");
        printf ("4) Определение объекта\n");
        printf ("5) Выход\n");
        printf ("6) Выход с сохранением\n");
        printf ("Ваш выбор: ");
        scanf (" %d", &choice);

        char ch = 0;
        while ((ch = getchar()) != '\n');

        switch (choice)
        {
        case 1:
            Play_Akinator (root);
            break;

        case 2:
            Compare_Objects (root);
            break;

        case 3:
            Display_Tree_With_Graphviz (root);
            break;

        case 4:
            Define_Object (root);
            break;

        case 5:
            Free_Tree (root);
            printf ("Выход из программы.\n");
            return 0;

        case 6:
        {
            FILE *file = fopen (filename, "w");

            if (!file) {

                printf ("Не удалось открыть файл '%s' для записи.\n", filename);

            } else {

                Save_Tree_To_File (root, file);
                fclose (file);
                printf ("База данных сохранена в файл '%s'.\n", filename);
            }

            Free_Tree (root);
            printf ("Выход из программы.\n");
            return 0;
        }

        default:
                printf ("Некорректный ввод. Попробуйте снова.\n");
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

        Tree_Node* newNode = (Tree_Node*) calloc (1, sizeof (Tree_Node));
        strcpy (newNode->data, newObject);

        Tree_Node* oldNode = node;

        Tree_Node* questionNode = (Tree_Node*) calloc (1, sizeof (Tree_Node));
        strcpy (questionNode->data, question);

        questionNode->left = newNode;
        questionNode->right = oldNode;
        newNode->parent = oldNode;


        questionNode->parent = node->parent;
        if (node->parent) {
            if (node->parent->left == node) {

                node->parent->left = questionNode;

            } else {

                node->parent->right = questionNode;
            }
        }

        printf ("&parent = %p , &data = %p , data = %s" , newNode->parent , &newNode->data , newNode->data);

        node = questionNode;

    } else {

        printf ("Некорректный ответ. Попробуйте снова.\n");
    }
}


Tree_Node* Find_Node (Tree_Node* node, const char* name)
{
    if (!node) {
        return NULL;
    }

    // printf("Проверяется узел: %s\n", node->data);

    if (stricmp (node->data, name) == 0) {

        // printf("Найден узел: %s\n", node->data);
        return node;
    }

    Tree_Node* foundNode = Find_Node (node->left, name);

    if (foundNode) {

        return foundNode;
    }

    return Find_Node (node->right, name);
}


void Remove_New_line (char* str)
{
    char* newline = strchr (str, '\n');
    if (newline) {
        *newline = '\0';
    }
}


void Get_Object_Definition (Tree_Node* node, const char* name)
{
    if (!node) {
        printf("Объект не найден в дереве.\n");
        return;
    }

    Tree_Node* Name_Node = Find_Node (node, name);
    if (!Name_Node) {
        printf("Объект '%s' не найден в дереве.\n", name);
        return;
    }

    Tree_Node* path[256] = {};
    int pathSize = 0;
    node = Name_Node;
    while (node) {

        path[pathSize++] = node;
        node = node->parent;
    }

    printf("Определение объекта: ");
    printf ("Это - ");
    // printf ("%s" , path[0]->data);
    for (int i = pathSize - 1; i > 0; i--) {

        Remove_New_line (path[i]->data);

            if (path[i - 1] == path[i]->right) {

                printf ("не ");
            }
        printf("%s, ", path[i]->data);

    }
    printf ("\n");

}



void Define_Object (Tree_Node* root)
{
    char objectName[256] = "";
    printf ("Введите имя объекта для определения: ");
    scanf ("%s", objectName);

    Tree_Node* node = Find_Node (root, objectName);
    printf ("\"%s\" " , node->data);
    Get_Object_Definition (node , objectName);
}


Tree_Node* Find_Common_Ancestor (Tree_Node *node1, Tree_Node *node2)
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


void Compare_Objects (Tree_Node* root)
{
    char obj1[256] = "", obj2[256] = "";
    printf ("Введите первый объект: ");
    scanf ("%s", obj1);
    printf ("Введите второй объект: ");
    scanf ("%s", obj2);

    Tree_Node *node1 = Find_Node (root, obj1);
    Tree_Node *node2 = Find_Node (root, obj2);

    if (!node1 || !node2) {
        printf ("Один или оба объекта не найдены в дереве.\n");
        return;
    }

    Tree_Node *commonAncestor = Find_Common_Ancestor (node1, node2);
    if (!commonAncestor) {
        printf ("Общий родитель не найден.\n");
        return;
    }

    printf ("Общий родитель: %s\n", commonAncestor->data);

    Tree_Node *path1[256] = {};
    Tree_Node *path2[256] = {};
    int path1Size = 0, path2Size = 0;

    Tree_Node* current1 = node1;
    while (current1 && current1 != commonAncestor) {

        path1[path1Size++] = current1;
        current1 = current1->parent;
    }

    Tree_Node* current2 = node2;
    while (current2 && current2 != commonAncestor) {

        path2[path2Size++] = current2;
        current2 = current2->parent;
    }

    printf ("Но %s: ", obj1);
    for (int i = path1Size - 1; i > 0; i--) {

        Remove_New_line (path1[i]->data);
        if (path1[i]->right == path1[i - 1]) {
            printf("не ");
        }
        printf ("%s", path1[i]->data);
        if (i > 1) {
            printf (" -> ");
        }
    }

    printf ("\n");

    printf ("А %s: ", obj2);
    for (int i = path2Size - 1; i > 0; i--) {

        Remove_New_line (path2[i]->data);

        if (path2[i]->right == path2[i - 1]) {
            printf ("не ");
        }

        printf ("%s", path2[i]->data);
        if (i > 1) {
            printf (" -> ");
        }
    }
    printf ("\n");
}



void Save_Tree_To_File (Tree_Node *node, FILE *file)
{
    if (!node) {
        return;
    }

    fprintf (file, "{");

    fprintf (file, "%s", node->data);

    if (node->left || node->right) {

        Save_Tree_To_File (node->left, file);
        Save_Tree_To_File (node->right, file);
    }

    fprintf (file, "}");
}


void Generate_Dot(Tree_Node *node, FILE *file)
{
    if (node == NULL) {
        return;
    }

    if (node->left && node->right)
        fprintf (file, "    \"%s\" [label=\"{ Адрес: %p |  %s  | left: %p | right: %p }\"",
                node->data, node, node->data, node->left, node->right);
    else
        fprintf (file, "    \"%s\" [label=\"{ Адрес: %p |  %s  | left: NULL | right: NULL }\"",
                node->data, node, node->data);

    if (node->parent == NULL) {
        fprintf (file, ", fillcolor=yellow");
    }

    fprintf(file, "];\n");

    if (node->left) {

        fprintf (file, "    \"%s\" -> \"%s\" [label=\"Да\", color=green];\n", node->data, node->left->data);
        Generate_Dot (node->left, file);
    }

    if (node->right) {

        fprintf (file, "    \"%s\" -> \"%s\" [label=\"Нет\", color=red];\n", node->data, node->right->data);
        Generate_Dot (node->right, file);
    }
}




void Create_Dot_File (Tree_Node *root, const char *filename)
{
    FILE *file = fopen (filename, "w");
    if (!file) {
        fprintf (stderr, "PARTIA NE RAZRESHILA OPEN FILE.\n");
        return;
    }

    fprintf (file, "digraph Tree {\n");
    fprintf (file, "node [shape=record, style=filled, fillcolor=lightblue fontname=\"Arial\"];\n");
    Generate_Dot (root, file);
    fprintf (file, "}\n");

    fclose (file);
}

void Display_Tree_With_Graphviz (Tree_Node *root)
{
    const char* dotFilename = "akinator.dot";
    const char* imageFilename = "akinator.png";

    Create_Dot_File (root, dotFilename);
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
