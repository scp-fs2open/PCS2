//****************************************************************************
// BSPHandler.h
// Author: Derek Meek
// BSP/IDTA Data Handler class
//****************************************************************************

/*
 * $Logfile: /BSPHandler.h $
 * $Revision: 1.4 $
 * $Date: 2008/02/09 22:28:08 $
 * $Author: kazan $
 *
 * $Log: BSPHandler.h,v $
 * Revision 1.4  2008/02/09 22:28:08  kazan
 * checkin before i do something drastic (pull out my templates - don't need them anymore)
 *
 * Revision 1.3  2005/09/19 21:47:12  kazan
 * Linux compat changes
 *
 * Revision 1.2  2005/08/23 22:36:42  kazan
 * s/std::{vector:string}/kaz_{vector:string}
 * renderer motion works
 * location PMF::LoadFromPOF bottleneck
 *
 * Revision 1.1.1.1  2004/12/31 06:19:41  kazan
 * no message
 *
 *
 *
 */

#include "BSPDataStructs.h"
#include <ios>


#if !defined(_BSP_HANDLER_H_)
#define _BSP_HANDLER_H_


class BSP
{
	public:
		BSP_BoundBox	*bounders;
		BSP_DefPoints	*points;
		BSP_FlatPoly	*fpolys;
		BSP_SortNorm	*snorms;
		BSP_TmapPoly	*tpolys;

		int numbounders, numpoints, numfpolys, numsnorms, numtpolys;
		void Clear()
		{
			numbounders = numpoints = numfpolys = numsnorms = numtpolys = 0;
			bounders = NULL;
			points = NULL;
			fpolys = NULL;
			snorms = NULL;
			tpolys = NULL;
		}

		void Reset()
		{
			delete[] bounders;
			delete[] points;
			delete[] fpolys;
			delete[] snorms;
			delete[] tpolys;
			
			Clear();
		}
		BSP()
			{ Clear(); }
		BSP(char *buffer, int size)
			{ Clear();
			  DataIn(buffer, size); }
		//~BSP();

		//--------------------------------
		std::string DataIn(char *buffer, int size);
		std::ostream& BSPDump(std::ostream &os); // dumps human readable BSP information into ostream;

		int Count_Bounding()
			{ return numbounders; }

		int Count_Points()
			{ return numpoints; }
		
		int Count_FlatPolys()
			{ return numfpolys; }

		int Count_SortNorms()
			{ return numsnorms; }

		int Count_TmapPolys()
			{ return numtpolys; }

		//--------------------------------
		
		void Add_BoundBox(BSP_BoundBox bound);
		bool Del_BoundBox(int index);

		void Add_DefPoints(BSP_DefPoints pnts);
		bool Del_DefPoints(int index);

		void Add_FlatPoly(BSP_FlatPoly fpol);
		bool Del_FlatPoly(int index);

		void Add_SortNorm(BSP_SortNorm sn);
		bool Del_SortNorm(int index);

		void Add_TmapPoly(BSP_TmapPoly tpol);
		bool Del_TmapPoly(int index);

};


std::ostream& operator<<( std::ostream &os, BSP_TmapPoly tpoly);
std::ostream& operator<<( std::ostream &os, BSP_FlatPoly fpoly);
bool operator==(BSP_TmapPoly &a, BSP_TmapPoly &b);
bool operator==(BSP_FlatPoly &a, BSP_FlatPoly &b);

#endif //_BSP_HANDLER_H_
