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
		std::vector<BSP_BoundBox> bounders;
		std::vector<BSP_DefPoints> points;
		std::vector<BSP_FlatPoly> fpolys;
		std::vector<BSP_SortNorm> snorms;
		std::vector<BSP_TmapPoly> tpolys;

		int numbounders, numpoints, numfpolys, numsnorms, numtpolys;
		void Clear()
		{
			bounders.clear();
			points.clear();
			fpolys.clear();
			snorms.clear();
			tpolys.clear();
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
			{ return bounders.size(); }

		int Count_Points()
		{
			return points.size();
		}
		
		int Count_FlatPolys()
		{
			return fpolys.size();
		}

		int Count_SortNorms()
		{
			return snorms.size();
		}

		int Count_TmapPolys()
		{
			return tpolys.size();
		}

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
