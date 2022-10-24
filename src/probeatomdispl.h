#ifndef PROBEATOMDISPL_H
#define PROBEATOMDISPL_H
 
#include "ui_dprobeatomdispl.h"

#include "darray.h"
#include "pltwin.h"
#include "qcustomplot.h"


class ProbeAtomDispl : public QDialog, public Ui::DProbeAtomDispl
{
  Q_OBJECT
 
public:
  QCustomPlot *customPlot;
  QVector<double> posInit, posRel, dUZ, ddScrew, ddEdge_mag;
  QVector< QVector<double> > ddEdge;
  PltWin *pw;
  double xmin, xmax, ymin, ymax;

  ProbeAtomDispl(QDialog *parent=0, PltWin *_pw=0);
  ~ProbeAtomDispl();

public slots:
  void printFileView();
  void plotGraphs();
  void saveData();
};

#endif
