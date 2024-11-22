#include "t27.h"

int get_char_val(char ch);
bool check_unique_then_add(dict* n, char* wd, int idx);
dict* create_node(void);
void free_tree(dict* p);

int main(void) {

    dict* d = dict_init();
    dict_addword(d, "car");

    dict_free(&d);
    return 0;
}

dict* dict_init(void) {

    dict* d = calloc(1, sizeof(dict));

    if (d == NULL) {
        fprintf(stderr, "ERROR: Memory Allocation Failure\n");
        exit(EXIT_FAILURE);
    }
    return d;
}

bool dict_addword(dict* p, const char* wd) {

    if (p == NULL || wd == NULL) {
        return false;
    }
    char* word = calloc(strlen(wd)+1, sizeof(char));
    strcpy(word, wd);

    for (int i=0; word[i]; i++) {

        if (!isalpha(word[i]) && word[i] != '\'') {
            return false;
        }
        word[i] = tolower(word[i]);
    }
    if (check_unique_then_add(p, word, 0)) {
        return true;
    }

    free(word);
    return false;
}

bool check_unique_then_add(dict* n, char* wd, int idx) {

    int pos = get_char_val(wd[idx]);

    if (n->dwn[pos] == NULL) {
        n->dwn[pos] = create_node();
        n->dwn[pos]->up = n;

        if (wd[idx+1] == '\0') {
            n->dwn[pos]->terminal = true;
            return true;
        }
        check_unique_then_add(n->dwn[pos], wd, idx+1);
        return true;
    }
    if (wd[idx+1] == '\0') {

        if (n->dwn[pos]->terminal == true) {
            n->dwn[pos]->freq++;
            return false;
        }
        n->dwn[pos]->terminal = true;
        return true;
    }
    if (check_unique_then_add(n->dwn[pos], wd, idx+1)) {
        return true;
    }
    return true;
}

void dict_free(dict** p) {

    if (p == NULL) {
        return;
    }
    if (*p == NULL) {
        free(p);
        return;
    }
    free_tree(*p);
    //free(p);
    p = NULL;
}

void free_tree(dict* p) {

    for (int i=0; i<ALPHA; i++) {

        if (p->dwn[i] != NULL) {
            free_tree(p->dwn[i]);
            free(p->dwn[i]);
            p->dwn[i] = NULL;
        }
    }
    //free(p);
    p = NULL;
}

dict* create_node(void) {

    dict* d = calloc(1, sizeof(dict));

    if (d == NULL) {
        fprintf(stderr, "ERROR: Memory Allocation Failure\n");
        exit(EXIT_FAILURE);
    }
    return d;
}

int get_char_val(char ch) {

    if (ch == '\'') {
        return ALPHA - 1;
    }
    return ch - 'a';
}




// void add_character(dict* n, char* wd, int idx) {
//
//     if (wd[idx+1] == '\0') {
//         n->terminal = true;
//         return;
//     }
//     int pos = get_char_val(wd[idx+1]);
//     n->dwn[pos] = calloc(1, sizeof(dict));
//
//     if (n->dwn[pos] == NULL) {
//         fprintf(stderr, "ERROR: Memory Allocation Failure\n");
//         exit(EXIT_FAILURE);
//     }
//
//     add_character(n->dwn[pos], wd, idx+1);
// }
