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
	tmp_node = (m_tree_t *) malloc (sizeof(m_tree_t));
	tmp_node->key = 0;
	tmp_node->height = 0;
	tmp_node->leftmin = 0;
	tmp_node->rightmax = 0;
	tmp_node->l = -INFINITY;


}













