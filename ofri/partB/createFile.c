#include <stdio.h>
#include <stdlib.h>

/*
 * help function to create 100MB file
 * source - https://stackoverflow.com/questions/16776565/how-to-quickly-create-large-files-in-c
 */
int main() {
    const int FILE_SIZE = 1024*98; //size in MB
    const int BUFFER_SIZE = 1024;
    char buffer [BUFFER_SIZE + 1];
    int i;
    for(i = 0; i < BUFFER_SIZE; i++)
        buffer[i] = (char)('a');
    buffer[BUFFER_SIZE] = '\0';

    FILE *pFile = fopen ("send.txt", "w");
    for (i = 0; i < FILE_SIZE-1; i++)
        fprintf(pFile, "%s", buffer);
    fprintf(pFile, "%s", "0");


    fclose(pFile);
    return 0;
}