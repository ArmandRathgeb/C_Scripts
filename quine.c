#include <stdio.h>
int main() {
    const char* prog[] = {
        "#include <stdio.h>",
        "int main() {",
        "    const char* prog[] = {",
        "    };",
        "    for (int i = 0; i < 3; ++i) {",
        "        puts(prog[i]);",
        "    }",
        "    for (int i = 0; i < 11; ++i) {",
        "        printf(\"        \\\"\%s\\\"\\n, prog[i]);",
        "    }",
        "    for (int i = 3; i < 14; ++i) {",
        "        puts(prog[i]);",
        "    }",
        "}"
    };
    for (int i = 0; i < 3; ++i) {
        puts(prog[i]);
    }
    for (int i = 0; i < 14; ++i) {
        printf("        \"%s\",\n", prog[i]);
    }
    for (int i = 3; i < 14; ++i) {
        puts(prog[i]);
    }
}
