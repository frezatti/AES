#include "aes_helpers.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

static int hex_nibble(char c) {
  if (c >= '0' && c <= '9') {
    return c - '0';
  }
  if (c >= 'a' && c <= 'f') {
    return 10 + (c - 'a');
  }
  if (c >= 'A' && c <= 'F') {
    return 10 + (c - 'A');
  }
  return -1;
}

size_t count_hex_digits(const char *s) {
  size_t n = 0;
  size_t i = 0;
  while (s[i] != '\0') {
    char c = s[i];
    if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
        (c >= 'A' && c <= 'F')) {
      n++;
    }
    i++;
  }
  return n;
}

int parse_hex_exact(const char *s, uint8_t *out, size_t out_len) {
  size_t need_digits = out_len * 2;
  size_t have_digits = count_hex_digits(s);
  size_t i = 0;
  size_t out_i = 0;
  int hi = -1;

  if (have_digits != need_digits) {
    return -1;
  }

  while (s[i] != '\0' && out_i < out_len) {
    char c = s[i++];
    if (c == ' ' || c == ':' || c == '\t' || c == '\n' || c == '\r') {
      continue;
    }
    if (c == 'x' || c == 'X') {
      continue;
    }

    {
      int v = hex_nibble(c);
      if (v < 0) {
        return -1;
      }
      if (hi < 0) {
        hi = v;
      } else {
        out[out_i++] = (uint8_t)((hi << 4) | v);
        hi = -1;
      }
    }
  }

  if (out_i != out_len || hi != -1) {
    return -1;
  }
  return 0;
}

void print_hex_line(const uint8_t *buf, size_t len) {
  size_t i;
  for (i = 0; i < len; i++) {
    printf("%02x", (int)buf[i]);
  }
  printf("\n");
}
