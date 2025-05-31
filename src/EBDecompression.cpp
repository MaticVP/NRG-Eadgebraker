#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <vector>
#include "EBdatatypes.h"
#include "EBUtils.h"
#include "EdgebreakerDecompress.h"

using namespace std;


void EdgebreakerDecompress::initDecompression()
{
    int i = 0;

    T = 0;
    N = 2;
    A = 0;

    InitDecompressionModule();
    DecompressConnectivity(2);
    G[0] = DecodeDelta(0);

    if (eMeshType == MANIFOLD) M[0] = 1;

    G[1] = DecodeDelta(2);
    M[1] = 1;

    G[2] = DecodeDelta(1);
    M[2] = 1;

    N = 2;
    U[0] = 1;

    DecompressVertices(O[2]);
}

void EdgebreakerDecompress::InitDecompressionModule()
{
    int nNumOfTriangles = C.size() + 1;
    int nNumOfVertices = G_in.size();

    V = new int[3*nNumOfTriangles];
    O = new int[3*nNumOfTriangles];

    for (int i = 0; i<3*nNumOfTriangles; i++) {O[i] = -3; V[i] = 0;}

    V[0] = 0; V[1] = 2; V[2] = 1;
    O[0] = -1; O[1] = -1;

    U = new int[nNumOfTriangles];
    M = new int[nNumOfVertices];

    for (int i = 0; i < nNumOfTriangles; i++) U[i] = 0;
    for (int i = 0; i < nNumOfVertices; i++) M[i] = 0;

    G = new Vertex[nNumOfVertices];
}


void EdgebreakerDecompress::DecompressConnectivity(int c)
{
    do {
        T++;

        O[c] = 3*T;	O[3*T] = c;

        V[3*T+1] = V[PrevEdge(c)];
        V[3*T+2] = V[NextEdge(c)];

        c = NextEdge(O[c]);

        switch (C[T-1])	{
            case 'C':
                O[NextEdge(c)] = -1;
                V[3*T] = ++N;
                break;
            case 'L':
                O[NextEdge(c)] = -2;
                if (!CheckHandle(NextEdge(c)))
                    Zip(NextEdge(c));
                break;
            case 'R':
                O[c] = -2;
                CheckHandle(c);
                c = NextEdge(c);
                break;
            case'S':
                DecompressConnectivity(c);
                c = NextEdge(c);
                if (O[c] >= 0)
                    return;
                break;
            case 'E':
                O[c] = -2;
                O[NextEdge(c)] = -2;
                CheckHandle(c);
                if (!CheckHandle(NextEdge(c)))
                    Zip(NextEdge(c));
                return;
        }
    } while (true);
}


bool EdgebreakerDecompress::CheckHandle(int c)
{

    if (A >= H.size() || c != H[A+1])
        return false;
    else {
        O[c] = H[A];
        int len = (sizeof(O)/sizeof(O[c]));
        if(len < H[A] && 0 > H[A]){
            return false;
        }
        O[H[A]] = c;

        int a = PrevEdge(c);
        while ( (O[a] >=0) && (a != H[A]) )
            a = PrevEdge(O[a]);

        if (O[a] == -2)
            Zip(a);

        a = PrevEdge(O[c]);
        while ( (O[a] >=0) && (a != c) )
            a = PrevEdge(O[a]);

        if (O[a] == -2)
            Zip(a);

        //next handle
        A += 2;
        return true;
    }
}

void EdgebreakerDecompress::Zip(int c)
{
    int b = NextEdge(c);

    while (O[b] >= 0 && O[b] != c) {
        b = NextEdge(O[b]);
    }

    if (O[b] != -1) {
        return;
    }

    O[c] = b; O[b] = c;

    int a = NextEdge(c);
    V[NextEdge(a)]=V[NextEdge(b)];
    while (O[a]>=0 && a!=b) {
        a = NextEdge(O[a]);
        V[NextEdge(a)]=V[NextEdge(b)];
    }

    c = PrevEdge(c);
    while (O[c] >=0 && c!= b) {
        c = PrevEdge(O[c]);
    }

    if (O[c] == -2)
        Zip(c);
}

void EdgebreakerDecompress::DecompressVertices(int c)
{
    do {
        U[E2T(c)] = 1;

        if (M[V[c]] == 0) {
            N++;
            G[N] = DecodeDelta(c);
            M[V[c]] = 1;
            c = RightTri(c, O);
        }
        else
        if (U[E2T(RightTri(c, O))] == 1) {
            if (U[E2T(LeftTri(c, O))] == 1)
            {
                return;
            }
            else
            {
                c = LeftTri(c, O);
            }
        }
        else
        if (U[E2T(LeftTri(c, O))] == 1) {
            c = RightTri(c, O);
        }
        else {
            DecompressVertices(RightTri(c, O));
            c = LeftTri(c, O);
            if (U[E2T(c)]>0)
                return;
        }
    } while (true);
}


Vertex EdgebreakerDecompress::DecodeNoPrediction(int c)
{
    return G_in[V[c]];
}

Vertex EdgebreakerDecompress::DecodeWithPrediction(int c)
{
    static int EBVcount = 0;

    Vertex vPred;
    Vector delta = G_in[EBVcount];
    EBVcount++;

    if (M[V[O[c]]] > 0 && M[V[PrevEdge(c)]] > 0) {
        vPred = VPlus(G[V[NextEdge(c)]], G[V[PrevEdge(c)]]);
        vPred = VMinus(vPred, G[V[O[c]]]);
        vPred = VPlus(delta, vPred);
        return vPred;
    }

    if (M[V[O[c]]] > 0) {
        vPred = VMult(G[V[NextEdge(c)]], 2);
        vPred = VMinus(vPred, G[V[O[c]]]);
        vPred = VPlus(delta, vPred);
        return vPred;
    }

    if (M[V[NextEdge(c)]] > 0 && M[V[PrevEdge(c)]] > 0) {
        vPred = VPlus(G[V[NextEdge(c)]], G[V[PrevEdge(c)]]);
        vPred = VMult(vPred, 0.5f);
        vPred = VPlus(delta, vPred);
        return vPred;
    }

    if (M[V[NextEdge(c)]] > 0) {
        vPred = G[V[NextEdge(c)]];
        vPred = VPlus(delta, vPred);
        return vPred;
    }
    else if (M[V[PrevEdge(c)]] > 0) {
        vPred = G[V[PrevEdge(c)]];
        vPred = VPlus(delta, vPred);
        return vPred;
    }
    else {
        vPred = delta;
        return vPred;
    }
}

Vertex EdgebreakerDecompress::DecodeDelta(int c)
{
    return DecodeWithPrediction(c);
}