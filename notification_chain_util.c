#include <assert.h>
#include <errno.h>
#include <math.h>
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

void
convert_octet_decimal_to_binary(int value, char *binary_mask){
    int i, b;

    /* Calculate eight(octet) times */
    for (i = OCTET - 1; i >= 0; i--){
	b = pow(2, i);
	if (value - b >= 0){
	    value = value - b;
	    binary_mask[OCTET - 1 - i] = '1';
	}else{
	    binary_mask[OCTET - 1 - i] = '0';
	}
    }
}

void
get_binary_format_subnet_mask(char mask, char *output_buffer){
    int i;

    memset(output_buffer, '0', IPV4_BIN_SIZE);
    /*
     * IPV4_BIN_SIZE - 1 : '\0'
     * IPV4_BIN_SIZE - 2 : the last bit of the last octet
     */
    output_buffer[IPV4_BIN_SIZE - 1] = '\0';
    for (i = 0; i < IPV4_BIN_SIZE - 2; i++){
	/* Put dots per each 8 bits */
	if ((i + 1) % 9 == 0){
	    output_buffer[i] = '.';
	}else{
	    if (mask > 0){
		output_buffer[i] = '1';
		mask -= 1;
	    }
	}
    }
}

int
find_dot_index(char *str, int start, int end){
    int i = 0;

    for (i = start; i < end; i++){
	if (str[i] == '.'){
	    return i;
	}
    }

    return -1;
}

void
get_binary_format_ipaddr(char *ip_addr, char *output_buffer){
    char copied_ip_addr[IPV4_DEC_MAX_SIZE];
    char *dec_octet_ary[NUM_OF_OCTETS];
    char *p;
    char one_octet_binary_format[OCTET_SIZE];
    int i, start_pos, dot_index, last_char_index = strlen(ip_addr) - 1;
    int dec_val;
    bool success = false;

    start_pos = 0;
    strncpy(copied_ip_addr, ip_addr, IPV4_DEC_MAX_SIZE);
    one_octet_binary_format[OCTET_SIZE - 1] = '\0';

    for (i = 0; i < NUM_OF_OCTETS; i++){
	/*
	 * Until the last octet conversion, find the dot delimiter index.
	 * For the last octet, process input string until the termination char.
	 */
	if (i < NUM_OF_OCTETS - 1){
	    if ((dot_index = find_dot_index(copied_ip_addr,
					start_pos, last_char_index)) == -1){
		printf("invalid input string for ip v4 format\n");
		exit(-1);
	    }
	}else
	    dot_index = strlen(ip_addr);

	/* Make one string in the copied buffer */
	copied_ip_addr[dot_index] = '\0';
	/* And, save it as an one string */
	p = &copied_ip_addr[start_pos];

	dec_val = nfc_strtol(p, &success);

	if (!success){
	    printf("invalid input string for ip v4 format\n");
	    exit(-1);
	}

	convert_octet_decimal_to_binary(dec_val, one_octet_binary_format);

	printf("%d was convert to binary format %s\n",
	       dec_val, one_octet_binary_format);

	start_pos = dot_index + 1;

	dec_octet_ary[i] = one_octet_binary_format;
	printf("dec_octet_ary[%d]=%s\n", i, dec_octet_ary[i]);
    }

    printf("debug : %i => %p\n", 0, dec_octet_ary[0]);
    printf("debug : %i => %p\n", 1, dec_octet_ary[1]);
    snprintf(output_buffer, IPV4_BIN_SIZE, "%s.%s.%s.%s",
    	     dec_octet_ary[0], dec_octet_ary[1],
    	     dec_octet_ary[2], dec_octet_ary[3]);
}

void
get_broadcast_address(char *ip_addr, char mask, char *output_buffer){
    ;
}

unsigned int
get_ip_integer_equivalent(char *ip_address){
    return 0;
}
void
get_abcd_ip_format(unsigned int ip_address, char *output_buffer){
}

void
get_network_id(char *ip_addr, char mask, char * output_buffer){
}

unsigned int
get_subnet_cardinality(char mask){
    assert(mask <= 32);

    return (int) (pow(2, (32 - mask))) - 2;
}

int
check_ip_subnet_membership(char *network_id, char mask, char *check_ip){
    return 0;
}
