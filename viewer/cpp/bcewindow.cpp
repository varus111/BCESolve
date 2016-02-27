#include "bcewindow.hpp"
#include "bcegame.hpp"
#include <QtWidgets>
#include "bcelogstream.hpp"

BCEWindow::BCEWindow(BCELogHandler &logHandler) {

  // Point the stored logTab pointer at the supplied logHandler
  logTab = &logHandler;

  // Set the default path for loading examples.
  path=QString("../examples/");

  // Initialization of Pointers
  solutionTab = new BCEPlotHandler(this);
  gameTab = new BCEGameHandler(this);
  callback = new BCEGurobiCallback();

  // Menu Bar
  QMenu * fileMenu = menuBar()->addMenu(tr("&File"));
  QMenu * viewMenu = menuBar()->addMenu(tr("&View"));
  QAction * loadSolutionAction = new QAction(tr("&Load Solution"),this);
  QAction * loadGameAction = new QAction(tr("&Load Game"),this);
  QAction * saveSolutionAction = new QAction(tr("&Save Solution"),this);
  QAction * saveGameAction = new QAction(tr("&Save Game"),this);
  QAction * linearScale = new QAction(tr("&Linear/Log Color Scale Toggle"),this);
  QAction * colorfulDistn = new QAction(tr("&Colorful/Blue Theme Toggle"),this);
  QAction * screenShotAction = new QAction(tr("&Save a screen shot"),this);
  QAction * quitAction = new QAction(tr("&Quit GUI"),this);
  fileMenu->addAction(loadSolutionAction);
  fileMenu->addAction(loadGameAction);
  fileMenu->addAction(saveSolutionAction);
  fileMenu->addAction(saveGameAction);
  viewMenu->addAction(linearScale);
  fileMenu->addAction(quitAction);
  linearScale->setCheckable(true);
  linearScale->setChecked(true);
  viewMenu->addAction(colorfulDistn);
  colorfulDistn->setCheckable(true);
  colorfulDistn->setChecked(true);
  viewMenu->addAction(screenShotAction);
  loadSolutionAction->setShortcut(tr("Ctrl+L"));
  screenShotAction->setShortcut(tr("Ctrl+P"));
  quitAction->setShortcut(tr("Ctrl+W"));
  saveSolutionAction->setShortcut(tr("Ctrl+S"));

  // Menu Connections
  connect(loadSolutionAction,SIGNAL(triggered()),this,SLOT(loadSolution()));
  connect(loadGameAction,SIGNAL(triggered()),this,SLOT(loadGame()));
  connect(saveSolutionAction,SIGNAL(triggered()),this,SLOT(saveSolution()));
  connect(saveGameAction,SIGNAL(triggered()),this,SLOT(saveGame()));
  connect(quitAction,SIGNAL(triggered()),this,SLOT(close()));
  connect(linearScale,SIGNAL(toggled(bool)),solutionTab,SLOT(toggleLinearScale(bool)));
  connect(colorfulDistn,SIGNAL(toggled(bool)),solutionTab,SLOT(toggleColorfulTheme(bool)));
  connect(screenShotAction,SIGNAL(triggered()),solutionTab,SLOT(screenShot()));

  // Loading Connection
  connect(this,SIGNAL(dataPathChanged(QString)),
  	  solutionTab,SLOT(loadData(QString)));

  // Solve Routine Connections
  connect(gameTab,SIGNAL(startSolveRoutine(vector<double>&)),
	  this,SLOT(runSolve(vector<double>&)));
  connect(gameTab,SIGNAL(cancelSolveRoutine()),
	  this,SLOT(cancelSolve()));

  // Layout Setup
  tabWidget = new QTabWidget(this);
  QWidget *solutionTabWidget = new QWidget(this);
  solutionTabWidget->setLayout(solutionTab->getLayout());
  QWidget *gameTabWidget = new QWidget(this);
  gameTabWidget->setLayout(gameTab->getLayout());
  QWidget *logTabWidget = new QWidget(this);
  logTabWidget->setLayout(logTab->getLayout());

  tabWidget->addTab(solutionTabWidget,"Solution");
  tabWidget->addTab(gameTabWidget,"Game");
  tabWidget->addTab(logTabWidget,"Log");

  QHBoxLayout *mainLayout = new QHBoxLayout();
  mainLayout->addWidget(tabWidget);

  QWidget *mainPanel = new QWidget(this);
  mainPanel->setLayout(mainLayout);

  // Set Default Tab 
  setCentralWidget(mainPanel);

  // Maximize Window by Default
  setWindowState(Qt::WindowMaximized);

  // Window Title
  setWindowTitle(QApplication::translate("bceviewer","BCE Solution Viewer"));

} // Default Constructor

void BCEWindow::loadSolution() {

  QString newPath = QFileDialog::getOpenFileName(this,tr("Select a Solution File"),
						 path,
						 tr("BCESolve Solution Files (*.bce)"));


  if (newPath.isEmpty()) {
    cout << "isEmpty" << endl;
    return;
  }

  try
    {
    QByteArray ba = newPath.toLocal8Bit();
    const char* newPath_c = ba.data();

    BCESolution loadedSolution;
    BCESolution::load(loadedSolution,newPath_c);
    BCEGame loadedGame = loadedSolution.getGame();

    solutionTab->setSolution(loadedSolution);
    gameTab->setGame(loadedGame);

    emit(dataPathChanged(newPath));

    }
  catch (std::exception & e)
    {
      qDebug() << "Load solution didnt work :( from BCEPlotHandler" << endl;
    }

}

void BCEWindow::loadGame() {

  QString newPath = QFileDialog::getOpenFileName(this,tr("Select a Game File"),
						 path,
						 tr("BCESolve Game Files (*.bgm)"));


  if (newPath.isEmpty()) {
    cout << "isEmpty" << endl;
    return;
  }

  try
    {
    QByteArray ba = newPath.toLocal8Bit();
    const char* newPath_c = ba.data();

    BCEGame loadedGame;
    BCEGame::load(loadedGame,newPath_c);

    gameTab->setGame(loadedGame);

    emit(dataPathChanged(newPath));

    /* Note that the solution remains the same and isn't
       necessarily associated with the game. */

    }
  catch (std::exception & e)
    {
      qDebug() << "Load solution didnt work :( from BCEPlotHandler" << endl;
    }

}

void BCEWindow::saveSolution() {

  QFileDialog *saveSolnDialog = new QFileDialog();
  QString newPath = saveSolnDialog->getSaveFileName(this,tr("Save a solution file"),
						    "../examples/",
						    tr("BCEViewer solution files (*.bce)"));

  if (newPath.isEmpty())
    return;

  newPath = newPath + ".bce";

  QFileInfo fi(newPath);
  path = fi.canonicalPath();

  try
    {
      QByteArray ba = newPath.toLocal8Bit();
      const char * newPath_c = ba.data();

      BCESolution::save(solutionTab->getSolutionData(),
		       newPath_c);
    }
  catch (std::exception & e)
    {
      qDebug() << "Save solution didnt work :(" << endl;
    }
} // saveSolution

void BCEWindow::saveGame() {

  QFileDialog *saveGameDialog = new QFileDialog();
  QString newPath = saveGameDialog->getSaveFileName(this,tr("Save a game file"),
						    "../examples/",
						    tr("BCEViewer game files (*.bgm)"));
  if (newPath.isEmpty())
    return;

  newPath = newPath + ".bgm";

  QFileInfo fi(newPath);
  path = fi.canonicalPath();

  try
    {
      QByteArray ba = newPath.toLocal8Bit();
      const char * newPath_c = ba.data();

      BCEGame::save(gameTab->getGame(),
		   newPath_c);
    }
  catch (std::exception & e)
    {
      qDebug() << "Save game didnt work :(" << endl;
    }
} // saveGame

void BCEWindow::runSolve(vector<double> & weightData) {
  try
    {
      delete callback;
      callback = new BCEGurobiCallback();

      // Switch to the Log Tab (the third tab, so indexed at 2).
      tabWidget->setCurrentIndex(2);

      logTab->logText->append(QString(""));
      logTab->logText->append(QString("..........STARTING A NEW COMPUTATION.........."));
      logTab->logText->append(QString(""));
      
      // Reimplement when adding cancelGame()
      // cancelSolveFlag = false;
      
      QThread *solverWorkerThread = new QThread(this);
      solverWorker = new BCESolverWorker(gameTab->getGame(),
					 weightData,*callback);
      solverWorker->moveToThread(solverWorkerThread);
      connect(solverWorkerThread,SIGNAL(started()),
	      solverWorker,SLOT(startSolve()));
      connect(solverWorker,SIGNAL(workFinished()),
	      solverWorkerThread,SLOT(quit()));
      connect(solverWorker,SIGNAL(sendSolution(BCESolution*)),
	      this,SLOT(tabToSolution(BCESolution*)));
      connect(solverWorkerThread,SIGNAL(finished()),
      	      solverWorkerThread,SLOT(deleteLater()));
      connect(solverWorkerThread,SIGNAL(finished()),
      	      solverWorker,SLOT(deleteLater()));
      solverWorkerThread->start();
    }
  catch (exception & e)
    {
      QMessageBox::critical(this,tr("Solver failed"),
			    tr("CPLEX was not able to solve your game."),
			    QMessageBox::Ok);
    }
}

void BCEWindow::cancelSolve() {
  callback->setCancelFlag();
  cout << "cancel hit bcewindow" << endl;
}

void BCEWindow::tabToSolution(BCESolution *soln) {
  solutionTab->setSolution(*soln);
  tabWidget->setCurrentIndex(0);
}
