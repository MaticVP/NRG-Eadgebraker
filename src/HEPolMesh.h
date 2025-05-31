//
// Created by matic on 30/03/2025.
//

#ifndef EDGEBRAKER_HEPOLMESH_H
#define EDGEBRAKER_HEPOLMESH_H

#include <vector>
#include "EBdatatypes.h"
#include <iostream>

using std::vector;
class HEedge;
class HEpolygon;



struct HEvertex {
    HEvertex(float x, float y, float z) {
        pt.x = x; pt.y = y; pt.z = z;
        edge = -1;
    }

    HEvertex(Vertex p) {
        pt = p;
        edge = -1;
    }

    Vertex pt;
    int edge;
};


class HEedge {
public:
    HEedge() { v = nextHE = invHE = polygon = -1; }

public:
    int v;
    int nextHE;
    int invHE;
    int polygon;
    int meshID;
};


class HEpolygon {
public:
    HEpolygon() { firstHE = -1; }
    int firstHE;

};


class HEmesh {
public:
    HEmesh() {}
    ~HEmesh();

    void addVertex(Vertex v);
    void addVertex(float x, float y, float z);
    void addPolygon(int n, int* indices);
    vector<HEmesh> findDisconnectedMeshes();

    // Queries...
    void size(int& numpts, int& numpol);
    int vertexNeighbours(int vtx_no, vector<int>& neighvtx);
    void adjacentPolygons(int pol_no, vector<int>& neighpol);
    void getVertices(int pol_no, vector<int>& verts);
    int oppositeVertices(int tri_no, int& o1, int& o2, int& o3);
    void oppositeTrisVerts(int tri_no, int* t, int* v);
    int getBoundaries(vector<int>& bVertices, vector<int>& loopExtents);
    bool checkNonManifoldEdges();
    int triangulateLoops(vector<int>& bVertices, vector<int>& loopExtents,
                         vector<int>& dummyVtxIndices);

    void clearTable();

    void dumpall();

    vector<HEvertex>  vertexlist;
    vector<HEedge>    edgelist;
    vector<HEpolygon> polylist;
    vector<int> startCorners;
    vector<int> startPolygon;
    int numberOfDisconnectedMesh;

    void fixWindingOrders();

private:
    vector< vector<int> > _tmptable;

    void _growBoundary(int edg, vector<bool>& visited,
                       vector<int>& bVertices, vector<int>& loopExtents);

};

#endif
