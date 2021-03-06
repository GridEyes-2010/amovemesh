/*! \file ageometry.hpp
 * \brief Classes to reconstruct a C^1 (or C^2) piecewise polynomial (cubic) boundary from a piecewise linear boundary given by a linear mesh.
 * \author Aditya Kashi
 * \date September 4, 2015
 */

#ifndef __AGEOMETRY_H

#ifndef __AMESH2DGENERAL_H
#include "amesh2d.hpp"
#endif

#ifndef __ALINALG_H
#include "alinalg.hpp"
#endif

#define __AGEOMETRY_H 1

namespace amc {

using namespace std;
using namespace amat;

/** \brief Constructs a piecewise cubic C^2 spline curve 
 * 
 * The spline curve interpolates the boundary points contained by the boundary faces either 
 * (a) having marker in rfl, or (b) listed in facelist. 
 * An overloaded setup() function is provided to distinguish the two situations. Whether the curve is open or closed needs to be specified in isClosed.
 * NOTE: The boundary markers specified must form a continuous boundary.
 */
class CSpline
{
	UMesh2d* m;
	amat::Matrix<int> rfl;					///< Contains boundary markers to be reconstructed to a spline
	std::vector<int> facelist;				///< Alternative to rfl; stores an ordered list of faces to be reconstructed
	int ndf;							///< number of degrees of freedom per spline - for cubic this is 4
	int dim;
	int nseg;							///< number of spline pieces
	int nspoin;							///< number of control points
	amat::Matrix<int> seq_spoin;				///< stores sequence of global point numbers for use in spline construction
	amat::Matrix<int> seq_bface;				///< for each bface, stores an order number indicating its occurrence order according to contiguity
	amat::Matrix<int> segface;				///< inverse of seq_bface; stores bface number for each segment
	amat::Matrix<double>* scf;
	amat::Matrix<int> toRec;					///< stores for each bface face whether that face is to be reconstricted
	bool isClosed;						///< is the spline curve open or closed?
	bool issequenced;					///< is the list of faces already in sequence?
	bool face_list_available;			///< true if face list is available, false if rfl is available
	double tol;
	int maxiter;

	amat::Matrix<double>* D;					///< D[idim](i) will contain the slope at point 0 of the ith spline piece
	amat::SpMatrix slhs;						///< LHS of the system which is solved for D
	amat::Matrix<double>* srhs;				///< RHS for each dimention

public:
	
	void setup(UMesh2d* mesh, amat::Matrix<int> recmarkers, bool closed, bool sequenced, double _tol, int _maxiter);
	/**< Use if you want to supply boundary markers for faces to reconstruct. */

	void setup(UMesh2d* mesh, std::vector<int> recmarkers, bool closed, bool sequenced, double _tol, int _maxiter);
	/**< Use if you provide a pre-ordered list of faces to reconstruct. */

	~CSpline();
	
	/// This function arranges the faces to be reconstructed in their sequence of contiguity.
	/** It calculates seq_poin and seq_bface, such that seq_bface(iface) contains the iface-th bface in geometrical order, 
	 * and seq_poin(ibpoin) is the first bpointsb point number of seq_bface(iface).
	 */
	void sequence();

	void compute();
	///< This function computes the spline coeffs and stores them in scf. Depends on sequenced bfaces and points.

	double getspline(int iface, int idim, double t);
	///< returns idim-coordinate of iface-th spline segement with parameter t
};



/** \brief Handles spline reconstruction of multiple parts of the boundary into seperate c-splines.
 *
 * It accepts an arbitrary number of boundary parts (BPs) to be reconstructed independently of each other, each consisting of an arbitrary number of boundary markers.
 * It scans each boundary part for corners, and splits them at the corners to get several boundary parts with no corners.
 * NOTE: This process of splitting parts at corners modifies the original mesh file!
 * The class can also store and retrieve spline coefficients for such multi-boundary-part meshes.
 */

class BoundaryReconstruction2d
{
	UMesh2d* m;								///< NOTE: make sure bpointsb has been computed!
	std::vector<std::vector<int>> marks;				///< to hold boundary markers of all parts
	double cangle;							///< minimum corner angle, above which an intersection is considered a corner
	int nparts;
	int nnparts;
	CSpline* sparts;
	std::vector<int> ncorners;
	std::vector<bool> isClosed;					///< contins true if a (parent) part is closed.
	std::vector<bool> isSplitClosed;				///< contains true if a split part is closed.
	std::vector<int> startface;
	amat::Matrix<int> toRec;						///< nparts x nface array that stores 1 if a face belongs to a part.
	std::vector<std::vector<std::vector<int>>> corners;	///< contains a list of point number and two containing bfaces for each corner point in each part.
	std::vector<std::vector<int>> partfaces;			///< stores a list of ordered faces for each part
	amat::Matrix<int> facepart;					///< stores part no. and local face number in that part, for each boundary face

public:
	void setup(UMesh2d* mesh, int num_parts, std::vector<std::vector<int>> boundary_markers, double angle_threshold);

	~BoundaryReconstruction2d();
	
	void preprocess();
	/**< Determines whether each part is open or closed, and stores a starting bface number for each part 
		NOTE: Make sure amesh2d::compute_boundary_points() has been executed.*/
	
	/// Detect corners in each part based on dot-product of normals of adjacent faces becoming too small.
	/**	Detects corner points by computing dot-products of normals of the two faces associated with a point and comparing it with cos(theta) for each point, for a given angle theta.
	 * Computes the corners data structure.
	 * Each element of corners is a list of corner points structures for each corner point, so corner[ipart][0] refers to the first corner point of part number ipart.
	 * Each corner point structure contains the global point number of the corner and the two faces associated with that point. So,
	 * - corner[ipart][0][0] refers the global point number of the first corner of part ipart,
	 * - corner[ipart][0][1] contains the left face associated with the corner, and
	 * - corner[ipart][0][2] contians the right face associated with the corner.
	 */
	void detect_corners();

	void read_corners(std::string cname);
	/**< Can accept corners from a file rather than trying to detect them */

	void split_parts();
	/**< Splits parts based on corner points. */
	
	void compute_splines(double tol, int maxiter);
	/**< Calls the compute() function of class CSpline to compute spline coefficients of all parts. */
	
	/**	Function to return coordinates of the curve.
		NOTE: the argument iface must correspond to a face which was reconstructed!!
	*/
	double getcoords(int iface, int idim, double u);
	
	//void writeCoeffs(std::string fname);
	
	//void readCoeffs(std::string fname);
};


/** Class HermiteSpline2d constructs one spline curve from all the boundary faces of a mesh which have markers contained in rfl.
 *
 * Currently reconstructs a C^1 boundary.
 * But we need to ensure that out of the two possible tangents for each face, the correct one is chosen for consistency. 
 * For this, we calculate normal based on intfac, as face nodes in intfac are ordered to point outwards.
 * NOTE: Make sure the boundaries with markers in rfl are contiguous.
 * CAUTION: There are issues with the functionality of this implmenetation. Spurious loops are created at control points, strangely.
 */

class HermiteSpline2d
{
	typedef double(HermiteSpline2d::*Basispointer)(double);
	UMesh2d* m;												// NOTE: make sure compute_topological() has been done for this mesh before passing!
	amat::Matrix<int> rfl;										// the markers of the boundaries to be reconstructed
	int nseg;												// number of curve segments - equal to number of boundary faces to reconstruct
	int ndeg;												// degree of spline interpolation - usually 3
	int ndf;												// Number of 'degrees of freedom' per face - this 1+ndeg.
	amat::Matrix<double>* cf;										// to store geometric coefficients
	amat::Matrix<double> gallfa;									// Normals etc of boundary faces in bfaces
	amat::Matrix<int> toRec;										// Contains 1 if the corresponding intfac is to be reconstructed, otherwise contains 0
	amat::Matrix<int> isCorner;									// contains 1 of this point is a corner point
	amat::Matrix<double> ptangents;								// Contains average tangent at each boundary point. Uses UMesh2d::bpoints.
	Basispointer* F;										// Array of function pointers for Hermite basis functions
	double angle_threshold;									// Minimum angle (in radians) between two tangents for point to qualify as corner point

	bool store_intfac;										// Whether cf is stored according to intfac (true) or bface (false)
	int nndim;
	int nnbface;

	// definitions of the 4 Hermite basis functions
	double f0(double u) { return 2*pow(u,3) - 3*u*u + 1; }
	double f1(double u) { return -2*pow(u,3) + 3*u*u; }
	double f2(double u) { return pow(u,3) - 2*u*u + u; }
	double f3(double u) { return u*u*u - u*u; }

public:
	void setup(UMesh2d* mesh, amat::Matrix<int> rflags, double angle)
	{
		m = mesh;
		rfl = rflags;
		ndeg = 3;
		ndf = 1+ndeg;

		angle_threshold = angle;

		F = new Basispointer[ndeg+1];

		F[0] = &HermiteSpline2d::f0;
		F[1] = &HermiteSpline2d::f1;
		F[2] = &HermiteSpline2d::f2;
		F[3] = &HermiteSpline2d::f3;
		//std::cout << (this->*F[0])(2.1);

		toRec.setup(m->gnbface(),1);
		toRec.zeros();
		for(int iface = 0; iface < m->gnbface(); iface++)
		{
			for(int im = 0; im < rfl.msize(); im++)
			{
				if(m->gbface(iface,m->gnnofa()) == rfl(im)) toRec(m->gifbmap(iface)) = 1;
			}
		}

		isCorner.setup(m->gnpoin(),1);
		isCorner.zeros();

		ptangents.setup(m->gnpoin(), m->gndim());			// stores components of unit tangent at each boundary point by averaging tangents of faces around that point.
		ptangents.zeros();

		cf = new amat::Matrix<double>[m->gndim()];
		for(int idim = 0; idim < m->gndim(); idim++) {
			cf[idim].setup(m->gnbface(),ndeg+1);
			cf[idim].zeros();
		}

		gallfa.setup(m->gnbface(),4);		// gallfa(iseg,0) contains normalized tangent x-component at point 0 of corresponding intfac segment
		gallfa.zeros();

		store_intfac = true;
	}

	~HermiteSpline2d()
	{
		delete [] F;
		delete [] cf;
	}

	void compute_splines()
	{
		// get point tangents first
		std::cout << "HermiteSpline2D: compute_splines(): Getting tangents of boundary points." << std::endl;
		for(int ipoin = 0; ipoin < m->gnbpoin(); ipoin++)
		{
			// in 2D, bpoints is not an array of stl std::vectors - we know there are only two faces surrounding each boundary point
			// if edge is bounded by (x1,y1) and (x2,y2), tangent to edge is (x2-x1)i + (y2-y1)j

			// first check if this point belongs to a face that needs to be reconstructed
			if(toRec(m->gbpoints(ipoin,1))==1  || toRec(m->gbpoints(ipoin,2))==1)
			{

				std::vector<int> en(2);

				// get intfac faces containing this point
				en[0] = m->gbpoints(ipoin,1);
				en[1] = m->gbpoints(ipoin,2);
				//std::cout << en[0] << " " << en[1] << std::endl;

				double xt, yt, dotx = 1, doty = 1, mag;

				// iterate over the two faces containing ipoin
				for(int i = 0; i < 2; i++)
				{
					xt = m->gcoords(m->gintfac(en[i],3),0) - m->gcoords(m->gintfac(en[i],2),0);
					yt = m->gcoords(m->gintfac(en[i],3),1) - m->gcoords(m->gintfac(en[i],2),1);
					mag = sqrt(xt*xt + yt*yt);
					ptangents(m->gbpoints(ipoin,0),0) += xt;
					ptangents(m->gbpoints(ipoin,0),1) += yt;
					dotx *= xt/mag;
					doty *= yt/mag;
				}
				ptangents(m->gbpoints(ipoin,0),0) /= 2.0;
				ptangents(m->gbpoints(ipoin,0),1) /= 2.0;

				// now normalize the averaged tangent std::vector
				mag = sqrt(ptangents(m->gbpoints(ipoin,0),0)*ptangents(m->gbpoints(ipoin,0),0) + ptangents(m->gbpoints(ipoin,0),1)*ptangents(m->gbpoints(ipoin,0),1));
				ptangents(m->gbpoints(ipoin,0),0) /= mag;
				ptangents(m->gbpoints(ipoin,0),1) /= mag;

				// check if this is a corner point
				double dotp = dotx+doty;			// dot product of tangents of the two faces
				if(dotp < cos(angle_threshold)) { 
					isCorner(m->gbpoints(ipoin,0)) = 1;				// set isCorner for global point number corresponding to boundary point ipoin
					std::cout << "Boundary point " << m->gbpoints(ipoin,0) << " is a corner point!" << std::endl;
				}
			}
		}

		// iterate over boundary faces of the mesh
		std::cout << "HermiteSpline2D: compute_splines(): Iterating over boundary faces to get tangents." << std::endl;
		std::vector<int> pnts(m->gnnofa());		// to store point numbers of points making up iface
		for(int iface = 0; iface < m->gnbface(); iface++)
		{
			if(toRec(iface) == 1)
			{
				// get tangent for this face
				for(int inofa = 0; inofa < m->gnnofa(); inofa++)
					pnts[inofa] = m->gintfac(iface,inofa+2);

				// magnitude of tangent (x2-x1)i + (y2-y1)j
				double mag = sqrt((m->gcoords(pnts[1],0)-m->gcoords(pnts[0],0))*(m->gcoords(pnts[1],0)-m->gcoords(pnts[0],0)) + (m->gcoords(pnts[1],1)-m->gcoords(pnts[0],1))*(m->gcoords(pnts[1],1)-m->gcoords(pnts[0],1)));
				
				for(int inofa = 0; inofa < m->gnnofa(); inofa++)
				{
					if(isCorner(pnts[inofa]) == 0)		// if not a corner
					{
						gallfa(iface,2*inofa) = ptangents(pnts[inofa],0);
						gallfa(iface,2*inofa+1) = ptangents(pnts[inofa],1);
					}
					else								// if pnts[inofa] is a corner, use this face's tangent at that point
					{
						gallfa(iface,2*inofa) = (m->gcoords(pnts[1],0) - m->gcoords(pnts[0],0))/mag;
						gallfa(iface,2*inofa+1) = (m->gcoords(pnts[1],1) - m->gcoords(pnts[0],1))/mag;
					}
				}

				/* Note that gallfa(*,0) contains x-component of tangent at point 1, gallfa(*,1) contains y-component of tangent at point 1,
				    gallfa(*,2) contains x-component of tangent at point 2, and gallfa(*,3) contains y-component of tangent at points 2. */
			}
		}

		// iterate over boundary faces of mesh again to calculate geometric spline coeffs from gallfa and intfac
		std::cout << "HermiteSpline2d: compute_splines(): Iterating over boundary faces again to compute geometric coefficients of the splines" << std::endl;
		for(int iface = 0; iface < m->gnbface(); iface++)
			if(toRec(iface) == 1)
			{
				for(int idim = 0; idim < m->gndim(); idim++)
				{
					cf[idim](iface,0) = m->gcoords(m->gintfac(iface,2),idim);
					cf[idim](iface,1) = m->gcoords(m->gintfac(iface,3),idim);
					cf[idim](iface,2) = gallfa(iface,idim);
					cf[idim](iface,3) = gallfa(iface,idim+2);
				}
			}
	}

	double spline(int iface, int idim, double u)
	// returns idim-coordinate of the spline-reconstruction of iface (of intfac or bface) corresponding to parameter u.
	{
		double val = 0;
		for(int i = 0; i < ndf; i++)
			val += (this->*F[i])(u)*cf[idim](iface,i);
		return val;
	}

	void writeGeomCoeffsToFile(std::string fname)
	{
		std::ofstream ofs(fname);
		if(store_intfac == true)
		{
			ofs << "Dims " << m->gndim() <<  " DFs " << ndf << " nbface " << m->gnbface() <<  '\n';
			for(int idim = 0; idim < m->gndim(); idim++)
			{
				ofs << idim << '\n';
				for(int inface = 0; inface < m->gnbface(); inface++)
				{
					ofs << m->gbifmap(inface);
					for(int idf = 0; idf < ndf; idf++)
						ofs << " " << cf[idim](inface,idf);
					ofs << '\n';
				}
			}
		}
		else
		{
			ofs << "Dims " << nndim  << " DFs " << ndf << " nbface " << nnbface  <<  '\n';
			for(int idim = 0; idim < nndim; idim++)
			{
				ofs << idim << '\n';
				for(int inface = 0; inface < nnbface; inface++)
				{
					ofs << inface;
					for(int idf = 0; idf < ndf; idf++)
						ofs << " " << cf[idim](inface,idf);
					ofs << '\n';
				}
			}
		}
		ofs.close();
	}

	void readGeomCoeffsFromFile(std::string fname)
	// Reads a file written by writeGeomCoeffsToFile() and stores the data in cf.
	// NOTE: Faces are arranged in cf according to BFACE numbers now.
	{
		std::ifstream fin(fname);
		std::string dum; int ddim; int nbface;
		fin >> dum; fin >> nndim; fin >> dum; fin >> ndf; fin >> dum; fin >> nnbface;
		F = new Basispointer[ndf];

		F[0] = &HermiteSpline2d::f0;
		F[1] = &HermiteSpline2d::f1;
		F[2] = &HermiteSpline2d::f2;
		F[3] = &HermiteSpline2d::f3;

		cf = new amat::Matrix<double>[nndim];
		for(int idim = 0; idim < nndim; idim++) {
			cf[idim].setup(nnbface,ndf);
			cf[idim].zeros();
		}
		
		int iface;		// to hold bface number of each face

		for(int idim = 0; idim < nndim; idim++)
		{
			fin >> dum;
			for(int inface = 0; inface < nnbface; inface++)
			{
				fin >> iface;
				for(int idf = 0; idf < ndf; idf++)
					fin >> cf[idim](iface,idf);
			}
		}

		fin.close();
		store_intfac = false;
	}
};

} // end namespace amc

#endif
