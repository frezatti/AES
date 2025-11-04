#include "aes.h"
#include "aes_helpers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*  Uso:
     ./aes encrypt <key_hex> <plaintext_hex>
     ./aes decrypt <key_hex> <ciphertext_hex>
   - key_hex deve ser 32, 48, or 64 hex chars (128/192/256 bits).
   - data  deve ser 32 hex chars (16 bytes).
*/

static int detect_key_bits_from_hex_len(size_t hex_digits) {
  if (hex_digits == 32)
    return 128;
  if (hex_digits == 48)
    return 192;
  if (hex_digits == 64)
    return 256;
  return -1;
}

static void print_usage(const char *prog) {
  printf("Usage:\n");
  printf("  %s encrypt <key_hex> <plaintext_hex>\n", prog);
  printf("  %s decrypt <key_hex> <ciphertext_hex>\n", prog);
  printf("\n");
  printf("Notes:\n");
  printf("- key_hex length: 32/48/64 hex chars for AES-128/192/256\n");
  printf("- data_hex length: 32 hex chars (exactly one block)\n");
  printf("- prints result as hex on one line\n");
  printf("\n");
  printf("Test example (FIPS 197):\n");
  printf("  key = 2b7e151628aed2a6abf7158809cf4f3c\n");
  printf("  pt  = 3243f6a8885a308d313198a2e0370734\n");
  printf("  ct  = 3925841d02dc09fbdc118597196a0b32\n");
}

int main(int argc, char **argv) {
  const char *mode;
  const char *key_hex;
  const char *data_hex;
  int key_bits;
  AES_ctx ctx;
  uint8_t key[32]; /* up to 256-bit key */
  uint8_t in[16];
  uint8_t out[16];

  if (argc != 4) {
    print_usage(argv[0]);
    return 1;
  }

  mode = argv[1];
  key_hex = argv[2];
  data_hex = argv[3];

  {
    size_t key_hex_digits = count_hex_digits(key_hex);
    key_bits = detect_key_bits_from_hex_len(key_hex_digits);
    if (key_bits < 0) {
      fprintf(stderr, "Error: key_hex must be 32/48/64 hex chars.\n");
      return 1;
    }
    if (parse_hex_exact(key_hex, key, (size_t)(key_bits / 8)) != 0) {
      fprintf(stderr, "Error: invalid key_hex.\n");
      return 1;
    }
  }

  {
    size_t data_hex_digits = count_hex_digits(data_hex);
    if (data_hex_digits != 32) {
      fprintf(stderr, "Error: data hex must be 32 hex chars (16 bytes).\n");
      return 1;
    }
    if (parse_hex_exact(data_hex, in, 16) != 0) {
      fprintf(stderr, "Error: invalid data hex.\n");
      return 1;
    }
  }

  if (aes_init(&ctx, key, key_bits) != 0) {
    fprintf(stderr, "Error: unsupported key size.\n");
    return 1;
  }

  if (strcmp(mode, "encrypt") == 0) {
    aes_encrypt_block(&ctx, in, out);
    print_hex_line(out, 16);
  } else if (strcmp(mode, "decrypt") == 0) {
    aes_decrypt_block(&ctx, in, out);
    print_hex_line(out, 16);
  } else {
    fprintf(stderr, "Error: mode must be 'encrypt' or 'decrypt'.\n");
    return 1;
  }

  return 0;
}
