#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <time.h>

using namespace std;

int main(int argc, char** argv)
{
    bool random = false;
    char flag;
    cout << "Randomly generate testcase?(y or n): ";
    cin >> flag;
    if(flag == 'y') random = true;

    int V, E;
    cout << "Please input # of vertices:";
    cin >> V;
    cout << "Please input # of edges (at most " << V*(V-1)/2 << " edges):";
    cin >> E;

    freopen(argv[1], "w", stdout);
    cout << V << " " << E << endl;

    if(random) {

        srand(time(NULL));

        int **edges = new int*[V];
        for(int i = 0; i < V; i++)
            edges[i] = new int[V]{0};

        int min = 0;
        int max = V;

        for(int i = 0; i < E; i++) {

            int vertex_id1;
            int vertex_id2;
            do {
                vertex_id1 = rand() % (max-min) + min;
                vertex_id2 = rand() % (max-min) + min;
            } while(edges[vertex_id1][vertex_id2] || vertex_id1 == vertex_id2);

            edges[vertex_id1][vertex_id2] = 1;
            edges[vertex_id2][vertex_id1] = 1;
            cout << vertex_id1 << " " << vertex_id2 << endl;

        }
        fclose(stdout);

        for(int i = 0; i < V; i++)
            delete [] edges[i];
        delete [] edges;

        cout << "Randomly generate testcase successfully!" << endl;

    }
    else {

        cout << "Please input edges:" << endl;

        for(int i = 0; i < E; i++) {
            int e[2];
            cin >> e[0] >> e[1];
            cout << e[0] << " " << e[1] << endl;
        }
        fclose(stdout);

        cout << "Manually generate testcase successfully!" << endl;

    }

}
