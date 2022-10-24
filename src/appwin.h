#ifndef APPWIN_H
#define APPWIN_H

#include <QLabel>
#include <QMainWindow>
#include <QMenu>
#include <QTabWidget>

#include "ui_appwin.h"

#include "pltwin.h"
#include "strings.h"

QT_BEGIN_NAMESPACE
namespace Ui { class AppWin; }
QT_END_NAMESPACE

class AppWin : public QMainWindow
{
  Q_OBJECT

public:
  int num = 145;
  QPrinter *printer;
  QTabWidget *tabWidget;
  QVector<PltWin *> plotWidget;
  QLabel *statusLabel;

public:
  AppWin(QWidget *parent = nullptr);
  ~AppWin();

private:
  Ui::AppWin *ui;

protected:
  void closeEvent( QCloseEvent* );
  virtual void resizeEvent( QResizeEvent * );

public slots:
  // declared in appwin-init.cpp
  bool loadFile(QStringList file);
  void loadSettings(QString file);

  // declared in appwin-file.cpp
  void animate();
  void closeCurrent();
  void closeAll();
  void exportCFG();
  void exportEPS();
  void exportPLT();
  void exportXYZ();
  void exportVTK();
  void open();
  void print();
  void quit();
  void saveDD();

  // declared in appwin-view.cpp
  void fitWin();
  void panRight();
  void panLeft();
  void panUp();
  void panDown();
  void panFastRight();
  void panFastLeft();
  void panFastUp();
  void panFastDown();
  void zoomIn();
  void zoomOut();

  // declared in appwin-plot.cpp
  void burgVect();
  void defineBurgersCircuit_end();
  void defDisloPos();
  void disloCenter();
  void plotAtomLayers();
  void plotAtomTypes();
  void plotAtomNeighbors();
  void plotEdgeComponent();
  void plotNoComponent();
  void plotScrewComponent();
  void plotUnrelaxed();
  void plotRelaxed();
  void plotComposite();
  void plotInertAtoms();
  void printMagDisp();
  void printAtomNum();
  void relaxInfo();
  void selectFirstPlot();
  void selectLastPlot();
  void selectNextPlot();
  void selectPrevPlot();
  void selectPlots();
  void showCells();
  void showCSys();
  void showGb();
  void showPlanes();

  // declared in appwin-layers.cpp
  void coordAtoms();
  void moveZLayersDown();
  void moveZLayersUp();
  void showZLayers();
  void selectZLayers();

  // declared in appwin-transforms.cpp
  bool coordSys();
  void compareDisp();
  void projVector();
  void reflectBlock(QAction *action);
  void rotateBlock(QAction *action);
  void slot_actXZReflect() { reflectBlock(ui->actXZReflect); }
  void slot_actYZReflect() { reflectBlock(ui->actYZReflect); }
  void slot_actXYReflect() { reflectBlock(ui->actXYReflect); }
  void slot_actXRot180() { rotateBlock(ui->actXRot180); }
  void slot_actYRot180() { rotateBlock(ui->actYRot180); }
  void slot_actZRot180() { rotateBlock(ui->actZRot180); }
  void slot_actXRot90() { rotateBlock(ui->actXRot90); }
  void slot_actYRot90() { rotateBlock(ui->actYRot90); }
  void slot_actZRot90() { rotateBlock(ui->actZRot90); }
  void transform();

  // declared in appwin-calculations.cpp
  void calcRDF();
  void calcDPosPeierlsNabarro_interm();
  void calcDPosPeierlsNabarro_end();
  void calcDPosDecimate_end();
  void defineBurgersCircuit();
  void dlgDPosDecimate();
  void dlgDPosPeierlsNabarro();
  void probeAtomDispl();
  void defineCalcAtomDispl_end();

  // declared in appwin-specials.cpp
  void applyYDiad();
  void arrowsPlot();
  void correctEdgeCompBCC();
  void crystalPlot();

  // declared in appwin-appearance.cpp
  void arrNeighbors();
  void lookAndFeel();
  void scaleArrow();
  void scalePos();
  void setZTolerance();

  // declared in appwin-help.cpp
  void about();

  // declared in appwin-misc.cpp
  void enableMenuItems(bool flag);
  void repaintStatusBar();
};

#endif // APPWIN_H
