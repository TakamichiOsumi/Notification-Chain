#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include "routing_table.h"
#include "notification_chain.h"
#include "notification_chain_util.h"
#include "thread_publisher.h"

/* Data source of the publisher */
rt_entry *publisher_rt_table;

static int callback_counter = 0;

rt_entry*
pub_get_rt_table(void){
    return publisher_rt_table;
}

static void
pub_test_cb(void *arg, size_t arg_size,
            nfc_op nfc_op_code, uintptr_t client_id){
    printf("'%p' called '%s' with '%s' and client id = %ld\n",
           pthread_self(), __FUNCTION__,
           nfc_get_str_op_code(nfc_op_code), client_id);

    callback_counter++;
}

static void
nfc_component_check_list_len(notif_chain *chain, int expected){
    int len;

    len = glthread_list_length(chain->notif_chain_gldll);
    if (len != expected){
	fprintf(stderr, "expected the list length to be %d, but it was %d.\n",
		expected, len);
	exit(-1);
    }
}

static void
nfc_component_removal_tests(){
    notif_chain *mychain;

    mychain = nfc_create_notification_chain("chain for test");
    nfc_register_notif_chain(mychain,
			     nfc_create_notif_chain_elem("key1", strlen("key1"), NULL, pub_test_cb, 1));
    nfc_register_notif_chain(mychain,
			     nfc_create_notif_chain_elem("key2", strlen("key2"), NULL, pub_test_cb, 2));
    nfc_register_notif_chain(mychain,
			     nfc_create_notif_chain_elem("key3", strlen("key3"), NULL, pub_test_cb, 3));
    nfc_register_notif_chain(mychain,
			     nfc_create_notif_chain_elem("key4", strlen("key4"), NULL, pub_test_cb, 4));
    nfc_register_notif_chain(mychain,
			     nfc_create_notif_chain_elem("key5", strlen("key5"), NULL, pub_test_cb, 5));
    nfc_register_notif_chain(mychain,
			     nfc_create_notif_chain_elem("key6", strlen("key6"), NULL, pub_test_cb, 6));
    nfc_component_check_list_len(mychain, 6);
    glthread_remove_entry_from_lists(&mychain->notif_chain_gldll, 0, "key5");
    nfc_component_check_list_len(mychain, 5);
    glthread_remove_entry_from_lists(&mychain->notif_chain_gldll, 0, "key dummy");
    nfc_component_check_list_len(mychain, 5);
    glthread_remove_entry_from_lists(&mychain->notif_chain_gldll, 0, "key1");
    nfc_component_check_list_len(mychain, 4);
    glthread_remove_all_list_entries(&mychain->notif_chain_gldll, 0);

    if (mychain->notif_chain_gldll != NULL)
	exit(-1);
    else
	free(mychain);
}

static void
nfc_component_invoke_nfc_tests(){
    notif_chain *mychain;

    mychain = nfc_create_notification_chain("chain for test");

    nfc_register_notif_chain(mychain,
			     nfc_create_notif_chain_elem("key1", strlen("key1"), NULL, pub_test_cb, 1));
    nfc_register_notif_chain(mychain,
			     nfc_create_notif_chain_elem("key2", strlen("key2"), NULL, pub_test_cb, 2));
    nfc_register_notif_chain(mychain,
			     nfc_create_notif_chain_elem("key3", strlen("key3"), NULL, pub_test_cb, 3));
    if (callback_counter != 0)
	exit(-1);
    nfc_invoke_notif_chain(mychain, "foo", strlen("foo"),
			   "key1", strlen("key1"), NFC_ADD);
    if (callback_counter != 1)
	exit(-1);
    nfc_invoke_notif_chain(mychain, "bar", strlen("bar"),
			   "key dummy", strlen("key dummy"), NFC_ADD);
    if (callback_counter != 1)
	exit(-1);

    glthread_remove_all_list_entries(&mychain->notif_chain_gldll, 0);
    if (mychain->notif_chain_gldll != NULL)
	exit(-1);
    else
	free(mychain);
}

int
main(int argc, char **argv){

    nfc_component_removal_tests();
    nfc_component_invoke_nfc_tests();

    return 0;
}
