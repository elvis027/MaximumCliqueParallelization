#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <omp.h>

using namespace std;

int V, E;
int** edges;
vector<set<int> > N;  // neighbors of each vertices
set<int> Max;


// return a pivot from (P union X)
int getPivot(set<int> P, set<int> X)
{
    set<int> PX;
    set_union(P.begin(), P.end(), X.begin(), X.end(), inserter(PX, PX.begin()));

    // TODO: find a vertex that have max neighbors from PX
    int max_pivot = *PX.begin();
    int max_size = N[max_pivot].size();

    for(set<int>::iterator it = PX.begin(); it != PX.end(); it++) {
        if(N[*it].size() > max_size) {
            max_size = N[*it].size();
            max_pivot = *it;
        }
    }

    return max_pivot;
}


void BK(set<int> R, set<int> P, set<int> X)
{
    if(P.empty() && X.empty()) {
        if(Max.size() < R.size()) {
            Max = R;
        }
        return;
    }

    int pivot = getPivot(P, X);
    set<int> PE;     // all the vextices need to be visited in this phase
    set_difference(P.begin(), P.end(), N[pivot].begin(), N[pivot].end(), inserter(PE, PE.begin()));

    for(set<int>::iterator it = PE.begin(); it != PE.end(); it++) {
        R.insert(*it);
        set<int> PN, XN;
        set_intersection(P.begin(), P.end(), N[*it].begin(), N[*it].end(), inserter(PN, PN.begin()));
        set_intersection(X.begin(), X.end(), N[*it].begin(), N[*it].end(), inserter(XN, XN.begin()));
        BK(R, PN, XN);
        R.erase(*it);
        P.erase(*it);
        X.insert(*it);
    }
}


int main(int argc, char** argv)
{
    double input = omp_get_wtime();

    freopen(argv[1], "r", stdin);

    cin >> V >> E;
    cout << endl;
    cout << "V = " << V << endl;
    cout << "E = " << E << endl;
    cout << endl;

    edges = new int*[V];
    for(int i = 0; i < V; i++) {
        edges[i] = new int[V]{0};
    }

    for(int i = 0; i < E; i++) {
        int e[2];
        cin >> e[0] >> e[1];
        edges[e[0]][e[1]] = 1;
        edges[e[1]][e[0]] = 1;
    }

    double cal = omp_get_wtime();

    for(int i = 0; i < V; i++) {
        set<int> neighbor;
        for(int j = 0; j < V; j++) {
            if(edges[i][j]) {
                neighbor.insert(j);
            }
        }
        N.push_back(neighbor);
    }

    set<int> R, P, X;

    for(int i = 0; i < V; i++) {
        P.insert(i);
    }

    BK(R, P, X);

    double output = omp_get_wtime();

    for(set<int>::iterator it = Max.begin(); it != Max.end(); it++) {
        cout << *it << endl;
    }
    cout << "size: " << Max.size() << endl;

/*
    for(set<int>::iterator it1 = Max.begin(); it1 != Max.end(); it1++) {
        for(set<int>::iterator it2 = Max.begin(); it2 != Max.end(); it2++) {
            if(!edges[*it1][*it2] && *it1 != *it2) {
                cout << "Wrong at node " << *it1 << " and node " << *it2 << endl;
            }
        }
    }
*/

    fclose(stdin);
    for(int i = 0; i < V; i++)
        delete [] edges[i];
    delete [] edges;

    double end = omp_get_wtime();

    cout << "input : " << (cal - input) << endl;
    cout << "cal : " << (output - cal) << endl;
    cout << "output : " << (end - output) << endl;
}
