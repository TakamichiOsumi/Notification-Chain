#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "notification_chain_util.h"

static void
compare_strings(char *expected, char *result, int compare_length){
    if (strncmp(expected, result, compare_length) != 0){
	fprintf(stderr,
		"the string was expected to be '%s', but it was '%s'\n",
		expected, result);
	exit(-1);
    }
}

static void
test_subnet_mask(void){
    char bit_ipv4[IPV4_BIN_SIZE];

    get_binary_format_subnet_mask(24, bit_ipv4);
    compare_strings("11111111.11111111.11111111.00000000", bit_ipv4, IPV4_BIN_SIZE);
    get_binary_format_subnet_mask(8, bit_ipv4);
    compare_strings("11111111.00000000.00000000.00000000", bit_ipv4, IPV4_BIN_SIZE);
    get_binary_format_subnet_mask(10, bit_ipv4);
    compare_strings("11111111.11000000.00000000.00000000", bit_ipv4, IPV4_BIN_SIZE);
    get_binary_format_subnet_mask(31, bit_ipv4);
    compare_strings("11111111.11111111.11111111.11111110", bit_ipv4, IPV4_BIN_SIZE);
}

static void
test_broadcast_address_calculation(){
    char bit_flags[OCTET + 1]; /* include "\0" */

    memset(bit_flags, '\0', OCTET_SIZE);

    convert_octet_decimal_to_binary(10, bit_flags);
    compare_strings("00001010", bit_flags, OCTET_SIZE);
    convert_octet_decimal_to_binary(4, bit_flags);
    compare_strings("00000100", bit_flags, OCTET_SIZE);
    convert_octet_decimal_to_binary(255, bit_flags);
    compare_strings("11111111", bit_flags, OCTET_SIZE);
    convert_octet_decimal_to_binary(188, bit_flags);
    compare_strings("10111100", bit_flags, OCTET_SIZE);
}

void
test_subnet_cardinality_calculation(){
    int expected = 254, result;

    if ((result = get_subnet_cardinality(24)) != expected){
	fprintf(stderr,
		"the subnet cardinality was expected to be %u, but it was %d",
		expected, result);
	exit(-1);
    }
}

void
test_binary_format_ipaddr(void)
{
    char *ip;
    char buf[IPV4_BIN_SIZE];

    memset(buf, '\0', IPV4_BIN_SIZE);

    /* Class A */
    ip = "10.67.95.255";
    get_binary_format_ipaddr(ip, buf);
    compare_strings("00001010.01000011.01011111.11111111",
		    buf, IPV4_BIN_SIZE);
    
    /* Class B */
    ip = "172.16.45.8";
    get_binary_format_ipaddr(ip, buf);
    compare_strings("10101100.00010000.00101101.00001000",
		    buf, IPV4_BIN_SIZE);

    /* Class C */
    ip = "192.168.1.128";
    get_binary_format_ipaddr(ip, buf);
    compare_strings("11000000.10101000.00000001.10000000",
		    buf, IPV4_BIN_SIZE);
}

void
test_network_id(void){
    char AND_op_result[IPV4_BIN_SIZE];
    char *ip, *answer_networkid, mask;
    int len1, len2;

    /* Class C */
    ip = "192.168.1.128";
    mask = 24;
    answer_networkid = "192.168.1.0";

    get_network_id(ip, mask, AND_op_result);
    len1 = strlen(answer_networkid);
    len2 = strlen(AND_op_result);
    compare_strings(answer_networkid, AND_op_result,
		    len1 <= len2 ? len1 : len2);

    /* Class B */
    ip = "172.16.45.8";
    mask = 22;
    answer_networkid = "172.16.44.0";

    get_network_id(ip, mask, AND_op_result);
    len1 = strlen(answer_networkid);
    len2 = strlen(AND_op_result);
    compare_strings(answer_networkid, AND_op_result,
		    len1 <= len2 ? len1 : len2);

    /* Class A */
    ip = "10.8.60.122";
    mask = 10;
    answer_networkid = "10.0.0.0";

    get_network_id(ip, mask, AND_op_result);
    len1 = strlen(answer_networkid);
    len2 = strlen(AND_op_result);
    compare_strings(answer_networkid, AND_op_result,
		    len1 <= len2 ? len1 : len2);
}

void
test_broadcast_address(void){
    char *ip = "192.168.1.10";
    char mask = 25;
    char *answer_broadcast_ip, broadcast_ip[IPV4_DEC_MAX_SIZE];
    int len1, len2;

    get_broadcast_address(ip, mask, broadcast_ip);
    answer_broadcast_ip = "192.168.1.127";
    len1 = strlen(answer_broadcast_ip);
    len2 = strlen(broadcast_ip);
    compare_strings(answer_broadcast_ip, broadcast_ip,
		    len1 <= len2 ? len1 : len2);
}

int
main(int argc, char **argv){

    test_subnet_mask();
    test_binary_format_ipaddr();
    test_broadcast_address_calculation();
    test_subnet_cardinality_calculation();
    test_network_id();
    test_broadcast_address();

    return 0;
}
