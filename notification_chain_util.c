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

int
convert_octet_binary_to_decimal(char *binary){
    int i, sum = 0;

    /* Calculate eight(octet) times */
    for (i = 0; i < OCTET; i++){
	if (binary[i] == '1'){
	    sum += pow(2, OCTET - 1 - i);
	}
    }

    return sum;
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
    for (i = 0; i < IPV4_BIN_SIZE - 1; i++){
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

/*
 * The caller must ensure 'output_buffer' has the length of IPV4_BIN_SIZE.
 */
void
get_binary_format_ipaddr(char *ip_addr, char *output_buffer){
    char *p, copied_ip_addr[IPV4_DEC_MAX_SIZE];
    char binary_octets[NUM_OF_OCTETS][OCTET_SIZE];
    int iter, decimal_val, dot_index, start_pos = 0;
    bool success = false;

    /* Create a copied input to make each octet separate */
    strncpy(copied_ip_addr, ip_addr, IPV4_DEC_MAX_SIZE);

    /*
     * Until the last octet conversion, find the dot delimiter index.
     * For the last octet, process input string until the termination character,
     * since there is no dot any more.
     */
    for (iter = 0; iter < NUM_OF_OCTETS; iter++){

	if (iter == NUM_OF_OCTETS - 1){
	    dot_index = strlen(ip_addr);
	}else{
	    if ((dot_index = find_dot_index(copied_ip_addr,
					    start_pos, strlen(ip_addr) - 1)) == -1){
		fprintf(stderr, "invalid input string for ip v4 format\n");
		return;
	    }
	}

	/* Make one string, by utilizing the copied buffer */
	p = &copied_ip_addr[start_pos];
	copied_ip_addr[dot_index] = '\0';

	/* Make ready for the buffer of a new octet string */
	binary_octets[iter][OCTET_SIZE - 1] = '\0';

	/* Convert the string decimal value to binary format */
	decimal_val = nfc_strtol(p, &success);
	if (!success){
	    fprintf(stderr, "failed to convert a parsed string\n");
	    return;
	}
	convert_octet_decimal_to_binary(decimal_val, binary_octets[iter]);

	/* For the next iteration, shift the start position to search dots */
	start_pos = dot_index + 1;
    }

    /* Build the output string */
    snprintf(output_buffer, IPV4_BIN_SIZE, "%s.%s.%s.%s",
	     binary_octets[0], binary_octets[1],
	     binary_octets[2], binary_octets[3]);
    output_buffer[IPV4_BIN_SIZE - 1] = '\0';
}

void
make_mask_complement(char mask, char *mask_complement){
    int i, num_of_zeros;

    num_of_zeros = 32 - (32 - mask);

    for (i = 0; i < IPV4_BIN_SIZE - 1; i++){
	/* Put dots per each 8 bits */
	if ((i + 1) % 9 == 0){
	    mask_complement[i] = '.';
	}else{
	    if (num_of_zeros > 0){
		mask_complement[i] = '0';
		num_of_zeros--;
	    }else{
		mask_complement[i] = '1';
	    }
	}
    }

}

/*
 * The caller must ensure 'output_buffer' has the length of IPV4_DEC_MAX_SIZE;
 */
void
get_broadcast_address(char *ip_addr, char mask, char *output_buffer){
    char network_id[IPV4_DEC_MAX_SIZE];
    char binary_network_id[IPV4_BIN_SIZE];
    char mask_complement[IPV4_BIN_SIZE];
    char or_result[IPV4_BIN_SIZE];
    int i;

    output_buffer[0] = '\0';

    memset(network_id, '\0', IPV4_DEC_MAX_SIZE);
    memset(binary_network_id, '\0', IPV4_BIN_SIZE);
    memset(mask_complement, '\0', IPV4_BIN_SIZE);
    memset(or_result, '\0', IPV4_BIN_SIZE);

    get_network_id(ip_addr, mask, network_id);
    get_binary_format_ipaddr(network_id, binary_network_id);
    make_mask_complement(mask, mask_complement);

    /* printf("debug : binary network id = %s\n", binary_network_id); */
    /* printf("debug : mask complement   = %s\n", mask_complement); */

    /* OR operation */
    for (i = 0; i < IPV4_BIN_SIZE - 1; i++){
	if (binary_network_id[i] == '.' && mask_complement[i] == '.'){
	    or_result[i] = '.';
	    continue;
	}

	if (binary_network_id[i] == '1' || mask_complement[i] == '1'){
	    or_result[i] = '1';
	}else{
	    or_result[i] = '0';
	}
    }
    or_result[IPV4_BIN_SIZE - 1] = '\0';

    /* printf("debug : or_result = %s\n", or_result); */

    get_decimal_ipaddr_from_binary(or_result, output_buffer);
}

unsigned int
get_ip_integer_equivalent(char *ip_address, bool *valid){
    unsigned long result = 0;
    char *p, copied_ip_addr[IPV4_DEC_MAX_SIZE];
    int iter, decimal_val, start_pos = 0, dot_index;
    bool success = false;

    strncpy(copied_ip_addr, ip_address, IPV4_DEC_MAX_SIZE);

    for (iter = 0; iter < NUM_OF_OCTETS; iter++){

	/* Find the dot character at first */
	if (iter == NUM_OF_OCTETS - 1){
	    /*
	     * The last octet doesn't have a dot at the end.
	     * Its substitute is the string length.
	     */
	    dot_index = strlen(ip_address);
	}else{
	    if ((dot_index = find_dot_index(copied_ip_addr,
					    start_pos, strlen(ip_address) - 1)) == -1){
		fprintf(stderr, "invalid input string for ip v4 format\n");
		*valid = false;
		return 0;
	    }
	}

	/* Make one string, by utilizing the copied buffer */
	p = &copied_ip_addr[start_pos];
	copied_ip_addr[dot_index] = '\0';

	/* Convert the string decimal value to integer */
	decimal_val = nfc_strtol(p, &success);
	if (!success){
	    fprintf(stderr, "failed to convert a parsed string\n");
	    *valid = false;
	    return 0;
	}

	result += decimal_val * pow(256, (NUM_OF_OCTETS - 1 - iter));

	start_pos = dot_index + 1;
    }

    *valid = true;
    return result;
}

/*
 * The caller must ensure 'output_buffer' has the length of IPV4_DEC_MAX_SIZE.
 */
void
get_abcd_ip_format(unsigned int ip_address, char *output_buffer){
    int i;
    unsigned int remainder, quotient, divisor, copied_ip_address;
    int tmp_dec_buffer[4]; /* for decimal values of all octets */

    copied_ip_address = ip_address;
    memset(output_buffer, '\0', IPV4_DEC_MAX_SIZE);

    for (i = NUM_OF_OCTETS - 1; i >= 0; i--){
	divisor = pow(256, (i));
	quotient = copied_ip_address / divisor;
	remainder = copied_ip_address % divisor;
	copied_ip_address = copied_ip_address - (quotient * divisor);
	tmp_dec_buffer[i] = quotient;
    }

    snprintf(output_buffer, IPV4_DEC_MAX_SIZE, "%d.%d.%d.%d",
	     tmp_dec_buffer[3], tmp_dec_buffer[2],
	     tmp_dec_buffer[1], tmp_dec_buffer[0]);
}

/*
 * The caller must ensure 'output_buffer' has the length of IPV4_DEC_MAX_SIZE.
 */
void
get_decimal_ipaddr_from_binary(char *binary_ipaddr, char *output_buffer){
    char *p, copied_binary_ipaddr[IPV4_BIN_SIZE];
    char decimal_buf[IPV4_DEC_MAX_SIZE];
    int i, decimal_val, terminal_index = 0;

    memset(copied_binary_ipaddr, '\0', IPV4_BIN_SIZE);
    strncpy(copied_binary_ipaddr, binary_ipaddr, IPV4_BIN_SIZE);

    for (i = 0; i < NUM_OF_OCTETS; i++){
	/* Get one octet sequence */
	p = &copied_binary_ipaddr[(i * 9)];

	/* Replace dot delimiter with termination character */
	copied_binary_ipaddr[((i + 1) * 9) - 1] = '\0';

	/* Convert the octet sequence to decimal notation and make it string */
	decimal_val = convert_octet_binary_to_decimal(p);

	/* Change the decimal value from integer to string */
	sprintf(decimal_buf, (i == NUM_OF_OCTETS - 1) ? "%d" : "%d.", decimal_val);
	terminal_index += strlen(decimal_buf);

	/* Paste the string decimal notation to the output buffer */
	strlcat(output_buffer, decimal_buf, IPV4_DEC_MAX_SIZE);
    }

    /* Write the termination character in the output buffer */
    output_buffer[terminal_index] = '\0';
}

/*
 * The caller must ensure 'output_buffer' has the length of IPV4_BIN_SIZE.
 */
void
get_network_id(char *ip_addr, char mask, char *output_buffer){
    char binary_ipv4[IPV4_BIN_SIZE];
    char binary_subnet_mask[IPV4_BIN_SIZE];
    char binary_network_id[IPV4_BIN_SIZE];
    int i;
    char c1, c2;

    /*
     * For safe strlcat(), insert a terminal character.
     *
     * Otherwise, newly created strings for the output
     * will be pasted after some junk characters in the buffer.
     */
    output_buffer[0] = '\0';
    memset(binary_subnet_mask, '\0', IPV4_BIN_SIZE);
    memset(binary_network_id, '\0', IPV4_BIN_SIZE);

    get_binary_format_ipaddr(ip_addr, binary_ipv4);
    get_binary_format_subnet_mask(mask, binary_subnet_mask);

    /* AND operation */
    for (i = 0; i < IPV4_BIN_SIZE - 1; i++){
	c1 = binary_ipv4[i];
	c2 = binary_subnet_mask[i];

	if (c1 == '.' && c2 == '.'){
	    binary_network_id[i] = '.';
	    continue;
	}

	if (c1 == '1' && c2 == '1'){
	    binary_network_id[i] = '1';
	}else if ((c1 == '1' && c2 == '0') ||
		 (c1 == '0' && c2 == '1') ||
		 (c1 == '0' && c2 == '0'))
	    binary_network_id[i] = '0';
	else
	    assert(0);
    }
    /* Terminate the string */
    binary_network_id[IPV4_BIN_SIZE - 1] = '\0';

    printf("debug : Input IPv4 address    : '%s'\n", ip_addr);
    printf("debug : Binary IPv4 address   : '%s'\n", binary_ipv4);
    printf("debug : Binary subnet mask    : '%s'\n", binary_subnet_mask);
    printf("debug : Execute AND operation : '%s'\n", binary_network_id);

    get_decimal_ipaddr_from_binary(binary_network_id, output_buffer);
}

unsigned int
get_subnet_cardinality(char mask){
    assert(mask <= 32);

    return (int) (pow(2, (32 - mask))) - 2;
}

int
check_ip_subnet_membership(char *network_id, char mask, char *check_ip){
    char broadcast_decimal_ip[IPV4_BIN_SIZE];
    unsigned int int_network_ip, int_broadcast_ip, target_ip;
    bool valid;

    int_network_ip = get_ip_integer_equivalent(network_id, &valid);
    if (valid != true){
	fprintf(stderr,
		"invalid input for the conversion of ip address to integer. the ip was %s\n",
		network_id);
	exit(-1);
    }

    get_broadcast_address(network_id, mask, broadcast_decimal_ip);
    int_broadcast_ip = get_ip_integer_equivalent(broadcast_decimal_ip, &valid);
    if (valid != true){
	fprintf(stderr,
		"invalid input for the conversion of ip address to integer. the ip was %s\n",
		broadcast_decimal_ip);
	exit(-1);
    }

    target_ip = get_ip_integer_equivalent(check_ip, &valid);
    if (valid != true){
	fprintf(stderr,
		"invalid input for the conversion of ip address to integer. the ip was %s\n",
		check_ip);
	exit(-1);
    }

    if (int_network_ip <= target_ip && target_ip <= int_broadcast_ip)
	return 1;
    else
	return 0;
}
