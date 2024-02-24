#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "notification_chain_util.h"

static void
compare_strings(char *expected, char *result){
    if (strncmp(expected, result, OCTET + 1) != 0){
	fprintf(stderr,
		"the string was expected to be %s, but it was %s",
		expected, result);
	exit(-1);
    }
}

static void
test_broadcast_address_calculation(){
    char bit_flags[OCTET + 1]; /* include "\0" */

    memset(bit_flags, '0', OCTET + 1);
    bit_flags[OCTET] = '\0';

    convert_octet_decimal_to_binary(10, bit_flags);
    compare_strings("00001010", bit_flags);
    convert_octet_decimal_to_binary(4, bit_flags);
    compare_strings("00000100", bit_flags);
    convert_octet_decimal_to_binary(255, bit_flags);
    compare_strings("11111111", bit_flags);
    convert_octet_decimal_to_binary(188, bit_flags);
    compare_strings("10111100", bit_flags);
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

int
main(int argc, char **argv){

    test_broadcast_address_calculation();
    test_subnet_cardinality_calculation();

    return 0;
}
