#ifndef FASTA_H
#define FASTA_H

// Load the entire content of a file into a buffer.
char *load_file(const char *fname);

// Reads a header from a string in fasta format.
// Move buf forward to the beginning of the following sequence
// and return the header.
char *read_fasta_header(char **buf);

// Reads a sequence from a string in fasta format. Like above, buf moves
// forward to the next header or the end of file while we compact
// the sequence and return a pointer to it.
char *read_fasta_sequence(char **buf);

#endif // FASTA_H
