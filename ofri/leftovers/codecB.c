#include <ctype.h>

void encode(char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = (char) (str[i] + 3);
    }
}

void decode(char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = (char) (str[i] - 3);
    }
}
