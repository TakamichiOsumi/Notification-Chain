#ifndef __NFC_UTIL__
#define __NFC_UTIL__

#define BUF_SIZE 1024

void *nfc_malloc(size_t size);
int nfc_strtol(char *str, bool *success);
void nfc_get_string(char *dest, size_t size_limit);
int nfc_get_integer_within_range(char *description,
				 int lower_limit, int upper_limit);


#endif
