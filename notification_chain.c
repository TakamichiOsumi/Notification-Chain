#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "Glued-Doubly-Linked-List/glthreads.h"
#include "notification_chain.h"
#include "notification_chain_util.h"

char *
nfc_get_str_op_code(nfc_op op){
    switch(op){
	case NFC_ADD:
	    return "NFC_ADD";
	case NFC_UPDATE:
	    return "NFC_UPDATE";
	case NFC_DELETE:
	    return "NFC_DELETE";
	default:
	    return NULL;
    }
}

static int
nfc_search_elem_by_key(void *entry, void *key){
    notif_chain_elem *elem = entry;

    if (elem != NULL && key != NULL &&
	strncmp(elem->key, key, elem->key_size) == 0){
	return 0;
    }else{
	return 1;
    }
}

static void
nfc_free_elem(void **lists, void *entry){
    gldll *gllist = lists[0]; /* Expect only one list for this program */
    notif_chain_elem *elem = entry;
    glthread_node *reconnected = &elem->glue;

    if (reconnected->prev != NULL && reconnected->next != NULL){
	reconnected->prev->next = reconnected->next;
	reconnected->next->prev = reconnected->prev;
    }else if (reconnected->prev == NULL && reconnected->next != NULL){
	gllist->head = reconnected->next;
	reconnected->next->prev = NULL;
    }else if(reconnected->prev != NULL && reconnected->next == NULL){
	reconnected->prev->next = NULL;
    }

    /*
     * No special memory allocation to notif_chain_elem.
     * Just freeing the element after its list reconnection suffices.
     */
    free(elem);
}

notif_chain*
nfc_create_notification_chain(char *ntc_name){
    notif_chain *new_chain;

    new_chain = nfc_malloc(sizeof(notif_chain));
    /* As of now, suppose the nfc will not be referenced by other data structure */
    new_chain->notif_chain_gldll = nfc_malloc(sizeof(gldll *));
    /* Prepare internal data */
    strncpy(new_chain->ntc_name, ntc_name, MAX_STR_LEN);
    new_chain->notif_chain_gldll = glthread_create_list(nfc_search_elem_by_key,
							NULL,
							nfc_free_elem,
							offsetof(notif_chain_elem, glue));
    return new_chain;
}

notif_chain_elem*
nfc_create_notif_chain_elem(char *key, size_t key_size, pthread_t subs_thread,
			    nfc_app_cb app_cb, uint32_t subs_id){
    notif_chain_elem *elem = nfc_malloc(sizeof(notif_chain_elem));

    strncpy(elem->key, key, key_size);
    elem->key_size = key_size;
    elem->subs_thread = subs_thread;
    elem->app_cb = app_cb;
    elem->subs_id = subs_id;
    elem->glue.prev = elem->glue.next = NULL;

    return elem;
}

/* Subscription request called by the subscriber */
void
nfc_register_notif_chain(notif_chain *nfc, notif_chain_elem *nfce){
    glthread_insert_entry(nfc->notif_chain_gldll, &nfce->glue);
}

/* Invoke request called by the publisher */
void
nfc_invoke_notif_chain(notif_chain *nfc,
		       void *arg, size_t arg_size,
		       char *key, size_t key_size, nfc_op op){
    glthread_node *curr = nfc->notif_chain_gldll->head;
    notif_chain_elem *nfce = NULL;

    while(curr){
	nfce = glthread_get_app_structure(nfc->notif_chain_gldll, curr);
	if (nfc->notif_chain_gldll->key_match_cb(nfce, key) == 0){
	    /* The publisher calls the function the subscriber requested */
	    if (nfce->app_cb != NULL){
		nfce->app_cb(arg, arg_size, op, nfce->subs_id);
	    }

	    /* The publisher sends a signal to subscriber to let it do some work */
	    if (nfce->subs_thread != NULL){
		pthread_kill(nfce->subs_thread, SIGUSR1);
	    }
	}
	curr = curr->next;
    }
}
