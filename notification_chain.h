#ifndef __NOTIF_CHAIN__
#define __NOTIF_CHAIN__
#include <stdbool.h>
#include <pthread.h>
#include "Glued-Doubly-Linked-List/glthreads.h"

#define MAX_STR_LEN 64

typedef enum nfc_op {
    NFC_ADD = 0,
    NFC_UPDATE,
    NFC_DELETE
} nfc_op;

/* Notification chain managed by the publisher */
typedef struct notif_chain {
    char ntc_name[MAX_STR_LEN];
    gldll *notif_chain_gldll;
} notif_chain;

typedef void (*nfc_app_cb)(void *, size_t, nfc_op, uintptr_t);

/* One chain element for notification chain */
typedef struct notif_chain_elem {
    char key[MAX_STR_LEN];
    size_t key_size;
    uint32_t subs_id;
    nfc_app_cb app_cb;
    pthread_t subs_thread;
    glthread_node glue;
} notif_chain_elem;

char *nfc_get_str_op_code(nfc_op op);
notif_chain *nfc_create_notification_chain(char *);
notif_chain_elem *
nfc_create_notif_chain_elem(char *key, size_t key_size,
			    pthread_t subs_thread,
			    nfc_app_cb app_cb, uint32_t subs_id);
void nfc_register_notif_chain(notif_chain *nfc, notif_chain_elem *nfce);
void nfc_invoke_notif_chain(notif_chain *nfc,
			    void *arg, size_t arg_size,
			    char *key, size_t key_size, nfc_op op);
#endif
