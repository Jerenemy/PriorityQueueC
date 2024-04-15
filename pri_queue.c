/* COMP 211 Challenge 2:  More sorting.
 *
 * Jeremy Zay
 * 
 * Priority queue interface.
 *
 * A priority queue is a linear sequence of integers sorted in non-decreasing
 * order.  We write <<x_0,...,x_{n-1}>> for a priority queue with n keys and
 * x_0 ≤ x_1 ≤ ... ≤ x_{n-1}.
 *
 * Remember, the "sorted order" is a perspective of the *abstract value*.  An
 * implementation need not represent the keys as a sorted sequence.
 *
 * You may assume that no client will create a priority queue with more than
 * SORT_MAX many items.
 *
 */

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "sorting.h"
#include "pri_queue.h"

#ifndef SORT_MAX
#endif



/* pq_swap(xs, i, j):  
 *
 * Pre-condition:   0 <= i, j < length xs.
 * 
 * Post-condition:  swaps xs[i] and xs[j].
 */
void pq_swap(int xs[], int i, int j) {
    int t = xs[i] ;
    xs[i] = xs[j] ;
    xs[j] = t ;
}

/* A bin_tree represents a left-complete heap-ordered binary tree.
 * We write <x_0, x_1, ..., x_{n-1}> to represent such a tree with n keys, where
 * x_0 ≤ x_1 ≤ ... ≤ x_{n-1}.
 *
 * repr(tree) = <x_0, x_1, ..., x_{n-1}>, where
 *
 *      - tree->size = n, for 0 <= n <= SORT_MAX
 *      - tree->keys[i] = x_i, for 0 <= i < n
 *
 *      - x_i is the parent of x_((i+1)*2-1) and x_((i+1)*2), for 0 <= i < n/2, / is integer division
 *      - x_((i+1)*2-1) and x_((i+1)*2) are the children of x_i, for 0 <= n/2, / is integer division
 *      - x_i ≤ x_((i+1)*2-1) and x_i ≤ x_((i+1)*2), for 0 <= i < n/2, / is integer division
 *
 * Invariants:
 *      1. The tree is a left-complete heap-ordered binary tree.
 *      2. For every key in the tree, the parent key is smaller than or equal to its children keys.
 */
struct bin_tree {
    int keys[SORT_MAX] ; // array that backs the tree
    int size ; // size of the tree 
} ;

typedef struct bin_tree bin_tree ;

/* The type of a priority queue.
 * A priority queue is a linear sequence of integers sorted in non-decreasing
 * order.  We write <<x_0,...,x_{n-1}>> for a priority queue with n keys and
 * x_0 ≤ x_1 ≤ ... ≤ x_{n-1}.
 * 
 *  - 0 <= n <= SORT_MAX
 */
struct pri_queue {
    bin_tree* tree ; // pointer to bin_tree abstract type
} ;

typedef struct pri_queue pri_queue ;

bool pq_ok(pri_queue* pq) {

    int n = pq->tree->size ; // size of tree
    
    bool is_valid_size = n >= 0 && n <= SORT_MAX ; // size is within 0 and SORT_MAX
    
    // assert that the parent is always smaller than or equal to the children
    bool parent_child = true ;

    for (int i=0; i<(n/2); i+=1) {

        int first_child_i = (i + 1) * 2 - 1 ;
        int second_child_i = (i + 1) * 2 ;

        if (first_child_i<n) {
            if (pq->tree->keys[i] > pq->tree->keys[first_child_i]) {
                parent_child = false ;
            }
        }

        if (second_child_i<n) {
            if (pq->tree->keys[i] > pq->tree->keys[second_child_i]) {
                parent_child = false ;
            }
        }
    }

    return is_valid_size && parent_child;
}

/* pq_create() = << >>.
 */
pri_queue* pq_create() {
    bin_tree* tree = malloc(sizeof(bin_tree)) ;
    
    pri_queue* pq = malloc(sizeof(pri_queue)) ;

    pq->tree = tree ;
    pq->tree->size = 0 ; // create empty tree

    assert(pq_ok(pq)) ;
    return pq;
}

/* pq_empty(pq) = true,  pq = << >>
 *                false, pq = <<x_0,...,x_{n-1}>> with n > 0.
 */
bool pq_empty(pri_queue* pq) {
    return pq->tree->size == 0 ;
}

/* pq_free_tree_when_empty(pq):  frees the resources associated to pq->tree, when pq is empty.
 * 
 * Frees the tree when the last item has been popped from the tree.
 */
void pq_free_tree_when_empty(pri_queue* pq) {
    if (pq_empty(pq)) {
       free(pq->tree) ; 
    }
}


/* get_parent_i(pq, i) = j, where j is equal to (i + 1) / 2 - 1, where / is integer division, and pq->tree->keys[j] represents
 * the parent of pq->tree->keys[i].
 * 
 * Pre-condition:   0 < i < pq->tree->size
 *                  pq->tree->size > 1
 */
int get_parent_i(pri_queue* pq, int i) {
    // calculate index of parent key in tree 
    return (i + 1) / 2 - 1 ; 
}

/* get_first_child_i(pq, i) = j, where j is equal to (i + 1) * 2 - 1 and pq->tree->keys[j] represents
 * the first child of pq->tree->keys[i].
 * 
 * Pre-condition:   0 < (i + 1) * 2 - 1 < pq->tree->size
 *                  pq->tree->size > 1
 */
int get_first_child_i(pri_queue* pq, int i) {
    // calculate index of first child key in tree 
    return (i + 1) * 2 - 1 ; 
}

/* get_second_child_i(pq, i) = j, where j is equal to (i + 1) * 2 and pq->tree->keys[j] represents
 * the second child of pq->tree->keys[i].
 * 
 * Pre-condition:   0 < (i + 1) * 2 < pq->tree->size
 *                  pq->tree->size > 2
 */
int get_second_child_i(pri_queue* pq, int i) {
    // calculate index of second child key in tree
    return (i + 1) * 2 ;  
}

/* pq_push(pq, x):  push x into pq.
 *
 * Pre-condition:   pq = <<x_0,...,x_{n-1}>>
 * Post-condition:  pq = <<x_0,...,x_{i-1},x,x_i,...,x_{n-1}>>, where
 *   x_{i-1} < x ≤ x_i.
 *
 * In other words, pq_push(pq, x) ensures that x is put into the priority
 * queue, maintaining the sorted order.
 */
void pq_push(pri_queue* pq, int x) {

    int x_i = pq->tree->size ; // index of x
    pq->tree->keys[x_i] = x ; 
    int parent_i = get_parent_i(pq, x_i); // index of parent
    
    // bubbling up
    while(x < pq->tree->keys[parent_i] && x_i > 0) { // test for if x is smaller than its parent, and if x is the root of the tree)
        pq_swap(pq->tree->keys, x_i, parent_i) ;
        x_i = parent_i ;
        parent_i = get_parent_i(pq, x_i) ;
    }

    pq->tree->size += 1 ;

    assert(pq_ok(pq)) ;
    return ;
}

/* pq_pop(pq) = x_0, where x_0 is the smallest item in pq.
 *
 * Pre-condition:  pq = <<x_0,...,x_{n-1}>>, n > 0.
 * Post-condition: pq = <<x_1,...,x_{n-1}>>, n > 0.
 */
int pq_pop(pri_queue* pq) {

    int moving_i = 0 ; // index of key that is bubbling down tree to correct position
    bool bubble_down = true ; // true if moving_i can bubble down (if it has children, which are smaller than it), false otherwise
    int priority = pq->tree->keys[0] ; // the smallest item in pri_queue, which I will return

    pq->tree->keys[0] = pq->tree->keys[pq->tree->size-1] ; // set first item to root of tree

    int first_child_i ; 
    int second_child_i ;
    int smallest_child_i ;

    while (bubble_down) {
        
        first_child_i = get_first_child_i(pq, moving_i) ;
        second_child_i = get_second_child_i(pq, moving_i) ;
        
        // Case 1: x_i has both children
        if (second_child_i < pq->tree->size) {
            // determine smallest child
            if (pq->tree->keys[first_child_i] < pq->tree->keys[second_child_i]) {
                smallest_child_i = first_child_i ;
            }
            else {
                smallest_child_i = second_child_i ;
            }
            // test if parent is bigger than child
            if (pq->tree->keys[moving_i] > pq->tree->keys[smallest_child_i])  {
                // perform swap and update moving_i to the index of the child it swapped with
                pq_swap(pq->tree->keys, moving_i, smallest_child_i) ;
                moving_i = smallest_child_i ;
            }
            else {
                bubble_down = false ; 
            }
        }

        // Case 2: x_i has one child: first child
        else if (first_child_i < pq->tree->size) { 
            smallest_child_i = first_child_i ;
            
            if (pq->tree->keys[moving_i] > pq->tree->keys[smallest_child_i])  {
                // perform swap and update moving_i to the index of the child it swapped with
                pq_swap(pq->tree->keys, moving_i, smallest_child_i) ;
                moving_i = smallest_child_i ;
            }
            else {
                bubble_down = false ;
            }
        }

        // Case 3: x_i has no children
        else {
            bubble_down = false ;
        }
    }

    pq->tree->size -= 1 ;

    // free the memory allocated to the tree once the last item has been popped off the tree (would do this in a pq_free function, but not in header file)
    pq_free_tree_when_empty(pq) ;

    assert(pq_ok(pq)) ;
    return priority ;
}



/* print_array(xs, j, n):  print "{xs[j], xs[j+1],...,xs[j+n-1]}" to the
 * terminal (without a newline).
 *
 * Pre-condition:  0 <= j and xs has length at least j+n.
 */
void print_array_pq(int xs[], int j, int n) {
    printf("{") ;
    for (int i=j; i<j+n; i+=1) {
        printf("%d", xs[i]) ;
        if (i < j+n-1) printf(", ") ;
    }
    printf("}") ;

    return ;
}

/* print_full_array(xs, n):  print "{xs[0], xs[1],...,xs[n-1]}" to the
 * terminal (without a newline).
 *
 * Pre-condition:  xs has length at least n.
 */
void print_full_array_pq(int xs[], int n) {
    print_array_pq(xs, 0, n) ;
}

/* pq_print(pq):  print information about pq.
 *
 * You may implement this function however you like; it will never be called
 * during testing.
 */
void pq_print(pri_queue* pq) {
    print_full_array_pq(pq->tree->keys, pq->tree->size) ;
    return ;
}
