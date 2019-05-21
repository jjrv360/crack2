#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "md5.h"

const int PASS_LEN=20;        // Maximum any password will be
const int HASH_LEN=33;        // Length of MD5 hash strings

int file_length(char *filename)
{
    struct stat info;
    if (stat(filename, &info) == -1) return -1;
    else return info.st_size;
}

// Given a hash and a plaintext guess, return 1 if
// the hash of the guess matches the given hash.
// That is, return 1 if the guess is correct.
int tryguess(char *hash, char *guess)
{
    // Hash the guess using MD5
    char *check = md5(guess, strlen(guess));
    // Compare the two hashes
    if (strcmp (check, hash) == 0)
    {
        free(check);
        return 1;
    }
    else
    {
        free(check);
        return 0;
    }
}

// Read in the dictionary file and return the array of strings
// and store the length of the array in size.
// This function is responsible for opening the dictionary file,
// reading from it, building the data structure, and closing the
// file.
char **read_dictionary(char *filename, int *size)
{
    int len = file_length(filename);
    char * contents = malloc(sizeof(char) * len);
    
    FILE *f = NULL;
    f = fopen(filename, "r");
    if (!f)
    {
        printf("Can't open %s for reading.\n", filename);
        exit(1);
    }
    
    fread(contents, 1, len, f);
    fclose(f);
    
    int passcount = 0;
    
    for (int k = 0; k < len; k++)
    {
        if (contents[k] == '\n')
        {
            contents[k] = '\0';
            passcount++;
        }
    }
    
    char **dictionary = malloc(passcount * sizeof(char *));
    
    dictionary[0] = contents;
    int j = 1;
    
    for (int i = 0; i < len; i++)
    {
        if (contents [i] == '\0')
        {
            dictionary[j] = &contents[i+1];
            j++;
        }
    }
    *size = passcount;
    return dictionary;
}


int main(int argc, char *argv[])
{
    if (argc < 3) 
    {
        printf("Usage: %s hash_file dict_file\n", argv[0]);
        exit(1);
    }
    
    int hlen = file_length(argv[1]);
    int dlen = file_length(argv[2]);
    // Read the dictionary file into an array of strings.
    char **dict = read_dictionary(argv[2], &dlen);
    char *hashfile = malloc(hlen);
    char hashwrite[HASH_LEN+1];

    // Open the hash file for reading.
    FILE *d = fopen(argv[1], "r");
    if (!d)
    {
        printf("Can't open file for reading\n");
        exit(1);
    }
    
    FILE *h = fopen(argv[2], "r");
    if (!h)
    {
        printf("Can't open file for reading\n");
        exit(1);
    }

    // For each hash, try every entry in the dictionary.
    // Print the matching dictionary entry.
    // Need two nested loops.
    
    
    while (fgets (hashwrite, HASH_LEN+1, d) != NULL)
    {
        hashwrite[strlen(hashwrite) - 1] = '\0';
        
        for (int i = 0; i < dlen; i++)
        {
            //printf("comparing %s, %s\n", hashwrite, dict[i]);
            if (tryguess(hashwrite, dict[i]) == 1)
            {
                printf("%s %s\n", hashwrite, dict[i]);
            }
        }
    }
    free(hashfile);
    fclose(d);
    fclose(h);
}
