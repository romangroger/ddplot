#include <QFile>
#include <QFileDialog>

#include "ddplot.h"
#include "rdfplot.h"
#include "rdf.h"


RdfPlot::RdfPlot(QDialog *parent, PltWin *_pw, double _rmin, double _rmax, int _nbins) : QDialog(parent)
{
  QString str;

  pw = _pw;
  rmin = _rmin;
  rmax = _rmax;
  nbins = _nbins;
  pInit = true;
  pRel = true;

  customPlot = new QCustomPlot(this);
  customPlot->setGeometry(QRect(40, 40, 639, 399));
  customPlot->setBackground(Qt::white);
  setupUi(this);

  connect(btCalcRDF, SIGNAL(released()), this, SLOT(calc()));
  connect(btSaveData, SIGNAL(released()), this, SLOT(saveData()));

  str = QString::asprintf("%0.4lf", rmin);
  input_rmin->setText(str);
  str = QString::asprintf("%0.4lf", rmax);
  input_rmax->setText(str);
  str = QString::asprintf("%i", nbins);
  input_nbins->setText(str);
}


RdfPlot::~RdfPlot()
{
  delete(customPlot);
}


void RdfPlot::calc()
{
  QString str;
  double gmax_init, gmax_rel, gmax;
  int i;
  bool ok;

  rmin = input_rmin->text().toDouble(&ok);
  rmax = input_rmax->text().toDouble(&ok);
  nbins = input_nbins->text().toInt(&ok);
  pInit = cbInit->isChecked();
  pRel = cbRel->isChecked();

  customPlot->clearGraphs();

  i = 0;
  gmax = 0;
  if (pInit) {
    RDF(pw, UNRELAXED, rmin, rmax, nbins, rdfrad_init, rdfg_init, gmax_init);
    if (gmax_init>gmax) gmax = gmax_init;
    customPlot->addGraph();
    customPlot->graph(i)->setPen(QPen(Qt::black));
    customPlot->graph(i)->setData(rdfrad_init, rdfg_init);
    customPlot->graph(i)->setName("perfect lattice");
    i++;
  }
  if (pRel) {
    RDF(pw, RELAXED, rmin, rmax, nbins, rdfrad_rel, rdfg_rel, gmax_rel);
    if (gmax_rel>gmax) gmax = gmax_rel;
    customPlot->addGraph();
    customPlot->graph(i)->setPen(QPen(Qt::red));
    customPlot->graph(i)->setData(rdfrad_rel, rdfg_rel);
    customPlot->graph(i)->setName("relaxed lattice");
  }

  customPlot->legend->setVisible(true);
  customPlot->xAxis->setLabel("separation of atoms:  r [A]");
  customPlot->yAxis->setLabel("number of atomic pairs with separation r:  g(r)/rho");
  customPlot->xAxis->setRange(rmin, rmax);
  customPlot->yAxis->setRange(0, gmax);
  customPlot->replot();
}


void RdfPlot::saveData()
{
  FILE *f;
  int i;
  
  QString fname = QFileDialog::getSaveFileName(this, tr("Save the data"), "rdf.dat", tr("data file (*.dat)"));
  if (fname!="") {
    f = fopen(fname.toLatin1(), "w+");
    if (pInit) {
      fprintf(f, "# Initial (unrelaxed) positions of atoms\n");
      for (i=0; i<rdfrad_init.size(); i++)
	fprintf(f, "%10.6f %10.6f\n", rdfrad_init[i], rdfg_init[i]);
      fprintf(f, "\n");
    }
    if (pRel) {
      fprintf(f, "# Relaxed positions of atoms\n");
      for (i=0; i<rdfrad_rel.size(); i++)
	fprintf(f, "%10.6f %10.6f\n", rdfrad_rel[i], rdfg_rel[i]);      
    }
    fclose(f);
  }
}
