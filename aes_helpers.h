#ifndef AES_HELPERS_H
#define AES_HELPERS_H

#include <stddef.h>
#include <stdint.h>

int parse_hex_exact(const char *s, uint8_t *out, size_t out_len);

void print_hex_line(const uint8_t *buf, size_t len);

size_t count_hex_digits(const char *s);

#endif /* AES_HELPERS_H */
