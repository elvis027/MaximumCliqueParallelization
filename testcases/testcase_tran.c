#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv)
{
    for(int count = 1; count < argc; count++) {

        FILE* fin = fopen(argv[count], "rb");
        char outName_text[50];
		strcpy(outName_text, argv[count]);
        strcat(outName_text, ".txt");
        freopen(outName_text, "w", stdout);

        int V, E;
        fread(&V, sizeof(int), 1, fin);
        fread(&E, sizeof(int), 1, fin);
        printf("%d ", V);

		int** edges = malloc(V * sizeof(int*));
	    for(int i = 0; i < V; i++)
            edges[i] = calloc(V, sizeof(int));

        for(int i = 0; i < E; i++) {
            int e[3];
            fread(&e, sizeof(int), 3, fin);
            edges[e[0]][e[1]] = 1;
			edges[e[1]][e[0]] = 1;
        }

        fclose(fin);

        E = 0;

    	for(int i = 0; i < V; i++) {
        	for(int j = i; j < V; j++) {
            	if(edges[i][j]) E++;
        	}
    	}

        printf("%d\n", E);

        for(int i = 0; i < V; i++) {
        	for(int j = i; j < V; j++) {
            	if(edges[i][j])
                    printf("%d %d\n", i, j);
        	}
    	}

        fclose(stdout);

        char outName_bin[50];
        strcpy(outName_bin, argv[count]);
        strcat(outName_bin, ".bin");
        FILE* fout = fopen(outName_bin, "w");

        fwrite(&V, sizeof(int), 1, fout);
        fwrite(&E, sizeof(int), 1, fout);

        for(int i = 0; i < V; i++) {
        	for(int j = i; j < V; j++) {
            	if(edges[i][j]) {
                    fwrite(&i, sizeof(int), 1, fout);
                    fwrite(&j, sizeof(int), 1, fout);
                }
        	}
    	}

        fclose(fout);

        for(int i = 0; i < V; i++)
            free(edges[i]);
        free(edges);

    }
}
