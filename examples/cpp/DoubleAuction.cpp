// This file is part of the BCESolve library for games of incomplete
// information
// Copyright (C) 2016 Benjamin A. Brooks and Robert J. Minton
//
// BCESolve free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// BCESolve is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see
// <http://www.gnu.org/licenses/>.
//
// Benjamin A. Brooks
// ben@benjaminbrooks.net
// Chicago, IL

// Partnership Dissolution
// BAB 11-3-2012

#include "DoubleAuction.hpp"

int main(int argc, char ** argv)
{
  double entryCost=0.0;
  double reservePrice=0;
  //double minAngleIncrement = 0.025;
  int nbids = 40;
  int nv = 40;
  double highBid = 1.0;
  double alpha = 0.5; //ask about change objective

  DoubleAuction cva(nbids,nv,alpha,highBid,
			 reservePrice,entryCost);

  BCESolver solver(cva);

  solver.populate();

  vector<double> objWeights(3);
  objWeights[2]=-1;

  solver.solve(objWeights);

  BCESolution soln;
  solver.getSolution(soln);

  stringstream fName;
  fName << "PartnershipDissolution_nv=" << nv
	<< "_nb=" << nbids
	<< "_ec=" << entryCost
	<< "_rp=" << reservePrice
	<< "_hb=" << highBid
	<< "_alpha=" << alpha << ".bce";
  string fNameStr = fName.str();
  const char * fNameC = fNameStr.c_str();

  BCESolution::save(soln,fNameC);

  return 0;
}
