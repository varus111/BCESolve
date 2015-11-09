#include "bceplothandler.hpp"
#include "qcustomplot.h"
#include <QtWidgets>
#include <QMenuBar>
#include <QMainWindow>
#include "bceslider.hpp"
#include "bceenumeration.hpp"
#include "bcelabelhandler.hpp"
#include "bcegamehandler.hpp"

BCEPlotHandler::BCEPlotHandler() 
{
  path=QString("../examples/");
  screenShotPath=QString("../examples/screenshots/");

  // Resolution Settings

  int resWidth = guiData.resWidth;
  int resHeight = guiData.resHeight;

  // Menu Bar
  QMenu * fileMenu = menuBar()->addMenu(tr("&File"));
  QMenu * viewMenu = menuBar()->addMenu(tr("&View"));
  QAction * loadSolutionAction = new QAction(tr("&Load solution"),this);
  QAction * linearScale = new QAction(tr("&Linear/Log Color Scale Toggle"),this);
  QAction * colorfulDistn = new QAction(tr("&Colorful/Blue Theme Toggle"),this);
  QAction * screenShotAction = new QAction(tr("&Save a screen shot"),this);
  QAction * quitAction = new QAction(tr("&Quit GUI"),this);
  fileMenu->addAction(loadSolutionAction);
  fileMenu->addAction(quitAction);
  viewMenu->addAction(linearScale);
  linearScale->setCheckable(true);
  linearScale->setChecked(true);
  viewMenu->addAction(colorfulDistn);
  colorfulDistn->setCheckable(true);
  colorfulDistn->setChecked(true);
  viewMenu->addAction(screenShotAction);
  loadSolutionAction->setShortcut(tr("Ctrl+L"));
  screenShotAction->setShortcut(tr("Ctrl+P"));
  quitAction->setShortcut(tr("Ctrl+W"));

  // Menu Connections
  connect(loadSolutionAction,SIGNAL(triggered()),this,SLOT(loadSolution()));
  connect(quitAction,SIGNAL(triggered()),this,SLOT(close()));
  connect(linearScale,SIGNAL(toggled(bool)),this,SLOT(toggleLinearScale(bool)));
  connect(colorfulDistn,SIGNAL(toggled(bool)),this,SLOT(toggleColorfulTheme(bool)));
  connect(screenShotAction,SIGNAL(triggered()),this,SLOT(screenShot()));

  // BCEDataState Connections to BCEWindow

  connect(&guiData,SIGNAL(selectedEqmChanged()),
	  this,SLOT(plotBCEValueSet()));
  connect(&guiData,SIGNAL(objectiveValuesChanged(int)),
	  this,SLOT(plotDeviationObjectives(int)));
  connect(&guiData,SIGNAL(equilibriumMatrixChanged()),
	  this,SLOT(plotEqm()));
  connect(this,SIGNAL(dataPathChanged(QString)),
  	  &guiData,SLOT(setData(QString)));
  connect(&guiData,SIGNAL(newDataLoaded()),
	  this,SLOT(setGUITitle()));

  // End Data Connections
  /////////////////////////////////////////
  // Plot Initializations and Organization
  
  // Payoff Plot Initialization
  BCELabel *setOfBCEPlotTitle = new BCELabel(ValueSetPlot);
  setOfBCEPlot = new BCEValueSetPlot();
  setOfBCEPlot->xAxis->setLabel("Player 0");
  setOfBCEPlot->yAxis->setLabel("Player 1");
  setOfBCEPlot->setMinimumSize(resWidth/4,resHeight/3.5);
  connect(setOfBCEPlot,SIGNAL(newEqmCoordinates(double,double)),
	  &guiData,SLOT(modifyEqmFocus(double,double)));
  connect(&guiData,SIGNAL(eqmCoordSignal(double,double)),
	  setOfBCEPlotTitle,SLOT(changeDisplayedCoords(double,double)));

  // Bar Plot Initialization 
  deviationBarGraphs.push_back(new QCustomPlot());
  deviationBarGraphs[0]->xAxis->setLabel("Player 0's Actions");
  deviationBarGraphs.push_back(new QCustomPlot());
  deviationBarGraphs[1]->xAxis->setLabel("Player 1's Actions");
  for (int player = 0; player < 2; player++) {
    deviationBarGraphs[player]->yAxis->setLabel("Expected Payoff");
    deviationBarGraphs[player]->setMinimumHeight(resHeight/3.5);

    devPlotTitles.push_back(new BCELabel(DeviationPlot,player));
    connect(&guiData,
	    SIGNAL(devPlotTitleChange(int,int,int,double)),
	    devPlotTitles[player],
	    SLOT(changeText(int,int,int,double)));
    connect(&guiData,
	    SIGNAL(devPlotPrChange(int,double)),
	    devPlotTitles[player],
	    SLOT(changeProbability(int,double)));
  }

  QGridLayout *controlsGrid = guiData.controlsLayout;

  // BCE Set Plot and Sliders Horizontal Layout
  QVBoxLayout *setOfBCEPlotWithTitle = new QVBoxLayout();
  setOfBCEPlotWithTitle->addWidget(setOfBCEPlotTitle);
  setOfBCEPlotWithTitle->addWidget(setOfBCEPlot);
  QHBoxLayout *topLeftPanel = new QHBoxLayout();
  topLeftPanel->addLayout(setOfBCEPlotWithTitle);
  topLeftPanel->addLayout(controlsGrid);

  // Left Viewer Panel, Bar Plots and Slider Box
  QVBoxLayout *leftSectorDivide = new QVBoxLayout();
  leftSectorDivide->addLayout(topLeftPanel);
  for (int player = 0; player < 2; player++) {
    leftSectorDivide->addWidget(devPlotTitles[player]);
    leftSectorDivide->addWidget(deviationBarGraphs[player]);
  }

  // Right Viewer Panel, Conditional-Marginal Distribution
  conditionalMarginalPlot = new BCEValueSetPlot();
  BCELabel *colorMapTitle = new BCELabel(HeatMap);
  colorMapTitle->setMaximumHeight(resHeight/54);
  connect(&guiData,SIGNAL(newStateSignal(int,int,int,bool)),
	  colorMapTitle,SLOT(changeDisplayedState(int,int,int,bool)));

  // Plot Layout and Interaction Settings
  colorMap = new QCPColorMap(conditionalMarginalPlot->xAxis,
			     conditionalMarginalPlot->yAxis);
  conditionalMarginalPlot->addPlottable(colorMap);
  conditionalMarginalPlot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom);
  conditionalMarginalPlot->axisRect()->setupFullAxesBox(true);
  conditionalMarginalPlot->setMinimumWidth(resWidth/2); // 1080p resolution
  conditionalMarginalPlot->xAxis->setLabel("Player 0");
  conditionalMarginalPlot->yAxis->setLabel("Player 1");

  // Color Plottable Set-Up and Interaction Settings
  colorScale = new QCPColorScale(conditionalMarginalPlot);

  conditionalMarginalPlot->plotLayout()->addElement(0, 1, colorScale); 
  colorMap->setColorScale(colorScale); 
  colorScale->setType(QCPAxis::atRight); 
  colorScale->axis()->setLabel("Action Probability");
  colorScale->setRangeDrag(true);

  // Color Scale Color Choice
  QCPColorGradient *mGradient = new QCPColorGradient();
  // mGradient->gpSpectrum;
  colorMap->setGradient(mGradient->gpSpectrum);
  colorMap->setInterpolate(false);

  // Line up Color Scale and Color Map
  QCPMarginGroup *marginGroup = new QCPMarginGroup(conditionalMarginalPlot);
  conditionalMarginalPlot->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
  colorScale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);

  // Layout of Left and Right Viewer Panels
  QVBoxLayout *colorMapWithTitle = new QVBoxLayout();
  colorMapWithTitle->addWidget(colorMapTitle);
  colorMapWithTitle->addWidget(conditionalMarginalPlot);
  QHBoxLayout *mainTab = new QHBoxLayout();
  mainTab->addLayout(leftSectorDivide);
  mainTab->addLayout(colorMapWithTitle);

  // End Plot Initializations and Organization
  /////////////////////////////////////////////// 

  // Graph Tab Setup and Main Panel Initialization
  QWidget * mainPanel = new QWidget();
  QHBoxLayout * mainLayout = new QHBoxLayout();
  QWidget * graphTab = new QWidget();

  graphTab->setLayout(mainTab);

  // Main Widget
  QTabWidget *tabWidget = new QTabWidget();
  tabWidget->addTab(graphTab,"Solution");
  QWidget *gameWidget = new QWidget();
  gameWidget->setLayout(gameTab.getLayout());
  tabWidget->addTab(gameWidget,"Game");
  mainLayout->addWidget(tabWidget);

  mainPanel->setLayout(mainLayout);

  // Set Default Tab 
  setCentralWidget(mainPanel);

  // Maximize Window by Default
  setWindowState(Qt::WindowMaximized);

  // Window Title
  setWindowTitle(QApplication::translate("bceviewer","BCE Solution Viewer"));


} // constructor

/////////////////////////////////////////////
// Load Solution Slot

void BCEPlotHandler::loadSolution() {

  QString newPath = QFileDialog::getOpenFileName(this,tr("Select a solution file"),
						 path,
						 tr("BCESolve solution files (*.bce)"));


  if (newPath.isEmpty()) {
    cout << "isEmpty" << endl;
    return;
  }

  try
    {
      emit(dataPathChanged(newPath));
    }
  catch (std::exception & e)
    {
      qDebug() << "Load solution didnt work :( from BCEPlotHandler" << endl;
    }
} // slot for loading a solution

/////////////////////////////////////////////
// Plot Conditional-Marginal Distribution

void BCEPlotHandler::plotEqm() {

  vector<vector<double>> eqmMatrix = guiData.getEqmMatrix();

  colorMap->clearData();
  int nx = eqmMatrix.size();
  int ny = eqmMatrix[0].size();
  colorMap->data()->setSize(nx, ny); 
  colorMap->data()->setRange(QCPRange(0,nx), QCPRange(0,ny)); 
  double x, y;
  double maxEntry = 0;
  for (int xIndex=0; xIndex<nx; ++xIndex)
    {
      for (int yIndex=0; yIndex<ny; ++yIndex)
	{
	  double dataPoint = eqmMatrix[xIndex][yIndex];
	  colorMap->data()->setCell(xIndex,yIndex,dataPoint);
	  if (dataPoint > maxEntry)
	    maxEntry = dataPoint;
	}
    }

  colorScale->setDataRange(QCPRange(0,maxEntry*1.3));

  conditionalMarginalPlot->rescaleAxes();
  conditionalMarginalPlot->xAxis->scaleRange(1.05,conditionalMarginalPlot->xAxis->range().center());
  conditionalMarginalPlot->yAxis->scaleRange(1.05,conditionalMarginalPlot->yAxis->range().center());
  conditionalMarginalPlot->replot();

} // Plot Conditional-Marginal Distribution

////////////////////////////////////
// Plot Set of BCE

void BCEPlotHandler::plotBCEValueSet() {

  // Erase Current Contents

  setOfBCEPlot->clearGraphs();
  setOfBCEPlot->clearPlottables();
  setOfBCEPlot->addGraph();
  setOfBCEPlot->addGraph();

  // Getting Data

  vector<vector<double>> allEqm = guiData.getAllEqm();
  QVector<double> objective0Payoffs;
  QVector<double> objective1Payoffs;
  vector<int> playerObjectives;

  for (int player = 0; player < 2; player++)
    playerObjectives.push_back(guiData.getPlayerObjective(player));

  for (int i = 0; i < allEqm.size(); i++) {
    objective0Payoffs.push_back(allEqm[i][playerObjectives[0]]);
    objective1Payoffs.push_back(allEqm[i][playerObjectives[1]]);
  }

  // Graphing Curve

  QCPCurve * boundaryCurve = new QCPCurve(setOfBCEPlot->xAxis,setOfBCEPlot->yAxis);
  
  boundaryCurve->setData(objective0Payoffs,objective1Payoffs);

  setOfBCEPlot->addPlottable(boundaryCurve);

  setOfBCEPlot->graph(1)->setData(objective0Payoffs,objective1Payoffs);
  setOfBCEPlot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssDisc,
							QPen(Qt::blue),
							QBrush(Qt::blue),
							5));
  setOfBCEPlot->rescaleAxes();
  setOfBCEPlot->xAxis->scaleRange(1.1, setOfBCEPlot->xAxis->range().center());
  setOfBCEPlot->yAxis->scaleRange(1.1, setOfBCEPlot->yAxis->range().center());

  // Highlighting Current Equilibrium

  QVector<double> xCoordCurrentEqm;
  QVector<double> yCoordCurrentEqm;
  int currentEqmIndex = guiData.getCurrentEqmIndex();

  xCoordCurrentEqm.push_back(objective0Payoffs[currentEqmIndex]);
  yCoordCurrentEqm.push_back(objective1Payoffs[currentEqmIndex]);

  QPen pen;
  pen.setColor(Qt::green);
  pen.setWidth(4);

  setOfBCEPlot->graph(0)->setData(xCoordCurrentEqm,yCoordCurrentEqm);
  setOfBCEPlot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross,
							pen,
							QBrush(Qt::green),
							15));
  // Replot

  setOfBCEPlot->replot();

}

////////////////////////////////////
// Plot Deviation Objectives

void BCEPlotHandler::plotDeviationObjectives(int player) {

  // Erase Current Graphics

  deviationBarGraphs[player]->clearGraphs();
  deviationBarGraphs[player]->clearPlottables();

  // Setup Main Bar Graph

  QCPBars *barGraph = new QCPBars(deviationBarGraphs[player]->xAxis,
				  deviationBarGraphs[player]->yAxis);
  deviationBarGraphs[player]->addPlottable(barGraph);
  barGraph->setName("Expected Payoffs from Deviation");

  vector<vector<double>> objectiveValues = guiData.getObjectiveValues();

  QVector<double> yData;

  yData = QVector<double>::fromStdVector(objectiveValues[player]);

  int sizeYData = yData.size();
  QVector<double> xData(sizeYData,0);

  for(int i = 0; i < sizeYData; i++) {
    xData[i]=(double)i;
  }

  barGraph->setData(xData,yData);
  barGraph->setPen(QColor(255, 131, 0));
  barGraph->setBrush(QColor(255, 131, 0, 50));

  // Highlight Recommended Action

  QCPBars *recAction = new QCPBars(deviationBarGraphs[player]->xAxis,
				   deviationBarGraphs[player]->yAxis);
  deviationBarGraphs[player]->addPlottable(recAction);

  int action = guiData.getCurrentSliderVal(Action,player);

  recAction->setData(QVector<double>(1,action),
		     QVector<double>(1,yData[action]));

  recAction->setPen(QPen(Qt::green));
  recAction->setBrush(QBrush(Qt::green));

  // Highlight Indifferent Deviations

  double maxDifference = .005; // .5% Difference for rounding
  double recValue = yData[action];

  QVector<double> xCoordIndiff;
  QVector<double> yCoordIndiff;

  for (int i = 0; i < sizeYData; i++) {
    if (abs((yData[i]-recValue)/recValue) < maxDifference) {
      xCoordIndiff.push_back(i);
      yCoordIndiff.push_back(yData[i]);
    }
    // cout << (yData[i]-recValue)/recValue << endl;
  }

  QCPBars *indiffAction = new QCPBars(deviationBarGraphs[player]->xAxis,
				      deviationBarGraphs[player]->yAxis);
  deviationBarGraphs[player]->addPlottable(indiffAction);

  indiffAction->setData(xCoordIndiff,yCoordIndiff);
  indiffAction->setPen(QColor(1, 92, 191));
  indiffAction->setBrush(QColor(1, 92, 191, 50));

  // Replot

  deviationBarGraphs[player]->rescaleAxes();
  deviationBarGraphs[player]->replot();

} // Plot Deviation Objectives

/////////////////////////////////
// View Slots

void BCEPlotHandler::toggleLinearScale(bool checked) {
  if (checked)
    colorScale->setDataScaleType(QCPAxis::stLinear);
  else 
    colorScale->setDataScaleType(QCPAxis::stLogarithmic);
  plotEqm();
} // Slot to set a linear color scale for the distribution's heat map

void BCEPlotHandler::toggleColorfulTheme(bool checked) {
  QCPColorGradient *newGradient = new QCPColorGradient();
  if (checked)
    colorMap->setGradient(newGradient->gpSpectrum);
  else
    colorMap->setGradient(newGradient->inverted());
  plotEqm();
} // Slot to change color theme of heat map for conditional marginal distribution

void BCEPlotHandler::screenShot() {
  QString newPath = QFileDialog::getSaveFileName(this, tr("Save PNG"),
						 screenShotPath, tr("PNG files (*.png)"));
  if (newPath.isEmpty())
    return;
  QFileInfo fi(newPath);
  grab().save(newPath);
}

void BCEPlotHandler::setGUITitle() {

  stringstream dynamicTitle;

  string guiTitle = guiData.getGUITitle();

  dynamicTitle << "BCE Solution Viewer, Current File = "
	       << guiTitle;
    
  string newTitleStr = dynamicTitle.str();
  QString newTitle = QString::fromStdString(newTitleStr);

  this->setWindowTitle(newTitle);
}
