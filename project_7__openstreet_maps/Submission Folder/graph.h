// graph.h <Starter Code>
// <Parth Tawde and Pranav Rathod>
//
// Basic graph class using adjacency matrix representation.  Currently
// limited to a graph with at most 100 vertices.
//
// University of Illinois at Chicago
// CS 251: Fall 2020
// Project #7 - Openstreet Maps
//

#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include <map>
#include <set>

using namespace std;

template<typename VertexT, typename WeightT>
class graph {
 private:
    int count;
    map <VertexT,map<VertexT, WeightT>> map_link;
    map <VertexT, WeightT> addMap;
  //
  // _LookupVertex
  //
  // Finds the vertex in the Vertices vector and returns it's
  // index position if found, otherwise returns -1.
  //
  bool _LookupVertex(VertexT v) const {
      if (map_link.count(v)) {  // already in the graph:
        return true;  // true
      }
    // if get here, not found:
    return false;  // false
  }
 public:
 //  default constructor:
  graph() {
      count = 0;
  }
  // NumVertices
  //
  // Returns the # of vertices currently in the graph.
  //
  int NumVertices() const {
      // returns the size of the map which is the number of vertices
      return this -> map_link.size();
  }

  //
  // NumEdges
  //
  // Returns the # of edges currently in the graph.
  //
  int NumEdges() const {
      return count;
  }

  //
  // addVertex
  //
  // Adds the vertex v to the graph if there's room, and if so
  // returns true.  If the graph is full, or the vertex already
  // exists in the graph, then false is returned.
  //
  bool addVertex(VertexT v) {
    if (_LookupVertex(v)) {  // vertex already exists.
      return false;
    }
    // adding an empty map as the value in map_link
    map_link[v] = addMap;
    return true;
  }

  //
  // addEdge
  //
  // Adds the edge (from, to, weight) to the graph, and returns
  // true.  If the vertices do not exist or for some reason the
  // graph is full, false is returned.
  //
  // NOTE: if the edge already exists, the existing edge weight
  // is overwritten with the new edge weight.
  //
  bool addEdge(VertexT from, VertexT to, WeightT weight) {
    if (!_LookupVertex(from) || !_LookupVertex(to)) {
        // vertex does not exist
        return false;
    }
    if (map_link.at(from).count(to) == 0) {
        // for non duplicate from and to vertices, then increment edge counter
        count ++;
    }
    // assigning the edge with the weight
    map_link[from][to] = weight;
    return true;
  }

  //
  // getWeight
  //
  // Returns the weight associated with a given edge.  If
  // the edge exists, the weight is returned via the reference
  // parameter and true is returned.  If the edge does not
  // exist, the weight parameter is unchanged and false is
  // returned.
  //
  bool getWeight(VertexT from, VertexT to, WeightT& weight) const {
    if (!_LookupVertex(from)) {
        //  if the vertex is not found
        return false;
    }
    if (map_link.at(from).count(to) == 0) {
        // for duplicate from and to vertex, return false
        return false;
    }
    weight = map_link.at(from).at(to);
    return true;
  }

  //
  // neighbors
  //
  // Returns a set containing the neighbors of v, i.e. all
  // vertices that can be reached from v along one edge.
  // Since a set is returned, the neighbors are returned in
  // sorted order; use foreach to iterate through the set.
  //
  set<VertexT> neighbors(VertexT v) const {
    set<VertexT>  S;
    // check if the vertex v exist in the graph
    if (!_LookupVertex(v)) {
        S.clear();
        // return empty set
        return S;
    }
    for (auto &e : map_link.at(v)) {
        // inserting the neighbors of the vertex v
        S.insert(e.first);
    }
    return S;
  }

  //
  // getVertices
  //
  // Returns a vector containing all the vertices currently in
  // the graph.
  //
  vector<VertexT> getVertices() const {
    vector<VertexT> vertices;
    // inserting vertices from the key of the map in a vector
    for (auto &e : map_link) {
        vertices.push_back(e.first);
    }
    return vertices;
  }

  //
  // dump
  //
  // Dumps the internal state of the graph for debugging purposes.
  //
  // Example:
  //    graph<string,int>  G(26);
  //    ...
  //    G.dump(cout);  // dump to console
  //
  void dump(ostream& output) const {
    output << "***************************************************" << endl;
    output << "********************* GRAPH ***********************" << endl;

    output << "**Num vertices: " << this->NumVertices() << endl;
    output << "**Num edges: " << this->NumEdges() << endl;
    output << endl;
    output << "**Edges:" << endl;
    for (auto &e : map_link) {
        output << e.first << ": ";
        for (auto &v : e.second) {
            output << "(" << e.first << "," << v.first << ","
                   << v.second << ") ";
        }
        output << endl;
    }
    output << endl;
    output << "**************************************************" << endl;
  }
};
