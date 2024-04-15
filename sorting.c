/* COMP 211 Challenge 2:  more sorting.
 *
 * Jeremy Zay
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

/* A struct tuple value t represents a tuple containing two integer values != -1,
 * start and end, along with a pointer to the next tuple in the list.
 *
 * repr(t) = (start, end), where
 *
 *      - t.start != -1
 *      - t.end != -1
 *      - t.next is a pointer to the next tuple in the list or NULL if t is the last tuple
 */
struct tuple {
    int start ;
    int end ;
    struct tuple* next ;
} ;

typedef struct tuple tuple ;

/* A struct ltup value l represents a sequence of linked tuples with
 * start and end integer values != -1.
 * We write <(s_0, e_0), (s_1, e_1), ..., (s_{n-1}, e_{n-1})> to represent
 * such a sequence with n tuples, where (s_i, e_i) is the i-th tuple
 * in the sequence.
 * 
 * repr(l) = <(s_0, e_0), (s_1, e_1), ..., (s_{n-1}, e_{n-1})>, where
 *      
 *      - l.hd is a dummy head tuple, where
 *          - l.hd.start = -1
 *          - l.hd.end = -1
 *      - l.tl is a dummy tail tuple, where
 *          - l.tl.start = -1
 *          - l.tl.end = -1
 *          - l.tl.next = NULL
 *      
 *      - (s_i, e_i) = l.hd.next^(i+1), for 0 <= i < n
 *      - (s_i, e_i) != -1, for for 0 <= i < n
 * 
 *      - s_i = l.hd.next^(i+1).start
 *      - e_i = l.hd.next^(i+1).end
 *      
 *      - l.hd.next^(n+2) = NULL
 *      - (s_i, e_i) != NULL for 0 <= i < n 
 */
struct ltup {
    tuple* hd ; // dummy head tuple
    tuple* tl ; // dummy tail tuple
} ;

typedef struct ltup ltup ;

bool ltup_ok(ltup* l) {
    // assert that dummy tuples are correct
    bool tl_points_to_null = l->tl->next == NULL ;
    bool hd_start_n_end_null = l->hd->start == -1 && l->hd->end == -1 ;
    bool tl_start_n_end_null = l->tl->start == -1 && l->tl->end == -1 ;

    // Tuples do not contain -1 start and end values (except for dummy tuples)
    bool not_negative_one = true ;
    
    tuple* p ;

    p = l->hd->next ;
    // don't test the dummy tuples (their start and end vals are -1)
    while (p != l->tl) {
        not_negative_one = p->start != -1 && p->end != -1 ; 
        p = p->next ;
    }

    return tl_points_to_null && hd_start_n_end_null && tl_start_n_end_null && not_negative_one ;
}

/* ltup_create() = l, where l = <>
 */
ltup* ltup_create() {
    tuple* dummy_head = malloc(sizeof(tuple)) ;
    tuple* dummy_tail = malloc(sizeof(tuple)) ;
    
    dummy_head->next = dummy_tail ; // connect head and tail
    // set vals to NULL equivalent (in my tuples, start or end != -1. This is because they are being used as array indices, and never have -1 as an array index)
    dummy_head->start = -1 ; 
    dummy_head->end = -1 ;

    dummy_tail->next = NULL ;
    dummy_tail->start = -1 ;
    dummy_tail->end = -1 ;

    // create empty ltup
    ltup* l = malloc(sizeof(ltup)) ;
    // set hd and tl fields to dummies
    l->hd = dummy_head ;
    l->tl = dummy_tail ;

    assert(ltup_ok(l)) ;
    // return pointer to ltup
    return l ;
}


/* ltup_push(l, p):  push p into l.
 *
 * Pre-condition:   l = <(s_0, e_0),...,(s_{n-1}, e_{n-1})>
 * Post-condition:  l = <p, (s_0, e_0),...,(s_{n-1}, e_{n-1})>
 */
void ltup_push(ltup* l, tuple* p) {
    // makes p the new starting tuple, sets its next field to the old starting tuple (was s->hd->next, now s->hd->next->next)

    // store previous starting tuple of seq
    tuple* t = l->hd->next ;
    // set starting tuple to new tuple p (stores partition start and end indices)
    l->hd->next = p ;
    // set current starting tuple next field to old prev tuple
    l->hd->next->next = t ;

    assert(ltup_ok(l)) ;
    return ;
}

/* ltup_pop(l) = t, where t = (s_0, e_0).
 *
 * Pre-condition:   l = <(s_0, e_0),...,(s_{n-1}, e_{n-1})> with n > 0
 * Post-condition:  l = <(s_1, e_1),...,(s_{n-1}, e_{n-1})>
 */
tuple* ltup_pop(ltup* l) {
    tuple* t = l->hd->next ;

    // if i remove the tuple, how do i free it? will need to free it in func this is called in, there i still have access to the tuple
    l->hd->next = l->hd->next->next ;
    
    assert(ltup_ok(l)) ;
    return t ;
}

/* ltup_is_empty(l) = true,  l = < >
 *                  = false, l = l = <(s_0, e_0),...,(s_{n-1}, e_{n-1})> with n > 0
 */
bool ltup_is_empty(ltup* l) {
    // test if head points to tail, if so, ltup is empty
    return l->hd->next == l->tl ;
}

/* p_swap(xs, i, j):  
 *
 * Pre-condition:   0 <= i, j < length xs.
 * 
 * Post-condition:  swaps xs[i] and xs[j].
 */
void p_swap(int xs[], int i, int j){
    int t = xs[i] ;
    xs[i] = xs[j] ;
    xs[j] = t ;
}

/* partition(lt, xs, start, end): Splits subarray into two subarrays, divided based on the first item of the array, with
 * all items in the subarray greater than the first item will be after the first item, all items less or equal will be before the first item
 * 
 * Pre-conditions:  l = <(s_0, e_0),...,(s_{n-1}, e_{n-1})>, where 
 *                      0 <= start < end < SORT_MAX
 *                  
 *                  xs = {x_0,...x_{n-1}}, where 
 *                      i <= 0 < n
 *                      0 <= n <= SORT_MAX
 * 
 * Post-conditions: l = <(start, end_a), (start_a, end), (s_0, e_0),...,(s_{n-1}, e_{n-1})>, where 
 *                      start_a - end_a = 2
 *                  
 *                  xs = {x'_0,...x'_{start_a}, x_0, x'{end_a}...,x'_{n-1}}, where 
 *                      for all elements in xs[i] for 0 <= i < start_a, xs[i] <= x_0
 *                      for all elements in xs[i] for end_a <= i < n, xs[i] > x_0
 */
void partition(ltup* l, int xs[], int start, int end) {

    tuple* p = malloc(sizeof(tuple)) ; // first partition start and end tuple
    tuple* q = malloc(sizeof(tuple)) ; // second partition start and end tuple

    // store initial start and end values
    int init_start = start ;
    int init_end = end ;
    
    // true when scanning through the array going from start to end, false when scanning from end to start
    bool start_to_end = true ;

    while (start < end) {

        if (start_to_end) {
            
            // compare initial x_0 to each value beginning from the end, one by one, incrementing backwards
            if (xs[start] >= xs[end]) {
                // swap initial x_0 with any values that are less or equal to it, moving initial x_0 to the other end of the array
                p_swap(xs, start, end) ;
                start += 1 ;
                start_to_end = false ;
            }
            else {
                end -= 1 ;
            }
        }

        else {
            
            // compare initial x_0 to each value beginning from the start, one by one, incrementing forwards
            if (xs[end] < xs[start]) {
                // swap initial x_0 with any values that are greater than it, moving initial x_0 to the other end of the array
                p_swap(xs, start, end) ;
                end -= 1 ;
                start_to_end = true ;
            }
            else {
                start += 1 ;
            }
        }
    }

    assert(start == end) ; // start should always equal end after the partitioning
    
    // know these right away
    p->start = init_start ; // set first index of first partition to current starting index
    q->end = init_end ; // set second index of second partition to current ending index

    p->end = start - 1 ; // same as end - 1
    q->start = end + 1 ; // same as start + 1
    
    ltup_push(l, q) ; // first put q in the linked tuple sequence
    ltup_push(l, p) ; // then put p in at front of the linked tuple sequence to point to q

    return ;
}




/* psort(xs, n):  sort xs.
 *
 * Pre-condition:  xs has length n, a_i = xs[i] for 0 ≤ i < n, n ≤ SORT_MAX.
 * Post-condition:  xs a sorted permutation of [a_0,...,a_{n-1}].
 *
 * This function must implement the partition sort function in the assignment.
 */
void psort211(int xs[], int n) {
    
    ltup *lt ; // declare ltup
    
    tuple* p = malloc(sizeof(tuple)) ; // tuple that will contain start and end indices of partition
    
    // start and end indices of array to partition
    int start = 0 ; // begins as first index of array
    int end = n - 1 ; // begins as last index of array

    // transfer start and end vals into p
    p->start = start ;
    p->end = end ;
    
    lt = ltup_create() ; // create empty lt
    
    ltup_push(lt, p) ; // push this tuple into lt (setting it's next value to the tail)

    // while there are subarrays to partition in xs (i.e while there are tuples in lt), continue partitioning xs until it is sorted in non-decreasing order
    while(!ltup_is_empty(lt)) {

        p = ltup_pop(lt) ; // pop first tuple in lt, capture popped tuple in p
        
        // get start and end values of p
        start = p->start ; 
        end = p->end ;
        
        // only partition subarrays that satisfy pre-condition of partition function, i.e subarrays must have at least length 2, indices must be positive
        if (start >= 0 && end-start > 0) {
            // partition subarray in xs based on indices from tuple
            partition(lt, xs, start, end) ;
        }
        free(p) ; // free popped tuple
    }
    free(lt->hd) ; // free dummy head tuple
    free(lt->tl) ; // free dummy tail tuple
    free(lt) ; // free linked tuple sequence
    
    return ;
}

typedef struct pri_queue pri_queue ;

/* psort(xs, n):  sort xs.
 *
 * Pre-condition:  xs has length n, a_i = xs[i] for 0 ≤ i < n, n ≤ SORT_MAX.
 * Post-condition:  xs a sorted permutation of [a_0,...,a_{n-1}].
 *
 * This function must implement the priority queue sort function in the
 * assignment.
 */
void pqsort211(int xs[], int n) {
    
    pri_queue* pri_q ;

    pri_q = pq_create() ; // create empty priority queue
    
    // push all items in xs into pri_q
    for (int i=0; i<n; i+=1) {
       pq_push(pri_q, xs[i]) ; 
    }
     
    // replace all items one by one in xs by with items popped from pri_q
    for (int i=0; i<n; i+=1) {
        // MUST pop all keys off the pri_q and empty the pri_q completely. once the pri_q is emptied, the tree will be freed from memory
        xs[i] = pq_pop(pri_q) ;
    }

    //free(pri_q->tree) ; // can't access pri_q->tree, so instead free the tree when the last item is popped off the tree
    free(pri_q) ;

    return ;
}