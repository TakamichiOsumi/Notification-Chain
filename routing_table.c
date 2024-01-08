#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "routing_table.h"
#include "notification_chain.h"
#include "notification_chain_util.h"

static int rt_table_index = 0;

/*
 * Depend on the notification_chain's size limit 'MAX_STR_LEN'
 * because this key will be utilized by nfc infrastructure.
 */
static void
rt_create_nfc_key(char *result, char *dest, char mask){
    snprintf(result, MAX_STR_LEN, "%s/%d", dest, mask);
}

rt_entry*
rt_init_rt_table(void){
    rt_entry *table;
    char buf[MAX_STR_LEN];
    int i;

    if ((table = nfc_malloc(sizeof(rt_entry) * RT_TABLE_SIZE)) == NULL){
	perror("malloc");
	exit(-1);
    }

    /* Initialize notification chain of all entries one by one */
    for (i = 0; i < RT_TABLE_SIZE; i++){
	table[i].head = nfc_malloc(sizeof(notif_chain));
	snprintf(buf, MAX_STR_LEN, "%d", i);
	table[i].head = nfc_create_notification_chain(buf);
    }

    return table;
}

void
rt_free_rt_table(rt_entry *table){
    if (table != NULL)
	free(table);
}

/*
 * The pair of the 'dest' and 'mask' is the key.
 */
rt_entry*
rt_look_up_rt_entry(rt_entry *table, char *dest, char mask){
    rt_entry *entry;
    int i = 0;

    for (; i < rt_table_index; i++){
	entry = &table[i];
	if (strncmp(entry->dest, dest, sizeof(entry->dest)) == 0
	    && entry->mask == mask){
	    return entry;
	}
    }

    return NULL;
}

rt_entry*
rt_add_or_update_rt_entry(rt_entry *table, char *dest, char mask,
			  char *gw_ip, char *oif){
    rt_entry *entry;
    bool new_entry_created = false, entry_updated = false;
    
    entry = rt_look_up_rt_entry(table, dest, mask);

    if (entry != NULL){
	/* Updated an existing entry */
	if (strncmp(entry->gw_ip, gw_ip, sizeof(entry->gw_ip)) != 0){
	    strncpy(entry->gw_ip, gw_ip, sizeof(entry->gw_ip));
	    entry_updated = true;
	}
	if (strncmp(entry->oif, oif, sizeof(entry->oif)) != 0){
	    strncpy(entry->oif, oif, sizeof(entry->oif));
	    entry_updated = true;
	}
    }else{
	/* Otherwise, insert a new entry */
	if (rt_table_index >= RT_TABLE_SIZE){
	    fprintf(stderr, "The routing table is already full\n");
	    return NULL;
	}

	entry = &table[rt_table_index++];
	strncpy(entry->dest, dest, sizeof(entry->dest));
	entry->mask = mask;
	strncpy(entry->gw_ip, gw_ip, sizeof(entry->gw_ip));
	strncpy(entry->oif, oif, sizeof(entry->oif));

	new_entry_created = true;
    }

    /* If any changes of the routing table occur, invoke notification chain */
    if (new_entry_created || entry_updated){
	char buf[MAX_STR_LEN];

	rt_create_nfc_key(buf, dest, mask);
	nfc_invoke_notif_chain(entry->head, (void *) entry, sizeof(rt_entry),
			       buf, strlen(buf), new_entry_created ? NFC_ADD : NFC_UPDATE);

    }else{
	fprintf(stderr, "No entry has changed. The key was '%s/%d'\n", dest, mask);
    }

    return entry;
}

void
rt_dump_rt_table(rt_entry *table){
    rt_entry *entry;
    notif_chain_elem *elem;
    gldll *gllist;
    glthread_node *node;
    int i;

    for (i = 0; i < rt_table_index; i++){
	/* Dump routing table entry first */
	entry = &table[i];
	printf("[%2d] dest : %s, mask : %d, gw_ip : %s, oif : %s | ",
	       i, entry->dest, entry->mask, entry->gw_ip, entry->oif);

	/* Then, dump connected notification chain */
	gllist = entry->head->notif_chain_gldll;
	node = gllist->head;
	printf("Chain '%s' ", entry->head->ntc_name);
	while(node){
	    elem = glthread_get_app_structure(gllist, node);
	    printf(" -> [subs_id = %u]", elem->subs_id);
	    node = node->next;
	}
	printf(" -> [null]\n");
    }
}

void
rt_table_register_for_notification(rt_entry *table, char *dest, char mask,
				   char *gw_ip, char *oif,
				   nfc_app_cb app_cb, uint32_t subs_id){
    rt_entry *entry;
    notif_chain_elem *nfce;
    char key[MAX_STR_LEN];

    /* Create or update an entry */
    entry = rt_add_or_update_rt_entry(table, dest, mask, gw_ip, oif);

    /* If the routing table is already full, do nothing. */
    if (entry == NULL)
	return;

    rt_create_nfc_key(key, dest, mask);
    nfce = nfc_create_notif_chain_elem(key, strlen(key),
				       pthread_self(), app_cb, subs_id);

    nfc_register_notif_chain(entry->head, nfce);

    printf("'%s' called by '%p' thread with subs_id '%u'\n",
	   __FUNCTION__, pthread_self(), subs_id);
}
