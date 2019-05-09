#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "md5.h"

const int PASS_LEN=20;        // Maximum any password will be
const int HASH_LEN=33;        // Length of MD5 hash strings

// Given a hash and a plaintext guess, return 1 if
// the hash of the guess matches the given hash.
// That is, return 1 if the guess is correct.
int tryguess(char *hash, char *guess)
{
    // Hash the guess using MD5
    char  *newhash = md5(guess, strlen(guess));
    // Compare the two hashes
    if (strcmp(newhash, hash) == 0)//returns a 0 if strings are equal
    {
        free(newhash);
        return 1;
    }
    // Free any malloc'd memory
    free(newhash);
    return 0;
}

int file_length(char *filename)
{
    struct stat info;
    if(stat(filename, &info) == -1) return -1;
    else return info.st_size;
}

// Read in the dictionary file and return the array of strings
// and store the length of the array in size.
// This function is responsible for opening the dictionary file,
// reading from it, building the data structure, and closing the
// file.
char **read_dictionary(char *filename, int *size)
{
    int filelength = file_length(filename);
    char *passwords = malloc(filelength);
    
    FILE *f = fopen(filename, "r");
    if(!f) // if it returned a null
    {
        printf("Can't open %s for reading\n", filename);
        exit(1);
    }
    
    //reads in entire file in 1 go and dumps it into memory
    fread(passwords, 1, filelength, f); // where to put passwords of file, how big each thing is, how many things there are, where we got it from
    fclose(f);
    
    int length = 0;
    
    for (int i = 0; i < filelength; i++)
    {
        if(passwords[i] == '\n') 
        {
            passwords[i] = '\0';
            length++;
        }    
    }
    
    char **strings = malloc(length * sizeof(char *));
    
    // Fill in each address
    strings[0] = passwords;
    int j = 1;
    
    for (int i = 0; i < filelength - 1; i++)
    {
            if (passwords[i] == '\0')
            {
                strings[j] = &passwords[i+1];
                j++;
            }
        filelength = length;
        return strings;
    }
    
    // Print out each word
    for (int i = 0; i < length; i++) // for (int i = 0; i < j; i++)
    {
        printf("%d %s\n", i ,strings[i]);
    }
    free(strings);
    free(passwords);
    
}


int main(int argc, char *argv[])
{
    if (argc < 3) 
    {
        printf("Usage: %s hash_file dict_file\n", argv[0]);
        exit(1);
    }

    // Read the dictionary file into an array of strings.
    int dlen;
    char **dict = read_dictionary(argv[2], &dlen);
    //printf("test");

    // Open the hash file for reading.
    FILE *h = fopen(argv[1], "r");
    if(!h) // if it returned a null
    {
        printf("Can't open %s for reading\n", argv[1]);
        exit(1);
    }

    // For each hash, try every entry in the dictionary.
    char hashes[34];
    printf("dlen: %d\n",dlen);
    while (fgets(hashes, 34, h)) // != NULL is implied
    {
        hashes[strlen(hashes)-1] = '\0';
        printf("Hash: %s\n",hashes);
        //printf("%d\n hashes", dlen);
        for(int i = 0; i < dlen; i++)//issue involving dlen
        {  
            printf("%s\n",dict[i]);
            //printf("%s",dict[i],);
            //printf("tryguess %s %s\n", hashes, dict[i]);
            if(tryguess(hashes,dict[i]))
            {
                //printf("test");
                // Print the matching dictionary entry.
                // Need two nested loops.
                printf("%s %s\n",hashes, dict[i]);
            }
        }
    }
}
