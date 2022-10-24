#ifndef RDFPLOT_H
#define RDFPLOT_H
 
#include "ui_drdfplot.h"

#include "pltwin.h"
#include "qcustomplot.h"


class RdfPlot : public QDialog, public Ui::DRdfPlot
{
  Q_OBJECT
 
public:
  QCustomPlot *customPlot;
  PltWin *pw;
  QVector<double> rdfrad_init, rdfg_init, rdfrad_rel, rdfg_rel;
  double rmin, rmax;
  int nbins;
  bool pInit, pRel;

  RdfPlot(QDialog *parent=0, PltWin *_pw=0, double _rmin=0.0, double _rmax=0.0, int _nbins=1);
  ~RdfPlot();

public slots:
  void calc();
  void saveData();
};

#endif
