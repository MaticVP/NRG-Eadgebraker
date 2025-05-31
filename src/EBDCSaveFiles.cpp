//
// Created by matic on 31/03/2025.
//


#include <assert.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "EBUtils.h"
#include "EdgebreakerDecompress.h"
#include <algorithm>
#include <filesystem>

using namespace std;

void saveOFF(char *filename, vector<HEmesh> hems) {
    ofstream ofs(filename);

    ofs << "OFF" << endl;

    int numberOfPoints = 0;
    int numberOfTriangles = 0;

    for(auto & hem : hems){
        numberOfPoints+=hem.vertexlist.size();
        numberOfTriangles+=hem.polylist.size();
    }

    ofs << numberOfPoints << " " << numberOfTriangles << " 0" << endl;

    for(auto hem : hems) {
        for (int i = 0; i < hem.vertexlist.size(); i++) {
            Vertex v = hem.vertexlist[i].pt;
            ofs << v.x << " " << v.y << " " << v.z << endl;
        }
    }

    int idOffset = 0;

    for(auto hem : hems) {

        for (int i = 0; i < hem.polylist.size(); ++i) {
            vector<int> vtxs;
            hem.getVertices(i, vtxs);
            ofs << "3 " << vtxs[0]+idOffset << " " << vtxs[1]+idOffset << " " << vtxs[2]+idOffset << endl;
        }

        idOffset+= hem.vertexlist.size();
    }

    ofs.close();
}

void saveOBJ(const char* filename, vector<HEmesh>& hems) {
    ofstream ofs(filename);
    if (!ofs) {
        cerr << "Error: Could not open file " << filename << " for writing.\n";
        return;
    }

    int vertexOffset = 0;

    // Write vertices
    for (const auto& hem : hems) {
        for (const auto& vertex : hem.vertexlist) {
            const Vertex& v = vertex.pt;
            ofs << "v " << v.x << " " << v.y << " " << v.z << endl;
        }
    }

    // Write faces
    for (auto& hem : hems) {
        for (int i = 0; i < hem.polylist.size(); ++i) {
            vector<int> vtxs;
            hem.getVertices(i, vtxs);

            // OBJ indexing is 1-based
            ofs << "f "
                << vtxs[0] + vertexOffset + 1 << " "
                << vtxs[1] + vertexOffset + 1 << " "
                << vtxs[2] + vertexOffset + 1 << endl;
        }

        vertexOffset += hem.vertexlist.size();
    }

    ofs.close();
}

int countNumberOfMeshes(char* dirPath){
    int count = 0;

    namespace fs = std::filesystem;

    try {
        for (const auto& entry : fs::directory_iterator(dirPath)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                std::string num = filename.substr(0,filename.find("."));
                count = max(std::stoi(num),count);

            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
        return -1;
    }

    return count;
}

void EdgebreakerDecompress::cleanupDummy()
{
    int numtri = C.size() + 1;
    vector<bool> trimask = vector<bool>(numtri, true);

    // mark triangles to be discarded and modify indices...
    int dummy_count = dummy.size();
    vector<int> dc = vector<int>(dummy_count);
    copy(dummy.begin(), dummy.end(), dc.begin());

    for (int i = 0; i < dummy_count; i++) {
        for (int j = 0; j < numtri; j++) {
            if (trimask[j] == false)
                continue;
            if ((V[3*j] == dc[i]) || (V[3*j+1] == dc[i]) || (V[3*j+2] == dc[i]))
                trimask[j] = false;
            else {
                for (int k = 0; k < 3; k++) {
                    if (V[3*j+k] > dc[i])
                        V[3*j+k] -= 1;
                }
            }
        }
        for (int j = i+1; j < dummy_count; j++) {
            dc[j] -= 1;
        }
    }

    // now populate HEmesh...
    int numpts = G_in.size();
    if (dummy_count > 0) {
        int from = 0;
        int to;
        for (int i = 0; i < dummy_count; i++) {
            to = dummy[i];
            for (int j = from; j < to; j++) {
                Vertex p = G[j];
                hem.addVertex(p);
            }
            from = to+1;
        }
        if (from < numpts) {
            to = numpts;
            for (int j = from; j < to; j++) {
                Vertex p = G[j];
                hem.addVertex(p);
            }
        }
    }
    else {
        for (int i = 0; i < numpts; i++) {
            Vertex p = G[i];
            hem.addVertex(p);
        }
    }

    for (int i = 0; i < numtri; i++) {
        if (trimask[i]) {
            int v[3];
            v[0] = V[3*i]; v[1] = V[3*i+1]; v[2] = V[3*i+2];
            hem.addPolygon(3, v);
        }
    }
}