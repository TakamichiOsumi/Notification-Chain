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
test_subnet_mask_binary_conversion(void){
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
test_one_octet_binary_conversion(){
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
test_ipaddr_binary_conversion(void)
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
test_network_id_calculation(void){
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
test_broadcast_address_calculation(void){
    char *ip;
    char mask;
    char *answer_broadcast_ip, broadcast_ip[IPV4_DEC_MAX_SIZE];
    int len1, len2;

    ip = "192.168.1.10";
    mask = 25;
    get_broadcast_address(ip, mask, broadcast_ip);
    answer_broadcast_ip = "192.168.1.127";
    len1 = strlen(answer_broadcast_ip);
    len2 = strlen(broadcast_ip);
    compare_strings(answer_broadcast_ip, broadcast_ip,
		    len1 <= len2 ? len1 : len2);

    ip = "10.8.60.122";
    mask = 12;
    get_broadcast_address(ip, mask, broadcast_ip);
    answer_broadcast_ip = "10.15.255.255";
    len1 = strlen(answer_broadcast_ip);
    len2 = strlen(broadcast_ip);
    compare_strings(answer_broadcast_ip, broadcast_ip,
		    len1 <= len2 ? len1 : len2);
}

void
compare_integers(char *ip, unsigned int value){
    bool valid;
    int tmp;

    tmp = get_ip_integer_equivalent(ip, &valid);
    if (valid == false){
	fprintf(stderr,
		"invalid ipv4 format of the input\n");
	exit(-1);
    }

    if (tmp != value){
	fprintf(stderr,
		"the integer value was expected to be '%d', but it was '%d'\n",
		value, tmp);
	exit(-1);
    }
}

void
test_ip_equivalent_integer_calculation(void){
    char output_buffer[IPV4_DEC_MAX_SIZE];
    unsigned int input;

    /* Test 1 */
    input = 3232250950;
    compare_integers("192.168.60.70", input);
    get_abcd_ip_format(input, output_buffer);
    compare_strings("192.168.60.70", output_buffer, IPV4_DEC_MAX_SIZE);

    /* Test 2 */
    input =  168460388;
    compare_integers("10.10.128.100", input);
    get_abcd_ip_format(input, output_buffer);
    compare_strings("10.10.128.100", output_buffer, IPV4_DEC_MAX_SIZE);
}

void
test_ip_memberships(){
    if (check_ip_subnet_membership("192.168.1.0", 24, "192.168.1.2") != 1)
	exit(-1);

    if (check_ip_subnet_membership("192.168.1.0", 24, "192.168.1.3") != 1)
	exit(-1);

    if (check_ip_subnet_membership("192.168.1.0", 24, "192.168.1.4") != 1)
	exit(-1);
}

int
main(int argc, char **argv){

    test_subnet_mask_binary_conversion();
    test_ipaddr_binary_conversion();
    test_one_octet_binary_conversion();
    test_subnet_cardinality_calculation();
    test_network_id_calculation();
    test_broadcast_address_calculation();
    test_ip_equivalent_integer_calculation();
    test_ip_memberships();

    return 0;
}
