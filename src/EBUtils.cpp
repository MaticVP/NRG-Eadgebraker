//
// Created by matic on 31/03/2025.
//

#include <iostream>
#include "EBUtils.h"

Vector VMinus(Vertex v1, Vertex v2){
    Vector tempVector;
    tempVector.x = v1.x - v2.x;
    tempVector.y = v1.y - v2.y;
    tempVector.z = v1.z - v2.z;

    return tempVector;
}

Vector VPlus(Vertex v1, Vector v2)
{
    Vector tempVector;
    tempVector.x = v2.x + v1.x;
    tempVector.y = v2.y + v1.y;
    tempVector.z = v2.z + v1.z;

    return tempVector;
}

Vector VMult(Vertex v1, float k)
{
    Vector tempVector;
    tempVector.x = v1.x*k;
    tempVector.y = v1.y*k;
    tempVector.z = v1.z*k;

    return tempVector;
}

void PrintErrorAndQuit(char* sErrorString)
{
    fprintf(stderr, "%s\n", sErrorString);
    //cerr << sErrorString << endl;
    exit(0);
}