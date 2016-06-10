/*
Author: Kyle Holmberg
Duck ID: kmh
SIN: 951 312 729
Project 0 for CIS 415 - Operating Systems
tldlist.c

This is my own work with the exception of utilizing the AVL Tree implementation

For the AVL Tree Implementation I took material from both of the following links:
http://geeksquiz.com/avl-with-duplicate-keys/
https://gist.github.com/tonious/1377768
 */

#include "tldlist.h"
#include "date.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* Didn't add functions to header because of submission requirements */
/* Function descriptions over function definitions */
static TLDNode *addtldnoder(TLDList *tld, char *tldInfo, TLDNode *node);
static TLDNode *newtldnode(char *tldInfo);
static void iteradder(TLDIterator *iter, TLDNode *node, int *i);
static int getheight(TLDNode *node);
static int balancefactor(TLDNode *node);
static TLDNode *rr(TLDNode *node);
static TLDNode *rl(TLDNode *node);
static TLDNode *ll(TLDNode *node);
static TLDNode *lr(TLDNode *node);
static TLDNode *balance(TLDNode *node);


struct tldlist {
    Date *begin;
    Date *end;
    TLDNode *root;
    long count;
    long size;
};


struct tldnode {
    TLDNode *lchild;
    TLDNode *rchild;
    char *tldInfo;
    long count;
};


struct tlditerator {
    TLDList *tld;
    TLDNode **next;
    int i;
    long size;
};


TLDList *tldlist_create(Date *begin, Date *end) {
	if (begin == NULL || end == NULL) {
        return NULL; //If either date is had a memory issue, return NULL
    }

    TLDList *tldlist = malloc(sizeof(TLDList));

    if (tldlist == NULL) {
        return NULL; //Not enough memory to create TDL List (AVL Tree Structure)
    }

    tldlist->count = 0;
    tldlist->size = 0;
    tldlist->root = NULL;
    tldlist->begin = begin;
    tldlist->end = end;

    return tldlist;
}

int tldlist_add(TLDList *tld, char *hostname, Date *d) {
    // Check if TLD access date is within date being checked
    if (date_compare(tld->begin, d) > 0 || date_compare(tld->end, d) < 0) {
        return 0; //failed to add to count
    }

    // Grab TLD from hostname
    char *suffix = strrchr(hostname, '.') + 1; //find last occurence of '.' in hostname to grab TLD
    int i = 0;
    while (i < sizeof(suffix)) {
        suffix[i] = tolower(suffix[i]); //case-insenitivity created
        i++;
    }

    // Prepare TLD of access to be added to TLDList
    char *tldInfo = (char *)malloc(sizeof(suffix));
    strcpy(tldInfo, suffix);
    tld->root = addtldnoder(tld, tldInfo, tld->root);
    tld->count++;

    return 1; //confirm added count
}

long tldlist_count(TLDList *tld) {
    return tld->count;
}

TLDIterator *tldlist_iter_create(TLDList *tld) {
    TLDIterator *iter = (TLDIterator *)malloc(sizeof(TLDIterator));

    // If the iterator is empty - clear it's memory allocation
    if (iter == NULL) {
        free(iter);
        return NULL;
    } else { //Iterator isn't empty, so 
        iter->tld = tld;
        iter->size = tld->size;
        iter->i = 0;

        iter->next = (TLDNode **)malloc(sizeof(TLDNode *) * iter->size);

        // Destroy memory allocation for current iteration if next points to something
        if (iter->next == NULL) {
            tldlist_iter_destroy(iter);
            return NULL;
        }

        int i = 0; 
        iteradder(iter, iter->tld->root, &i);
        return iter;
    }
}

TLDNode *tldlist_iter_next(TLDIterator *iter) {
    if (iter->i == iter->size) {
        return NULL;
    }

    return *(iter->next + iter->i++);
}

void tldlist_iter_destroy(TLDIterator *iter) {
	int i = 0;

	while (i < iter->size) {
	    free(iter->next[i]->tldInfo);
	    free(iter->next[i]);
	    i++;
	}

    free(iter->next);
    free(iter);
}

char *tldnode_tldname(TLDNode *node) {
    return node->tldInfo;
}

long tldnode_count(TLDNode *node) {
    return node->count;
}

void tldlist_destroy(TLDList *tld) {
    free(tld);
}

// addtldnoder creates a node for the TLD or adds to the count of a node that already holds the information for that TLD
static TLDNode *addtldnoder(TLDList *tld, char *tldInfo, TLDNode *node) {
    if (node == NULL) { //if node is empty, use newtldnode() because this is the first time weve seen this TLD
        node = newtldnode(tldInfo);
        tld->root = node;
        tld->size++;
        return node;
    } else if (strcmp(tldInfo, node->tldInfo) > 0) { //node isn't empty, but the tld included is alphabetically later than the tldInfo on the node being looked at
        node->rchild = addtldnoder(tld, tldInfo, node->rchild);
        node = balance(node);
    } else if (strcmp(tldInfo, node->tldInfo) < 0) { //node isn't empty, but the tld included is alphabetically sooner than the tldInfo on the node being looked at
        node->lchild = addtldnoder(tld, tldInfo, node->lchild);
        node = balance(node);
    } else { //node isn't empty, and the tld included is the same as the node being looked at... add to count
        free(tldInfo);
        node->count++;
    }

    return node;
}

// newtldnode adds a node whose TLD is not yet contained in a node on the AVL Tree
static TLDNode *newtldnode(char *tldInfo) {
    TLDNode *node = (TLDNode *)malloc(sizeof(TLDNode));
    if (node == NULL) {
		free(node);
		return NULL;
	} else {
    node->tldInfo = tldInfo;
    node->lchild = NULL;
    node->rchild = NULL;
    node->count = 1;
    return node;
	}
}

// iteradder adds an iteration pointer to the chain
static void iteradder(TLDIterator *iter, TLDNode *node, int *i) {
    if (node->lchild) { //if it has a lchild child...
        iteradder(iter, node->lchild, i);
    }

    *(iter->next + (*i)++) = node;

    if (node->rchild) {
        iteradder(iter, node->rchild, i);
    }
}

// getheight determines the height of the AVL Tree
static int getheight(TLDNode *node) {
    int heightval = 0;

    if (node != NULL) {
    	int lheight;
	    int rheight;
	    int last;
        lheight = getheight(node->lchild);
        rheight = getheight(node->rchild);
        if (lheight > rheight) {
            last = lheight;
        } else {
            last = rheight;
        } heightval = 1+last;
    }

    return heightval;
}

// balancefactor is called by balance and it returns the the height difference of a node's children
static int balancefactor(TLDNode *node) {
    int heightdiff;
    int lheight;
    int rheight;

    lheight = getheight(node->lchild);
    rheight = getheight(node->rchild);
    heightdiff = lheight - rheight;

    return heightdiff;
}

// ll does a left-left rotation on a specific node in the AVL Tree Structure
static TLDNode *ll(TLDNode *node) {
	TLDNode *a = node;
	TLDNode *b = a->lchild;
	a->lchild = b->rchild;
	b->rchild = a;

	return b;
}

// lr does a left-right rotation on a specific node in the AVL Tree Structure
static TLDNode *lr(TLDNode *node) {
	TLDNode *a = node;
	TLDNode *b = a->lchild;
	TLDNode *c = b->rchild;

	a->lchild = c->rchild;
	b->rchild = c->lchild;
	c->lchild = b;
	c->rchild = a;

	return c;
}

// rl does a right-left rotation on a specific node in the AVL Tree Structure
static TLDNode *rl(TLDNode *node) {
	TLDNode *a = node;
	TLDNode *b = a->rchild;
	TLDNode *c = b->lchild;

	a->rchild = c->lchild;
	b->lchild = c->rchild;
	c->rchild = b;
	c->lchild = a;

	return c;
}

// rr does a right-right rotation on a specific node in the AVL Tree Structure
static TLDNode *rr(TLDNode *node) {
	TLDNode *a = node;
	TLDNode *b = a->rchild;

	a->rchild = b->lchild;
	b->lchild = a;

	return b;
}

// balance checks the AVL Tree Structure in place for unbalance (any subtree with a height +2> its opposite side)
// If unbalanced, calls appropriate rotations
static TLDNode *balance(TLDNode *node) {
    int balf = balancefactor(node);
    if (balf > 1) {
        if (balancefactor(node->lchild) > 0) {
            node = ll(node);
        } else {
            node = lr(node);
        }
    } else if (balf < -1) {
            if (balancefactor(node->rchild) > 0) {
                node = rl(node);
            } else {
                node = rr(node);
            }
        }
    return node;
}