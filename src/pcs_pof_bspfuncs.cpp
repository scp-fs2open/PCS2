//****************************************************************************
//	pcs_pof_bspfuncs.cpp
//****************************************************************************

/*
 * $Logfile: /pcs_pof_bspfuncs.cpp $
 * $Revision: 1.35 $
 * $Date: 2008/11/08 15:13:19 $
 * $Author: kazan $
 *
 * $Log: pcs_pof_bspfuncs.cpp,v $
 * Revision 1.35  2008/11/08 15:13:19  kazan
 * Updated to wxWidgets 2.8.9 and switched from static linkage to DLL linkage.
 *
 * Revision 1.34  2008/02/09 23:01:46  kazan
 * post-kaz_templates removal cleanup
 *
 * Revision 1.33  2008/02/09 22:33:01  kazan
 * bye bye kaz_vector
 *
 * Revision 1.32  2008/02/09 22:31:27  kazan
 * finish removing string
 *
 * Revision 1.31  2008/01/31 17:02:24  kazan
 * Add error checking to BSP tree packing, this will slow packing somewhat, but will prevent crashes.
 * Correct error in calculating tree size.
 *
 * Revision 1.30  2008/01/10 18:28:10  bobboau
 * *** empty log message ***
 *
 * Revision 1.29  2007/10/18 15:01:02  kazan
 * Duplicate polygons still sent us into infinite recursion - made contained.size()==2 case check to see if they have the same centeroid, if so it immediately makes a polynode with both instead of a sortnorm
 *
 * Revision 1.28  2007/10/18 14:54:02  kazan
 * Fix accuracy problem with BSP depth counter, error BSP_COMPILATION_ERROR flag, reenable infinite recursion catch due to duplicate-polygon-centers still causing infinite recursion
 *
 * Revision 1.27  2007/10/18 14:18:56  kazan
 * Improve some error handling, including a critical thread errors causing premature termination
 *
 * Revision 1.26  2007/07/18 21:14:11  kazan
 * fix a fair of minus "oops"es
 *
 * Revision 1.25  2007/07/16 23:06:09  bobboau
 * fixed a rare but eaisily reproduced BSP bug that resulted in duplicate polygons, loading a simple cube is all that is needed to see the bug in action, this code manages it well, it forces it to try new splits so long as it is produceing duplicate polys.
 *
 * Revision 1.24  2007/07/16 06:28:07  kazan
 * GenerateTreeRecursion can return null, occasionally causing accessing null objects
 *
 * Revision 1.23  2007/07/13 18:22:21  kazan
 * implement SLDC (Shield Collision Tree) chunk
 *
 * Revision 1.22  2007/07/11 19:14:49  bobboau
 * added a 'load textureexternaly' button, made the experimental BSP code the normal BSP code,
 * the load texture feature uses a win32 specific comand because I couldn't find a good way to get it to work in a non-platform specific manner. I will continue to search for a better solution.
 *
 * and fixed a render (ships being drawn transparently when they shouldn't be) and crash (trying to update a progress bar that doesn't exsist) bug
 *
 * Revision 1.21  2007/07/09 00:07:13  bobboau
 * sence neither I nor Kazan seems to have a solid understanding on how to use branching, I've decided to commit the experimental BSP *within _BOB_BSP_ ifdefs* to the main branch. sence it's mostly a change to one function it makes even more sence to do this IMHO. I also added sevral statistic trackers to be added in the model coments section, I now have the BSP compiler track the maximum number of polygons packed into a single node, the maximum depth of BSP recursion for a model, and compile time (both total and tree generation), in both versions.
 *
 * Revision 1.20  2007/07/03 02:52:21  bobboau
 * trying to undo the commit to the wrong branch..
 *
 * Revision 1.18  2007/07/02 23:40:09  kazan
 * this didn't fix the problem, but it's still better than the old code
 *
 * Revision 1.17  2007/07/02 20:17:49  kazan
 * undo bob's changes to the bounding box functions
 *
 * Revision 1.16  2007/07/01 20:16:21  bobboau
 * commiting experimental multi-poly-per-node code
 *
 * Revision 1.15  2007/06/30 14:13:13  kazan
 * put back in a little safer version of the contained.size() == 2 optimization
 * potentially fix pof shields
 * add error message for group with no polymodels
 *
 * Revision 1.14  2007/06/27 21:13:58  kazan
 * maybe, possibly, we hope i just fixed BSP collision trees
 *
 * Revision 1.13  2007/06/27 00:01:15  kazan
 * trying to squash bugs
 *
 * Revision 1.12  2007/06/25 19:39:33  kazan
 * Fix bug related to std::string and operator+= w/ char (not char*)
 * Fix autoturret bugs
 * Fix POF BSP compilation... finally.. confirmed models work in FS2!
 *
 * Revision 1.11  2007/06/23 19:27:15  kazan
 * should hopefully fix POFs
 *
 * Revision 1.10  2007/06/18 18:43:26  kazan
 * added BSP Boundbox/Sortnorm debug renderer
 * used said renderer to find and solve several BSP compilation bugs
 *
 * Revision 1.9  2007/06/18 01:15:02  kazan
 * Resolve #005, add cache-bsp-on-generate, add .25s sleeps to each cycle on the GUI side while waiting for threads... was spiking both CPUs in my big machine.. and that's not right
 *
 * Revision 1.8  2007/06/05 06:29:25  bobboau
 * initial Bobboau UI commit, everything should be minimuly functional but working includeing editors for all major chuncks and the tree navigation control.
 *
 * Revision 1.7  2007/03/25 17:13:50  kazan
 * more signed/unsigned cleanup
 *
 * Revision 1.6  2007/03/25 03:17:46  kazan
 * Implemented COB importer/autogen features - i haven't implemented them
 * cleaned up a bunch of signed/unsigned bitching - more cleaning to come
 *
 * Revision 1.5  2005/09/19 21:47:12  kazan
 * Linux compat changes
 *
 * Revision 1.4  2005/08/30 04:45:15  kazan
 * load/save progress bars, wireframe/solid/texture toggle, app icon, enabled bsp splitting special case count=2
 *
 * Revision 1.3  2005/08/29 20:28:21  kazan
 * cleanup header and remove dead code
 *
 * Revision 1.2  2005/08/29 02:26:43  kazan
 * don't triangulate, and fall back to point-average when the centroid function panics
 *
 * Revision 1.1  2005/08/29 01:25:06  kazan
 * Saves POFs succesfully -- completely new BSP Gen routine
 * Loads and Renders Textures
 *
 *
 *
 */


#include "pcs_pof_bspfuncs.h"
#include <algorithm>
#include <cmath>
#include <limits>
#include <wx/stopwatch.h>

#undef max

vector3d POFTranslate(vector3d v)
{
	v.x = -v.x;
	return v;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

bool operator==(const bsp_vert &one, const bsp_vert &two)
{
	return one.point == two.point;
}


//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-


pcs_polygon RebuildCleanPolygon(pcs_polygon &src)
{
	pcs_polygon dst;

	if (src.verts.size() < 3)
		return dst; // not really a valid polygon
	int used = 1;
	dst.norm = src.norm;
	dst.texture_id = src.texture_id;
	dst.verts.resize(src.verts.size());
	dst.verts[0] = src.verts[0];
	vector3d last_point = src.verts[0].point;

	for (unsigned int i = 1; i < src.verts.size(); i++)
	{
		if (i < src.verts.size()-1)
		{
			if (Distance(last_point, src.verts[i].point) > 0.01)
			{
				dst.verts[used] = src.verts[i];
				used++;
			}
		}
		else
		{
			if (Distance(last_point, src.verts[i].point) > 0.01 && Distance(src.verts[i].point, src.verts[0].point) > 0.01)
			{
				dst.verts[used] = src.verts[i];
				used++;
			}
		}
	}
	dst.verts.resize(used);
	return dst;
}
//****************************************************************************
// New BSP Functions... beware
//****************************************************************************


int PackTreeInBSP(bsp_tree_node* root, int offset, char *buffer, std::vector<pcs_polygon> &polygons,
	std::unordered_map<vector3d, int> &norms, std::unordered_map<vector3d, int> &verts, BSP_DefPoints &dpnts, vector3d geo_center, int buffsize, int &error_flags)
{
	// ----------- error detection ---------------
	// abort if error detected
	if (error_flags != BSP_NOERRORS)
		return 0; 

	// we're going to write off the end of the buffer gauranteed (smallest element is EOF - 8 bytes
	if (offset >= buffsize-7)
	{
		error_flags |= BSP_PACK_PREOVERFLOW;
		return 0;
	}
	// either we're written this node already, or it didn't get counted in size calculation.. this is bad, skip
	if (root != NULL && root->used == true)
	{
		error_flags |= BSP_PACK_DOUBLEUSE;
		return 0;
	}

	if (root != NULL && root->counted == false)
	{
		error_flags |= BSP_PACK_UNCOUNTED;
		return 0;
	}

	// ----------- finish error detection ---------------
	int size = 0;
	BSP_BlockHeader EndOfFile;
	EndOfFile.id = 0;
	EndOfFile.size = 0;

	BSP_BoundBox Bounding;
	Bounding.head.id = 5;
	Bounding.head.size = Bounding.MySize();

	BSP_TmapPoly tpoly;
	BSP_FlatPoly fpoly;

	BSP_SortNorm snorm;	
	
	if (root == NULL)
	{
		EndOfFile.Write(buffer+offset);
		return 8;
	}
	switch(root->Type)
	{
		case POLY:
			// write BBOX
			Bounding.max_point = root->bound_max;
			Bounding.min_point = root->bound_min;
			Bounding.Write(buffer+offset+size);
			size += Bounding.MySize();

			if (offset+CalculateTreeSize(root, polygons) > buffsize)
			{
				error_flags |= BSP_PACK_PREPOLYOVERFLOW;
				return 0;
			}
			// Size is BBOX (32 bytes) + Poly (variable on type and count) + EOF ( 8 bytes)
			
			// write POLY
			for(unsigned int i = 0; i<root->poly_num.size(); i++){
				if (polygons[root->poly_num[i]].texture_id == -1)
				{
					MakeFlatPoly(fpoly, polygons[root->poly_num[i]], norms, verts, dpnts);

					fpoly.Write(buffer+offset+size);
					size += fpoly.MySize();
				}
				else 
				{
					MakeTmapPoly(tpoly, polygons[root->poly_num[i]], norms, verts, dpnts);

					tpoly.Write(buffer+offset+size);
					size += tpoly.MySize();
				}
			}
			// write EOF
			EndOfFile.Write(buffer+offset+size);
			size += EndOfFile.MySize();
			root->used = true;

			if (offset+size > buffsize)
			{
				error_flags |= BSP_PACK_POLYOVERFLOW;
			}
			return size;

		default: //SPLIT
			size = 80;
			memset((char*)&snorm, 0, sizeof(BSP_SortNorm));
			snorm.head.id = 4;
			snorm.head.size = snorm.MySize();
			snorm.plane_point = root->point;
			snorm.plane_normal = root->normal;
			snorm.max_bounding_box_point = root->bound_max;
			snorm.min_bounding_box_point = root->bound_min;

			if (offset+CalculateTreeSize(root, polygons) > buffsize)
			{
				error_flags |= BSP_PACK_PRESPLITOVERFLOW;
				return 0;
			}

			snorm.prelist_offset = size;
			size += PackTreeInBSP(NULL, offset+size, buffer, polygons, norms, verts, dpnts, geo_center, buffsize, error_flags);
				
			snorm.postlist_offset = size;
			size += PackTreeInBSP(NULL, offset + size, buffer, polygons, norms, verts, dpnts, geo_center, buffsize, error_flags);
				
			snorm.online_offset = size;
			size += PackTreeInBSP(NULL, offset + size, buffer, polygons, norms, verts, dpnts, geo_center, buffsize, error_flags);
			
			snorm.front_offset = size;
			size += PackTreeInBSP(root->front.get(), offset+size, buffer, polygons, norms, verts, dpnts, geo_center, buffsize, error_flags);

			snorm.back_offset = size;
			size += PackTreeInBSP(root->back.get(), offset+size, buffer, polygons, norms, verts, dpnts, geo_center, buffsize, error_flags);

			snorm.Write(buffer+offset);

			// write a trailing BSP::EOF to make ourselves safe

			//EndOfFile.Write(buffer+offset+size);
			//size += EndOfFile.MySize();
			
			root->used = true;
			if (offset+size > buffsize)
			{
				error_flags |= BSP_PACK_SPLITOVERFLOW;
			}
			return size;
	}
	return 0;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

int CalculateTreeSize(bsp_tree_node* root, std::vector<pcs_polygon> &polygons)
{
	if (root == NULL)
		return 8; // size of an BSP::EOF

	int ret_size = 0;
	switch(root->Type)
	{
		case POLY:
			// Size is BBOX (32 bytes) + Poly (variable on type and count) + EOF ( 8 bytes)
			ret_size += 32;
			for(unsigned int i = 0; i<root->poly_num.size(); i++){
				if (polygons[root->poly_num[i]].texture_id == -1)
					ret_size += 44 + 4 * polygons[root->poly_num[i]].verts.size();// Fplatpoly
				else 
					ret_size += 44 + 12 * polygons[root->poly_num[i]].verts.size();//Tmappoly
			}
			ret_size += 8;
			root->counted = true;
			return ret_size;

		default: //SPLIT
			root->counted = true;
			return 104 + CalculateTreeSize(root->front.get(), polygons) + CalculateTreeSize(root->back.get(), polygons); // 80 + 3*8 bytes for pre/post/onlist EOFs
	}
	return 0;

}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-


int CalcSLDCTreeSize(bsp_tree_node* root)
{
	if (root == NULL)
		return 0;
	switch(root->Type)
	{
		case POLY:
			return 33 + root->poly_num.size()*sizeof(int);

		default: //SPLIT
			return 37 + CalcSLDCTreeSize(root->front.get()) + CalcSLDCTreeSize(root->back.get());
	}
	return 0;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

int PackTreeInSLDC(bsp_tree_node* root, int offset, char *buffer, int bufsz)
{
	if (root == NULL)
		return 0;

	int size = 0, szt;
	SLDC_node_head head;
	SLDC_node_split split;
	switch(root->Type)
	{
		case POLY:
			head.size = 33 + root->poly_num.size()*sizeof(int);
			head.type = 1;
			
			// write head
			memcpy((buffer+offset+size), (char*)&head.type, sizeof(char));
			size += sizeof(char);

			memcpy((buffer+offset+size), (char*)&head.size, sizeof(int));
			size += sizeof(int);

			//write bbox
			memcpy((buffer+offset+size), (char*)&root->bound_min, sizeof(vector3d));
			size += sizeof(vector3d);

			memcpy((buffer+offset+size), (char*)&root->bound_max, sizeof(vector3d));
			size += sizeof(vector3d);
			
			// write polygon index list
			szt = root->poly_num.size();
			memcpy((buffer+offset+size), (char*)&szt, sizeof(int));
			size += sizeof(int);

			for (unsigned int i = 0; i < root->poly_num.size(); i++)
			{
				szt = root->poly_num[i];
				memcpy((buffer+offset+size), (char*)&szt, sizeof(int));
				size += sizeof(int);
			}
			return head.size;

		default: //SPLIT
			size = 37;
			split.header.type = 0;
			split.header.size = size;
			split.bound_min = root->bound_min;
			split.bound_max = root->bound_max;

			split.front_offset = size;
			size += PackTreeInSLDC(root->front.get(), offset+size, buffer, bufsz);

			split.back_offset = size;
			size += PackTreeInSLDC(root->back.get(), offset + size, buffer, bufsz);

			szt = 0;
			// write head
			memcpy((buffer+offset+szt), (char*)&split.header.type, sizeof(char));
			szt += sizeof(char);

			memcpy((buffer+offset+szt), (char*)&split.header.size, sizeof(int));
			szt += sizeof(int);

			//write bbox
			memcpy((buffer+offset+szt), (char*)&split.bound_min, sizeof(vector3d));
			szt += sizeof(vector3d);

			memcpy((buffer+offset+szt), (char*)&split.bound_max, sizeof(vector3d));
			szt += sizeof(vector3d);

			// write offsets
			memcpy((buffer+offset+szt), (char*)&split.front_offset, sizeof(int));
			szt += sizeof(int);

			memcpy((buffer+offset+szt), (char*)&split.back_offset, sizeof(int));
			szt += sizeof(int);
	
			return size;
	}
	return 0;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void DebugPrintTree(bsp_tree_node* root, std::ostream &out)
{
	if (root == NULL)
		return;
	switch(root->Type)
	{
		case POLY:
			out << "BBOX MAX" << root->bound_max << ", MIN" << root->bound_min << std::endl;
			for(unsigned int i = 0; i<root->poly_num.size(); i++)
				out << "POLYGON " << root->poly_num[i] << std::endl;
			out << "END" << std::endl;
			break;

		default: //SPLIT
			out << "SPLIT MAX" << root->bound_max << ", MIN" << root->bound_min << std::endl;
			out << "  +Plane Point" << root->point << ", Normal" << root->normal << std::endl;
			if (root->front)
			{
				out << "  @Front" << std::endl;
				DebugPrintTree(root->front.get(), out);
			}
			if (root->back)
			{
				out << "  @Back" << std::endl;
				DebugPrintTree(root->back.get(), out);
			}
			break;
	}

}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

std::unique_ptr<bsp_tree_node> MakeTree(std::vector<pcs_polygon> &polygons, vector3d &Max, vector3d &Min)
{
	std::vector<int> polylist(polygons.size());
	for (unsigned int i = 0; i < polylist.size(); i++)
	{
		polylist[i] = i;
	}
	MakeBound(Max, Min, polylist, polygons);

	// we want to give padding so as to not make the outermost bounding regions too tight
	Max = Max +vector3d(0.1f, 0.1f, 0.1f);
	Min = Min -vector3d(0.1f, 0.1f, 0.1f);

	if (polygons.empty()) {
		return std::unique_ptr<bsp_tree_node>((bsp_tree_node*)NULL);
	}

	wxLongLong time = wxGetLocalTimeMillis();
	PCS_Model::BSP_CUR_DEPTH = 0;
	std::unique_ptr<bsp_tree_node> node = GenerateTreeRecursion(polygons, polylist);

	PCS_Model::BSP_TREE_TIME += (wxGetLocalTimeMillis() - time).ToLong();
	return node;
}


//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

class polylist_comparator {
public:
	polylist_comparator(const std::vector<pcs_polygon> *polygons_in, int axis_in)
		: polygons(polygons_in), axis(axis_in) {}
	bool operator()(int a, int b) {
		return (*polygons)[a].centeroid[axis] < (*polygons)[b].centeroid[axis];
	}
private:
	const std::vector<pcs_polygon>* polygons;
	int axis;
};

std::unique_ptr<bsp_tree_node> GenerateTreeRecursion(std::vector<pcs_polygon> &polygons, std::vector<int>&contained)
{
	PCS_Model::BSP_CUR_DEPTH++;
	if(PCS_Model::BSP_MAX_DEPTH < PCS_Model::BSP_CUR_DEPTH)
		PCS_Model::BSP_MAX_DEPTH = PCS_Model::BSP_CUR_DEPTH;
	if (PCS_Model::BSP_CUR_DEPTH > 500)
	{
		PCS_Model::BSP_COMPILE_ERROR = true;
		return std::unique_ptr<bsp_tree_node>((bsp_tree_node*)NULL); //WHOA wtf infinite recursion!
	}
	std::unique_ptr<bsp_tree_node> node(new bsp_tree_node);
	MakeBound(node->bound_max, node->bound_min, contained, polygons);
	if (contained.size() == 1)
	{
		//we're a polygon.. w00t
		node->Type = POLY;
		node->poly_num = contained;
	}
	else
	{
		// we're a sortnorm
		vector3d cmax = polygons[contained[0]].centeroid;
		vector3d cmin = polygons[contained[0]].centeroid;
		for (std::vector<int>::iterator it = contained.begin() + 1; it < contained.end(); ++it) {
			ExpandBoundingBoxes(cmax, cmin, polygons[*it].centeroid);
		}
		std::vector<int> front, back;
		if (!Bisect(cmax, cmin, node->point, node->normal, polygons, contained, front, back)) {
			node->Type = POLY;
			node->poly_num = contained;
		} else {
			node->Type = SPLIT;
			node->front = GenerateTreeRecursion(polygons, front);
			node->back = GenerateTreeRecursion(polygons, back);
		}
	}
	PCS_Model::BSP_CUR_DEPTH--;
	return node;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

bool Bisect(const vector3d& cmax, const vector3d& cmin, 
			vector3d &p_point, vector3d &p_norm,
			const std::vector<pcs_polygon>& polygons,
			std::vector<int>& contained,
			std::vector<int>& front,
			std::vector<int>& back,
			vector3d *centera, vector3d *centerb)
{
	float x,y,z;
	vector3d difference;
	if (centera==NULL || centerb==NULL)
	{
		x = std::fabs(cmax.x-cmin.x);
		y = std::fabs(cmax.y-cmin.y);
		z = std::fabs(cmax.z-cmin.z);

		difference = vector3d(x,y,z);
		difference = difference / 2;
		p_point = cmin + (difference);
	}
	else
	{
		x = std::fabs(centera->x-centerb->x);
		y = std::fabs(centera->y-centerb->y);
		z = std::fabs(centera->z-centerb->z);

		difference = *centera + *centerb;
		p_point = difference / 2;
	}

	int axis;

	if (x >= y && x >= z)
	{
		axis = 0;
		p_norm = MakeVector(1.0, 0.0, 0.0); // facing +X
	}
	else if (y >= z)
	{
		axis = 1;
		p_norm = MakeVector(0.0, 1.0, 0.0);
	}
	else
	{
		axis = 2;
		p_norm = MakeVector(0.0, 0.0, 1.0);
	}
	if (difference[axis] < 10 * std::numeric_limits<float>::epsilon() * std::max(std::fabs(cmax[axis]), std::fabs(cmin[axis]))) {
		return false;
	}
	polylist_comparator comparator(&polygons, axis);
	std::sort(contained.begin(), contained.end(), comparator);
	int median = contained.size() / 2;
	const pcs_polygon& before = polygons[contained[median - 1]];
	const pcs_polygon& after = polygons[contained[median]];
	float split = (before.centeroid[axis] + after.centeroid[axis]) / 2;
	p_point[axis] = split;
	front.assign(contained.begin(), contained.begin() + median);
	back.assign(contained.begin() + median, contained.end());
	return true;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-


void TriangulateMesh(std::vector<pcs_polygon> &polygons)
{
	std::vector<pcs_polygon> temp, new_polygons;
	unsigned int oldsize, i, j;
	for (i = 0; i < polygons.size(); i++)
	{
		if (polygons[i].verts.size() > 3)
		{
			temp.resize(polygons[i].verts.size() - 2);

			// split up into triangles
			for (j = 0; j < temp.size()-2; j++)
			{
				temp[j].verts.resize(3);
				temp[j].norm = polygons[i].norm;
				temp[j].texture_id = polygons[i].texture_id;
				temp[j].verts[0] = polygons[i].verts[0];
				temp[j].verts[1] = polygons[i].verts[1+j];
				temp[j].verts[2] = polygons[i].verts[2+j];
			}
			// replace this polygon with the first triangle
			polygons[i] = temp[0];

			// place the rest of the triangles into the new polygon list
			oldsize = new_polygons.size();
			new_polygons.resize(new_polygons.size()+temp.size()-1);
			for (j = 0; j < temp.size()-1; j++)
			{
				new_polygons[oldsize+j] = temp[j+1];
			}
		}
	}

	oldsize = polygons.size();
	polygons.resize(oldsize+new_polygons.size());
	for (i = 0; i < new_polygons.size(); i++)
	{	
		polygons[oldsize+i] = new_polygons[i];
	}
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

/*
Math Notes

Point on Plane - P0 = (X0, Y0, Z0)
Normal = N = <A,B,C>
Euclidian: AX0, BY0, CZ0 + D = 0

  */

float FindIntersection(vector3d &intersect, vector3d p1, vector3d p2, vector3d plane_point, vector3d plane_normal)
{
	float den = plane_normal.x*(p1.x - p2.x) + plane_normal.y*(p1.y - p2.y) + plane_normal.z*(p1.z - p2.z);
	if (den == 0)
		return 0.0; // perpendicular to the normal
	float d = -(plane_point.x*plane_normal.x + plane_point.y*plane_normal.y + plane_point.z*plane_normal.z);
	float num = plane_normal.x*p1.x + plane_normal.y*p1.y + plane_normal.z*p1.z + d;
	
	vector3d temp(p2-p1);
	temp = temp * (num / den);
	intersect = p1 + temp;

	//colinear check
	//vector3d tempa = MakeUnitVector(p2-p1), tempb = MakeUnitVector(intersect-p1);
	return (num/den);
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void AddIfNotInList(std::vector<pcs_vertex> &list, pcs_vertex &point)
{
	int idx;
	for (unsigned int i = 0; i < list.size(); i++)
	{
		if (list[i].point == point.point)
			return;
	}
	idx = list.size();
	list.resize(idx+1);
	list[idx] = point;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-


void SplitPolygon(std::vector<pcs_polygon> &polygons, int polynum, vector3d plane_point, vector3d plane_normal, std::vector<pcs_polygon> &newpolys)
{
	std::vector<pcs_polygon> splitpolys(2); // 0 = front vert, 1 = back vert
	std::vector<int> pairs;
	std::vector<pcs_vertex> points;
	pairs.resize(polygons[polynum].verts.size() * 2);
	pcs_vertex temp;
	unsigned int i, j = 0;
	float uvdelta;

	for (i = 0; i < polygons[polynum].verts.size() * 2; i += 2)
	{
		pairs[i] = j % polygons[polynum].verts.size();
		pairs[i+1] = (j+1) % polygons[polynum].verts.size();
		j++;
	}

	float dtempa, dtempb;
	// compile the new list of points
	for (i = 0; i < pairs.size(); i += 2)
	{
		dtempa = DistanceToPlane(polygons[polynum].verts[pairs[i]].point, plane_point, plane_normal);
		dtempb = DistanceToPlane(polygons[polynum].verts[pairs[i+1]].point, plane_point, plane_normal);
		if ((dtempa <= 0.0001 && dtempa >= -0.0001) || (dtempb <= 0.0001 && dtempb >= -0.0001))
		// one of these points lays on the plane... add them both without modification
		{

			AddIfNotInList(points, polygons[polynum].verts[pairs[i]]);
			AddIfNotInList(points, polygons[polynum].verts[pairs[i+1]]);
		}
		else // neither of them was not on the plane.. are they on the same side?
		{

			if (InFrontofPlane(polygons[polynum].verts[pairs[i]].point, plane_point, plane_normal) == 
				InFrontofPlane(polygons[polynum].verts[pairs[i+1]].point, plane_point, plane_normal))
			// both on same side - add them
			{
				AddIfNotInList(points, polygons[polynum].verts[pairs[i]]);
				AddIfNotInList(points, polygons[polynum].verts[pairs[i+1]]);
			}
			else
			// different sides - cut and add
			{
				uvdelta = FindIntersection(temp.point, polygons[polynum].verts[pairs[i]].point, 
						polygons[polynum].verts[pairs[i+1]].point, plane_point, plane_normal);
				temp.norm = polygons[polynum].norm;
				temp.u = uvdelta * (polygons[polynum].verts[pairs[i]].u - polygons[polynum].verts[pairs[i+1]].u);
				temp.v = uvdelta * (polygons[polynum].verts[pairs[i]].v - polygons[polynum].verts[pairs[i+1]].v);

				AddIfNotInList(points, polygons[polynum].verts[pairs[i]]);
				AddIfNotInList(points, temp);
				AddIfNotInList(points, polygons[polynum].verts[pairs[i+1]]);
			}
		}
	}


	// split the polygons with the list we have

	int in = 0;
	for (i = 0; i < points.size(); i++)
	{
		dtempa = DistanceToPlane(points[i].point, plane_point, plane_normal) ;
		if (dtempa <= 0.0001 && dtempa >= -0.0001)
		// there WILL be two points in the list this is true for
		{
			AddIfNotInList(splitpolys[0].verts, points[i]);
			AddIfNotInList(splitpolys[1].verts, points[i]);
			
			if (in == 0)
				in = 1;
			else
				in = 0;
		}
		else
		{
			AddIfNotInList(splitpolys[in].verts, points[i]);
		}
	}

	// triangle our new polylist
	TriangulateMesh(splitpolys);

	// replace our current poly with polygon zero - add the others
	polygons[polynum] = splitpolys[0];
	in = newpolys.size();
	newpolys.resize(in+splitpolys.size());
	for (i = 1; i < splitpolys.size(); i++)
	{
		newpolys[in+i] = splitpolys[i];
	}
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

float DistanceToPlane(vector3d point, vector3d plane_point, vector3d plane_normal)
{
	float d = -(plane_point.x*plane_normal.x + plane_point.y*plane_normal.y + plane_point.z*plane_normal.z);
	
	return (point.x*plane_normal.x + point.y*plane_normal.y + point.z*plane_normal.z + d)/
			 sqrt(plane_normal.x*plane_normal.x + plane_normal.y*plane_normal.y + plane_normal.z*plane_normal.z);
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-


bool InFrontofPlane(vector3d point, vector3d plane_point, vector3d plane_normal)
{
	return DistanceToPlane(point, plane_point, plane_normal) >= 0;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

bool Intersects(pcs_polygon &polygon, vector3d plane_point, vector3d plane_normal)
{
	if (polygon.verts.size() < 3)
		return false; // um.. this is a line not a polygon

	bool infront = InFrontofPlane(polygon.verts[0].point, plane_point, plane_normal);

	for (unsigned int i = 1; i < polygon.verts.size(); i++)
	{
		if (DistanceToPlane(polygon.verts[i].point, plane_point, plane_normal) != 0 && // if it's on the plane we don't care
			infront != InFrontofPlane(polygon.verts[i].point, plane_point, plane_normal))
			return true;
	}

	return false;
}


//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

// Please only call this on triangulated meshes.. that makes the rest of my coding easier
void SplitIntersecting(std::vector<pcs_polygon> &polygons, vector3d plane_point, vector3d plane_normal)
{
	std::vector<pcs_polygon> newpolys;
	unsigned int i;
	for (i = 0; i < polygons.size(); i++)
	{
		if (Intersects(polygons[i], plane_point, plane_normal))
		{
			SplitPolygon(polygons, i, plane_point, plane_normal, newpolys);
		}
	}
	// add new polygons

	int in = polygons.size();
	polygons.resize(in+newpolys.size());
	for (i = 1; i < newpolys.size(); i++)
	{
		polygons[in+i] = newpolys[i];
	}
}



//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-


vector3d PolygonCenterFallback(pcs_polygon &polygon)
{
	vector3d empty;

	for (unsigned int i = 0; i < polygon.verts.size(); i++)
	{
		empty += polygon.verts[i].point;
	}

	empty = empty / float(polygon.verts.size());
	return empty;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-


vector3d PolygonCenter(pcs_polygon &polygon)
{

	float TotalArea=0, triarea;
	vector3d Centroid = MakeVector(0,0,0), midpoint;

	for (unsigned int i = 0; i < polygon.verts.size()-2; i++)
	{
		midpoint = polygon.verts[i].point + polygon.verts[i+1].point + polygon.verts[i+2].point;
		midpoint = midpoint/3;

		// Area of Triangle defined by P1, P2, P3 = vector3d(crossProduct(p2-p1, p3-p1)).magnitude()/2

		triarea = Magnitude(CrossProduct(polygon.verts[i+1].point-polygon.verts[i].point, 
										 polygon.verts[i+2].point-polygon.verts[i].point)); // this needs to be area * 2
		if (triarea == 0)
		{
			return PolygonCenterFallback(polygon);
			//exit(1); //panic
		}
		midpoint = triarea*midpoint;
		TotalArea += triarea;
		Centroid += midpoint;

	}

	Centroid = float(1.0 / TotalArea) * Centroid;
	return Centroid;

}


//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void BoundPolygon(vector3d &Max, vector3d &Min, int polygon, std::vector<pcs_polygon> &polygons)
{
	 if (polygon < 0 || (unsigned)polygon > polygons.size() || polygons[polygon].verts.size() < 1)
		 return;
	 Min = Max = polygons[polygon].verts[0].point;
	 vector3d minbuf(-0.01f, -0.01f, -0.01f), maxbuf(0.01f, 0.01f, 0.01f);
	 for (unsigned int i = 1; i < polygons[polygon].verts.size(); i++)
	 {
		 ExpandBoundingBoxes(Max, Min, polygons[polygon].verts[i].point);
	 }

	// add some buffer space
	Max = Max + maxbuf;
	Min = Min + minbuf;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void MakeBound(vector3d &Max, vector3d &Min, std::vector<int> &polylist, std::vector<pcs_polygon> &polygons)
{
	if (polylist.size() < 1 || polygons.size() < 1)
		return;

	BoundPolygon(Max, Min, polylist[0], polygons);
	vector3d pmin, pmax, minbuf(-0.01f, -0.01f, -0.01f), maxbuf(0.01f, 0.01f, 0.01f);
	for (unsigned int i = 1; i < polylist.size(); i++)
	{
		BoundPolygon(pmax, pmin, polylist[i], polygons);
		ExpandBoundingBoxes(Max, Min, pmax);
		ExpandBoundingBoxes(Max, Min, pmin);

	}
	// add some buffer space
	Max = Max + maxbuf;
	Min = Min + minbuf;
}


//****************************************************************************
// BSP Functions... beware
//****************************************************************************

void MakeDefPoints(BSP_DefPoints& dpnts, std::vector<bsp_vert> &pntslist)
{
	dpnts.head.id = 1;
	dpnts.n_verts = pntslist.size();
	dpnts.n_norms = 0;
	dpnts.offset = 20 + pntslist.size();
	
	dpnts.norm_counts.resize(pntslist.size());
	dpnts.vertex_data.resize(pntslist.size());

	for (unsigned int i = 0; i < pntslist.size(); i++)
	{
		dpnts.norm_counts[i] = (unsigned char)pntslist[i].norms.size();
		dpnts.vertex_data[i].vertex = pntslist[i].point;

		dpnts.vertex_data[i].norms.resize(pntslist[i].norms.size());
		dpnts.n_norms += pntslist[i].norms.size();
		for (unsigned int j = 0; j < pntslist[i].norms.size(); j++)
		{
			dpnts.vertex_data[i].norms[j] = pntslist[i].norms[j];
		}
	}

	// last thing
	dpnts.head.size = dpnts.MySize();
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void MakeFlatPoly(BSP_FlatPoly &dst, pcs_polygon &src, std::unordered_map<vector3d, int> &norms, std::unordered_map<vector3d, int> &verts, BSP_DefPoints &dpnts)
{
	dst.head.id = 2;
	dst.normal = src.norm;
	dst.nverts = src.verts.size();
	dst.green = dst.blue = dst.pad = 0;
	dst.red = 0xFF;

	dst.verts.resize(dst.nverts);
	std::vector<vector3d> vertices;
	vertices.reserve(dst.nverts);

	for (unsigned int i = 0; i < (unsigned)dst.nverts; i++)
	{
		vertices.push_back(src.verts[i].point);
		dst.verts[i].vertnum = verts[src.verts[i].point];
		dst.verts[i].normnum = norms[src.verts[i].norm];
	}
	dst.center = src.centeroid;

	// recalculate these just for accracy
	dst.center = dst.MyCenter(vertices);
	dst.radius = dst.MyRadius(dst.center, vertices);


	// last thing
	dst.head.size = dst.MySize();
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void MakeTmapPoly(BSP_TmapPoly &dst, pcs_polygon &src, std::unordered_map<vector3d, int> &norms, std::unordered_map<vector3d, int> &verts, BSP_DefPoints &dpnts)
{
	dst.head.id = 3;
	dst.normal = src.norm;
	dst.nverts = src.verts.size();
	dst.tmap_num = src.texture_id;
	std::vector<vector3d> vertices;
	vertices.reserve(dst.nverts);

	dst.verts.resize(dst.nverts);
	for (int i = 0; i < dst.nverts; i++)
	{
		vertices.push_back(src.verts[i].point);
		dst.verts[i].vertnum = verts[src.verts[i].point];
		dst.verts[i].normnum = norms[src.verts[i].norm];
		dst.verts[i].u = src.verts[i].u;
		dst.verts[i].v = src.verts[i].v;
	}

	// recalculate these just for accracy
	dst.center = dst.MyCenter(vertices);
	dst.radius = dst.MyRadius(dst.center, vertices);


	// last thing
	dst.head.size = dst.MySize();
}

//****************************************************************************
// DIRTY BSP FUNCTIONS - DON'T READ OR YOU'LL HURT YOUR EYE! AAAARGGGGGHHHHHH
// PARSING
//****************************************************************************


void BSPTransPMF(unsigned int offset, unsigned char *data, 
		BSP_DefPoints &points, std::vector<pcs_polygon> &polygons,
		unsigned int &upolys)
{
	BSP_BlockHeader blkhdr;
	unsigned char *curpos = data + offset;
	//BSP_BoundBox *bbox;

	blkhdr.Read((char *) curpos);

	switch (blkhdr.id)
	{
		case 0: // End of Tree
			break;

		case 1: // DEFPOINTS
			points.Read((char *) curpos + blkhdr.MySize(), blkhdr); // interpret block
			BSPTransPMF(offset + blkhdr.size, data, points, polygons, upolys); // continue traversal
			break;

		case 2: // Untextured Poly
			TranslateFPoly(offset, data, points, polygons, upolys); // interpret and continue
			break;

		case 3: // Textured Poly
			TranslateTPoly(offset, data, points, polygons, upolys); // interpret and continue
			break;

		case 4: // Sortnorm
			InterpretSortNorm(offset, data, points, polygons, upolys); // interpret and continue
			break;
			
		case 5: //boundbox
			BSPTransPMF(offset + blkhdr.size, data, points, polygons, upolys); // continue traversal
			break;
		default:
			break;
	}

}

// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void TranslateFPoly(unsigned int offset, unsigned char *data, 
				 BSP_DefPoints &points, std::vector<pcs_polygon> &polygons,
				 unsigned int &upolys)
{
	BSP_BlockHeader blkhdr;
	unsigned char *curpos = data + offset;
	blkhdr.Read((char *) curpos);

	vector3d point, norm;
	pcs_polygon temp_poly;

	BSP_FlatPoly fpoly;
	fpoly.Read((char *) curpos + blkhdr.MySize(), blkhdr);

	
	temp_poly.norm = POFTranslate(fpoly.normal);
	temp_poly.texture_id = -1;
	temp_poly.verts.resize(fpoly.nverts);

	
	for (int i = 0; i < fpoly.nverts; i++)
	{
		temp_poly.verts[i].point = POFTranslate(points.vertex_data[fpoly.verts[i].vertnum].vertex);
		temp_poly.verts[i].norm = POFTranslate(points.normals[fpoly.verts[i].normnum]);

		temp_poly.verts[i].u = 0;
		temp_poly.verts[i].v = 0;

	}

	if (upolys >= polygons.size())
	{
		polygons.resize(polygons.size() * 2);
	}
	polygons[upolys] = temp_poly;
	upolys++;

	BSPTransPMF(offset + blkhdr.size, data, points, polygons, upolys); // continue traversal
}

// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void TranslateTPoly(unsigned int offset, unsigned char *data, 
				 BSP_DefPoints &points, std::vector<pcs_polygon> &polygons,
				 unsigned int &upolys)
{
	BSP_BlockHeader blkhdr;
	unsigned char *curpos = data + offset;
	blkhdr.Read((char *) curpos);

	vector3d point, norm;

	BSP_TmapPoly tpoly;
	pcs_polygon temp_poly;

	tpoly.Read((char *) curpos + blkhdr.MySize(), blkhdr);
	// 2^32-1 = 4294967295
	// 2^8-1 = 255

	temp_poly.norm = POFTranslate(tpoly.normal);
	temp_poly.texture_id = tpoly.tmap_num;
	temp_poly.verts.resize(tpoly.nverts);
	//norm = tpoly.normal

	for (int i = 0; i < tpoly.nverts; i++)
	{
		temp_poly.verts[i].point = POFTranslate(points.vertex_data[tpoly.verts[i].vertnum].vertex);
		temp_poly.verts[i].norm = POFTranslate(points.normals[tpoly.verts[i].normnum]);

		temp_poly.verts[i].u = tpoly.verts[i].u;
		temp_poly.verts[i].v = tpoly.verts[i].v;

	}


	if (upolys >= polygons.size())
	{
		polygons.resize(polygons.size() * 2);
	}
	polygons[upolys] = temp_poly;
	upolys++;

	BSPTransPMF(offset + blkhdr.size, data, points, polygons, upolys); // continue traversal
}

// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

void InterpretSortNorm(unsigned int offset, unsigned char *data, 
					   BSP_DefPoints &points, std::vector<pcs_polygon> &polygons,
						unsigned int &upolys)
{
	BSP_BlockHeader blkhdr;
	BSP_SortNorm snorm;
	unsigned char *curpos = data + offset;
	
	blkhdr.Read((char *) curpos);
	
	snorm.Read((char *) curpos + blkhdr.MySize(), blkhdr);

	if (snorm.front_offset)
		BSPTransPMF(offset + snorm.front_offset, data, points, polygons, upolys); // continue traversal

	if (snorm.back_offset)
		BSPTransPMF(offset + snorm.back_offset, data, points, polygons, upolys); // continue traversal

	if (snorm.prelist_offset)
		BSPTransPMF(offset + snorm.prelist_offset, data, points, polygons, upolys); // continue traversal
	
	if (snorm.postlist_offset)
		BSPTransPMF(offset + snorm.postlist_offset, data, points, polygons, upolys); // continue traversal

	if (snorm.online_offset)
		BSPTransPMF(offset + snorm.online_offset, data, points, polygons, upolys); // continue traversal


	
	// No tail recursion on sortnorms
}	


//-----------------------------------------------
// functions for rendering a BSP's sortnorms and BBoxes
//-----------------------------------------------

//#define __BSP_DEBUG_DRAWNORMS_
void RenderBSP(unsigned int offset, unsigned char *data, vector3d obj_center)
{
	BSP_BlockHeader blkhdr;
	unsigned char *curpos = data + offset;
	//BSP_BoundBox *bbox;

	blkhdr.Read((char *) curpos);

	switch (blkhdr.id)
	{
		case 0: // End of Tree
			break;

		case 1: // DEFPOINTS
			// skip, don't need the contents
			RenderBSP(offset + blkhdr.size, data, obj_center); // continue traversal
			break;


		case 2: // Untextured Poly 
			RenderUntextured(offset, data, obj_center);
			break;


		case 3: // Textured Poly
			RenderTextured(offset, data, obj_center);
			break;

		case 4: // Sortnorm
			RenderSortnorm(offset, data, obj_center); // interpret and continue
			break;
			
		case 5: //boundbox
			RenderBBox(offset, data, obj_center); // continue traversal
			break;

		default:
			break;
	}

}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void RenderUntextured(unsigned int offset, unsigned char *data, vector3d obj_center)
{
	BSP_BlockHeader blkhdr;
	unsigned char *curpos = data + offset;
	blkhdr.Read((char *) curpos);

#if defined(__BSP_DEBUG_DRAWNORMS_)
	BSP_FlatPoly fpoly;
	fpoly.Read((char *) curpos + blkhdr.MySize(), blkhdr);

	// just rendering a point a line along the norm for now - just need center and normal
	glColor3f(0.5, 0.5, 1.0);
	vector3d point = POFTranslate(fpoly.center);
	vector3d norm = point+(POFTranslate(fpoly.normal)*5);

	glBegin(GL_POINTS);	
		glVertex3fv((float *)&point);
	glEnd();

	glBegin(GL_LINE_STRIP);	
		glVertex3fv((float *)&point);
		glVertex3fv((float *)&norm);
	glEnd();
	// tail recurse
	fpoly.Destroy();
#endif
	RenderBSP(offset + blkhdr.size, data, obj_center);
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void RenderTextured(unsigned int offset, unsigned char *data, vector3d obj_center)
{
	BSP_BlockHeader blkhdr;
	unsigned char *curpos = data + offset;
	blkhdr.Read((char *) curpos);

#if defined(__BSP_DEBUG_DRAWNORMS_)
	BSP_TmapPoly tpoly;
	tpoly.Read((char *) curpos + blkhdr.MySize(), blkhdr);

	// just rendering a point a line along the norm for now - just need center and normal

	glColor3f(0.5, 0.5, 1.0);
	vector3d point = POFTranslate(tpoly.center);
	vector3d norm = point+(POFTranslate(tpoly.normal)*5);

	glBegin(GL_POINTS);	
		glVertex3fv((float *)&point);
	glEnd();

	glBegin(GL_LINE_STRIP);	
		glVertex3fv((float *)&point);
		glVertex3fv((float *)&norm);
	glEnd();
	// tail recurse
	tpoly.Destroy();
#endif
	RenderBSP(offset + blkhdr.size, data, obj_center);
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-


void RenderSortnorm(unsigned int offset, unsigned char *data, vector3d obj_center)
{
	BSP_BlockHeader blkhdr;
	BSP_SortNorm snorm;
	unsigned char *curpos = data + offset;
	
	blkhdr.Read((char *) curpos);
	
	snorm.Read((char *) curpos + blkhdr.MySize(), blkhdr);

	// draw a cube defined by the bounding region
	glColor3f(1.0, 0.0, 0.0);
	OpenGL_RenderBox(POFTranslate(snorm.min_bounding_box_point), POFTranslate(snorm.max_bounding_box_point));

	// continue traversal
	if (snorm.front_offset)
		RenderBSP(offset + snorm.front_offset, data, obj_center); // continue traversal

	if (snorm.back_offset)
		RenderBSP(offset + snorm.back_offset, data, obj_center); // continue traversal

	if (snorm.prelist_offset)
		RenderBSP(offset + snorm.prelist_offset, data, obj_center); // continue traversal
	
	if (snorm.postlist_offset)
		RenderBSP(offset + snorm.postlist_offset, data, obj_center); // continue traversal

	if (snorm.online_offset)
		RenderBSP(offset + snorm.online_offset, data, obj_center); // continue traversal


	
	// No tail recursion on sortnorms
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void RenderBBox(unsigned int offset, unsigned char *data, vector3d obj_center)
{
	BSP_BlockHeader blkhdr;
	BSP_BoundBox bbox;
	unsigned char *curpos = data + offset;

	blkhdr.Read((char *) curpos);
	bbox.Read((char *) curpos + blkhdr.MySize(), blkhdr);

	if (blkhdr.id != 5)
		return;
	
	glColor3f(0.0, 1.0, 0.0);
	OpenGL_RenderBox(POFTranslate(bbox.min_point), POFTranslate(bbox.max_point));


	// tail recurse
	RenderBSP(offset + blkhdr.size, data, obj_center);

}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-

void OpenGL_RenderBox(vector3d min, vector3d max)
{

	vector3d points[8];
	
	points[0] = vector3d(max.x, max.y, max.z); 
	points[1] = vector3d(max.x, max.y, min.z); 
	points[2] = vector3d(min.x, max.y, min.z); 
	points[3] = vector3d(min.x, max.y, max.z); 
	points[4] = vector3d(max.x, min.y, max.z); 
	points[5] = vector3d(max.x, min.y, min.z); 
	points[6] = vector3d(min.x, min.y, min.z); 
	points[7] = vector3d(min.x, min.y, max.z);

	int polygons[6][4] = {	{ 3, 0, 4, 7 },
							{ 0, 1, 5, 4 },
							{ 0, 1, 2, 3 },
							{ 4, 5, 6, 7 },
							{ 1, 2, 6, 5 },
							{ 2, 3, 7, 6 }};

	for (int i = 0; i < 6; i++)
	{
		glBegin(GL_LINE_STRIP);	
			glVertex3fv((float *)&points[polygons[i][0]]);
			glVertex3fv((float *)&points[polygons[i][1]]);
			glVertex3fv((float *)&points[polygons[i][2]]);
			glVertex3fv((float *)&points[polygons[i][3]]);
			glVertex3fv((float *)&points[polygons[i][0]]);

		glEnd();
	}

}
