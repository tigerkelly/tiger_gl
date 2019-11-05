
#ifndef _MYATOMIC_H_
#define _MYATOMIC_H_

/* AtomicExchange is used to compare and set p and return 1 on success else 0
 * p pointer to location to test.
 * e pointer to expected value at p.
 * if p equal e then set p to n and return 1 else 0
 * All three arguments are pointers. */
#define AtomicExchange(p, e, n) \
	__atomic_compare_exchange(p, e, n, 1, __ATOMIC_ACQUIRE, __ATOMIC_ACQUIRE)

// These defines return the new value after operation.
#define AtomicAdd(p, n) 		__atomic_add_fetch(p, n, __ATOMIC_SEQ_CST)
#define AtomicSub(p, n) 		__atomic_sub_fetch(p, n, __ATOMIC_SEQ_CST)

#define AtomicAnd(p, n)			__atomic_and_fetch(p, n, __ATOMIC_SEQ_CST)
#define AtomicXor(p, n)			__atomic_xor_fetch(p, n, __ATOMIC_SEQ_CST)
#define AtomicOr(p, n)			__atomic_or_fetch(p, n, __ATOMIC_SEQ_CST)
#define AtomicNand(p, n)		__atomic_nand_fetch(p, n, __ATOMIC_SEQ_CST)

// These defines return the previous value before the operation.
#define AtomicFetchAdd(p, n) 	__atomic_fetch_add(p, n, __ATOMIC_SEQ_CST)
#define AtomicFetchSub(p, n) 	__atomic_fetch_sub(p, n, __ATOMIC_SEQ_CST)

#define AtomicFetchAnd(p, n)	__atomic_fetch_and(p, n, __ATOMIC_SEQ_CST)
#define AtomicFetchXor(p, n)	__atomic_fetch_xor(p, n, __ATOMIC_SEQ_CST)
#define AtomicFetchOr(p, n)		__atomic_fetch_or(p, n, __ATOMIC_SEQ_CST)
#define AtomicFetchNand(p, n)	__atomic_fetch_nand(p, n, __ATOMIC_SEQ_CST)

// returns the value at type *v.
#define AtomicGet(p)			__atomic_load_n(p, __ATOMIC_SEQ_CST)
// no return value. type *p, type n
#define AtomicSet(p, n)			__atomic_store_n(p, n, __ATOMIC_SEQ_CST)
//  It writes n into p, and returns the previous contents of p.
#define AtomicFetchSet(p, n) 	__atomic_exchange_n(p, n, __ATOMIC_SEQ_CST)

// This performs an atomic test-and-set operation on the byte at *ptr. The byte is
// set to some implementation defined nonzero "set" value and the return value is true if and
// only if the previous contents were "set". It should be only used for operands of type bool or char.
#define AtomicTestSet(p)		__atomic_test_and_set(p, __ATOMIC_SEQ_CST)
// This performs an atomic clear operation on *ptr. After the operation, *ptr contains 0.
// It should be only used for operands of type bool or char and in conjunction with __atomic_test_and_set.
// For other types it may only clear partially. If the type is not bool prefer using __atomic_store.
#define AtomicClear(p)			__atmoic_clear(p, __ATOMIC_SEQ_CST)

#endif
