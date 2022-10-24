#include <QString>
#include <QDir>
#include <stdio.h>

#include "appwin.h"
#include "readbl.h"
#include "cline.h"
#include "ddplot.h"
#include "fformat.h"
#include "messages.h"


/* Read the plot file */

bool AppWin::loadFile(QStringList file)
{
  QString fname;
  PltWin *initConf=NULL;
  long int blstart=0;
  int i, icurplot, fmt, blnum=0, idx;
  bool ok;

  for (i=0; i<file.count(); i++) {
    if (VERBOSE)
      printf("* trying to decide the format of the block file\n");
    
    if (tabWidget == NULL) {
      tabWidget = new QTabWidget(this);
      tabWidget->setGeometry(0, 23, width(), height()-46);
      tabWidget->setToolTip(file.at(i));
    }
    
    // determine the format of the input file
    fmt = FFormat(file.at(i));

    while (fmt != FORMAT_GB || (fmt == FORMAT_GB && blstart >= 0)) {
      icurplot = tabWidget->count();
      PltWin *pw = new PltWin(tabWidget);
      if (pw == NULL) {
	msgCritical(crit_OutOfMemory);
	exit(0);
      }

      switch(fmt) {
      case FORMAT_DD:  // ddplot internal format
	if (VERBOSE) printf("* the chosen file has the .dd format of ddplot\n");
	ok = pw->LoadDD(file.at(i));
	break;
	
      case FORMAT_BOP:  // block format of the BOP code (see block.out)
	if (VERBOSE) printf("* the chosen file has the .bl format of PennBOP\n");
	ok = pw->LoadBOP(file.at(i));
	break;
	
      case FORMAT_PLT:  // plot format of the BOP code
	if (VERBOSE) printf("* the chosen file has the .plt format of PennBOP\n");
	ok = pw->LoadPLT(file.at(i));
	break;
	
      case FORMAT_GB:  // grain boundary data file (multiple blocks in one file)
	if (VERBOSE) printf("* the chosen file contains grain boundary data\n");
	blnum++;
	ok = pw->LoadGB(file.at(i), &blstart, initConf);
	if (blnum == 1)  initConf = pw;
	break;
	
      case FORMAT_XYZ: // format of JMol
	if (VERBOSE) printf("* the chosen file has the .xyz format of JMol\n");
	ok = pw->LoadXYZ(file.at(i));
	break;
	
      case FORMAT_UNKNOWN:
	msgError(err_InputFmtUnknown);
      }
      
      // an error during reading the plot file has occurred
      if (!ok) {
	delete pw;
	delete tabWidget;
	return false;
      }
      
      // catch an error that may occur when constructing the neighbor list
      ok = pw->InitBlockGeometry(false);
      if (!ok) {
	delete pw;
	delete tabWidget;
	return false;
      }

      // allocate the space for data
      pw->scaleArr.Allocate(pw->NInit, 10, 3);  
      pw->scaleArr.EnlargeStep(0, 10, 0);
      pw->atomScreen.Allocate(pw->NInit, 2);
      
      // calculate the relative displacements between atoms
      switch(pw->DispComponent) {
        case EDGE:
	  pw->calcEdgeRelDisp();
	  break;
	  
        case SCREW:
	  pw->calcScrewRelDisp();
      }
      
      // only the file name is printed (use the relative path)
      fname = QDir::current().relativeFilePath(file.at(i));

      if (fmt==FORMAT_GB)
        fname.asprintf("%s (#%d)", fname.toLatin1().data(), blnum);
      
      if (VERBOSE)
	printf("* inserting a widget with name: %s\n", fname.toLatin1().data());

      QPalette pal;
      if (BACKGROUND_COLOR != "") {
	pal.setColor(QPalette::Window, BACKGROUND_COLOR);
	pw->setPalette(pal);
      }
      pw->setAutoFillBackground(true);      
      plotWidget.append(pw);
      tabWidget->addTab(pw, fname);
      tabWidget->show();
      
      if (fmt!=FORMAT_GB)  break;
    }
  }
  repaintStatusBar();
  enableMenuItems(true);

  // manage menu item visibility
  idx = tabWidget->currentIndex();
  ui->actFirstPlot->setEnabled(idx > 0);
  ui->actPrevPlot->setEnabled(idx > 0);
  ui->actNextPlot->setEnabled(idx < tabWidget->count()-1);
  ui->actLastPlot->setEnabled(idx < tabWidget->count()-1);
  ui->actZLayDown->setEnabled(!plotWidget[idx]->zLayerSel(1));
  ui->actZLayUp->setEnabled(!plotWidget[idx]->zLayerSel(plotWidget[idx]->NumZLayers));

  return true;
}


/* Read the settings file .ddplot and interpret the commands */

void AppWin::loadSettings(QString file)
{
  QString cmd;
  FILE *f;
  char *buf;
  bool eof;

  f = fopen(file.toLatin1().data(), "r+");
  if (f != NULL) {
    if (VERBOSE) 
      printf("* reading settings from file %s\n", file.toLatin1().data());
  } else {
    if (VERBOSE) 
      printf("* no configuration file ~/.ddplot found => default settings will be used\n");
    return;
  }

  buf = (char *) malloc(128);

  while (!feof(f)) {
    eof = (fgets(buf, 128, f) == NULL);
    if (eof) break;
    cmd = buf;
    InterpretCommand(cmd);
  }

  free(buf);
  fclose(f);
}
