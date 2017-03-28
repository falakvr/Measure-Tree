/*
 * measure.c
 *
 *  Created on: Mar 27, 2017
 *      Author: falak
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define debug 0

struct m_tree_t {
	int key;
	int height;
	int leftmin;
	int rightmax;
	int l;
	int r;
	int measure;
	struct m_tree_t *left;
	struct m_tree_t *right;
};

struct listnode {
	int a;
	int b;
	struct listnode *next;
};

m_tree_t * create_m_tree() {
	m_tree_t *tmp_node;
	tmp_node = (m_tree_t *) malloc(sizeof(m_tree_t));
	tmp_node->key = 0;
	tmp_node->height = 0;
	tmp_node->leftmin = 0;
	tmp_node->rightmax = 0;
	tmp_node->l = -INFINITY;
	tmp_node->r = +INFINITY;
	tmp_node->measure = 0;
	tmp_node->left = NULL;
	tmp_node->right = NULL;

	return tmp_node;
}

int min(int a, int b) {
	if (a < b)
		return a;
	else
		return b;
}

int max(int a, int b) {
	if (a > b)
		return a;
	else
		return b;
}

void left_rotation(m_tree_t *n) {
	m_tree_t *tmp_node;
	int tmp_key;

	tmp_node = n->left;
	tmp_key = n->key;
	n->left = n->right;
	n->key = n->right->key;
	n->right = n->left->right;
	n->left->right = n->left->left;
	n->left->left = tmp_node;

	n->left->key = tmp_key;
	n->left->leftmin = min(n->left->left->leftmin, n->left->right->leftmin);
	n->left->rightmax = max(n->left->left->rightmax, n->left->right->rightmax);

	//TODO: Update measure
}

void right_rotation(m_tree_t *n) {
	m_tree_t *tmp_node;
	int tmp_key;

	tmp_node = n->right;
	tmp_key = n->key;
	n->right = n->left;
	n->key = n->left->key;
	n->left = n->right->left;
	n->right->left = n->right->right;
	n->right->right = tmp_node;

	n->right->key = tmp_key;
	n->right->leftmin = min(n->right->left->leftmin, n->right->right->leftmin);
	n->right->rightmax = max(n->right->left->rightmax, n->right->right->rightmax);

	//TODO: Update Measure
}

