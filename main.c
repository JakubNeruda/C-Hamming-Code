
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


bool encode_word(uint8_t const *flags);

bool decode_word(uint8_t *flags, int index);

const uint64_t encoding_bits[] = {1UL << 38, 1UL << 37, 1UL << 35,
                                  1UL << 31, 1UL << 23, 1UL << 7};
const uint64_t information_bits[] = {0x15555555d5, 0x1333333333, 0x70f0f0f0f,
                                     0x7f00ff00, 0x7fff00, 0xff};

bool code(bool encode) {
    int input_int = getchar();
    int index = 0;
    bool complete = true;

    const int ninth_bit = 1 << 8;
    const int eighth_bit = 1 << 7;
    const int first_bit = 1;

    uint8_t flags[5] = {0};
    uint8_t number_of_bits = encode ? 4 : 5;

    while (input_int != EOF) {
        complete = false;
        flags[index % number_of_bits] = input_int % ninth_bit;

        if (index % number_of_bits == number_of_bits - 1) {
            if (!encode) {
                if (flags[0] & eighth_bit || flags[4] & first_bit) {
                    fprintf(stderr, "Wrong code word\n");
                    return false;
                }
                decode_word(flags, index);
            } else {
                encode_word(flags);
            }
            complete = true;
            for (int i = 0; i < number_of_bits; i++) {
                flags[i] = 0;
            }
        }
        if (input_int >= ninth_bit) {
            input_int >>= 8;
        } else {
            input_int = getchar();
        }
        index++;
    }

    if (!complete) {
        if (encode) {
            encode_word(flags);
        } else {
            fprintf(stderr, "Wrong code word\n");
            return false;
        }
    }
    return true;
}

bool encode(void) { return code(true); }

bool parity_check(uint64_t word) {
    int number_of_bits = 0;
    while (word != 0) {
        if (word & 0x1) {
            number_of_bits++;
        }
        word >>= 1;
    }
    return number_of_bits % 2 == 1;
}

bool encode_word(uint8_t const *flags) {
    uint8_t byte_array[5] = {0};

    byte_array[4] = flags[3] << 1;
    byte_array[4] &= 0x7e;
    byte_array[3] = flags[2] << 2;
    byte_array[3] |= (flags[3] >> 6);
    byte_array[2] = flags[1] << 2;
    byte_array[2] |= (flags[2] >> 6);
    byte_array[2] &= 0x7f;
    byte_array[1] = flags[0] << 3;
    byte_array[1] |= (flags[1] >> 5);
    byte_array[1] &= 0x7f;
    byte_array[0] = flags[0] >> 4;
    byte_array[0] |= ((flags[0] >> 3) & 0x10);
    byte_array[0] &= 0x17;

    uint64_t word = byte_array[0];
    for (int i = 1; i < 5; i++) {
        word <<= 8;
        word |= byte_array[i];
    }

    for (unsigned int i = 0; i < sizeof(encoding_bits) / sizeof(uint64_t); i++) {
        if (parity_check(word & information_bits[i])) {
            word |= encoding_bits[i];
        }
    }

    uint8_t encoded_bytes[5];
    for (int i = 4; i >= 0; i--) {
        encoded_bytes[i] = word;
        word >>= 8;
    }
    for (int i = 0; i < 5; i++) {
        putchar(encoded_bytes[i]);
    }
    return true;
}

bool decode(void) { return code(false); }

bool decode_word(uint8_t *flags, int index) {
    uint8_t byte_array[4] = {0};

    uint8_t error_bit = 0;
    for (int i = 0; i < 40; i++) {
        uint8_t byte = i / 8;
        uint8_t bit = 0x80 >> (i % 8);
        if (flags[byte] & bit) {
            error_bit ^= i;
        }
    }

    if (error_bit) {
        uint8_t byte = error_bit / 8;
        uint8_t bit = 0x80 >> (error_bit % 8);
        flags[byte] ^= bit;
        fprintf(stderr, "One-bit error in byte %d\n", (index - 4 + byte));
    }

    byte_array[0] = (flags[0] & 0x07) << 4;
    byte_array[0] |= (flags[0] & 0x10) << 3;
    byte_array[0] |= (flags[1] & 0x78) >> 3;
    byte_array[1] = flags[1] << 5;
    byte_array[1] |= (flags[2] & 0x7c) >> 2;
    byte_array[2] = flags[2] << 6;
    byte_array[2] |= (flags[3] & 0xfc) >> 2;
    byte_array[3] = flags[3] << 6;
    byte_array[3] |= (flags[4] & 0x7e) >> 1;

    for (int i = 0; i < 4; i++) {
        putchar(byte_array[i]);
    }
    return true;
}

int main(int argc, char *argv[]) {
    if (argc > 2) {
        fprintf(stderr, "Usage: %s (-e|-d)\n", argv[0]);
        return EXIT_FAILURE;
    }
    bool is_success = false;
    if (argc == 1 || (strcmp(argv[1], "-e") == 0)) {
        is_success = encode();
    } else if (strcmp(argv[1], "-d") == 0) {
        is_success = decode();
    } else {
        fprintf(stderr, "Unknown argument: %s\n", argv[1]);
    }
    return is_success ? EXIT_SUCCESS : EXIT_FAILURE;
}
