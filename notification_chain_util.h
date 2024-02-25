#ifndef __NFC_UTIL__
#define __NFC_UTIL__

#include <stdbool.h>

#define BUF_SIZE 1024
#define OCTET 8
#define NUM_OF_OCTETS 4
#define MAX_BIT (OCTET * NUM_OF_OCTETS)
#define OCTET_SIZE ((int) strlen("XXXXXXXX") + 1)
#define IPV4_DEC_MAX_SIZE ((int) strlen("XXX.XXX.XXX.XXX") + 1)
#define IPV4_BIN_SIZE ((int) strlen("XXXXXXXX.XXXXXXXX.XXXXXXXX.XXXXXXXX") + 1)

void *nfc_malloc(size_t size);
int nfc_strtol(char *str, bool *success);
void nfc_get_string(char *dest, size_t size_limit);
int nfc_get_integer_within_range(char *description,
				 int lower_limit, int upper_limit);

/* Network utilities for future extension */
void convert_octet_decimal_to_binary(int value, char *binary_mask);
void get_broadcast_address(char *ip_addr, char mask, char *output_buffer);
void get_binary_format_subnet_mask(char mask, char *output_buffer);
void get_binary_format_ipaddr(char *ip_addr, char *output_buffer);
unsigned int get_ip_integer_equivalent(char *ip_address);
void get_abcd_ip_format(unsigned int ip_address, char *output_buffer);
void get_network_id(char *ip_addr, char mask, char * output_buffer);
unsigned int get_subnet_cardinality(char mask);
int check_ip_subnet_membership(char *network_id, char mask, char *check_ip);

#endif
