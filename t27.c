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

// TEST FUNCTIONS
void test_check_word(void);
void test_check_unique_then_add(void);
void test_get_char_val(void);
void test_create_node(void);
void test_free_functions(void);
void test_counting_functions(void);
void test_dict_spell_and_get_terminal(void);
void test_get_word_and_character(void);
void test_get_shared_chars(void);
void test_get_completion_node(void);

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
        //free(word);
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

        if (str == NULL) {
            fprintf(stderr, "ERROR: Memory Allocation Failure\n");
            exit(EXIT_FAILURE);
        }
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

    if (p == NULL || wd == NULL || ret == NULL  || strlen(wd) == 0) {
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

    test_check_word();
    test_check_unique_then_add();
    test_get_char_val();
    test_create_node();
    test_free_functions();
    test_counting_functions();
    test_dict_spell_and_get_terminal();
    test_get_word_and_character();
    test_get_shared_chars();
    test_get_completion_node();
}

void test_check_word(void) {

    // str argument is checked for NULL and empty str in parent function
    char* word = check_word(" ");
    assert(!word);
    word = check_word("test-");
    assert(!word);
    word = check_word("5test");
    assert(!word);
    word = check_word("!?@£$%&");
    assert(!word);

    word = check_word("test");
    assert(word);
    assert(strcmp(word, "test") == 0);
    free(word);
    word = check_word("TEST");
    assert(word);
    assert(strcmp(word, "test") == 0);
    free(word);
    word = check_word("test'S");
    assert(word);
    assert(strcmp(word, "test's") == 0);
    free(word);
    word = check_word("abcdefghijklmnopqrstuvwxyz'");
    assert(word);
    assert(strcmp(word, "abcdefghijklmnopqrstuvwxyz'") == 0);
    free(word);
    word = check_word("'ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    assert(word);
    assert(strcmp(word, "'abcdefghijklmnopqrstuvwxyz") == 0);
    free(word);
    word = check_word("t");
    assert(word);
    assert(strcmp(word, "t") == 0);
    free(word);
    word = check_word("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
                       "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
    assert(word);
    assert(strcmp(word, "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
                        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
                        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
                        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
                        "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx") == 0);
    free(word);
}

void test_check_unique_then_add(void) {

    // Only legal, lower-case strings are ever passed to function
    dict* d = dict_init();
    assert(check_unique_then_add(d, "bcdefg", 0));
    assert(check_unique_then_add(d, "cbdefg", 0));
    assert(check_unique_then_add(d, "x", 0));
    assert(check_unique_then_add(d, "qwertyuiopasdfghjklzxcvbnmqwertyuiopasdfghjklzxcvbnm", 0));
    assert(check_unique_then_add(d, "don't", 0));
    assert(check_unique_then_add(d, "dont", 0));
    assert(check_unique_then_add(d, "''''''''''", 0));
    assert(!check_unique_then_add(d, "bcdefg", 0));
    assert(!check_unique_then_add(d, "x", 0));
    assert(!check_unique_then_add(d, "qwertyuiopasdfghjklzxcvbnmqwertyuiopasdfghjklzxcvbnm", 0));
    assert(!check_unique_then_add(d, "''''''''''", 0));

    char str1[100] = "";
    for (int i=0; i<99; i++) {
        char str2[2] = "";
        str2[0] = i % 26 + 'a';
        strcat(str1, str2);
        assert(check_unique_then_add(d, str1, 0));
        assert(!check_unique_then_add(d, str1, 0));
    }
    dict_free(&d);
}

void test_get_char_val(void) {

    for (char ch='a'; ch<='z'; ch++) {
        assert(get_char_val(ch) == ch - 'a');
    }
    assert(get_char_val('\'') == 26);
}

void test_create_node(void) {

    dict* d = create_node();
    assert(d);
    free(d);
}

void test_free_functions(void) {

    dict* d = dict_init();

    char str1[100] = "";
    for (int i=0; i<99; i++) {
        char str2[2] = "";
        str2[0] = i % 26 + 'a';
        strcat(str1, str2);
        dict_addword(d, str1);
    }
    free_tree(d);
    for (int i=0; i<ALPHA; i++) {
        assert(d->dwn[i] == NULL);
    }
    dict_free(&d);
    assert(!d);

}

void test_counting_functions(void) {

    dict* d = dict_init();

    char str1[100] = "";
    for (int i=0; i<99; i++) {
        char str2[2] = "";
        str2[0] = i % 26 + 'a';
        strcat(str1, str2);
        dict_addword(d, str1);
        assert(dict_wordcount(d) == i + 1);
        assert(dict_nodecount(d) == i + 2);
    }
    dict_addword(d, "test");
    assert(dict_wordcount(d) == 100);
    assert(get_child_counts(d, 0, dict_wordcount) == 100);
    assert(dict_nodecount(d) == 104);
    assert(get_child_counts(d, 1, dict_nodecount) == 104);
    dict_addword(d, "TEST");
    assert(dict_wordcount(d) == 101);
    assert(get_child_counts(d, 0, dict_wordcount) == 101);
    assert(dict_nodecount(d) == 104);
    assert(get_child_counts(d, 1, dict_nodecount) == 104);
    dict_addword(d, "testing");
    assert(dict_wordcount(d) == 102);
    assert(get_child_counts(d, 0, dict_wordcount) == 102);
    assert(dict_nodecount(d) == 107);
    assert(get_child_counts(d, 1, dict_nodecount) == 107);
    dict_free(&d);
}

void test_dict_spell_and_get_terminal(void) {

    dict* d = dict_init();
    assert(dict_spell(d, "test") == NULL);
    assert(get_terminal(d, "test", 0) == NULL);
    dict_addword(d, "test");
    dict_addword(d, "AAAAA");
    dict_addword(d, "abcdefghijklmnopqrstuvwxyz");
    dict_addword(d, "X'X'X'X");
    dict_addword(d, "'''''");

    dict* s = dict_spell(d, "test");
    assert(get_character(s) == 't');
    s = get_terminal(d, "test", 0);
    assert(get_character(s) == 't');
    s = dict_spell(d, "aaaaa");
    assert(get_character(s) == 'a');
    s = get_terminal(d, "aaaaa", 0);
    assert(get_character(s) == 'a');
    s = dict_spell(d, "abcdefghijklmnopqrstuvwxyz");
    assert(get_character(s) == 'z');
    s = get_terminal(d, "abcdefghijklmnopqrstuvwxyz", 0);
    assert(get_character(s) == 'z');
    s = dict_spell(d, "x'x'x'x");
    assert(get_character(s) == 'x');
    s = get_terminal(d, "x'x'x'x", 0);
    assert(get_character(s) == 'x');
    s = dict_spell(d, "'''''");
    assert(get_character(s) == '\'');
    s = get_terminal(d, "'''''", 0);
    assert(get_character(s) == '\'');
    dict_free(&d);
}

void test_get_word_and_character(void) {

    dict* d = dict_init();

    dict_addword(d, "test");
    dict_addword(d, "AAAAAAAAAA");
    dict_addword(d, "qwertyuiopasdfghjklzxcvbnmqwertyuiopasdfghjklzxcvbnm");
    dict_addword(d, "abcdefg'");
    dict_addword(d, "''''''''''");

    int idx = 0; dict* p = dict_spell(d, "test");
    char* str = get_word(p, 1, &idx);
    assert(strcmp(str, "test") == 0);
    assert(get_character(p) == 't');

    idx = 0; p = dict_spell(d, "aaaaaaaaaa");
    str = get_word(p, 1, &idx);
    assert(strcmp(str, "aaaaaaaaaa") == 0);
    assert(get_character(p) == 'a');
    char str2[53] = "qwertyuiopasdfghjklzxcvbnmqwertyuiopasdfghjklzxcvbnm";
    idx = 0; p = dict_spell(d, str2);
    str = get_word(p, 1, &idx);
    assert(strcmp(str, str2) == 0);
    assert(get_character(p) == 'm');
    idx = 0; p = dict_spell(d, "abcdefg'");
    str = get_word(p, 1, &idx);
    assert(strcmp(str, "abcdefg'") == 0);
    assert(get_character(p) == '\'');
    idx = 0; p = dict_spell(d, "''''''''''");
    str = get_word(p, 1, &idx);
    assert(strcmp(str, "''''''''''") == 0);
    assert(get_character(p) == '\'');

    char str3[100] = "";
    for (int i=0; i<99; i++) {
        char str4[2] = "";
        str4[0] = i % 26 + 'a'; idx = 0;
        strcat(str3, str4);
        dict_addword(d, str3); p = dict_spell(d, str3);
        str = get_word(p, 1, &idx);
        assert(strcmp(str, str3) == 0);
        assert(get_character(p) == str4[0]);
    }
    dict_free(&d);
}

void test_get_shared_chars(void) {

    assert(count_shared_chars("test", "test") == 4);
    assert(count_shared_chars("test", "tests") == 4);
    assert(count_shared_chars("abcdefghijklmnopqrstuvwxyz", "abcdefghijklmnpqrstuvwxyz") == 14);
    assert(count_shared_chars("xxxxxxx'x", "xxxxxxxx") == 7);
    assert(count_shared_chars("q", "qwertyuiopasdfghjklzxcvbnmqwertyuiopasdfghjklzxcvbnm") == 1);
}

void test_get_completion_node(void) {

    dict* d = dict_init();

    dict_addword(d, "aaa");
    dict_addword(d, "abb");
    dict_addword(d, "abb");
    assert(get_completion_node(d, "a", 0) == dict_spell(d, "abb"));
    dict_addword(d, "acc");
    dict_addword(d, "acc");
    dict_addword(d, "acc");
    assert(get_completion_node(d, "a", 0) == dict_spell(d, "acc"));
    dict_addword(d, "a'");
    dict_addword(d, "a'");
    dict_addword(d, "a'");
    dict_addword(d, "a'");
    assert(get_completion_node(d, "a", 0) == dict_spell(d, "a'"));
    char str[] = "qwertyuiopasdfghjklzxcvbnm'qwertyuiopasdfghjklzxcvbnm'";
    for (int i=0; i<5; i++) {
        dict_addword(d, str);
    }
    assert(get_completion_node(d, "qwerty", 0) == dict_spell(d, str));
    for (int i=0; i<6; i++) {
        dict_addword(d, "abbbb");
        dict_addword(d, "acccc");
    }
    dict* s1 = dict_spell(d, "abbbb"); dict* s2 = dict_spell(d, "acccc");
    assert(get_completion_node(d, "a", 0) == s1);
    assert(calc_return_priority(s1, s2) == s1);
    char str2[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    for (int i=0; i<7; i++) {
        dict_addword(d, str2);
        dict_addword(d, "ab");
    }
    s1 = dict_spell(d, str2); s2 = dict_spell(d, "ab");
    assert(get_completion_node(d, "a", 0) == dict_spell(d, str2));
    assert(calc_return_priority(s1, s2) == s1);
    dict_addword(d, "abcdefghijklmnopqrstuvwxyz'"); s1 = dict_spell(d, "abcdefghijklmnopqrstuvwxyz'");
    dict_addword(d, "abcdefghijklmnopqrstuvwxyz"); s2 = dict_spell(d, "abcdefghijklmnopqrstuvwxyz");
    assert(calc_return_priority(s1, s2) == s2);
    dict_addword(d, "aaaaaaaaaa"); s1 = dict_spell(d, "aaaaaaaaaa");
    dict_addword(d, "'"); s2 = dict_spell(d, "'");
    assert(calc_return_priority(s1, s2) == s2);
    dict_addword(d, "'"); s1 = dict_spell(d, "'");
    dict_addword(d, "''"); s2 = dict_spell(d, "''");
    assert(calc_return_priority(s1, s2) == s1);
    dict_free(&d);
}
