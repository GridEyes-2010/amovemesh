#include <abowyerwatson.hpp>
#include <adistbinsort.hpp>
#include <amesh2dh.hpp>

using namespace std;
using namespace amat;
using namespace amc;

int main(int argc, char* argv[])
{
	string confile(argv[1]);
	string inp;
	string outp;
	string dum;
	int numbins[2];
	ifstream conf(confile);
	conf >> dum; conf >> inp;
	conf >> dum; conf >> outp;
	conf >> dum; conf >> numbins[0];
	conf >> dum; conf >> numbins[1];
	conf.close();

	Matrix<double>* points, *sortedpoints;
	UMesh2dh m;
	m.readGmsh2(inp);
	points = m.getcoords();

	DistBinSort<2>* sorter;

	/*ofstream outfile("coarse_points.dat");
	points.fprint(outfile);
	outfile.close();*/

	/*ifstream infile(inp);
	Matrix<double> points;
	points.fread(infile);
	infile.close();*/

	if(numbins[0] == 0 || numbins[1] == 0)
	{
		sortedpoints = points;
		cout << "No bin sort" << endl;
	}
	else
	{
		sortedpoints = new Matrix<amc_real>(m.gnpoin(),2);
		cout << "Bin sort with x- and y-bins " << numbins[0] << " " << numbins[1] << endl;
		sorter = new DistBinSort<2>(points, numbins, sortedpoints);
	}

	std::ofstream fout("../../output/DG/check.dat");
	fout << std::setprecision(MESHDATA_DOUBLE_PRECISION);
	int idim;
	for(int ipoin = 0; ipoin < m.gnpoin(); ipoin++)
	{
		for(idim = 0; idim < 2; idim++)	
			fout << points->get(ipoin,idim) - sortedpoints->get(sorter->gbmap(ipoin),idim) << " ";
		fout << ", ";
		for(idim = 0; idim < 2; idim++)
			fout << points->get(ipoin,idim) << ' ';
		fout << ", ";
		for(idim = 0; idim < 2; idim++)
			fout << sortedpoints->get(sorter->gbmap(ipoin),idim) << ' ';
		fout << '\n';
	}
	fout.close();
	// the output of the above check is fine. So sortedpoints has all points in the mesh.

	Delaunay2D prob(sortedpoints);
	prob.bowyer_watson();
	prob.writeGmsh2(outp);
	prob.compute_jacobians();
	prob.detect_negative_jacobians();

	if(numbins[0] != 0 && numbins[1] != 0)
	{
		delete sortedpoints;
		delete sorter;
	}

	cout << endl;
	return 0;
}
