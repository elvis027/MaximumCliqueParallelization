#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    for(int i = 1; i < argc; i++) {
        freopen(argv[i], "r", stdin);
        int V, E;
        scanf("%d %d", &V, &E);
        fclose(stdin);

        printf("\n%s\n", argv[i]);
        printf("V = %d\n", V);
        printf("E = %d\n\n", E);
    }
}
