#ifndef DPOSPNRESULTS_H
#define DPOSPNRESULTS_H

#include <qstring.h>
#include "ui_ddpospnresults.h"

class DPosPNResults : public QDialog, public Ui::DDPosPNResults
{
  Q_OBJECT
  
public:
  QString txtXY, txtDPos;

  DPosPNResults(QDialog *parent=0);
  
public slots:
  void saveDPoly();
  void saveDPos();
};

#endif
