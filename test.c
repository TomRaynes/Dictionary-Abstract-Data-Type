#include "t27.h"

int get_char_val(char ch);
bool check_unique_then_add(dict* n, char* wd, int idx);
dict* create_node(void);
void free_tree(dict* p);
int get_child_counts(const dict* p, int count, int (*mode)(const dict* p));
dict* get_terminal(const dict* p, const char* str, int idx);
char get_character(dict* p);
char* get_word(dict* p, int level, int* idx);
int count_shared_chars(char* word1, char* word2);
dict* calc_return_priority(dict* p1, dict* p2);
dict* get_completion_node(const dict* p, const char* wd, int idx);

int main(void) {

    dict* d = dict_init();
    // assert(dict_addword(d, "car"));
    // assert(dict_addword(d, "carp"));
    // assert(dict_addword(d, "cart"));
    dict_addword(d, "carting");
    dict_addword(d, "carter");
    dict_addword(d, "carting");
    dict_addword(d, "carted");
    dict_addword(d, "carting");
    dict_addword(d, "cart");
    dict_addword(d, "carting");
    dict_addword(d, "car");
    dict_addword(d, "carting");

    char str[50];
    dict_autocomplete(d, "car", str);
    printf("CMP = %s\n", str);
    //assert(strcmp(str, "arting")==0);
    dict_autocomplete(d, "carte", str);
    // Alphabetically carted not carter
    assert(strcmp(str, "d")==0);


    printf("WORD COUNT = %i\n", dict_wordcount(d));
    printf("NODE COUNT = %i\n", dict_nodecount(d));

    // dict* n = dict_spell(d, "cart");
    // assert(n->up->dwn[get_char_val('t')] != NULL);
    //assert(dict_mostcommon(d)==2);

    dict* q1 = dict_spell(d, "cart");
    dict* q2 = dict_spell(d, "cart");
    dict_cmp(q1, q2);



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

void dict_autocomplete(const dict* p, const char* wd, char* ret) {

    if (p == NULL || wd == NULL || ret == NULL) {
        return;
    }
    dict* most_freq = get_completion_node(p, wd, 0);
    int idx = 0;
    char* word = get_word(most_freq, 1, &idx);
    int wd_len = strlen(wd);

    for (int i=0; i<wd_len; i++) {
        for (int j=0; word[j]; j++) {
            word[j] = word[j+1];
        }
    }
    strcpy(ret, word);
    free(word);
}

dict* get_completion_node(const dict* p, const char* wd, int idx) {

    if (idx < (int) strlen(wd)) {
        int pos = get_char_val(wd[idx]);
        return get_completion_node(p->dwn[pos], wd, idx+1);
    }
    dict* ret = (dict*) p; //dict_init();

    for (int i=0; i<ALPHA; i++) {

        if (p->dwn[i] != NULL) {
            dict* tmp = get_completion_node(p->dwn[i], wd, idx);

            if (tmp->freq > ret->freq) {
                ret = tmp;
            }
            else if (tmp->freq == ret->freq && tmp->freq > 0) {
                ret = calc_return_priority(tmp, ret);
            }
        }
    }
    return ret;
}

dict* calc_return_priority(dict* p1, dict* p2) {

    int idx1 = 0, idx2 = 0;
    char* word1 = get_word(p1, 1, &idx1);
    char* word2 = get_word(p2, 1, &idx2);

    int priority = strcmp(word1, word2);
    free(word1);
    free(word2);
    return priority < 0 ? p1 : p2;
}

unsigned dict_cmp(dict* p1, dict* p2) {

    if (p1 == NULL || p2 == NULL) {
        return 0;
    }

    int idx1 = 0, idx2 = 0;
    char* word1 = get_word(p1, 1, &idx1);
    char* word2 = get_word(p2, 1, &idx2);
    int shared_chars = count_shared_chars(word1, word2);

    int branch1 = strlen(word1) - shared_chars;
    int branch2 = strlen(word2) - shared_chars;

    // printf("1 = %s, 2 = %s\n", word1, word2);
    // printf("CMP = %i\n", branch1 + branch2);
    // printf("B1 = %i, B2 = %i\n", branch1, branch2);
    // printf("shared = %i\n", shared_chars);

    free(word1);
    free(word2);
    return branch1 + branch2;
}

int count_shared_chars(char* word1, char* word2) {

    int idx = 0;

    for (; word1[idx] && word2[idx]; idx++) {

        if (word1[idx] != word2[idx]) {

            // if (word1[idx] == '\0' || word2[idx] == '\0') {
            //
            // }
            return idx;
        }
    }
    return idx;
}

char* get_word(dict* p, int level, int* idx) {

    if (p->up->up == NULL) {
        // level+1 to account for null terminator
        char* str = calloc(level+1, sizeof(char));
        str[(*idx)++] = get_character(p);
        return str;
    }
    char* str = get_word(p->up, level+1, idx);
    str[(*idx)++] = get_character(p);

    if (level == 0) {
        str[*idx] = '\0';
    }
    return str;
}

char get_character(dict* p) {

    for (int i=0; i<ALPHA; i++) {

        if (p->up->dwn[i] == p) {

            if (i == ALPHA-1) {
                return '\'';
            }
            return i + 'a';
        }
    }
    return '\0';
}

bool dict_addword(dict* p, const char* wd) {

    if (p == NULL || wd == NULL) {
        return false;
    }
    char* word = calloc(strlen(wd)+1, sizeof(char));
    strcpy(word, wd);

    for (int i=0; word[i]; i++) {

        if (!isalpha(word[i]) && word[i] != '\'') {
            free(word);
            return false;
        }
        word[i] = tolower(word[i]);
    }
    if (check_unique_then_add(p, word, 0)) {
        free(word);
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
            n->dwn[pos]->freq++;
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
    return false;
}

int get_child_counts(const dict* p, int count, int (*mode)(const dict* p)) {

    for (int i=0; i<ALPHA; i++) {

        if (p->dwn[i] != NULL) {
            count += mode(p->dwn[i]);
        }
    }
    return count;
}

int dict_wordcount(const dict* p) {

    if (p == NULL) {
        return 0;
    }
    int count = 0;

    if (p->terminal) {
        count += p->freq;
    }
    return get_child_counts(p, count, dict_wordcount);
}

int dict_nodecount(const dict* p) {

    if (p == NULL) {
        return 0;
    }
    int count = 1;

    return get_child_counts(p, count, dict_nodecount);
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
    free(*p);
    p = NULL;
}

dict* dict_spell(const dict* p, const char* str) {

    if (p == NULL || str == NULL) {
        return NULL;
    }
    return get_terminal(p, str, 0);
}

int dict_mostcommon(const dict* p) {

    if (p == NULL) {
        return 0;
    }
    int count = p->freq;

    for (int i=0; i<ALPHA; i++) {

        if (p->dwn[i] != NULL) {
            int child_count = dict_mostcommon(p->dwn[i]);
            count = child_count > count ? child_count : count;
        }
    }
    return count;
}

dict* get_terminal(const dict* p, const char* str, int idx) {

    int pos = get_char_val(str[idx]);

    if (str[idx+1] == '\0') {
        return p->dwn[pos]->terminal ? p->dwn[pos] : NULL;
    }
    if (p->dwn[pos] == NULL) {
        return NULL;
    }
    return get_terminal(p->dwn[pos], str, idx+1);

}

void free_tree(dict* p) {

    for (int i=0; i<ALPHA; i++) {

        if (p->dwn[i] != NULL) {
            free_tree(p->dwn[i]);
            free(p->dwn[i]);
            p->dwn[i] = NULL;
        }
    }
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
