/*
 * @file threadpool.h
 * @author prtitrz <prtitrz@gmail.com>
 */

#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

typedef struct threadpool_t threadpool_t;

typedef enum {
    threadpool_invalid        = -1,
    threadpool_lock_failure   = -2,
    threadpool_queue_full     = -3,
    threadpool_shutdown       = -4,
    threadpool_thread_failure = -5
} threadpool_error_t;


threadpool_t* threadpool_init(int thread_num, int queue_size, int flag);

int threadpool_add(threadpool_t *pool, void (*routine)(void *), void *arg, int flag);

int threadpool_destroy(threadpool_t *pool, int flag);

#endif /* _THREADPOOL_H_ */
