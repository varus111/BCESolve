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

// Header file for DoubleAuction example.
// BAB 10-3-2018

#ifndef DOUBLEAUCTION_HPP
#define DOUBLEAUCTION_HPP

#include "bceabstractgame.hpp"
#include "bce.hpp"

class DoubleAuction : public BCEAbstractGame
{
private:
  int numValues;
  double entryCost;
  double reservePrice;
  double highbid;
  double lowbid;
  double alpha;

public:
  BCEDistrArray distribution;

  DoubleAuction() {distribution.push_back(new vToTheAlpha(1.0),1.0);}

  DoubleAuction(int na, int nv, double _alpha,
	     double _highbid,double _reservePrice,double _entryCost)
    : BCEAbstractGame(nv*nv,na,1,3), alpha(_alpha), numValues(nv),
      entryCost(_entryCost), highbid(_highbid),
      lowbid(0.0), reservePrice(_reservePrice)
  {
    distribution.push_back(new vToTheAlpha(alpha),1.0);
    setHasProductStructureData(true);
    setNumPrivateStates(vector<int>(2,nv));
  }

  int getNumValues(){return numValues;} const

  int stateToPrivateValues(int state, vector<int> &values) const
  {
    values.resize(2);

    values[0]=state%numValues;
    state-=values[0];
    values[1]=state/numValues;
  }

  double prior (int state, const vector<int> &types) const
  {
    vector<int> values;
    stateToPrivateValues(state,values);
    double v0 = static_cast<double>(values[0]+1)/numValues;
    double v1 = static_cast<double>(values[1]+1)/numValues;
    double incr = 1.0/numValues;

    // vector<int> values(2,0);
    // stateToPrivateValues(state,values);
    //
    // if (values[0]==types[0] && values[1]==types[1])
    //   {
  	// return FPAGame::prior(state,types);
    //   }
    // else
    //   return 0.0;
    
    // if (numValues == 2)
    //   {
    // 	double p = 0.25;
    // 	if (v0 == 1 && v1 == 0
    // 	    || v0 == 0 && v1 == 1)
    // 	  return p;
    // 	return (1.0-2*p)/2.0;
    //   }

    // return PDF(distribution,v0,v1,incr);
    return distribution.PDF(v0,v1,incr);
  }

  double objective(int state, const vector<int> &actions, int objectiveIndex) const
  {
    // Convert the state into a pair of valuations
      vector<int> values(2,0);
      stateToPrivateValues(state,values);
      //double alpha = 0.5;
      double obj = 0;

      if (objectiveIndex < 2)
      {
          int player = objectiveIndex;
          double mybid = (highbid)*actions[player]/(numActions[player]-1.0);
          double otherbid = (highbid)*actions[1-player]/(numActions[1-player]-1.0);
          double val = (1.0*values[player]/(numValues-1.0));

          if (actions[player]>actions[1-player])
          {
              return val - (alpha)*mybid - (1-alpha)*otherbid;
          }
          else if (actions[player]<actions[1-player])
          {
              return (alpha)*otherbid - (1-alpha)*mybid;
          }
          else if (actions[obj]==actions[1-obj])
          {
              return 0.5*val;
          }
      }

      else if (objectiveIndex==2)
      {
          if (actions[0]==actions[1])
          {
              obj = ((1.0*values[0]/(numValues-1.0)) + (1.0*values[1]/(numValues-1.0))) / 2.0;
          }
          else
          {
              int w = (actions[0]>actions[1]? 0: 1);
              obj = (1.0*values[w]/(numValues-1.0)) - 2*(1-alpha)*(highbid)*actions[1-w]/(numActions[1-w]-1.0);
          }
      }

      return obj;
  } // objective

  // bool dominated(int a, int t, int player) const
  // {
  //   return false;
  // }
};

#endif
