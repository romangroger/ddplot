#ifndef SELECTPLOTS_H
#define SELECTPLOTS_H
 
#include "ui_dselectplots.h"


class SelectPlots : public QDialog, public Ui::DSelectPlots
{
  Q_OBJECT
 
public:
  SelectPlots(QDialog *parent = 0);

public slots:
  void selectAll();
  void unselectAll();
};

#endif
