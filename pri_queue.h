/* COMP 211 Challenge 2:  More sorting.
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
 * N. Danner
 */

#include <stdbool.h>

/* The type of a priority queue.
 */
struct pri_queue ;

/* pq_create() = << >>.
 */
struct pri_queue* pq_create() ;

/* pq_empty(pq) = true,  pq = << >>
 *                false, pq = <<x_0,...,x_{n-1}>> with n > 0.
 */
bool pq_empty(struct pri_queue*) ;

/* pq_push(pq, x):  push x into pq.
 *
 * Pre-condition:   pq = <<x_0,...,x_{n-1}>>
 * Post-condition:  pq = <<x_0,...,x_{i-1},x,x_i,...,x_{n-1}>>, where
 *   x_{i-1} < x ≤ x_i.
 *
 * In other words, pq_push(pq, x) ensures that x is put into the priority
 * queue, maintaining the sorted order.
 */
void pq_push(struct pri_queue*, int) ;

/* pq_pop(pq) = x_0, where x_0 is the smallest item in pq.
 *
 * Pre-condition:  pq = <<x_0,...,x_{n-1}>>, n > 0.
 */
int pq_pop(struct pri_queue*) ;

/* pq_print(pq):  print information about pq.
 *
 * You may implement this function however you like; it will never be called
 * during testing.
 */
void pq_print(struct pri_queue*) ;

