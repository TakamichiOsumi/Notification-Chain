#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "routing_table.h"
#include "notification_chain.h"
#include "notification_chain_util.h"
#include "thread_publisher.h"
#include "thread_subscriber.h"

/* Data source of the publisher */
rt_entry *publisher_rt_table;

rt_entry*
pub_get_rt_table(void){
    return publisher_rt_table;
}

static int
pub_fetch_user_option(void){
    int val;

    printf("Choose one of the numbers :\n"
	   "[1] Add/Update routing table entry\n"
	   "[2] Delete routing table entry (TODO) \n"
	   "[3] Dump routing table\n");

    val = nfc_get_integer_within_range(">>> ", 1, 3);

    return val;
}

static void
pub_handle_user_input(void){
    char dest[DEST_LEN], gw_ip[DEST_LEN], oif[OIF_LEN];
    unsigned char mask;

    printf("dest >>> ");
    nfc_get_string(dest, DEST_LEN);

    mask = nfc_get_integer_within_range("mask >>> ", 0, 255);

    printf("gw_ip >>> ");
    nfc_get_string(gw_ip, DEST_LEN);

    printf("oif >>> ");
    nfc_get_string(oif, OIF_LEN);

    /*
     * For debugging purpose, do not start another iteration of
     * user operation for the routing table for a little while.
     * Let the subscribers write some logs to check whether
     * they get signals.
     */
    printf("----- <The subscriber may write logs> -----\n");
    rt_add_or_update_rt_entry(pub_get_rt_table(), dest, mask, gw_ip, oif);
    sleep(3);
    printf("-------------------------------------------\n");
}

static void
pub_main_menu(){
    while(1){
	switch(pub_fetch_user_option()){
	    case 1:
		pub_handle_user_input();
		break;
	    case 2:
		/* TODO */
		break;
	    case 3:
		rt_dump_rt_table(pub_get_rt_table());
		break;
	    default:
		fprintf(stderr, "invalid input has been detected\n");
		exit(-1);
		break;
	}
    }
}

static void *
pub_thread_fn(void *arg){
    printf("Publisher thread '%p' will start its own task\n",
	   pthread_self());
    rt_dump_rt_table(pub_get_rt_table());
    pub_main_menu();
    return NULL;
}

static void
pub_create_publisher_thread(){
    pthread_attr_t attr;
    pthread_t pub_thread;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,
				PTHREAD_CREATE_DETACHED);
    if (pthread_create(&pub_thread, &attr, pub_thread_fn, (void *) 0) != 0){
	perror("pthread_create");
	exit(-1);
    }

    printf("Publisher thread with id = %d has been created\n", 0);
}

void
pub_create_subscriber_thread(void *(*sub_thread_fn)(void *arg),
			     uintptr_t client_id){
    pthread_attr_t attr;
    pthread_t subs_thread;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,
				PTHREAD_CREATE_DETACHED);

    if (pthread_create(&subs_thread, &attr, sub_thread_fn,
		       (void *) client_id) != 0){
	perror("pthread_create");
	exit(-1);
    }
    printf("Subscriber thread with id = %lu has been created\n",
	   client_id);
}

int
main(int argc, char **argv){
    publisher_rt_table = rt_init_rt_table();

    /* Create subscriber threads */
    pub_create_subscriber_thread(sub_thread_fn1, 1);
    sleep(1);
    pub_create_subscriber_thread(sub_thread_fn2, 2);
    sleep(1);
    pub_create_subscriber_thread(sub_thread_fn3, 3);
    sleep(1);

    /* Create publisher main thread */
    pub_create_publisher_thread();
    pthread_exit(0);

    return 0;
}
