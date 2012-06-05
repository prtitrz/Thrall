/*
 * @file threadpool.c
 * @author prtitrz <prtitrz@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "threadpool.h"
#include "debug.h"


typedef struct {
    void (*routine)(void *);
    void *arg;
} threadpool_task_t;

struct threadpool_t {
  pthread_mutex_t lock;
  pthread_cond_t notify;
  pthread_cond_t ready;
  pthread_t *threads;
  threadpool_task_t *queue;
  int thread_num;
  int queue_size;
  int head;
  int tail;
  int shutdown;
  int started;
};

static void *threadpool_thread(void *threadpool);

int threadpool_free(threadpool_t *pool);

threadpool_t *threadpool_init(int thread_num, int queue_size, int flag)
{
    threadpool_t *pool;
    int i;

    /* TODO: Check for negative or otherwise very big input parameters */

    if((pool = (threadpool_t *)malloc(sizeof(threadpool_t))) == NULL) {
		return NULL;
    }

    /* Initialize */
    pool->thread_num = thread_num;
    pool->queue_size = queue_size;
    pool->head = pool->tail = 0;
    pool->shutdown = pool->started = 0;

    pool->threads = (pthread_t *)malloc(sizeof (pthread_t) * thread_num);
    pool->queue = (threadpool_task_t *)malloc
        (sizeof (threadpool_task_t) * queue_size);

    /* Initialize mutex and conditional variable */
    if((pthread_mutex_init(&(pool->lock), NULL) != 0) || !(pool->threads) ||
       (pthread_cond_init(&(pool->notify), NULL) != 0) || !(pool->queue) ||
	   (pthread_cond_init(&(pool->ready), NULL) != 0)) {
		/* pool free? */
        return NULL;
    }

    /* Start threads */
    for(i = 0; i < thread_num; i++) {
        if(pthread_create(&(pool->threads[i]), NULL,
                          threadpool_thread, (void*)pool) != 0) {
            threadpool_destroy(pool, 0);
            return NULL;
        } else {
            pool->started++;
        }
    }

    return pool;
}

int threadpool_add(threadpool_t *pool, void (*routine)(void *),
                   void *arg, int flag)
{
    int err = 0;
	int next;

    if(pool == NULL || routine == NULL) {
        return threadpool_invalid;
    }

    if(pthread_mutex_lock(&(pool->lock)) != 0) {
        return threadpool_lock_failure;
    }

	next = pool->tail + 1;
	next = (next == pool->queue_size) ? 0 : next;

    do {
        while(((next) == (pool->head)) && (!pool->shutdown)) {
            pthread_cond_wait(&(pool->ready), &(pool->lock));
        }
        /* Full ? */
        if(next == pool->head) {
            err = threadpool_queue_full;
			debug_print("%d %d %d\n", pool->tail, pool->head, err);
            break;
        }

        /* Shutting down ? */
        if(pool->shutdown) {
            err = threadpool_shutdown;
            break;
        }

        /* Add task to queue */
		pool->tail = next;
        pool->queue[pool->tail].routine = routine;
        pool->queue[pool->tail].arg = arg;

        /* pthread_cond_broadcast */
        if(pthread_cond_signal(&(pool->notify)) != 0) {
            err = threadpool_lock_failure;
            break;
        }
    } while(0);

    if(pthread_mutex_unlock(&pool->lock) != 0) {
        err = threadpool_lock_failure;
    }

    return err;
}

int threadpool_destroy(threadpool_t *pool, int flag)
{
    int i, err = 0;

    if(pool == NULL) {
        return threadpool_invalid;
    }

    if(pthread_mutex_lock(&(pool->lock)) != 0) {
        return threadpool_lock_failure;
    }

    do {
        /* Already shutting down */
        if(pool->shutdown) {
            err = threadpool_shutdown;
            break;
        }

        pool->shutdown = 1;

        /* Wake up all worker threads */
        if((pthread_cond_broadcast(&(pool->notify)) != 0) ||
           (pthread_cond_broadcast(&(pool->ready)) != 0) ||
           (pthread_mutex_unlock(&(pool->lock)) != 0)) {
            err = threadpool_lock_failure;
            break;
        }

        /* Join all worker thread */
        for(i = 0; i < pool->thread_num; i++) {
            if(pthread_join(pool->threads[i], NULL) != 0) {
                err = threadpool_thread_failure;
            }
        }
    } while(0);

    if(pthread_mutex_unlock(&pool->lock) != 0) {
        err = threadpool_lock_failure;
    }
    
    /* Only if everything went well do we deallocate the pool */
    if(!err) {
        threadpool_free(pool);
    }
    return err;
}

int threadpool_free(threadpool_t *pool)
{
    if(pool == NULL || pool->started > 0) {
        return -1;
    }

    /* Did we manage to allocate ? */
    if(pool->threads) {
        free(pool->threads);
        free(pool->queue);
 
        /* Because we allocate pool->threads after initializing the
           mutex and condition variable, we're sure they're
           initialized. Let's lock the mutex just in case. */
        pthread_mutex_lock(&(pool->lock));
        pthread_mutex_destroy(&(pool->lock));
        pthread_cond_destroy(&(pool->notify));
        pthread_cond_destroy(&(pool->ready));
    }
    free(pool);    
    return 0;
}


static void *threadpool_thread(void *threadpool)
{
    threadpool_t *pool = (threadpool_t *)threadpool;
    threadpool_task_t task;

    for(;;) {
        /* Lock must be taken to wait on conditional variable */
        pthread_mutex_lock(&(pool->lock));

        if(pthread_cond_signal(&(pool->ready)) != 0) {
            break;
        }

        /* Wait on condition variable, check for spurious wakeups.
           When returning from pthread_cond_wait(), we own the lock. */
        while(((pool->tail) == (pool->head)) && (!pool->shutdown)) {
            pthread_cond_wait(&(pool->notify), &(pool->lock));
        }

        if(pool->shutdown) {
            break;
        }

        /* Grab our task */
        pool->head += 1;
        pool->head = ((pool->head) == pool->queue_size) ? 0 : pool->head;
        task.routine = pool->queue[pool->head].routine;
        task.arg = pool->queue[pool->head].arg;

        /* Unlock */
        pthread_mutex_unlock(&(pool->lock));

        /* Get to work */
        (*(task.routine))(task.arg);
    }

    pool->started--;

    pthread_mutex_unlock(&(pool->lock));
    pthread_exit(NULL);
    return(NULL);
}
