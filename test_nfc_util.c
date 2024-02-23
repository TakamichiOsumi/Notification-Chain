#include <stdio.h>
#include <stdlib.h>
#include "notification_chain_util.h"

void
test_broadcast_address_calculation(){
    ;
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
