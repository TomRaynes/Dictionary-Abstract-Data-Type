#include <stdbool.h>

typedef struct dict dict;

// Creates new dictionary
dict* dict_init(void);

/* Top of Dictionary = p, add word str. Return false if p or str is NULL,
 * or if word is already in the dictionary.True otherwise. */
bool dict_addword(dict* p, const char* wd);

/* The total number of nodes in the tree. */
int dict_nodecount(const dict* p);

/* Total number of times that any words have been added into
 * the tree, i.e., sum of freq count of all terminals.*/
int dict_wordcount(const dict* p);

/* Returns the dict* where str is marked as 'terminal', or else NULL.*/
dict* dict_spell(const dict* p, const char* str);

/* Frees all memory used by dictionary p. Sets the original pointer back to NULL */
void dict_free(dict** p);

/* Returns number of times most common word in dictionary has been added */
int dict_mostcommon(const dict* p);

/* For two nodes, count the nodes that separate them */
unsigned dict_cmp(dict* p1, dict* p2);

/* For dictionary 'p', and word 'wd', find the path down to the most frequently used word
 * below this node, adding these letters to 'ret'. (In the event of ties, use the word that
 * comes first alphabetically). */
void dict_autocomplete(const dict* p, const char* wd, char* ret);
