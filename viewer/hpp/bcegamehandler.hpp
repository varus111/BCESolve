// This file is part of the BCESolve library for games of incomplete
// information
// Copyright (C) 2016 Benjamin A. Brooks, Robert J. Minton
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

#ifndef BCEGAMEHANDLER_HPP
#define BCEGAMEHANDLER_HPP

#include <QtWidgets>
#include <QAbstractTableModel>
#include <QTableView>
#include "bcegame.hpp"
#include "bcepayofftablemodel.hpp"
// #include "bceprobabilitytablemodel.hpp"
#include "bcetableview.hpp"
#include "bcepriortablemodel.hpp"
#include "bceconditionaltablemodel.hpp"
#include "bceobjweightstablemodel.hpp"
#include "bcepushbutton.hpp"

//! This class handles the widgets for editing/displaying the game.
/*! All of the widgets in the game tab and their slots are members of
  this class. It also contains the pointer to the BCEGame, and handles
  the interfaces to the game objects.

  The class contains table models for editing the game, which
  interface with the respective data using the BCEGame class's
  interface methods. 

  The class also contains a table model for editing weights
  on several objectives. This data interfaces with 
  BCESolverWorker, the class for solving custom games the user 
  creates in the game tab.

  \ingroup viewer
 */
class BCEGameHandler : public QMainWindow
{
  Q_OBJECT;
  friend class BCEPlotHandler;
  
private:
  //! The game object.
  /*! This is the game that is represented in the game tab. Note that
      this can be a different game than the one that is associated
      with the solution in the solution tab. Alterations to the game
      can be saved using options in the file menu.
  */
  BCEGame game;
  //! The model for interfacing with payoffs
  BCEPayoffTableModel* payoffModel;
  //! Model for interfacing with prior
  BCEPriorTableModel* priorModel;
  //! Model for interfacing with conditional distn of types
  BCEConditionalTableModel * conditionalModel;
  //! Model for setting weights on objectives
  BCEObjWeightsTableModel * weightsModel;

  //! Layout for the game tab.
  QVBoxLayout * layout;

  //! Button that triggers solve routine.
  QPushButton * solveButton;
  //! Button that cancels solve.
  QPushButton * cancelButton;
  //! Button that clears currently displayed game.
  QPushButton *clearButton;

  // Edits

  //! Edits for number of actions
  vector<QLineEdit*> numActionsEdits;
  //! Edit for number of states.
  QLineEdit * numStatesEdit;
  //! Edit for number of types.
  vector<QLineEdit*> numTypesEdits;
  //! Edit for number of objectives.
  QLineEdit *numObjectivesEdit;

  // Combo box
  //! Drop down menu for selecting a state.
  QComboBox * currentStateCombo;
  
  // Tables
  //! Table for displaying stage payoffs
  BCETableView * payoffTableView;
  //! Table for displaying prior
  BCETableView* priorTableView;
  //! Table for displaying conditional distn of types
  BCETableView* conditionalTableView;
  //! Table for displaying weights data
  BCETableView *objWeightsTableView;

public:
  //! Constructor
  /*! Constructs edits and buttons, connects signals/slots, calls
      BCEGameHandler::initializeModels. */
  BCEGameHandler(QWidget *parent);
  //! Destructor.
  /*! Destroys table views, models, etc. */
  ~BCEGameHandler();

  //! Replaces the current game with _game.
  /*! Calls initializeModels to reinitialize references for table
      models. */
  void setGame(const BCEGame & _game);

  //! Returns constant reference to the current game.
  const BCEGame & getGame() const
  { return game; }

  //! Returns the layout
  QVBoxLayout * getLayout() const
  { return layout; }
  
  //! Returns the solveButton
  QPushButton * getSolveButton() const
  { return solveButton; }
  
  //! Returns the cancelButton
  QPushButton * getCancelButton() const
  { return cancelButton; }
  
  //! Changes the current state
  /*! Switches all of the models over to new state and updates table
      views. */
  void setState(int state);
  
private:
  //! Delete old data models and create new ones.
  /*! Called in constructor and whenever game changes. */
  void initializeModels();
  //! Adds/removes models to achieve correct number of states.
  void changeNumberOfStates(int newS);
  //! Sets up the layout for the game handler.
  void setupLayout();
				     
private slots:
  //! Slot for changing the state. Calls setState.
  void currentStateChanged(int newS);
  //! Adds a new state. Calls changeNumberOfStates.
  void stateAdded();
  //! Adds a new action for the indicated player.
  void actionAdded(int player);
  //! Adds a new type for the indicated player.
  void typeAdded(int player);
  //! Removes a type for the indicated player.
  void typeRemoved(int player);
  //! Adds an objective to the game.
  void objectiveAdded();
  //! Removes an objective from the game.
  void objectiveRemoved();
  //! Removes the current state.
  void stateRemoved();
  //! Removes action for the given player. 
  /*! Will remove the action that is currently selected in the payoff
      table, if one is selected. Otherwise, this function removes the
      last action for the player. */
  void actionRemoved(int player);

  //! Advances currentState to the next state.
  void nextState();
  //! Decreases currentState to the previous state.
  void prevState();
  //! Emits the startSolveRoutine signal to BCEWindow.
  void emitSolveSignal();
  //! Clears the game currently displayed in the game tab.
  /*! Replaces the cleared game with a new game. */
  void clearCurrentGame();

signals:

  //! Signals that the user has clicked the solve button.
  void startSolveRoutine(vector<double>& weightData);
  //! Signals that the user has clicked the cancel solve button.
  void cancelSolveRoutine();

};


#endif
