#include "hybridentryreserve.hpp"

int main() {

  int numBids = 50;
  int numVals = 50;
  double weightOnOwnBid = 1;
  double reservePrice = .1;
  double entryFee = 0;

  HybridEntryReserve hera(numVals,
			  numBids,
			  weightOnOwnBid,
			  reservePrice,
			  entryFee);

  BCESolver solver(hera);
  
  solver.populate();
  
  // Minimize Revenue
  IloCplex cplex = solver.getCplex();
  cplex.getObjective().setSense(IloObjective::Minimize);
  cplex.getObjective().setExpr(solver.getObjectiveFunction(2));
  
  solver.solve();

  BCESolution data;
  solver.getSolution(data);
  
  stringstream fName;
  fName << "hybridauction_nv=" << numVals
	<< "_nb=" << numBids
	<< "_w=" << weightOnOwnBid
	<< "_rp=" << reservePrice 
	<< "_ef=" << entryFee << ".bce";
  string fNameStr = fName.str();
  const char * fNameC = fNameStr.c_str();
  BCESolution::save(data,fNameC);

}
