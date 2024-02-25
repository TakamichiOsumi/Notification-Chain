#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "notification_chain_util.h"

static void
compare_strings(char *expected, char *result){
    if (strncmp(expected, result, IPV4_BIN_SIZE) != 0){
	fprintf(stderr,
		"the string was expected to be %s, but it was %s\n",
		expected, result);
	exit(-1);
    }
}

static void
test_subnet_mask(void){
    char bit_ipv4[IPV4_BIN_SIZE];

    get_binary_format_subnet_mask(24, bit_ipv4);
    compare_strings("11111111.11111111.11111111.00000000", bit_ipv4);
    get_binary_format_subnet_mask(8, bit_ipv4);
    compare_strings("11111111.00000000.00000000.00000000", bit_ipv4);
    get_binary_format_subnet_mask(10, bit_ipv4);
    compare_strings("11111111.11000000.00000000.00000000", bit_ipv4);

}

static void
test_broadcast_address_calculation(){
    char bit_flags[OCTET + 1]; /* include "\0" */
    char bin_ipv4[MAX_BIT + 1];

    memset(bit_flags, '\0', OCTET + 1);

    convert_octet_decimal_to_binary(10, bit_flags);
    compare_strings("00001010", bit_flags);
    convert_octet_decimal_to_binary(4, bit_flags);
    compare_strings("00000100", bit_flags);
    convert_octet_decimal_to_binary(255, bit_flags);
    compare_strings("11111111", bit_flags);
    convert_octet_decimal_to_binary(188, bit_flags);
    compare_strings("10111100", bit_flags);

    get_broadcast_address("192.168.1.3", 24, bin_ipv4);
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

    /* Class A */
    ip = "10.67.95.255";
    get_binary_format_ipaddr(ip, NULL);
    /* Class B */
    ip = "172.16.45.8";
    get_binary_format_ipaddr(ip, NULL);
    /* Class C */
    ip = "192.168.1.128";
    get_binary_format_ipaddr(ip, NULL);
}

int
main(int argc, char **argv){

    test_subnet_mask();
    test_binary_format_ipaddr();
    test_broadcast_address_calculation();
    test_subnet_cardinality_calculation();

    return 0;
}
