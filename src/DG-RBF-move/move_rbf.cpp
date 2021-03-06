/** @brief Program to carry out rotational motion of a mesh using RBF method
 * @author Aditya Kashi
 * 
 * Updated Feb 22, 2016: "Modernized"
 */

#include <arotation2d.hpp>
#include <arbf.hpp>

using namespace amat;
using namespace std;
using namespace amc;

int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		cout << "Please provide a control file name.\n";
		return -1;
	}
	
	string confile = argv[1];
	string inp, outp, outdg, jacs, dum, anglestr, solver;
	double suprad, tol;
	int nmarks;		// number of boundary markers to read
	int nrbfsteps, maxiter;
	vector<double> centre(2);
	Matrix<int> n_rot;

	ifstream conf(confile);
	conf >> dum; conf >> inp;
	conf >> dum; conf >> outp;
	conf >> dum; conf >> jacs;
	conf >> dum; conf >> anglestr;
	conf >> dum; conf >> centre[0] >> centre[1];
	conf >> dum; conf >> nmarks;

	conf >> dum;
	n_rot.setup(nmarks,1);
	for(int i = 0; i < nmarks; i++)
		conf >> n_rot(i);
	
	conf >> dum; conf >> suprad;
	conf >> dum; conf >> nrbfsteps;
	conf >> dum; conf >> solver;
	conf >> dum; conf >> tol;
	conf >> dum; conf >> maxiter;
	conf.close();

	double angle = stod(anglestr)*PI/180.0;			// convert to radians
	cout << "support radius is " << suprad << endl;
	cout << "Centre is " << centre[0] << " " << centre[1] << endl;

	// read mesh
	UMesh2d m;
	m.readGmsh2(inp,2);

	// create a vector do distinguish boundary points from interior points
	Matrix<int> flags(m.gnpoin(),1);
	flags.zeros();
	for(int i = 0; i < m.gnface(); i++)
		for(int j = 0; j < m.gnnofa(); j++)
			flags(m.gbface(i,j)) = 1;

	//calculate boundary displacement
	MRotation2d rot(&m, angle, centre[0], centre[1], n_rot);
	Matrix<double> bc = rot.rhsvect_rotate();

	// get number of interior points and boundary points
	int n_bpoin=0, n_inpoin=0;
	for(int i = 0; i < m.gnpoin(); i++)
		n_bpoin += flags(i);
	n_inpoin = m.gnpoin() - n_bpoin;

	// Split interior data and boundary data
	Matrix<double> inpoints(n_inpoin, m.gndim());
	Matrix<double> bpoints(n_bpoin, m.gndim());
	Matrix<double> bcb(n_bpoin, m.gndim());
	int k = 0;
	for(int i = 0; i < flags.rows(); i++)
		if(flags(i) == 0)
		{
			for(int j = 0; j < m.gndim(); j++)
				inpoints(k,j) = m.gcoords(i,j);
			k++;
		}
	
	k = 0;
	for(int i = 0; i < flags.rows(); i++)
		if(flags(i) == 1)
		{
			for(int j = 0; j < m.gndim(); j++){
				bpoints(k,j) = m.gcoords(i,j);
				bcb(k,j) = bc.get(i,j);
			}
			k++;
		}

	// carry out DG mapping procedure
	RBFmove d;
	d.setup(&inpoints, &bpoints, &bcb, 2, suprad, nrbfsteps, tol, maxiter,solver);
	d.move();
	inpoints = d.getInteriorPoints();
	bpoints = d.getBoundaryPoints();

	// create a coords matrix with same point numbering as initial matrix and return it
	Matrix<double> newcoords(m.gnpoin(),m.gndim());
	int a = 0, b = 0; k = 0;
	for(int i = 0; i < flags.rows(); i++)
	{
		if(flags(i) == 0)
		{
			for(int dim = 0; dim < m.gndim(); dim++)
				newcoords(k,dim) = inpoints(a,dim);
			k++;
			a++;
		}
		else
		{
			for(int dim = 0; dim < m.gndim(); dim++)
				newcoords(k,dim) = bpoints.get(b,dim);
			k++;
			b++;
		}
	}

	m.setcoords(&newcoords);
	m.writeGmsh2(outp);

	m.compute_jacobians();
	cout << "Checking jacobians\n";
	ofstream ojac(jacs);
	m.detect_negative_jacobians(ojac);
	ojac.close();

	cout << "Done.\n" << endl;
	return 0;
}
