/*
 * measure.cc
 *
 *  Created on: Mar 27, 2017
 *      Author: falak
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

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

struct interval {
	int a;
	int b;
	struct interval *next;
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

int update_measure(m_tree_t *n) {
	int measure;

	if (n->right == NULL) {
		measure = (min(n->r, n->rightmax)) - (max(n->l, n->leftmin));
	} else {
		if ((n->right->leftmin < n->l) && (n->left->rightmax >= n->r)) {
			measure = n->r - n->l;
		} else if ((n->right->leftmin >= n->l) && (n->left->rightmax >= n->r)) {
			measure = (n->r - n->key) + n->left->measure;
		} else if ((n->right->leftmin < n->l) && (n->left->rightmax < n->r)) {
			measure = n->right->measure + (n->key - n->l);
		} else if ((n->right->leftmin >= n->l) && (n->left->rightmax < n->r)) {
			measure = n->right->measure + n->left->measure;
		}
	}
	return measure;
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
	n->left->l = n->l;
	n->left->r = n->key;
	n->left->leftmin = min(n->left->left->leftmin, n->left->right->leftmin);
	n->left->rightmax = max(n->left->left->rightmax, n->left->right->rightmax);

	n->left->measure = update_measure(n->left);
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
	n->right->l = n->key;
	n->right->r = n->r;
	n->right->leftmin = min(n->right->left->leftmin, n->right->right->leftmin);
	n->right->rightmax = max(n->right->left->rightmax,
			n->right->right->rightmax);

	n->right->measure = update_measure(n->right);
}

interval * create_interval(int a, int b) {
	interval * new_interval;
	new_interval = (interval *) malloc(sizeof(interval));
	new_interval->a = min(a, b);
	new_interval->b = max(a, b);
	new_interval->next = NULL;

	return new_interval;
}

void insert(m_tree_t * tree, int new_key, int endpoint) {
	m_tree_t *tmp_node = tree;
	interval * new_interval;
	int finished = 0;
	if (tmp_node->left == NULL) {
		new_interval = create_interval(new_key, endpoint);
		tmp_node->left = (m_tree_t *) new_interval;
		tmp_node->key = new_key;
		tmp_node->height = 0;
		tmp_node->leftmin = new_interval->a;
		tmp_node->rightmax = new_interval->b;
		tmp_node->measure = update_measure(tmp_node);
		tmp_node->right = NULL;
	} else {
		m_tree_t * path_stack[100];
		int path_st_p = 0;
		while (tmp_node->right != NULL) {
			path_stack[path_st_p++] = tmp_node;
			if (new_key < tmp_node->key)
				tmp_node = tmp_node->left;
			else
				tmp_node = tmp_node->right;
		}

		// Node identified, now insert the interval based on 3 cases

		// Case 1 where new interval is added as a new node in the existing lined list

		if (tmp_node->key == new_key) {

			interval *old_interval = (interval *) tmp_node->left;
			while (old_interval->next != NULL) {
				old_interval = old_interval->next;
			}
			new_interval = create_interval(new_key, endpoint);
			old_interval->next = new_interval;

			//Update leftmin, rightmax, and measure of tmp_node
			//Traverse the linked list of intervals and update

			int min, max;
			min = tmp_node->leftmin;
			max = tmp_node->rightmax;
			old_interval = (interval *) tmp_node->left;
			while (old_interval != NULL) {
				if (old_interval->a < min) {
					min = old_interval->a;
				}
				if (old_interval->b > max) {
					max = old_interval->b;
				}
				old_interval = old_interval->next;
			}
			tmp_node->leftmin = min;
			tmp_node->rightmax = max;
			tmp_node->measure = update_measure(tmp_node);

		} else {

			// Cases 2 and 3

			m_tree_t *old_leaf, *new_leaf;
			old_leaf = create_m_tree();

			// Update all the variables of old leaf

			old_leaf->left = tmp_node->left;
			old_leaf->key = tmp_node->key;
			old_leaf->height = 0;

			old_leaf->leftmin = tmp_node->leftmin;
			old_leaf->rightmax = tmp_node->rightmax;
			old_leaf->right = NULL;

			new_leaf = create_m_tree();

			//Update all the variables of new_leaf

			new_interval = create_interval(new_key, endpoint);
			new_leaf->left = (m_tree_t *) new_interval;
			new_leaf->key = new_key;
			new_leaf->height = 0;
			new_leaf->leftmin = new_interval->a;
			new_leaf->rightmax = new_interval->b;
			new_leaf->right = NULL;

			if (tmp_node->key < new_key) {

				//case 2

				tmp_node->left = old_leaf;
				tmp_node->right = new_leaf;
				tmp_node->key = new_key;
				//update new_leaf's and old_leaf's l, r and measure
				new_leaf->l = tmp_node->key;
				new_leaf->r = tmp_node->r;
				new_leaf->measure = update_measure(new_leaf);

				old_leaf->l = tmp_node->l;
				old_leaf->r = tmp_node->key;
				old_leaf->measure = update_measure(old_leaf);

			} else {

				// case 3

				tmp_node->left = new_leaf;
				tmp_node->right = old_leaf;
				//update new leaf's and old leaf's l, r and measure
				new_leaf->l = tmp_node->l;
				new_leaf->r = tmp_node->key;
				new_leaf->measure = update_measure(new_leaf);

				old_leaf->l = tmp_node->key;
				old_leaf->r = tmp_node->r;
				old_leaf->measure = update_measure(old_leaf);

			}
			tmp_node->height = 1;

			//Update leftmin, rightmax, and measure of tmp_node

			tmp_node->leftmin = min(tmp_node->left->leftmin,
					tmp_node->right->leftmin);
			tmp_node->rightmax = max(tmp_node->left->rightmax,
					tmp_node->right->rightmax);
			tmp_node->measure = update_measure(tmp_node);
		}
		tmp_node->measure = update_measure(tmp_node);

		/* update variables of all the nodes in the stack */

		int path_st_p2 = path_st_p;

		while (path_st_p2 > 0) {
			tmp_node = path_stack[--path_st_p2];
			tmp_node->leftmin = min(tmp_node->left->leftmin,
					tmp_node->right->leftmin);
			tmp_node->rightmax = max(tmp_node->left->rightmax,
					tmp_node->right->rightmax);
			tmp_node->measure = update_measure(tmp_node);
		}

		/* rebalance */

		finished = 0;
		while (path_st_p > 0 && !finished) {
			int tmp_height, old_height;
			tmp_node = path_stack[--path_st_p];
			old_height = tmp_node->height;
			if (tmp_node->left->height - tmp_node->right->height == 2) {
				if (tmp_node->left->left->height - tmp_node->right->height
						== 1) {
					right_rotation(tmp_node);
					tmp_node->right->height = tmp_node->right->left->height + 1;
					tmp_node->height = tmp_node->right->height + 1;
				} else {
					left_rotation(tmp_node->left);
					right_rotation(tmp_node);
					tmp_height = tmp_node->left->left->height;
					tmp_node->left->height = tmp_height + 1;
					tmp_node->right->height = tmp_height + 1;
					tmp_node->height = tmp_height + 2;
				}
			} else if (tmp_node->left->height - tmp_node->right->height == -2) {
				if (tmp_node->right->right->height - tmp_node->left->height
						== 1) {
					left_rotation(tmp_node);
					tmp_node->left->height = tmp_node->left->right->height + 1;
					tmp_node->height = tmp_node->left->height + 1;
				} else {
					right_rotation(tmp_node->right);
					left_rotation(tmp_node);
					tmp_height = tmp_node->right->right->height;
					tmp_node->left->height = tmp_height + 1;
					tmp_node->right->height = tmp_height + 1;
					tmp_node->height = tmp_height + 2;
				}
			} else {
				/* update height even if there was no rotation */
				if (tmp_node->left->height > tmp_node->right->height)
					tmp_node->height = tmp_node->left->height + 1;
				else
					tmp_node->height = tmp_node->right->height + 1;
			}
			if (tmp_node->height == old_height)
				finished = 1;
		}
	}
}

void insert_interval(m_tree_t * tree, int a, int b) {
	if (a > b)
		return;
	else {
		insert(tree, a, b);
		insert(tree, b, a);
	}
}

int query_length(m_tree_t * tree) {
	return tree->measure;
}

void delete_int(m_tree_t *tree, int delete_key, int endpoint) {
	m_tree_t *tmp_node, *upper_node, *other_node;
	m_tree_t *super_node = NULL;
	tmp_node = NULL;
	upper_node = NULL;
	other_node = NULL;
	interval *deleted_object;
	int finished;
	if (tree->left == NULL) {
		return;
	} else if (tree->right == NULL) {
		if (tree->key == delete_key) {
			deleted_object = (interval *) tree->left;
			tree->left = NULL;
			tree->key = 0;
			tree->height = 0;
			tree->l = -INFINITY;
			tree->r = +INFINITY;
			tree->leftmin = 0;
			tree->rightmax = 0;
			tree->measure = 0;
			free(tree->left);
			return;
		} else
			return;
	} else {
		m_tree_t * path_stack[100];
		int path_st_p = 0;
		tmp_node = tree;
		while (tmp_node->right != NULL) {
			path_stack[path_st_p++] = tmp_node;

			upper_node = tmp_node;
			if (delete_key < tmp_node->key) {
				tmp_node = upper_node->left;
				other_node = upper_node->right;
			} else {
				tmp_node = upper_node->right;
				other_node = upper_node->left;
			}
		}
		if (tmp_node->key != delete_key) {

			deleted_object = NULL;
			return;

		} else {

			//Check if only one node in linked list or multiple nodes
			//Find the interval from the linked list

			interval *old_interval, *interval_to_delete, *prev_interval;
			old_interval = (interval *) tmp_node->left;
			prev_interval = NULL;
			interval_to_delete = NULL;
			int counter = 1;
			while (old_interval != NULL) {
				if ((old_interval->a == min(delete_key, endpoint))
						&& (old_interval->b == max(delete_key, endpoint))) {
					interval_to_delete = old_interval;
					break;
				}
				prev_interval = old_interval;
				old_interval = old_interval->next;
				counter++;
			}
			if (interval_to_delete == NULL) {
				return;
			} else {
				if ((counter == 1) && (interval_to_delete->next == NULL)) {
					//only 1 node in linked list

					upper_node->key = other_node->key;
					upper_node->left = other_node->left;
					upper_node->right = other_node->right;
					upper_node->height = other_node->height;
					upper_node->leftmin = other_node->leftmin;
					upper_node->rightmax = other_node->rightmax;
					deleted_object = (interval *) tmp_node->left;
					if (upper_node->right != NULL) {
						upper_node->right->r = upper_node->r;
						upper_node->left->l = upper_node->l;
					}

					upper_node->measure = other_node->measure;

					free(tmp_node->left);

					/* Find the immediate successor key if it exists and replace the upper node key */

					if (tree->key == delete_key) {
						super_node = tree->right;
						while (super_node->right != NULL) {
							super_node = super_node->left;
						}
						tree->key = super_node->key;
						tree->left->r = tree->key;
						tree->right->l = tree->key;
						tree->left->measure = update_measure(tree->left);
						tree->right->measure = update_measure(tree->right);
					}

				} else if ((counter == 1)
						&& (interval_to_delete->next != NULL)) {
					//first node in the linked list
					tmp_node->left = (m_tree_t *) interval_to_delete->next;
					deleted_object = (interval *) interval_to_delete;
					free(interval_to_delete);
					//Traverse linked list and update variables of tmp_node
					int min, max;
					old_interval = (interval *) tmp_node->left;
					min = old_interval->a;
					max = old_interval->b;
					while (old_interval != NULL) {
						if (old_interval->a < min) {
							min = old_interval->a;
						}
						if (old_interval->b > max) {
							max = old_interval->b;
						}
						old_interval = old_interval->next;
					}
					tmp_node->leftmin = min;
					tmp_node->rightmax = max;
					tmp_node->measure = update_measure(tmp_node);

				} else {
					//some node in the linked list
					prev_interval->next = interval_to_delete->next;
					deleted_object = (interval *) interval_to_delete;
					free(interval_to_delete);
					int min, max;
					old_interval = (interval *) tmp_node->left;
					min = old_interval->a;
					max = old_interval->b;
					while (old_interval != NULL) {
						if (old_interval->a < min) {
							min = old_interval->a;
						}
						if (old_interval->b > max) {
							max = old_interval->b;
						}
						old_interval = old_interval->next;
					}
					tmp_node->leftmin = min;
					tmp_node->rightmax = max;
					tmp_node->measure = update_measure(tmp_node);
				}
			}
		}



		/* update variables of all the nodes in the stack */
		int path_st_p2 = path_st_p;

		while (path_st_p2 > 0) {
			tmp_node = path_stack[--path_st_p2];
			if (tmp_node->right != NULL) {
				tmp_node->leftmin = min(tmp_node->left->leftmin,
						tmp_node->right->leftmin);
				tmp_node->rightmax = max(tmp_node->left->rightmax,
						tmp_node->right->rightmax);
				tmp_node->measure = update_measure(tmp_node);
			} else {
				int min, max;
				interval *old_interval;
				old_interval = (interval *) tmp_node->left;
				min = old_interval->a;
				max = old_interval->b;
				while (old_interval != NULL) {
					if (old_interval->a < min) {
						min = old_interval->a;
					}
					if (old_interval->b > max) {
						max = old_interval->b;
					}
					old_interval = old_interval->next;
				}
				tmp_node->leftmin = min;
				tmp_node->rightmax = max;
				tmp_node->measure = update_measure(tmp_node);
			}
		}

		/* rebalance */
		finished = 0;
		path_st_p -= 1;
		while (path_st_p > 0 && !finished) {
			int tmp_height, old_height;
			tmp_node = path_stack[--path_st_p];
			old_height = tmp_node->height;
			if (tmp_node->left->height - tmp_node->right->height == 2) {
				if (tmp_node->left->left->height - tmp_node->right->height
						== 1) {
					right_rotation(tmp_node);
					tmp_node->right->height = tmp_node->right->left->height + 1;
					tmp_node->height = tmp_node->right->height + 1;
				} else {
					left_rotation(tmp_node->left);
					right_rotation(tmp_node);
					tmp_height = tmp_node->left->left->height;
					tmp_node->left->height = tmp_height + 1;
					tmp_node->right->height = tmp_height + 1;
					tmp_node->height = tmp_height + 2;
				}
			} else if (tmp_node->left->height - tmp_node->right->height == -2) {
				if (tmp_node->right->right->height - tmp_node->left->height
						== 1) {
					left_rotation(tmp_node);
					tmp_node->left->height = tmp_node->left->right->height + 1;
					tmp_node->height = tmp_node->left->height + 1;
				} else {
					right_rotation(tmp_node->right);
					left_rotation(tmp_node);
					tmp_height = tmp_node->right->right->height;
					tmp_node->left->height = tmp_height + 1;
					tmp_node->right->height = tmp_height + 1;
					tmp_node->height = tmp_height + 2;
				}
			} else {

				/* update height even if there was no rotation */

				if (tmp_node->left->height > tmp_node->right->height)
					tmp_node->height = tmp_node->left->height + 1;
				else
					tmp_node->height = tmp_node->right->height + 1;
			}
			if (tmp_node->height == old_height)
				finished = 1;
		}
	}
}

void delete_interval(m_tree_t * tree, int a, int b) {
	if (a > b)
		return;
	else {
		delete_int(tree, a, b);
		delete_int(tree, b, a);
	}
}

void destroy_m_tree(m_tree_t *tree) {
	free(tree);
}

