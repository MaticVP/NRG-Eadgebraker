//
// Created by matic on 30/03/2025.
//

#ifndef EDGEBRAKER_EBUTILS_H
#define EDGEBRAKER_EBUTILS_H

#ifndef _EB_UTILS_H_
#define _EB_UTILS_H_

#include "EBdatatypes.h"

Vector VMinus(Vertex v1, Vertex v2);
Vector VPlus(Vertex v1, Vector v2);
Vector VMult(Vertex v1, float k);

inline int NextEdge(int edge) { return (3*(edge / 3) + (edge + 1) % 3); }

inline int PrevEdge(int edge) { return NextEdge(NextEdge(edge)); }

//	c.r = c.n.r
inline int RightTri(int c, int* O_table) { return O_table[NextEdge(c)]; }

//c.l = c.n.n.r
inline int LeftTri(int c, int* O_table) { return O_table[NextEdge(NextEdge(c))]; }

inline int E2T(int edge) { return (edge / 3); }

void PrintErrorAndQuit(char* sErrorString);

#endif

#endif //EDGEBRAKER_EBUTILS_H
