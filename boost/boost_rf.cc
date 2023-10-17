

// (C) Copyright Andrew Sutton 2007
//
// Use, modification and distribution are subject to the
// Boost Software License, Version 1.0 (See accompanying file
// LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)

//[code_bron_kerbosch_clique_number
#include <iostream>

#include <string>
#include <cstdio>

#include <boost/graph/undirected_graph.hpp>
#include <boost/graph/bron_kerbosch_all_cliques.hpp>

#include "helper.hpp"

using namespace std;
using namespace boost;

// Declare the graph type and its vertex and edge types.
typedef undirected_graph<> Graph;
typedef graph_traits<Graph>::vertex_descriptor Vertex;
typedef graph_traits<Graph>::edge_descriptor Edge;

int
main(int argc, char *argv[])
{
    freopen(argv[1], "r", stdin);

    std::string firstLine;
    std::getline(cin, firstLine);
    std::size_t index = firstLine.find_first_of(' ');
    std::string V(firstLine, 0, index);
    std::string E(firstLine, index + 1);
    cout << "V = " << V << endl;
    cout << "E = " << E << endl;

    // Create the graph and read it from standard input.
    Graph g;
    read_graph(g, cin);

    // Use the Bron-Kerbosch algorithm to find all cliques, and
    size_t c = bron_kerbosch_clique_number(g);
    cout << "clique number: " << c << endl;

    fclose(stdin);
    return 0;
}
//]

