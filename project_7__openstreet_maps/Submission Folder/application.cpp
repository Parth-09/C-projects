// application.cpp <Starter Code>
// <Parth Tawde and Pranav Rathod>
//
// University of Illinois at Chicago
// CS 251: Fall 2020
// Project #7 - Openstreet Maps
//
// References:
// TinyXML: https://github.com/leethomason/tinyxml2
// OpenStreetMap: https://www.openstreetmap.org
// OpenStreetMap docs:
//   https://wiki.openstreetmap.org/wiki/Main_Page
//   https://wiki.openstreetmap.org/wiki/Map_Features
//   https://wiki.openstreetmap.org/wiki/Node
//   https://wiki.openstreetmap.org/wiki/Way
//   https://wiki.openstreetmap.org/wiki/Relation
//

#include <iostream>
#include <iomanip>  /*setprecision*/
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <stack>
#include "graph.h"
#include "tinyxml2.h"
#include "dist.h"
#include "osm.h"
#include "algs.h"

using namespace std;
using namespace tinyxml2;

// this functions build the graph by adding the footways as edges to the graphs
void readFootwaysToGraph(graph<long long, double> &G,
vector<FootwayInfo> Footways, map<long long, Coordinates>  Nodes) {
    for (auto &e : Footways) {
        // looping through the Footways vector, and placing them in the graph
        for (int i = 0; i < e.Nodes.size()-1; i++) {
            G.addEdge(e.Nodes[i], e.Nodes[i+1],
            distBetween2Points(Nodes[e.Nodes[i]].Lat, Nodes[e.Nodes[i]].Lon,
            Nodes[e.Nodes[i+1]].Lat, Nodes[e.Nodes[i+1]].Lon));
            // since this is a directed graph, adding edges for both ways
            G.addEdge(e.Nodes[i+1], e.Nodes[i],
            distBetween2Points(Nodes[e.Nodes[i]].Lat, Nodes[e.Nodes[i]].Lon,
            Nodes[e.Nodes[i+1]].Lat, Nodes[e.Nodes[i+1]].Lon));
        }
    }
}

// this function adds nodes as vertices to the directed graph
void readNodesToGraph(graph<long long, double> &G, map<long long,
Coordinates> Nodes) {
    for (auto &e : Nodes) {
        // adding verticed to the graph
        G.addVertex(e.first);
    }
}

// this fucntion checks if the a building name exists in the Buildings
// vector
bool checkBuildingExists(string buildingName, BuildingInfo & build,
vector <BuildingInfo> Buildings) {
    // first checking if the name being searched is an abbreviation
    for (auto e : Buildings) {
        if (buildingName == e.Abbrev) {
            build = e;
            return true;
        }
    }
    // now searching for the full name
    for (auto e : Buildings) {
        if (e.Fullname.find(buildingName) != std::string::npos) {
            build = e;
            return true;
        }
    }
    return false;
}

// printing the details of the path, the start and the destination
// and their coodinates
void printInfo(BuildingInfo start_build, BuildingInfo dest_build,
map<long long, Coordinates> Nodes) {
    // printing the information from the start and destination
    cout << "Starting point:" << endl;
    cout << " " << start_build.Fullname << endl;
    // printing coodinates of the start loaction
    cout << " (" << start_build.Coords.Lat << ", "
         << start_build.Coords.Lon << ")" << endl;
    cout << "Destination point:" << endl;
    cout << " " << dest_build.Fullname << endl;
    // printing coodinates of the Destination loaction
    cout << " (" << dest_build.Coords.Lat << ", "
         << dest_build.Coords.Lon << ")" << endl;
}

// searching for the nearest node
void ClosestNode(BuildingInfo build, long long & searchNode,
vector<FootwayInfo> Footways, map<long long, Coordinates> Nodes,
double compare) {
    for (auto e : Footways) {
        for (auto node : e.Nodes) {
            // if the distance between 2 nodes < max distance, unreachable
            if (distBetween2Points(build.Coords.Lat,
            build.Coords.Lon, Nodes[node].Lat,
            Nodes[node].Lon) < compare) {
                searchNode = node;
                compare = distBetween2Points(build.Coords.Lat,
                          build.Coords.Lon, Nodes[node].Lat,
                          Nodes[node].Lon);
            }
        }
    }
}

// this function is used to print nearest nodes form the start and end points
void printNodes(long long startNode, long long destNode,
map<long long, Coordinates> Nodes) {
    cout << endl;
    cout << "Nearest start node:" << endl;
    cout << " " << startNode << endl;
    cout << " (" << Nodes[startNode].Lat << ", " << Nodes[startNode].Lon
         << ")" << endl;
    cout << "Nearest destination node:" << endl;
    cout << " " << destNode << endl;
    cout << " (" << Nodes[destNode].Lat << ", " << Nodes[destNode].Lon
         << ")" << endl;
}

// this function is used to print the shortest path which was done using the
// Dijkstra's algorigthm.
void printPath(long long startNode, long long destNode,
map <long long, double> distances,
map <long long, long long> predecessor) {
    cout << "Navigating with Dijkstra..." << endl;
    long long end = destNode;
    if (distances.at(destNode) == INF) {
        cout << "Sorry, destination unreachable" << endl;
    } else {
        cout << "Distance to dest: " << distances[destNode]
             << " miles" << endl;
        // pusing all the nodes that are in the path into a stack
        stack <long long> list;
        while (startNode != end) {
            list.push(predecessor[end]);
            end = predecessor[end];
        }
        // printing the path
        cout << "Path: ";
        while (!list.empty()) {
            cout << list.top() << "->";
            list.pop();
        }
        cout << destNode << endl;
    }
}
int main() {
  // maps a Node ID to it's coordinates (lat, lon)
  map<long long, Coordinates>  Nodes;
  // info about each footway, in no particular order
  vector<FootwayInfo>          Footways;
  // info about each building, in no particular order
  vector<BuildingInfo>         Buildings;
  XMLDocument                  xmldoc;
  double compare = INF;
  graph<long long, double> G;
  cout << "** Navigating UIC open street map **" << endl;
  cout << endl;
  cout << std::setprecision(8);

  string def_filename = "map.osm";
  string filename;

  cout << "Enter map filename> ";
  getline(cin, filename);

  if (filename == "") {
    filename = def_filename;
  }

  //
  // Load XML-based map file
  //
  if (!LoadOpenStreetMap(filename, xmldoc)) {
    cout << "**Error: unable to load open street map." << endl;
    cout << endl;
    return 0;
  }

  //
  // Read the nodes, which are the various known positions on the map:
  //
  int nodeCount = ReadMapNodes(xmldoc, Nodes);
  readNodesToGraph(G, Nodes);
  //
  // Read the footways, which are the walking paths:
  //
  int footwayCount = ReadFootways(xmldoc, Footways);
  readFootwaysToGraph(G, Footways, Nodes);
  //
  // Read the university buildings:
  //
  int buildingCount = ReadUniversityBuildings(xmldoc, Nodes, Buildings);

  //
  // Stats
  //
  assert(nodeCount == (int)Nodes.size());
  assert(footwayCount == (int)Footways.size());
  assert(buildingCount == (int)Buildings.size());

  cout << endl;
  cout << "# of nodes: " << Nodes.size() << endl;
  cout << "# of footways: " << Footways.size() << endl;
  cout << "# of buildings: " << Buildings.size() << endl;


  //
  // TO DO: build the graph, output stats:
  //


  cout << "# of vertices: " << G.NumVertices() << endl;
  cout << "# of edges: " << G.NumEdges() << endl;
  cout << endl;

  //
  // Navigation from building to building
  //
  string startBuilding, destBuilding;
  BuildingInfo start_build, dest_build;
  cout << "Enter start (partial name or abbreviation), or #> ";
  getline(cin, startBuilding);

  while (startBuilding != "#") {
    cout << "Enter destination (partial name or abbreviation)> ";
    getline(cin, destBuilding);
    //
    // TO DO: lookup buildings, find nearest start and dest nodes,
    // run Dijkstra's alg, output distance and path to destination:
    //
    bool check = false;
    bool flag = true;  // made false when either start or end is not found
    check = checkBuildingExists(startBuilding, start_build, Buildings);
    if (!check) {
        cout << "Start building not found" << endl;
        flag = false;
    } else {
        check = false;
        check = checkBuildingExists(destBuilding, dest_build, Buildings);
        if (!check) {
            cout << "Destination building not found" << endl;
            flag = false;
        }
    }
    
    // flag is true only if both Buildings exist
    if (flag) {
        // printing the info about the node, it's coordinates
        printInfo(start_build, dest_build, Nodes);
        long long startNode, destNode;
        // finding the closest node to start and and end Buildings
        ClosestNode(start_build, startNode, Footways, Nodes, compare);
        ClosestNode(dest_build, destNode, Footways, Nodes, compare);
        // printing those close nodes
        printNodes(startNode, destNode, Nodes);
        cout << endl;
        // now to find the shortest path
        map <long long, double> distances;
        map <long long, long long> predecessor;
        // calling Dijkstra function to find the shortest path b/w the 2 points
        Dijkstra(G, startNode, distances, predecessor);
        // printing the path found
        printPath(startNode, destNode, distances, predecessor);
    }
    //
    // another navigation?
    //
    cout << endl;
    cout << "Enter start (partial name or abbreviation), or #> ";
    getline(cin, startBuilding);
  }

  //
  // done:
  //
  cout << "** Done **" << endl;
  return 0;
}
