#ifndef __ROUTING_TABLE__
#define __ROUTING_TABLE__

#include "notification_chain.h"

#define DEST_LEN 16
#define OIF_LEN 32
#define RT_TABLE_SIZE 20

typedef struct rt_entry {
    /*
     * The pair of 'dest' and 'mask' is the key
     * that represents the routing table entry.
     *
     * See rt_create_nfc_key().
     */
    char dest[DEST_LEN];
    char mask;

    /* Additional information */
    char gw_ip[DEST_LEN];
    char oif[OIF_LEN];

    /* Linked list of notification chain */
    notif_chain *head;
} rt_entry;

rt_entry *rt_init_rt_table(void);
void rt_free_rt_table(rt_entry *rt_table);
rt_entry *rt_add_or_update_rt_entry(rt_entry *table,
				    char *dest, char mask,
				    char *gw_ip, char *oif);
rt_entry* rt_look_up_rt_entry(rt_entry *table, char *dest, char mask);
void
rt_table_register_for_notification(rt_entry *table, char *dest,
				   char mask, char *gw_ip, char *oif,
				   nfc_app_cb app_cb, uint32_t subs_id);
void rt_dump_rt_table(rt_entry *table);

#endif
