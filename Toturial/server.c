#define _GNU_SOURCE
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "debug.h"
#include "ib.h"
#include "setup_ib.h"
#include "config.h"
#include "server.h"

void *server_thread (void *arg)
{
    int         ret             = 0, i = 0, n = 0;
    long        thread_id       = (long) arg;
    int         num_concurr_msgs= config_info.num_concurr_msgs;
    int         msg_size        = config_info.msg_size;

    pthread_t   self;
    cpu_set_t   cpuset;

    int                  num_wc       = 20;
    struct ibv_qp       *qp           = ib_res.qp;
    struct ibv_cq       *cq           = ib_res.cq;
    struct ibv_wc       *wc           = NULL;
    uint32_t             lkey         = ib_res.mr->lkey;
    char                *buf_ptr      = ib_res.ib_buf;
    int                  buf_offset   = 0;
    size_t               buf_size     = ib_res.ib_buf_size - msg_size;
    uint32_t             rkey         = ib_res.rkey;
    uint64_t             raddr_base   = ib_res.raddr;
    uint64_t             raddr        = ib_res.raddr;
    volatile char       *msg_start    = buf_ptr;
    volatile char       *msg_end      = msg_start + msg_size - 1;
    volatile char       *send_buf_ptr = buf_ptr;

    struct timeval      start, end;
    long                ops_count  = 0;
    double              duration   = 0.0;
    double              throughput = 0.0;

    //printf("server_thread\n");

    wc = (struct ibv_wc *) calloc (num_wc, sizeof(struct ibv_wc));
    check (wc != NULL, "thread[%ld]: failed to allocate wc.", thread_id);

    /* set thread affinity */
    CPU_ZERO (&cpuset);
    CPU_SET  ((int)thread_id, &cpuset);
    self = pthread_self ();
    ret  = pthread_setaffinity_np (self, sizeof(cpu_set_t), &cpuset);
    check (ret == 0, "thread[%ld]: failed to set thread affinity", thread_id);

    //printf("hello1\n");
    /* pre-post writes */
    //printf("send_buf_ptr1: %s\n", send_buf_ptr);
    for (i = 0; i < num_concurr_msgs; i++) {
    	post_write_unsignaled (msg_size, lkey, 0, qp, send_buf_ptr, raddr, rkey);
    	buf_offset = (buf_offset + msg_size) % buf_size;
    	raddr      = raddr_base + buf_offset;
    }


    //int prev_count = -1 ;
    //printf("hello2\n");
    while (ops_count < TOT_NUM_OPS) {
        /* loop till receive a msg from server */
        //printf("look here: %s\n" , msg_start);
        while (strcmp(msg_start , "") == 0) {
            //printf("ridim");
        }
        //prev_count = atoi(counter);
	//printf("send_buf_ptr2: %s\n", send_buf_ptr);
       	printf("hello5\n");

        /* send a msg back to the server */
	//prev_count = atoi(msg_start);
	ops_count += 1;
        if ((ops_count % SIG_INTERVAL) == 0) {
	    //printf("hello6\n");
            ret = post_write_signaled (msg_size, lkey, 0, qp, send_buf_ptr, raddr, rkey);
        } else {
	    //printf("hello7\n");
	    //printf("heaven is here: %d ----- %d ------ %s ----- %d ----- %d", msg_size, lkey, send_buf_ptr, raddr, rkey);
            ret = post_write_unsignaled (msg_size, lkey, 0, qp, send_buf_ptr, raddr, rkey);
        }
	/* reset recv buffer */
	//sleep(5);
	//memset ((char *)msg_start, '\0', msg_size);

        buf_offset = (buf_offset + msg_size) % buf_size;
        msg_start  = buf_ptr + buf_offset;
	send_buf_ptr = msg_start ;
        msg_end    = msg_start + msg_size - 1;
        raddr      = raddr_base + buf_offset;


        if (ops_count == NUM_WARMING_UP_OPS) {
            gettimeofday (&start, NULL);
        }

	n = ibv_poll_cq (cq, num_wc, wc);
	debug ("ops_count = %ld", ops_count);
    }
    gettimeofday (&end, NULL);
    //printf("hello3\n");
    /* dump statistics */
    duration   = (double)((end.tv_sec - start.tv_sec) * 1000000 +
                          (end.tv_usec - start.tv_usec));
    throughput = (double)(ops_count) / duration;
    log ("thread[%ld]: throughput = %f (Mops/s)",  thread_id, throughput);

    free (wc);
    pthread_exit ((void *)0);

 error:
    if (wc != NULL) {
    	free (wc);
    }
    pthread_exit ((void *)-1);
}

int run_server ()
{
    int   ret         = 0;
    long  num_threads = 1;
    long  i           = 0;

    pthread_t           *threads = NULL;
    pthread_attr_t       attr;
    void                *status;

    //printf("run_server\n");

    pthread_attr_init (&attr);
    pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_JOINABLE);

    threads = (pthread_t *) calloc (num_threads, sizeof(pthread_t));
    check (threads != NULL, "Failed to allocate threads.");

    //printf("hello world!\n");
    for (i = 0; i < num_threads; i++) {
	ret = pthread_create (&threads[i], &attr, server_thread, (void *)i);
	check (ret == 0, "Failed to create server_thread[%ld]", i);
    }

    //printf("hello world2!\n");
    bool thread_ret_normally = true;
    for (i = 0; i < num_threads; i++) {
        ret = pthread_join (threads[i], &status);
        check (ret == 0, "Failed to join thread[%ld].", i);
        if ((long)status != 0) {
            thread_ret_normally = false;
            log ("server_thread[%ld]: failed to execute", i);
        }
    }

    //printf("hello world3!\n");
    if (thread_ret_normally == false) {
        goto error;
    }
    //printf("hello world4!\n");
    pthread_attr_destroy    (&attr);
    free (threads);

    return 0;

 error:
    if (threads != NULL) {
        free (threads);
    }
    pthread_attr_destroy    (&attr);

    return -1;
}

