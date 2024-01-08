#include <pthread.h>
#include <memory.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdbool.h>
#include <unistd.h>
#include "routing_table.h"
#include "thread_publisher.h"
#include "thread_subscriber.h"

/*
 * Subscriber thread can do other tasks until it gets any update
 * of the routing table entry from the publisher.
 *
 * But, this program is written for an educational purpose.
 * Therefore, just print some logs to indicate the subscriber
 * gets the signal from the publisher.
 */
static void
sub_sigusr1_handler(int sig){ }

static void
sub_set_sig_handlers(void){
    struct sigaction act;

    act.sa_handler = sub_sigusr1_handler;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);

    sigaction(SIGUSR1, &act, NULL);
}

/*
 * The publisher task requested by the subscriber. This time,
 * just print some information.
 */
static void
sub_test_cb(void *arg, size_t arg_size,
	    nfc_op nfc_op_code, uintptr_t client_id){
    printf("'%p' called '%s' with '%s' and client id = %ld\n",
	   pthread_self(), __FUNCTION__,
	   nfc_get_str_op_code(nfc_op_code), client_id);
}

void *
sub_thread_fn1(void *arg){
    sub_set_sig_handlers();
    rt_table_register_for_notification(pub_get_rt_table(),
				       "122.1.1.1", 32, "10.1.1.1", "eth1",
				       sub_test_cb, (uintptr_t) arg);
    while(1){
	pause();
	printf("The pause function in '%s' called by '%p' thread has finished\n",
	       __FUNCTION__, pthread_self());
    }

    return NULL;
}

void *
sub_thread_fn2(void *arg){
    sub_set_sig_handlers();
    rt_table_register_for_notification(pub_get_rt_table(),
				       "122.1.1.1", 32, "10.1.1.1", "eth1",
				       sub_test_cb, (uintptr_t) arg);
    rt_table_register_for_notification(pub_get_rt_table(),
				       "122.1.1.2", 32, "10.1.1.1", "eth2",
				       sub_test_cb, (uintptr_t) arg);
    
    while(1){
	pause();
	printf("The pause function in '%s' called by '%p' thread has finished\n",
	       __FUNCTION__, pthread_self());
    }

    return NULL;
}

void *
sub_thread_fn3(void *arg){
    sub_set_sig_handlers();
    rt_table_register_for_notification(pub_get_rt_table(),
				       "122.1.1.1", 32, "10.1.1.1", "eth1",
				       sub_test_cb, (uintptr_t) arg);
    rt_table_register_for_notification(pub_get_rt_table(),
				       "122.1.1.3", 32, "10.1.1.1", "eth1",
				       sub_test_cb, (uintptr_t) arg);
    rt_table_register_for_notification(pub_get_rt_table(),
				       "122.1.1.4", 32, "10.1.1.1", "eth1",
				       sub_test_cb, (uintptr_t) arg);
    while(1){
	pause();
	printf("The pause function in '%s' called by '%p' thread has finished\n",
	       __FUNCTION__, pthread_self());
    }

    return NULL;
}
