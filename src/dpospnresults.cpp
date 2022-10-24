#include <qfiledialog.h>
#include <stdio.h>

#include "dpospnresults.h"


DPosPNResults::DPosPNResults(QDialog *parent) : QDialog(parent)
{
  setupUi(this);
  connect(btSaveDPoly, SIGNAL(released()), this, SLOT(saveDPoly()));
  connect(btSaveDPos, SIGNAL(released()), this, SLOT(saveDPos()));
}


void DPosPNResults::saveDPoly()
{
  QString fname = QFileDialog::getSaveFileName(this,
					       tr("Save the dislocation polygons"),
					       "dpoly.dat",
					       tr("data file (*.dat)"));
  if (fname!="") {
    FILE *f = fopen(fname.toLatin1(), "w+");
    fprintf(f,"%s\n",txtXY.toLatin1().data());
    fclose(f);
  }
}


void DPosPNResults::saveDPos()
{
  QString fname = QFileDialog::getSaveFileName(this,
					       tr("Save the positions of the dislocation"),
					       "dpos.dat",
					       tr("data file (*.dat)"));
  if (fname!="") {
    FILE *f = fopen(fname.toLatin1(), "w+");
    fprintf(f,"%s\n",txtDPos.toLatin1().data());
    fclose(f);
  }
}
