#ifndef SELECTZLAYERS_H
#define SELECTZLAYERS_H
 
#include "ui_dselectzlayers.h"


class SelectZLayers : public QDialog, public Ui::DSelectZLayers
{
  Q_OBJECT
 
public:
  SelectZLayers(QDialog *parent = 0);

public slots:
  void selectAll();
  void unselectAll();
};

#endif
