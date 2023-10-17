#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <omp.h>
#include <mpi.h>

using namespace std;

int V, E;
int** edges;
vector<set<int> > N;  // neighbors of each vertices
set<int> Max;

typedef struct {
    int vertex_value;
    set<int> P;
    set<int> X;
} PNarray;


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
        #pragma omp critical
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


void BK_pp(set<int> R, set<int> P, set<int> X)
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

    PNarray *PNarr = new PNarray[PE.size()];

    set<int>::iterator it;
    int i;
    for(it = PE.begin(), i = 0; it != PE.end(); it++, i++) {// for each threads doing BK correctly
        PNarr[i].vertex_value = *it;
        if(i==0) {
            PNarr[i].P = P;
            PNarr[i].X = X;
        }
        else {
            PNarr[i].P = PNarr[i-1].P;
            PNarr[i].X = PNarr[i-1].X;
            // erase and insert in advance
            PNarr[i].P.erase(PNarr[i-1].vertex_value);
            PNarr[i].X.insert(PNarr[i-1].vertex_value);
        }
    }

    #pragma omp parallel for schedule (dynamic)
    for(i = 0; i < PE.size(); i++) {
        set<int> r, PN, XN;
        r = R;
        r.insert(PNarr[i].vertex_value);
        set_intersection(PNarr[i].P.begin(), PNarr[i].P.end(), N[PNarr[i].vertex_value].begin(), N[PNarr[i].vertex_value].end(), inserter(PN, PN.begin()));
        set_intersection(PNarr[i].X.begin(), PNarr[i].X.end(), N[PNarr[i].vertex_value].begin(), N[PNarr[i].vertex_value].end(), inserter(XN, XN.begin()));
        BK(r, PN, XN);
    }

    delete [] PNarr;
}


int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_File f;
    MPI_File_open(MPI_COMM_WORLD, argv[1], MPI_MODE_RDONLY, MPI_INFO_NULL, &f);
    MPI_File_read(f, &V, 1, MPI_INT, MPI_STATUS_IGNORE);
    MPI_File_read(f, &E, 1, MPI_INT, MPI_STATUS_IGNORE);

    if(rank == 0) {
        cout << endl;
        cout << "V = " << V << endl;
        cout << "E = " << E << endl;
        cout << endl;
    }

    edges = new int*[V];
    for(int i = 0; i < V; i++) {
        edges[i] = new int[V]{0};
    }

    // draw graph
    for(int i = 0; i < E; i++) {
        int e[2];
        MPI_File_read(f, e, 2, MPI_INT, MPI_STATUS_IGNORE);
        edges[e[0]][e[1]] = 1;
        edges[e[1]][e[0]] = 1;
    }

    // find all vertex neighbors
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

    int pivot = getPivot(P, X);
    set<int> PE;     // all the vextices need to be visited in this phase
    set_difference(P.begin(), P.end(), N[pivot].begin(), N[pivot].end(), inserter(PE, PE.begin()));

    PNarray *PNarr = new PNarray[PE.size()];

    set<int>::iterator it;
    int k;
    for(it = PE.begin(), k = 0; it != PE.end(); it++, k++) {// for each threads doing BK correctly
        PNarr[k].vertex_value = *it;
        if(k==0) {
            PNarr[k].P = P;
            PNarr[k].X = X;
        }
        else {
            PNarr[k].P = PNarr[k-1].P;
            PNarr[k].X = PNarr[k-1].X;
            // erase and insert in advance
            PNarr[k].P.erase(PNarr[k-1].vertex_value);
            PNarr[k].X.insert(PNarr[k-1].vertex_value);
        }
    }

    double start, end;
    double com1, com2, com_time = 0;
    int task_id = 0;
    int count = 0;
    int data_tag = 0;
    int terminate_tag = 1;
    MPI_Status status;
    if(rank == 0) {
        start = MPI_Wtime();
        #pragma omp parallel sections
        {
            #pragma omp section
            {
                int init_send_times, worker_id;
                if(PE.size() < size-1) {// check if #vertices is too small to parallel
                    init_send_times = PE.size();
                }
                else {
                    init_send_times = size-1;
                }
                for(int i = 1; i < init_send_times+1; i++){

                    com1 = MPI_Wtime();
                    #pragma omp critical
                    MPI_Send(&task_id, 1, MPI_INT, i, data_tag, MPI_COMM_WORLD);
                    com2 = MPI_Wtime();
                    com_time += (com2 - com1);

                    count++;
                    task_id++;
                }
                while(count > 0)
                {
                    com1 = MPI_Wtime();
                    MPI_Recv(&worker_id, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    com2 = MPI_Wtime();
                    com_time += (com2 - com1);

                    count--;
                    #pragma omp critical
                    if(task_id < PE.size()){

                        com1 = MPI_Wtime();
                        MPI_Send(&task_id, 1, MPI_INT, worker_id, data_tag, MPI_COMM_WORLD);
                        com2 = MPI_Wtime();
                        com_time += (com2 - com1);

                        count++;
                        task_id++;
                    }
                    else{
                        com1 = MPI_Wtime();
                        MPI_Send(&task_id, 1, MPI_INT, worker_id, terminate_tag, MPI_COMM_WORLD);
                        com2 = MPI_Wtime();
                        com_time += (com2 - com1);
                    }
                }
            }
            #pragma omp section
            {
                int t;
                #pragma omp critical
                {
                    t = task_id;
                    task_id++;
                }
                set<int> r, PN, XN;
                r = R;
                r.insert(PNarr[t].vertex_value);
                set_intersection(PNarr[t].P.begin(), PNarr[t].P.end(), N[PNarr[t].vertex_value].begin(), N[PNarr[t].vertex_value].end(), inserter(PN, PN.begin()));
                set_intersection(PNarr[t].X.begin(), PNarr[t].X.end(), N[PNarr[t].vertex_value].begin(), N[PNarr[t].vertex_value].end(), inserter(XN, XN.begin()));
                BK(r, PN, XN);
            }
            #pragma omp section
            {
                int t;
                #pragma omp critical
                {
                    t = task_id;
                    task_id++;
                }
                set<int> r, PN, XN;
                r = R;
                r.insert(PNarr[t].vertex_value);
                set_intersection(PNarr[t].P.begin(), PNarr[t].P.end(), N[PNarr[t].vertex_value].begin(), N[PNarr[t].vertex_value].end(), inserter(PN, PN.begin()));
                set_intersection(PNarr[t].X.begin(), PNarr[t].X.end(), N[PNarr[t].vertex_value].begin(), N[PNarr[t].vertex_value].end(), inserter(XN, XN.begin()));
                BK(r, PN, XN);
            }
            #pragma omp section
            {
                int t;
                #pragma omp critical
                {
                    t = task_id;
                    task_id++;
                }
                set<int> r, PN, XN;
                r = R;
                r.insert(PNarr[t].vertex_value);
                set_intersection(PNarr[t].P.begin(), PNarr[t].P.end(), N[PNarr[t].vertex_value].begin(), N[PNarr[t].vertex_value].end(), inserter(PN, PN.begin()));
                set_intersection(PNarr[t].X.begin(), PNarr[t].X.end(), N[PNarr[t].vertex_value].begin(), N[PNarr[t].vertex_value].end(), inserter(XN, XN.begin()));
                BK(r, PN, XN);
            }
            #pragma omp section
            {
                int t;
                #pragma omp critical
                {
                    t = task_id;
                    task_id++;
                }
                set<int> r, PN, XN;
                r = R;
                r.insert(PNarr[t].vertex_value);
                set_intersection(PNarr[t].P.begin(), PNarr[t].P.end(), N[PNarr[t].vertex_value].begin(), N[PNarr[t].vertex_value].end(), inserter(PN, PN.begin()));
                set_intersection(PNarr[t].X.begin(), PNarr[t].X.end(), N[PNarr[t].vertex_value].begin(), N[PNarr[t].vertex_value].end(), inserter(XN, XN.begin()));
                BK(r, PN, XN);
            }
            #pragma omp section
            {
                int t;
                #pragma omp critical
                {
                    t = task_id;
                    task_id++;
                }
                set<int> r, PN, XN;
                r = R;
                r.insert(PNarr[t].vertex_value);
                set_intersection(PNarr[t].P.begin(), PNarr[t].P.end(), N[PNarr[t].vertex_value].begin(), N[PNarr[t].vertex_value].end(), inserter(PN, PN.begin()));
                set_intersection(PNarr[t].X.begin(), PNarr[t].X.end(), N[PNarr[t].vertex_value].begin(), N[PNarr[t].vertex_value].end(), inserter(XN, XN.begin()));
                BK(r, PN, XN);
            }
        }
        end = MPI_Wtime();
    }
    else {
        start = MPI_Wtime();
        MPI_Recv(&task_id, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        while(status.MPI_TAG==data_tag){
            set<int> r, PN, XN;
            r = R;
            r.insert(PNarr[task_id].vertex_value);
            set_intersection(PNarr[task_id].P.begin(), PNarr[task_id].P.end(), N[PNarr[task_id].vertex_value].begin(), N[PNarr[task_id].vertex_value].end(), inserter(PN, PN.begin()));
            set_intersection(PNarr[task_id].X.begin(), PNarr[task_id].X.end(), N[PNarr[task_id].vertex_value].begin(), N[PNarr[task_id].vertex_value].end(), inserter(XN, XN.begin()));
            BK_pp(r, PN, XN);

            com1 = MPI_Wtime();
            MPI_Send(&rank, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
            MPI_Recv(&task_id, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            com2 = MPI_Wtime();
            com_time += (com2 - com1);

        }// no need erase or insert due to preprocessing
        end = MPI_Wtime();
    }

    delete [] PNarr;
/*
    for(set<int>::iterator it1 = Max.begin(); it1 != Max.end(); it1++) {
        for(set<int>::iterator it2 = Max.begin(); it2 != Max.end(); it2++) {
            if(!edges[*it1][*it2] && *it1 != *it2) {
                cout << "Rank " << rank << ": Wrong at node " << *it1 << " and node " << *it2 << endl;
            }
        }
    }
*/
    int max;
    int clique_size = Max.size();
    MPI_Reduce(&clique_size, &max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
    if(rank == 0) {
        cout << "size: " << max << endl;
    }

    // for(set<int>::iterator it = Max.begin(); it != Max.end(); it++) {
    //     cout << *it << endl;
    // }
    // cout << "size: " << Max.size() << endl;
    // cout << "fp: " << fp << endl;
    // cout << "inter: " << inter << endl;

    for(int i = 0; i < V; i++) {
        delete [] edges[i];
    }
    delete [] edges;
    MPI_File_close(&f);

    cout << "rank " << rank << " : " << (end-start) << endl;
    cout << "rank " << rank << " : comm = " << com_time << endl;

    MPI_Finalize();
}
