#include <stdio.h>
#include <stdlib.h>

#include "fasta.h"

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s fasta-file\n", argv[0]);
        return 1;
    }

    char *fasta_buf = load_file(argv[1]), *buf = fasta_buf;
    if (!fasta_buf)
    {
        fprintf(stderr, "Error reading file %s.\n", argv[1]);
        return 1;
    }

    while (*buf)
    {
        char *header = read_fasta_header(&buf);
        char *seq = read_fasta_sequence(&buf);
        printf("%s\t%s\n", header, seq);
    }
    free(fasta_buf);

    return 0;
}
