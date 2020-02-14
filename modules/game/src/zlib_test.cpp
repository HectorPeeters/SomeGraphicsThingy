#include <zlib.h>
#include <stdio.h>

#include "modules/game/src/puff.c"

int main()
{
    unsigned char data[] = {0x08, 0xd7, 0x63, 0xf8, 0xcf, 0xc0, 0xc0, 0xf0, 0x9f, 0x81, 0x9f, 0x81, 0xe1, 0x3f, 0xc3, 0x7f, 0x06, 0x00, 0x1b, 0xf9, 0x03, 0xfe};

    // if ((data[0] * 256 + data[1]) % 31 != 0)
    // {
    //     printf("ERROR: 31");
    //     return 1;
    // }

    // if ((data[0] & 15) != 8 || ((data[0] >> 4) & 15) > 7)
    // {
    //     printf("ERROR: sliding window");
    //     return 1;
    // }

    // if (((data[1] >> 5) & 1) != 0)
    // {
    //     printf("ERROR: additional flags");
    //     return 1;
    // }

    unsigned char uncompressed[200];

    unsigned long compressed_size = 22;
    unsigned long uncompressed_size = 200;

    int result = uncompress(
        uncompressed,
        &uncompressed_size,
        data,
        compressed_size);

    // int result = puff(uncompressed, &uncompressed_size, data, &compressed_size);

    for (int i = 0; i < uncompressed_size; i++)
    {
        if (i % 7 == 0)
            printf("\n");
        if (i % 7 == 1)
            printf(": ");
        printf("%02x ", uncompressed[i]);
    }

    printf("\nresult: %d size: %lu \n", result, uncompressed_size);

    return 0;
}