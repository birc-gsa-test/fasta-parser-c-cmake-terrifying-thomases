#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fasta.h"

struct fasta_rec
{
    const char *name;
    const char *seq;
    struct fasta_rec *next;
};

static struct fasta_rec *
push_rec(const char *name, const char *seq, struct fasta_rec *next)
{
    struct fasta_rec *rec = malloc(sizeof *rec);
    assert(rec);
    *rec = (struct fasta_rec){.name = name, .seq = seq, .next = next};
    return rec;
}

static void free_recs(struct fasta_rec *rec)
{
    // Don't free the strings, they are in a separate buffer
    struct fasta_rec *next;
    for (; rec; rec = next)
    {
        next = rec->next;
        free(rec);
    }
}

static char const *get_seq(struct fasta_rec *rec, char const *name)
{
    for (; rec; rec = rec->next)
    {
        if (strcmp(name, rec->name) == 0)
            return rec->seq;
    }
    return 0; // no such record
}

struct fasta_rec *parse_recs(char *buf)
{
    struct fasta_rec *recs = NULL;
    while (*buf)
    {
        char *header = read_fasta_header(&buf);
        char *seq = read_fasta_sequence(&buf);
        recs = push_rec(header, seq, recs);
    }
    return recs;
}

int main(int argc, char const *argv[])
{
    if (argc < 2 || argc > 3)
    {
        fprintf(stderr, "Usage: get-subseqs fasta-file [coordinates]\n");
        return 1;
    }

    // Reading in Simple-FASTA records
    char *fasta_buf = load_file(argv[1]);
    if (!fasta_buf)
    {
        fprintf(stderr, "Error reading file %s.\n", argv[1]);
        perror("Open error");
        return 1;
    }
    struct fasta_rec *recs = parse_recs(fasta_buf);
    assert(recs); // recs should not be empty

    // Then get the coordinates from either a file or stdin
    FILE *coordf = (argc < 3 || (strcmp(argv[2], "-") == 0))
                       ? stdin
                       : fopen(argv[2], "r");
    if (!coordf)
    {
        fprintf(stderr, "Couldn't open coordinates file.\n");
        perror("Open error");
        return 1;
    }

    char name[500 + 1]; // Assuming here that names are less than 500 chars.
    int from, to;
    if (feof(coordf))
    {
        printf("the file is already at its end");
    }
    while (!feof(coordf))
    {
        int no_fields = fscanf(coordf, "%500s\t%d\t%d\n", name, &from, &to);
        if (no_fields == -1)
        {
            // This happens if we see an empty line. It happens with empty files,
            // but otherwise we don't really care about it for well formed input.
            // Just move along to the next line.
            continue;
        }

        char const *seq = get_seq(recs, name);
        if (!seq)
        {
            fprintf(stderr, "Error: %s is not a record.\n", name);
            return 2;
        }
        printf("%.*s\n", to - from, seq + from - 1);
    }

    // Done, so we clean up and return
    fclose(coordf);
    free_recs(recs);
    free(fasta_buf);

    return 1;
}