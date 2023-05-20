#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>

int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Usage: %s <codec> <message>\n", argv[0]);
        return 1;
    }

    char* codec_name = argv[1];
    char* message = argv[2];

    // Load the shared library for the selected codec
    char* lib_name = NULL;
    if (strcmp(codec_name, "codecA") == 0) {
        lib_name = "./codecA.so";
    } else if (strcmp(codec_name, "codecB") == 0) {
        lib_name = "./codecB.so";
    } else {
        printf("Unknown codec: %s\n", codec_name);
        return 1;
    }
    void* codec_lib = dlopen(lib_name, RTLD_LAZY);
    if (!codec_lib) {
        printf("Error loading codec library: %s\n", dlerror());
        return 1;
    }

    // Get the encode function from the library
    char* func_name = "encode";

    void (*encode_func)(char*);
    encode_func = dlsym(codec_lib, func_name);
    const char* dlsym_error = dlerror();
    if (dlsym_error) {
        printf("Error getting function %s: %s\n", func_name, dlsym_error);
        return 1;
    }

    // Encode the message
    encode_func(message);
    printf("%s\n", message);

    // Cleanup
    dlclose(codec_lib);
    return 0;
}
