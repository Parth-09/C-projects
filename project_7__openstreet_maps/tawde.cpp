// application.cpp <Starter Code>
// <Your name>
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
#include <stack>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include "graph.h"
#include "tinyxml2.h"
#include "dist.h"
#include "osm.h"
#include "algs.h"

using namespace std;
using namespace tinyxml2;
void printPath (long long startNode, long long destNode,
map <long long, double> distances,
map <long long, long long> predecessor) {
    cout << "Navigation with Dijkstra..." << endl;
    long long end = destNode;
    if (predecessor[end] == 0 && startNode != destNode) {
        cout << "Sorry, destination unreachable" << endl;
    } else {
        cout << "Distance to dest: " << distances[destNode]
             << " miles" << endl;
        stack <long long> list;
        while (predecessor[end] == 1) {
            list.push(predecessor[end]);
            end = predecessor[end];
        }
        cout << "Path: ";
        while (!list.empty()) {
            cout << list.top() << "->";
            list.pop();
        }
        cout << destNode << endl;
    }
}


void printClosestNodes (long long startNode, long long destNode,
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
void ClosestStartNode (BuildingInfo start_build, long long &startNode,
vector<FootwayInfo> Footways, map<long long, Coordinates> Nodes,
double compare) {
    for (auto e : Footways) {
        for (auto node : e.Nodes) {
            if (distBetween2Points(start_build.Coords.Lat,
            start_build.Coords.Lon, Nodes[node].Lat,
            Nodes[node].Lon) < compare) {
                startNode = node;
                compare = distBetween2Points(start_build.Coords.Lat,
                          start_build.Coords.Lon, Nodes[node].Lat,
                          Nodes[node].Lon);
            }
        }
    }
}
void ClosestDestNode (BuildingInfo dest_build, long long &destNode,
vector<FootwayInfo> Footways, map<long long, Coordinates> Nodes,
double compare) {
    for (auto e : Footways) {
        for (auto node : e.Nodes) {
            if (distBetween2Points(dest_build.Coords.Lat,
            dest_build.Coords.Lon, Nodes[node].Lat,
            Nodes[node].Lon) < compare) {
                destNode = node;
                compare = distBetween2Points(dest_build.Coords.Lat,
                          dest_build.Coords.Lon, Nodes[node].Lat,
                          Nodes[node].Lon);
            }
        }
    }
}
void printInfo (BuildingInfo start_build, BuildingInfo dest_build,
map<long long, Coordinates> Nodes) {
    
    cout << "Starting point:" << endl;
    cout << " " << start_build.Fullname << endl;
    cout << " (" << start_build.Coords.Lat << ", "
         << start_build.Coords.Lon << ")" << endl;
    cout << "Destination point:" << endl;
    cout << " " << dest_build.Fullname << endl;
    cout << " (" << dest_build.Coords.Lat << ", "
         << dest_build.Coords.Lon << ")" << endl;
}
bool checkDestFull (string destBuilding, BuildingInfo &dest_build,
vector<BuildingInfo> Buildings) {
    for (auto e : Buildings) {
        if (e.Fullname.find(destBuilding) != std::string::npos) {
            dest_build = e;
            return true;
        }
    }
    return false;
}
bool checkDestAbbrev (string destBuilding, BuildingInfo &dest_build,
vector<BuildingInfo> Buildings) {
    for (auto e : Buildings) {
        if (destBuilding == e.Abbrev) {
            dest_build = e;
            return true;
        }
    }
    return false;
}
bool checkStartFull (string startBuilding, BuildingInfo &start_build,
vector<BuildingInfo> Buildings) {
    for (auto e : Buildings) {
        if (e.Fullname.find(startBuilding) != std::string::npos) {
            start_build = e;
            return true;
        }
    }
    return false;
}
bool checkStartAbbrev (string startBuilding, BuildingInfo &start_build,
vector<BuildingInfo> Buildings) {
    for (auto e : Buildings) {
        if (startBuilding == e.Abbrev) {
            start_build = e;
            return true;
        }
    }
    return false;
}
void readFootways(graph<long long, double> &G,
vector<FootwayInfo> Footways, map<long long, Coordinates>  Nodes) {
    for (auto &e : Footways) {
        for (int i = 0; i < e.Nodes.size()-1; i++) {
            G.addEdge(e.Nodes[i], e.Nodes[i+1],
            distBetween2Points(Nodes[e.Nodes[i]].Lat, Nodes[e.Nodes[i]].Lon,
            Nodes[e.Nodes[i+1]].Lat, Nodes[e.Nodes[i+1]].Lon));
            G.addEdge(e.Nodes[i+1], e.Nodes[i],
            distBetween2Points(Nodes[e.Nodes[i]].Lat, Nodes[e.Nodes[i]].Lon,
            Nodes[e.Nodes[i+1]].Lat, Nodes[e.Nodes[i+1]].Lon));
        }
    }
}
void readNodes(graph<long long, double> &G,
map<long long, Coordinates> Nodes) {
    for (auto &e : Nodes) {
        G.addVertex(e.first);
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
  graph<long long, double> G;
  double compare = INF;
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
  readNodes(G,Nodes);
  //
  // Read the footways, which are the walking paths:
  //
  int footwayCount = ReadFootways(xmldoc, Footways);
  readFootways(G,Footways, Nodes);
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
    bool check;
    check = checkStartAbbrev(startBuilding, start_build, Buildings);
    if (!check) {
        check = checkStartFull(startBuilding, start_build, Buildings);
    }
    if (!check) {
        cout << "Start building not found" << endl;
    } else {
        check = checkDestAbbrev(destBuilding, dest_build, Buildings);
        if (!check) {
            check = checkDestFull(destBuilding, dest_build, Buildings);
        }
        if (!check) {
            cout << "Destination building not found" << endl;
        } else {
            printInfo(start_build, dest_build, Nodes);
            long long startNode, destNode;
            ClosestStartNode(start_build, startNode, Footways, Nodes,
            compare);
            ClosestDestNode(dest_build, destNode, Footways, Nodes,
            compare);
            printClosestNodes(startNode, destNode, Nodes);
            cout << endl;
            map <long long, double> distances;
            map <long long, long long> predecessor;
            Dijkstra(G, startNode, distances, predecessor);
            printPath(startNode, destNode, distances, predecessor);
        }
    }
    
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
