#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include "notification_chain_util.h"

int
nfc_strtol(char *str, bool *success){
    long result;
    errno = 0;
    *success = false;

    result = strtol(str, NULL, 10);
    if(errno != 0){
	return 0;
    }else if (result == LONG_MIN || result == LONG_MAX){
	return 0;
    }else if (result <= INT_MIN || result >= INT_MAX){
	return 0;
    }

    *success = true;
    return (int) result;
}

void *
nfc_malloc(size_t size){
    void *p;

    if ((p = malloc(size)) == NULL){
	perror("malloc");
	exit(-1);
    }

    return p;
}

void
nfc_get_string(char *dest, size_t size_limit){
    char buf[BUF_SIZE];
    int last_index;

    assert(BUF_SIZE > size_limit);

    fgets(buf, sizeof(buf), stdin);
    last_index = strlen(buf) - 1;

    if (buf[last_index] == '\n'){
	buf[last_index] = '\0';
    }

    strncpy(dest, buf, size_limit);
}

int
nfc_get_integer_within_range(char *description,
			     int lower_limit, int upper_limit){
    char buf[BUF_SIZE];
    bool success = false;
    int val = -1;

    do {
	/* Show the prompt for user */
	printf("%s", description);

	fgets(buf, sizeof(buf), stdin);
	buf[sizeof(buf) - 1] = '\0';

	val = nfc_strtol(buf, &success);

	if (success == false ||
	    (val < lower_limit || upper_limit < val)){
	    fprintf(stderr, "invalid input value. try again\n");
	    /*
	     * This 'success' flag can be true, if the input gets
	     * converted to integer in nfc_strtol(). In this case,
	     * reset the flag and let the user input something else.
	     */
	    success = false;
	}

    }while(!success);

    return val;
}
