/***********************************************************************\
 *
 * File:           RngStream.cpp for multiple streams of Random Numbers
 * Language:       C++ (ISO 1998)
 * Copyright:      Pierre L'Ecuyer, University of Montreal
 * Notice:         This code can be used freely for personal, academic,
 *                 or non-commercial purposes. For commercial purposes, 
 *                 please contact P. L'Ecuyer at: lecuyer@iro.umontreal.ca
 * Date:           14 August 2001
 *
 * MarTo additions:23 May 2017 	   
 *
 *
 * If you use this software for work leading to publications, 
 * please cite the following relevant articles in which MRG32k3a 
 * and the package with multiple streams were proposed:

 * P. L'Ecuyer, ``Good Parameter Sets for Combined Multiple Recursive Random Number Generators'', 
 * Operations Research, 47, 1 (1999), 159--164.

 * P. L'Ecuyer, R. Simard, E. J. Chen, and W. D. Kelton, 
 * ``An Objected-Oriented Random-Number Package with Many Long Streams and Substreams'', 
 * Operations Research, 50, 6 (2002), 1073--1075
 *
\***********************************************************************/

 
#ifndef RNGSTREAM_H
#define RNGSTREAM_H
 
#include <string>

class RngStream
{
public:

RngStream (const char *name = "");


static bool SetPackageSeed (const unsigned long seed[6]);


void ResetStartStream ();


void ResetStartSubstream ();


void ResetNextSubstream ();


void SetAntithetic (bool a);


void IncreasedPrecis (bool incp);


bool SetSeed (const unsigned long seed[6]);


void AdvanceState (long e, long c);


void GetState (unsigned long seed[6]) const;


void WriteState () const;


void WriteStateFull () const;


double RandU01 ();


int RandInt (int i, int j);



private:

double Cg[6], Bg[6], Ig[6];


bool anti, incPrec;


std::string name;


static double nextSeed[6];


double U01 ();


double U01d ();


};
 
#endif
 

