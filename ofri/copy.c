#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define BUFSIZE 4096

int main(int argc, char *argv[]) {
    FILE *source, *target;
    char buf[BUFSIZE];
    int count;
    int success = 0; // 0 on success, 1 on failure
    int verbose = 0; // 0 for non-verbose output, 1 for verbose output
    int force = 0;   // 0 for non-force mode, 1 for force mode

    // Parse command line arguments
    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Usage: %s <source_file> <target_file> [-v] [-f]\n", argv[0]);
        exit(1);
    }
    if (argc == 4) {
        if (strcmp(argv[3], "-v") == 0) {
            verbose = 1;
        } else if (strcmp(argv[3], "-f") == 0) {
            force = 1;
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[3]);
            exit(1);
        }
    }
    if (argc == 5) {
        if ((strcmp(argv[3], "-v") == 0 && strcmp(argv[4], "-f") == 0) ||
            (strcmp(argv[3], "-f") == 0 && strcmp(argv[4], "-v") == 0)) {
            verbose = 1;
            force = 1;
        } else {
            fprintf(stderr, "Unknown options: %s %s\n", argv[3], argv[4]);
            exit(1);
        }
    }

    // Open source file
    source = fopen(argv[1], "rb");
    if (source == NULL) {
        fprintf(stderr, "Error opening source file: %s\n", strerror(errno));
        exit(1);
    }

    // Open target file
    if (!force) {
        target = fopen(argv[2], "rb");
        if (target != NULL) {
            fprintf(stderr, "Target file exists: %s\n", argv[2]);
            fclose(target);
            exit(1);
        }
    }
    target = fopen(argv[2], "wb");
    if (target == NULL) {
        fprintf(stderr, "Error opening target file: %s\n", strerror(errno));
        fclose(source);
        exit(1);
    }

    // Copy contents of source file to target file
    while ((count = fread(buf, 1, BUFSIZE, source)) > 0) {
        fwrite(buf, 1, count, target);
    }
    if (ferror(source)) {
        fprintf(stderr, "Error reading source file: %s\n", strerror(errno));
        success = 1;
    }
    if (ferror(target)) {
        fprintf(stderr, "Error writing target file: %s\n", strerror(errno));
        success = 1;
    }

    // Close files
    fclose(source);
    fclose(target);

    // Output result
    if (verbose) {
        if (success == 0) {
            printf("Success\n");
        } else {
            printf("General failure\n");
        }
    }
    return success;
}
