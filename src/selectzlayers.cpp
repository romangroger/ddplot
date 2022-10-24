#include "selectzlayers.h"


SelectZLayers::SelectZLayers(QDialog *parent) : QDialog(parent)
{
  setupUi(this);
  connect(btSelAll, SIGNAL(released()), this, SLOT(selectAll()));
  connect(btUnselAll, SIGNAL(released()), this, SLOT(unselectAll()));
}


void SelectZLayers::selectAll()
{
  int i;
  for (i=0; i<zLayers->count(); i++)
    zLayers->item(i)->setSelected(true);
}


void SelectZLayers::unselectAll()
{
  int i;
  for (i=0; i<zLayers->count(); i++)
    zLayers->item(i)->setSelected(false);
}
