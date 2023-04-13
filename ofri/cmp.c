#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    FILE *fp1, *fp2;
    char buffer1[BUFFER_SIZE], buffer2[BUFFER_SIZE];
    int ignore_case = 0, verbose = 0, result = 0, line_number = 1;
    size_t len1, len2;

    // Parse command line arguments
    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Usage: %s <file1> <file2> [-v] [-i]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    if (argc == 4) {
        if (strcmp(argv[3], "-v") == 0) {
            verbose = 1;
        } else if (strcmp(argv[3], "-i") == 0) {
            ignore_case = 1;
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[3]);
            exit(EXIT_FAILURE);
        }
    }
    if (argc == 5) {
        if (strcmp(argv[3], "-v") == 0 && strcmp(argv[4], "-i") == 0) {
            verbose = 1;
            ignore_case = 1;
        } else if (strcmp(argv[3], "-i") == 0 && strcmp(argv[4], "-v") == 0) {
            verbose = 1;
            ignore_case = 1;
        } else {
            fprintf(stderr, "Unknown options: %s %s\n", argv[3], argv[4]);
            exit(EXIT_FAILURE);
        }
    }

    // Open the input files
    fp1 = fopen(argv[1], "r");
    if (fp1 == NULL) {
        fprintf(stderr, "Error opening file: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    fp2 = fopen(argv[2], "r");
    if (fp2 == NULL) {
        fprintf(stderr, "Error opening file: %s\n", argv[2]);
        fclose(fp1);
        exit(EXIT_FAILURE);
    }

    // Compare the files line by line
    while (fgets(buffer1, BUFFER_SIZE, fp1) != NULL && fgets(buffer2, BUFFER_SIZE, fp2) != NULL) {
        len1 = strlen(buffer1);
        len2 = strlen(buffer2);
        if (len1 != len2) {
            result = 1;
            break;
        }
        if (ignore_case) {
            for (size_t i = 0; i < len1; i++) {
                if (tolower(buffer1[i]) != tolower(buffer2[i])) {
                    result = 1;
                    break;
                }
            }
        } else {
            if (strcmp(buffer1, buffer2) != 0) {
                result = 1;
                break;
            }
        }
        line_number++;
    }

    // Close the input files
    fclose(fp1);
    fclose(fp2);

    // Print the result
    if (result == 0) {
        if (verbose) {
            printf("equal\n");
        }
        return 0;
    } else {
        if (verbose) {
            printf("distinct\n");
        }
        return 1;
    }
}
