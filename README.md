# AES (Advanced Encryption Standard) implementado em C, seguindo o FIPS 197, em código simples (estilo “amador”) para fins educacionais.

---

Atenção

- Não use em produção: não é constante no tempo, não possui contramedidas contra ataques de canal lateral (tempo/cache/faults).
- Opera apenas em um único bloco de 16 bytes (128 bits), sem padding e sem modos de operação (similar a ECB).
- Entrada/saída em hexadecimal de tamanho fixo.

---

Estrutura dos Arquivos

- aes.h / aes.c
  - Algoritmo AES completo:
    - Tabelas: S-box e Inv S-box (FIPS 197).
    - Transformações: SubBytes, ShiftRows, MixColumns, AddRoundKey.
    - Inversas: InvSubBytes, InvShiftRows, InvMixColumns.
    - Expansão de chave (Key Expansion).
    - API pública:
      - `int aes_init(AES_ctx *ctx, const uint8_t *key, int key_bits)`
      - `void aes_encrypt_block(const AES_ctx *ctx, const uint8_t in[16], uint8_t out[16])`
      - `void aes_decrypt_block(const AES_ctx *ctx, const uint8_t in[16], uint8_t out[16])`
  - Estrutura `AES_ctx`:
    - `Nk` (palavras da chave): 4 (128), 6 (192), 8 (256)
    - `Nr` (rounds): 10, 12, 14
    - `w`: agendamento de chaves (até 60 palavras x 4 bytes)

- aes_helpers.h / aes_helpers.c
  - Utilidades para hex:
    - `count_hex_digits`: conta dígitos hex válidos em uma string.
    - `parse_hex_exact`: converte string hex para buffer exigindo tamanho exato.
    - `print_hex_line`: imprime bytes em hex (minúsculo).

- main.c
  - Interface de linha de comando para um bloco:
    - `./aes encrypt <key_hex> <plaintext_hex>`
    - `./aes decrypt <key_hex> <ciphertext_hex>`
  - key_hex: 32/48/64 chars (AES-128/192/256)
  - data_hex: 32 chars (16 bytes)

- Makefile
  - `make` compila o executável `aes`
  - `make clean` remove artefatos

---

Cabeçalhos (.h)

- aes.h
  - Include guards: `#ifndef AES_H` … `#define AES_H`
  - Includes padrão: `<stdint.h>`, `<stddef.h>`
  - Constantes:
    - `AES_BLOCK_SIZE` (16), `AES_MAX_ROUNDS` (14), `AES_MAX_ROUND_WORDS` (4\*(Nr+1) máx = 60)
  - `typedef struct AES_ctx`:
    - Campos `Nk`, `Nr`
    - `uint8_t w[AES_MAX_ROUND_WORDS][4]` (key schedule em palavras de 4 bytes)
  - Protótipos:
    - `int aes_init(AES_ctx *ctx, const uint8_t *key, int key_bits);`
    - `void aes_encrypt_block(const AES_ctx *ctx, const uint8_t in[16], uint8_t out[16]);`
    - `void aes_decrypt_block(const AES_ctx *ctx, const uint8_t in[16], uint8_t out[16]);`

- aes_helpers.h
  - Include guards: `#ifndef AES_HELPERS_H`
  - Includes padrão: `<stddef.h>`, `<stdint.h>`
  - Protótipos:
    - `int parse_hex_exact(const char *s, uint8_t *out, size_t out_len);`
      - Lê uma string hex (ignorando espaços e ‘:’) para um buffer fixo, exigindo tamanho exato.
    - `void print_hex_line(const uint8_t *buf, size_t len);`
      - Imprime como hex minúsculo contínuo, com newline.
    - `size_t count_hex_digits(const char *s);`
      - Conta apenas os dígitos hex (0-9, a-f, A-F) na string (ignora outros chars).

Observação

- main.c inclui `aes.h` e `aes_helpers.h` para usar a API do AES e as funções de hex.

---

Como Compilar

- Requisitos: gcc/clang e make
- Passo:
  - `make`

---

Como Executar (Exemplo FIPS 197)

- Vetor de teste (AES-128):
  - key: 2b7e151628aed2a6abf7158809cf4f3c
  - pt: 3243f6a8885a308d313198a2e0370734
  - ct: 3925841d02dc09fbdc118597196a0b32

- Cifrar:
  - `./aes encrypt 2b7e151628aed2a6abf7158809cf4f3c 3243f6a8885a308d313198a2e0370734`
  - Esperado: `3925841d02dc09fbdc118597196a0b32`

- Decifrar:
  - `./aes decrypt 2b7e151628aed2a6abf7158809cf4f3c 3925841d02dc09fbdc118597196a0b32`
  - Esperado: `3243f6a8885a308d313198a2e0370734`

---

Resumo do Algoritmo AES no Código

1. Estado (State)
   - Bloco de 16 bytes vira matriz 4x4 (state[r][c]) por colunas:
     - `state[r][c] = in[r + 4*c]`

2. Rodadas (Rounds)
   - Dependem da chave:
     - 128 bits: Nr=10
     - 192 bits: Nr=12
     - 256 bits: Nr=14
   - Cifragem:
     - AddRoundKey (inicial)
     - Rounds 1..Nr-1: SubBytes → ShiftRows → MixColumns → AddRoundKey
     - Round final: SubBytes → ShiftRows → AddRoundKey
   - Decifragem (ordem inversa):
     - AddRoundKey (última round key)
     - Rounds Nr-1..1: InvShiftRows → InvSubBytes → AddRoundKey → InvMixColumns
     - Final: InvShiftRows → InvSubBytes → AddRoundKey

3. SubBytes / InvSubBytes
   - Tabelas S-box/Inv S-box do FIPS 197, aplicadas byte a byte.

4. ShiftRows / InvShiftRows
   - Deslocamentos cíclicos por linha:
     - L0: 0
     - L1: 1 à esquerda (inv: 1 à direita)
     - L2: 2 à esquerda (inv: 2 à direita)
     - L3: 3 à esquerda (inv: 3 à direita)

5. MixColumns / InvMixColumns
   - Mistura de cada coluna em GF(2^8):
     - MixColumns: [02 03 01 01]
     - InvMixColumns: [0e 0b 0d 09]
   - Multiplicação via `xtime` e `gf_mul`.

6. AddRoundKey
   - XOR do estado com 16 bytes da round key (4 palavras).

7. Expansão de Chave
   - Gera 4\*(Nr+1) palavras:
     - A cada `Nk` palavras: `RotWord` → `SubWord` → XOR com Rcon no primeiro byte.
     - Para AES-256: `SubWord` extra quando `i % Nk == 4`.
   - Armazenamento em bytes evita problemas de endianness.

---

Decisões de Implementação (Estilo “Amador”)

- Estruturas e loops simples; sem macros avançadas.
- Estado como `uint8_t s[4][4]`; conversão clara bytes↔state.
- `gf_mul` com loop bit a bit e `xtime` (entendimento fácil).
- Key schedule como `ctx->w[word][4]` (bytes).
- Sem otimizações (T-Tables, intrínsecos, AES-NI).

---

Limitações e Próximos Passos

- Somente 1 bloco por vez.
- Sem modos (CBC, CTR, GCM) e sem padding.
- Sem I/O de arquivos.
- Próximos passos sugeridos:
  - Implementar CBC/CTR (chave e IV em hex).
  - Adicionar padding (PKCS#7) e leitura/escrita de arquivo.
  - Testes automatizados com vetores do NIST.
  - Opções de modo e I/O via CLI.

---

Licença

- Uso educacional e livre, sem garantias. Utilize por sua conta e risco.

---

Referências

- NIST FIPS 197 (AES) — especificação oficial.
- NIST FIPS 197-upd1 (2023) — revisão editorial com explicações e referências.
