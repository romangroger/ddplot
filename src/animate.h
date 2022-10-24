#ifndef ANIMATE_H
#define ANIMATE_H
 
#include "ui_danimate.h"


class Animate : public QDialog, public Ui::DAnimate
{
  Q_OBJECT
 
public:
  Animate();

public slots:
  void selectAll();
  void unselectAll();
};

#endif
