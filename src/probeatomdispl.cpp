#include <QFile>
#include <QFileDialog>

#include "ddplot.h"
#include "probeatomdispl.h"


ProbeAtomDispl::ProbeAtomDispl(QDialog *parent, PltWin *_pw) : QDialog(parent)
{
  DVector dd(3);
  QString str;
  double blen;
  int i, a, a0;

  pw = _pw;
  customPlot = new QCustomPlot(this);
  customPlot->setGeometry(QRect(455, 20, 525, 230));
  customPlot->setBackground(Qt::white);
  customPlot->legend->setVisible(true);
  customPlot->xAxis->setLabel("position along the chain");
  customPlot->yAxis->setLabel("relative displacements");
  setupUi(this);

  connect(chkComment, SIGNAL(stateChanged(int)), this, SLOT(printFileView()));
  connect(chkAtomNumber, SIGNAL(stateChanged(int)), this, SLOT(printFileView()));
  connect(chkAtomPosInit, SIGNAL(stateChanged(int)), this, SLOT(printFileView()));
  connect(chkAtomPosRel, SIGNAL(stateChanged(int)), this, SLOT(printFileView()));
  connect(chkxyzInit, SIGNAL(stateChanged(int)), this, SLOT(printFileView()));
  connect(chkxyzRel, SIGNAL(stateChanged(int)), this, SLOT(printFileView()));
  connect(chkRelDispZ, SIGNAL(stateChanged(int)), this, SLOT(printFileView()));
  connect(chkScrew, SIGNAL(stateChanged(int)), this, SLOT(printFileView()));
  connect(chkEdge, SIGNAL(stateChanged(int)), this, SLOT(printFileView()));
  connect(btSaveData, SIGNAL(released()), this, SLOT(saveData()));

  xmin = ymin = 1e20;
  xmax = ymax = -1e20;
  
  posInit.resize(pw->napicked);
  posRel.resize(pw->napicked);
  dUZ.resize(pw->napicked);
  ddScrew.resize(pw->napicked);
  ddEdge.resize(pw->napicked);
  for (i=0; i<pw->napicked; i++) ddEdge[i].resize(2);
  ddEdge_mag.resize(pw->napicked);
  for (i=0; i<pw->napicked; i++) {
    if (i==0) {
      posInit[i] = posRel[i] = ddScrew[i] = ddEdge[i][0] = ddEdge[i][1] = ddEdge_mag[i] = dUZ[i] = 0.0;
    } else {
      a0 = pw->apicked(i);
      a = pw->apicked(i+1);
      posInit[i] = posInit[i-1] +
	sqrt(pow(pw->xyzInit(a,1),2)+pow(pw->xyzInit(a,2),2)+pow(pw->xyzInit(a,3),2));
      posRel[i] = posRel[i-1] +
	sqrt(pow(pw->xyzRel(a,1),2)+pow(pw->xyzRel(a,2),2)+pow(pw->xyzRel(a,3),2));

      blen = fabs(pw->Bvect(3));
      dd(1) = (pw->xyzRel(a,1)-pw->xyzRel(a0,1)) - (pw->xyzInit(a,1)-pw->xyzInit(a0,1));
      dd(2) = (pw->xyzRel(a,2)-pw->xyzRel(a0,2)) - (pw->xyzInit(a,2)-pw->xyzInit(a0,2));
      dd(3) = (pw->xyzRel(a,3)-pw->xyzRel(a0,3)) - (pw->xyzInit(a,3)-pw->xyzInit(a0,3));
      dd(3) -= floor(dd(3)/blen)*blen;
      dUZ[i] = dd(3);
      if (fabs(dd(3)) >= blen/2.0) dd(3) -= dd(3)/fabs(dd(3))*blen;
      ddScrew[i] = ddScrew[i-1] + dd(3);
      ddEdge[i][0] = ddEdge[i-1][0] + dd(1);
      ddEdge[i][1] = ddEdge[i-1][1] + dd(2);
      ddEdge_mag[i] = sqrt(pow(ddEdge[i][0],2) + pow(ddEdge[i][1],2));
      if (posInit[i] < xmin) xmin = posInit[i];
      if (posInit[i] > xmax) xmax = posInit[i];
      if (dUZ[i] < ymin) ymin = dUZ[i];
      if (dUZ[i] > ymax) ymax = dUZ[i];
      if (ddScrew[i] < ymin) ymin = ddScrew[i];
      if (ddScrew[i] > ymax) ymax = ddScrew[i];
      if (ddEdge_mag[i] < ymin) ymin = ddEdge_mag[i];
      if (ddEdge_mag[i] > ymax) ymax = ddEdge_mag[i];
    }
  }
  printFileView();
  plotGraphs();
}


ProbeAtomDispl::~ProbeAtomDispl()
{
  delete(customPlot);
}


void ProbeAtomDispl::printFileView()
{
  QString txt, part;
  int i, a;

  fileView->clear();

  if (chkComment->isChecked()) {
    txt = "# ";
    if (chkAtomNumber->isChecked()) txt.append("atom no., ");
    if (chkAtomPosInit->isChecked()) txt.append("pos. init., ");
    if (chkAtomPosRel->isChecked()) txt.append("pos. rel., ");
    if (chkxyzInit->isChecked()) txt.append("atomic coord. init., ");
    if (chkxyzRel->isChecked()) txt.append("atomic coord. rel., ");
    if (chkRelDispZ->isChecked()) txt.append("rel. disp. Z, ");
    if (chkScrew->isChecked()) txt.append("screw displ., ");
    if (chkEdge->isChecked()) txt.append("edge displ.");
    fileView->addItem(txt);
  }

  for (i=0; i<pw->napicked; i++) {
    a = pw->apicked(i+1);
    txt.clear();
    
    if (chkAtomNumber->isChecked()) {
      part.asprintf("%4d ", a);
      txt.append(part);
    }

    if (chkAtomPosInit->isChecked()) {
      part = QString::asprintf("%8.4f ", posInit[i]);
      txt.append(part);
    }
    
    if (chkAtomPosRel->isChecked()) {
      part = QString::asprintf("%8.4f ", posRel[i]);
      txt.append(part);
    }

    if (chkxyzInit->isChecked()) {
      part = QString::asprintf("%8.4f %8.4f %8.4f ",
		  pw->xyzInit(a,1), pw->xyzInit(a,2), pw->xyzInit(a,3));
      txt.append(part);
    }
    
    if (chkxyzRel->isChecked()) {
      part = QString::asprintf("%8.4f %8.4f %8.4f ",
		  pw->xyzRel(a,1), pw->xyzRel(a,2), pw->xyzRel(a,3));
      txt.append(part);
    }

    if (chkRelDispZ->isChecked()) {
      part = QString::asprintf("%8.4f ", dUZ[i]);
      txt.append(part);
    }

    if (chkScrew->isChecked()) {
      part = QString::asprintf("%8.4f ", ddScrew[i]);
      txt.append(part);
    }
    
    if (chkEdge->isChecked()) {
      part = QString::asprintf("%8.4f", ddEdge_mag[i]);
      txt.append(part);
    }

    if (txt != "") fileView->addItem(txt);
  }

  plotGraphs();
}


void ProbeAtomDispl::plotGraphs()
{
  int i=0;

  customPlot->clearGraphs();

  if (chkRelDispZ->isChecked()) {
    customPlot->addGraph();
    customPlot->graph(i)->setPen(QPen(Qt::green));
    customPlot->graph(i)->setData(posInit, dUZ);
    customPlot->graph(i)->setName("relative displ. in Z");
    i++;
  }

  if (chkScrew->isChecked()) {
    customPlot->addGraph();
    customPlot->graph(i)->setPen(QPen(Qt::red));
    customPlot->graph(i)->setData(posInit, ddScrew);
    customPlot->graph(i)->setName("screw component");
    i++;
  }

  if (chkEdge->isChecked()) {
    customPlot->addGraph();
    customPlot->graph(i)->setPen(QPen(Qt::blue));
    customPlot->graph(i)->setData(posInit, ddEdge_mag);
    customPlot->graph(i)->setName("edge component");
    i++;
  }

  customPlot->xAxis->setRange(xmin, xmax);
  customPlot->yAxis->setRange(ymin, ymax);  
  customPlot->replot();
}


void ProbeAtomDispl::saveData()
{
  FILE *f;
  int i;
  
  QString fname = QFileDialog::getSaveFileName(this, tr("Save the data"),
					       "reldisp.dat", tr("data file (*.dat)"));
  if (fname!="") {
    f = fopen(fname.toLatin1(), "w+");
    for (i=0; i<fileView->count(); i++)
      fprintf(f, "%s\n", fileView->item(i)->text().toLatin1().data());
    fclose(f);
  }
}
