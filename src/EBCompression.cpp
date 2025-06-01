//
// Created by matic on 30/03/2025.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "EBUtils.h"
#include "EdgebreakerCompress.h"

void EdgeBreakerCompress::initCompression(int c, MeshType eMeshType) {
    int i = 0;
    T = 0;

    c = PrevEdge(c);

    EncodeDelta(NextEdge(c));

    if (eMeshType == MANIFOLD) {
        M[V[NextEdge(c)]] = 1;
    }

    EncodeDelta(c);
    M[V[c]] = 1;

    EncodeDelta(PrevEdge(c));

    M[V[PrevEdge(c)]] = 1;

    U[E2T(c)] = 1;

    int a = O[c];

    int count = 1;

    while (a != PrevEdge(O[PrevEdge(c)])){
        count++;

        U[E2T(a)] = 1;
        T++;

        EncodeDelta(a);
        M[V[a]] = 1;

        a = O[NextEdge(a)];
    }

    U[E2T(a)] = 1;

    T++;
    count++;

    it1_count = (int) count;

    Compress(O[PrevEdge(a)]);
}

void EdgeBreakerCompress::Compress(int c){
    //start traversal for triangle tree
    do
    {
        //mark the triangle as visited
        U[E2T(c)] = 1;
        T++;

        //check for handles
        CheckHandle(c);

        //test whether tip vertex was visited
        if (M[V[c]] == 0)
        {
            //append encoding of C to clers
            // fprintf(fclers, "%c\n", 'C');
            clers.push_back('C');

            //estimate next vertex and mark it as visited
            EncodeDelta(c);
            M[V[c]] = 1;

            //continue with the right neighbor
            c = RightTri(c, O);
        }
        else
            //test whether right triangle was visited
        if (U[E2T(RightTri(c, O))] > 0)
        {
            //test whether left triangle was visited
            if (U[E2T(LeftTri(c, O))] > 0)
            {
                //append code for E and pop
                // fprintf(fclers, "%c\n", 'E');
                clers.push_back('E');
                return;
            }
            else
            {
                //append code for R, move to left triangle
                // fprintf(fclers, "%c\n", 'R');
                clers.push_back('R');
                c = LeftTri(c, O);
            }
        }
        else
            //test whether left triangle was visited
        if (U[E2T(LeftTri(c, O))] > 0)
        {
            //append code for L, move to right triangle
            // fprintf(fclers, "%c\n", 'L');
            clers.push_back('L');
            c = RightTri(c, O);
        }
        else
        {
            //store corner number in decompression, to support handles
            U[E2T(c)] = T*3+2;

            //append code for S
            // fprintf(fclers, "%c\n", 'S');
            clers.push_back('S');

            //recursive call to visit right branch first
            Compress(RightTri(c, O));

            //move to left triangle
            c = LeftTri(c, O);

            //if the triangle to the left was visited, then  return
            if (U[E2T(c)]>0)
                return;
        }
    } while(true);
}

void EdgeBreakerCompress::CheckHandle(int c){
    if(U[E2T(O[NextEdge(c)])] > 1){
        handles.push_back(U[E2T(O[NextEdge(c)])]);
        handles.push_back(T*3+1);
    }

    if(U[E2T(O[PrevEdge(c)])] > 1){

        handles.push_back(U[E2T(O[PrevEdge(c)])]);
        handles.push_back(T*3+2);

    }
}

void EdgeBreakerCompress::EncodeNoPrediction(int c) {
    vertices.push_back(G[V[c]]);
}

void EdgeBreakerCompress::EncodeWithPrediction(int c)
{
    Vector vPred, delta;
    Vertex zeroV = Vertex(0.0, 0.0, 0.0);

    if (M[V[O[c]]] > 0 && M[V[PrevEdge(c)]] > 0)
    {
        vPred = VPlus(G_est[V[NextEdge(c)]], G_est[V[PrevEdge(c)]]);
        vPred = VMinus(vPred, G_est[V[O[c]]]);
        delta = VMinus(G[V[c]], vPred);
        //return vPred;
    }
    else if (M[V[O[c]]] > 0)
    {
        vPred = VMult(G_est[V[NextEdge(c)]], 2);
        vPred = VMinus(vPred, G_est[V[O[c]]]);
        delta = VMinus(G[V[c]], vPred);
        //return vPred;
    }
    else if (M[V[NextEdge(c)]] > 0 && M[V[PrevEdge(c)]] > 0)
    {
        vPred = VPlus(G_est[V[NextEdge(c)]], G_est[V[PrevEdge(c)]]);
        vPred = VMult(vPred, 0.5f);
        delta = VMinus(G[V[c]], vPred);
        //return vPred;
    }
    else if (M[V[NextEdge(c)]] > 0)
    {
        vPred = G_est[V[NextEdge(c)]];
        delta = VMinus(G[V[c]], vPred);
        //return vPred;
    }
    else if (M[V[PrevEdge(c)]] > 0)
    {
        vPred = G_est[V[PrevEdge(c)]];
        delta = VMinus(G[V[c]], vPred);
        //return vPred;
    }
    else
    {
        vPred = zeroV;
        delta = VMinus(G[V[c]], vPred);
    }

    G_est[V[c]] = VPlus(delta, vPred);
    vertices.push_back(delta);
}

void EdgeBreakerCompress::EncodeDelta(int c)
{
    if ((dummyVertices.size() > 0) && (V[c] >= dummyVertices[0]))
        newDummy.push_back(vertices.size());

    EncodeNoPrediction(c);
    //EncodeWithPrediction(c);

}