#include "t27.h"

char* check_word(const char* wd);
bool check_unique_then_add(dict* n, char* wd, int idx);
int get_char_val(char ch);
dict* create_node(void);
void free_tree(dict* p);
int get_child_counts(const dict* p, int count, int (*mode)(const dict* p));
dict* get_terminal(const dict* p, const char* str, int idx);
char* get_word(dict* p, int level, int* idx);
int count_shared_chars(char* word1, char* word2);
char get_character(dict* p);
dict* get_completion_node(const dict* p, const char* wd, int idx);
dict* calc_return_priority(dict* p1, dict* p2);

dict* dict_init(void) {

    dict* d = calloc(1, sizeof(dict));

    if (d == NULL) {
        fprintf(stderr, "ERROR: Memory Allocation Failure\n");
        exit(EXIT_FAILURE);
    }
    return d;
}

bool dict_addword(dict* p, const char* wd) {

    if (p == NULL || wd == NULL || strlen(wd) == 0) {
        return false;
    }
    char* word = check_word(wd);

    if (word == NULL) {
        return false;
    }
    if (check_unique_then_add(p, word, 0)) {
        free(word);
        return true;
    }
    free(word);
    return false;
}

char* check_word(const char* wd) {

    char* word = calloc(strlen(wd)+1, sizeof(char));

    if (word == NULL) {
        fprintf(stderr, "ERROR: Memory Allocation Failed\n");
        exit(EXIT_FAILURE);
    }
    strcpy(word, wd);

    for (int i=0; word[i]; i++) {

        if (!isalpha(word[i]) && word[i] != '\'') {
            free(word);
            return NULL;
        }
        word[i] = tolower(word[i]);
    }
    return word;
}

bool check_unique_then_add(dict* n, char* wd, int idx) {

    int pos = get_char_val(wd[idx]);

    if (n->dwn[pos] == NULL) { // TURN IF STATEMENT INTO SUB FUNCTION
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
        n->dwn[pos]->freq++;
        return true;
    }
    if (check_unique_then_add(n->dwn[pos], wd, idx+1)) {
        return true;
    }
    return false;
}

int get_char_val(char ch) {

    if (ch == '\'') {
        return ALPHA - 1;
    }
    return ch - 'a';
}

dict* create_node(void) {

    dict* d = calloc(1, sizeof(dict));

    if (d == NULL) {
        fprintf(stderr, "ERROR: Memory Allocation Failure\n");
        exit(EXIT_FAILURE);
    }
    return d;
}

void dict_free(dict** d) {

    if (d == NULL) {
        return;
    }
    if (*d == NULL) {
        free(d);
        return;
    }
    free_tree(*d);
    free(*d);
    *d = NULL;
    d = NULL;
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

int get_child_counts(const dict* p, int count, int (*mode)(const dict* p)) {

    for (int i=0; i<ALPHA; i++) {

        if (p->dwn[i] != NULL) {
            count += mode(p->dwn[i]);
        }
    }
    return count;
}

dict* dict_spell(const dict* p, const char* str) {

    if (p == NULL || str == NULL || strlen(str) == 0) {
        return NULL;
    }
    char* word = check_word(str);

    if (word == NULL) {
        return NULL;
    }
    dict* spell = get_terminal(p, word, 0);
    free(word);
    return spell;
}

dict* get_terminal(const dict* p, const char* str, int idx) {

    int pos = get_char_val(str[idx]);

    if (p->dwn[pos] == NULL) {
        return NULL;
    }
    if (str[idx+1] == '\0') {
        return p->dwn[pos]->terminal ? p->dwn[pos] : NULL;
    }
    return get_terminal(p->dwn[pos], str, idx+1);
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

// CHALLENGE1
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

    free(word1);
    free(word2);
    return branch1 + branch2;
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

int count_shared_chars(char* word1, char* word2) {

    int idx = 0;

    for (; word1[idx] && word2[idx]; idx++) {

        if (word1[idx] != word2[idx]) {
            return idx;
        }
    }
    return idx;
}

// CHALLENGE2
void dict_autocomplete(const dict* p, const char* wd, char* ret) {

    if (p == NULL || wd == NULL || ret == NULL) {
        return;
    }
    dict* most_freq = get_completion_node(p, wd, 0);

    if (most_freq == NULL) {
        strcpy(ret, "");
        return;
    }
    int idx = 0;
    char* word = get_word(most_freq, 1, &idx);
    int wd_len = strlen(wd);
    strcpy(ret, word+wd_len);

    // for (int i=0; i<wd_len; i++) {
    //     for (int j=0; word[j]; j++) {
    //         word[j] = word[j+1];
    //     }
    // }
    // strcpy(ret, word);
    free(word);
}

dict* get_completion_node(const dict* p, const char* wd, int idx) {

    if (idx < (int) strlen(wd)) {
        int pos = get_char_val(wd[idx]);
        return get_completion_node(p->dwn[pos], wd, idx+1);
    }
    dict* ret = idx > (int) strlen(wd) ? (dict*) p : NULL;
    dict* tmp = NULL;

    for (int i=0; i<ALPHA; i++) {

        if (p->dwn[i] != NULL) {
            tmp = get_completion_node(p->dwn[i], wd, idx+1);
        }
        if (tmp != NULL) {

            if (ret == NULL || tmp->freq > ret->freq) {
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

void test(void) {

}
