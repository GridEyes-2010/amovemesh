/** @brief Abstract problem class.
 * @auhor Aditya Kashi
 * @date October 19, 2015
 */

#ifndef __AMATRIX2_H
#include <amatrix2.hpp>
#endif

#ifndef __ASPARSEMATRIX_H
#include <asparsematrix.hpp>
#endif

#define __APROBLEM_H 1

namespace amc {

/** @brief Abstract class from which to derive any class that requires solution to a linear system.
 *
 * Pointers to objects of this class can be passed to linear-solver functions, while the pointers actually point to the objects of classes that inherit from this one.
 * Virtual functions of this class (implemented in derived classes) can then be invoked in the linear solver.
 */
class Problem
{
public:
	virtual void lhs_action(const amat::Matrix<double>* x, amat::Matrix<double>* const ans) = 0;
	virtual amat::Matrix<double> get_rhs() = 0;
};

}	// end namespace amc
