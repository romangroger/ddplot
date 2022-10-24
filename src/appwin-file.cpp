#include <QFileDialog>
#include <QProgressDialog>
#include <QPrintDialog>
#include <QPrinter>

#include "qcustomplot.h"
#include "ui_danimate.h"
#include "ui_dsetupexportvtk.h"

#include "animate.h"
#include "appwin.h"
#include "calcs.h"
#include "pltwin.h"
#include "messages.h"
#include "misc.h"


void AppWin::animate()
{
  PltWin *pw;
  QString outf, cmd, args, animfile, item;
  int n, nbefore, i;
  int density, delay, nloops;
  bool exit, selFlag, rmFiles, anim;

  // checking to see if ImageMagick is present
  exit = system("convert -version > dd_log")==-1;
  if (exit) {
    msgError(err_NoImageMagick);    
    return;
  }
  system("rm -f dd_*");

  Animate *dialog = new Animate;

  for (i=0; i<tabWidget->count(); i++) {
    item = plotWidget[i]->FName;
    
    // only the file name is printed
    item = QDir::current().relativeFilePath(item);
    
    selFlag = plotWidget[i]->isSelected;

    dialog->plotFiles->addItem(item);
    dialog->plotFiles->item(i)->setSelected(selFlag);
  }

  dialog->exec();  
  if (dialog->result()!=QDialog::Accepted)
    return;

  density = dialog->density->value();
  delay = dialog->delay->value();
  nloops = dialog->nloops->value();
  args = dialog->args->text();
  rmFiles = !dialog->rmFiles->isChecked();

  nbefore = tabWidget->currentIndex();

  QProgressDialog pdlg("Collecting plots...", "&Abort", 0, tabWidget->count()-1, this);
  exit = false;

  anim = false;
  for (n=0; n<tabWidget->count(); n++) {
    pdlg.setValue(n);

    selFlag = dialog->plotFiles->item(n)->isSelected();
    if (!selFlag)
      continue;

    anim = true;
    pw = plotWidget[n];
    tabWidget->setCurrentIndex(n);

    outf.asprintf("dd_%03d.ps", n);
    pw->feps = fopen(outf.toLatin1().data(), "w+");
    fprintf(pw->feps, "%%!PS-Adobe-3.0 EPSF-3.0\n");
    fprintf(pw->feps, "%%%%BoundingBox: %d %d %d %d\n", 0, 0, pw->width(), pw->height());
    fprintf(pw->feps, "/Helvetica findfont 12 scalefont setfont\n");
    pw->paintEPS = true;    
    pw->repaint();
    fprintf(pw->feps,"%%%%Trailer\n");
    fclose(pw->feps);

    pw->paintEPS = false;

    if (pdlg.wasCanceled()) {  
      exit = true;
      break;
    }
  }

  pdlg.cancel();
  if (exit || !anim) return;

  tabWidget->setCurrentIndex(nbefore);
  plotWidget[nbefore]->repaint();

  animfile = CreateFName(pw->FName, ".gif");
  QString types("GIF (*.gif)");
  animfile = QFileDialog::getSaveFileName(
    this,
    "Export to a dynamic .GIF file",
    animfile,
    types);
  if (animfile=="") return;

  // calling the ImageMagick
  cmd.asprintf("convert -density %d -dispose previous -delay %d %s dd_*.ps %s",
	      density,delay,args.toLatin1().data(),animfile.toLatin1().data());
  system(cmd.toLatin1().data());
  if (rmFiles) system("rm -f dd_*.ps");
}


/* Close the currently opened plot widget. If no widget is left in the system after this delete, we
   also remove the tab widget and disable the relevant menu items. */

void AppWin::closeCurrent()
{
  int iplot = tabWidget->currentIndex();

  tabWidget->removeTab(iplot);
  delete(plotWidget[iplot]);
  plotWidget.remove(iplot);

  if (tabWidget->count() == 0) {
    delete(tabWidget);
    tabWidget = NULL;
    enableMenuItems(false);
  } else {
    if (iplot >= tabWidget->count())  iplot = tabWidget->count() - 1;
    tabWidget->setCurrentIndex(iplot);
  }
  repaintStatusBar();
}


/* Close all existing plot widgets, remove the tab widget and disable the relevant menu items. */

void AppWin::closeAll()
{
  int nplots = tabWidget->count();

  for (int i=0; i<nplots; i++) {
    tabWidget->removeTab(i);
    delete(plotWidget[i]);
  }
  plotWidget.clear();
  delete(tabWidget);
  tabWidget = NULL;
  repaintStatusBar();
  enableMenuItems(false);
}


void AppWin::exportCFG()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];
  QString aname, cfgfile, msg;
  DVector s(3);
  double amass;
  int i, d, atype;
  FILE *fcfg;

  cfgfile = CreateFName(pw->FName, ".cfg");
  QString types("CFG file (*.cfg)");
  cfgfile = QFileDialog::getSaveFileName(
    this,
    "Export to CFG format (AtomEye)",
    cfgfile,
    types);
  if (cfgfile=="") return;

  fcfg = fopen(cfgfile.toLatin1().data(), "w+");

  if (pw->InertAtoms)
    fprintf(fcfg, "Number of particles = %d\n", pw->NInit+pw->NInert);
  else
    fprintf(fcfg, "Number of particles = %d\n", pw->NInit);    
  fprintf(fcfg, "A = %0.4f Angstrom (basic length scale)\n", pw->latpar);

  // supercell's first edge (x-size of the block)
  fprintf(fcfg, "H0(1,1) = %0.4f A\n", pw->latpar*pw->blSize(1));
  fprintf(fcfg, "H0(1,2) = 0.0 A\n");
  fprintf(fcfg, "H0(1,3) = 0.0 A\n");

  // supercell's second edge (y-size of the block)
  fprintf(fcfg, "H0(2,1) = 0.0 A\n");
  fprintf(fcfg, "H0(2,2) = %0.4f A\n", pw->latpar*pw->blSize(2));
  fprintf(fcfg, "H0(2,3) = 0.0 A\n");

  // supercell's third edge (z-size of the block)
  fprintf(fcfg, "H0(3,1) = 0.0 A\n");
  fprintf(fcfg, "H0(3,2) = 0.0 A\n");
  fprintf(fcfg, "H0(3,3) = %0.4f A\n", pw->latpar*pw->blSize(3));

  // coordinates of atoms - initial configuration
  for (i=1; i<=pw->NInit; i++) {
    atype = pw->atomType(i);
    aname = ATOM_NAME[atype];
    amass = AtomicMass(aname.toUpper());

    // reduced coordinates (between 0 and 1)
    for (d=1; d<=3; d++)
      s(d) = (pw->xyzInit(i,d) - pw->xyzMin(d)) / pw->blSize(d);

    fprintf(fcfg,"%0.4f  %2s  %6.4f  %6.4f  %6.4f  0  0  0\n",
	    amass, aname.toLatin1().data(), s(1), s(2), s(3));
  }

  if (pw->InertAtoms) {
    for (i=1; i<=pw->NInert; i++) {
      aname = "H";
      amass = 1.0;
      for (d=1; d<=3; d++)
	s(d) = (pw->xyzInert(i,d) - pw->xyzMin(d)) / pw->blSize(d);
      fprintf(fcfg,"%0.4f  %2s  %6.4f  %6.4f  %6.4f  0  0  0\n",
	      amass, aname.toLatin1().data(), s(1), s(2), s(3));
    }
  }
  
  fclose(fcfg);
}


void AppWin::exportEPS()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];
  QString epsfile, msg;

  epsfile = CreateFName(pw->FName, ".eps");
  QString types("Postscript (*.ps *.eps)");
  epsfile = QFileDialog::getSaveFileName(
    this,
    "Export to Postscript",
    epsfile,
    types);
  if (epsfile=="") return;

  pw->feps = fopen(epsfile.toLatin1().data(), "w+");
  fprintf(pw->feps, "%%!PS-Adobe-3.0 EPSF-3.0\n");
  fprintf(pw->feps, "%%%%BoundingBox: %d %d %d %d\n", 0, 0, pw->width(), pw->height());
  fprintf(pw->feps, "/Helvetica findfont 12 scalefont setfont\n");
  pw->paintEPS = true;
  pw->drawIt(NULL);
  fprintf(pw->feps, "%%%%Trailer\n");
  fclose(pw->feps);

  pw->paintEPS = false;
}


void AppWin::exportPLT()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];
  QString aname, pltfile, msg;
  FILE *f;
  int n;

  pltfile = CreateFName(pw->FName,".plt");
  QString types( "PLT file (*.plt)");
  pltfile = QFileDialog::getSaveFileName(
    this,
    "Export to PLT format (PennBOP)",
    pltfile,
    types);
  if (pltfile=="") return;

  f = fopen(pltfile.toLatin1().data(), "w+");

  fprintf(f,"%d\n",pw->NRel);
  for (n=1; n<=pw->NRel; n++) 
    fprintf(f,"%12.6lf\n", pw->xyzRel(n,3));
  for (n=1; n<=pw->NRel; n++)
    fprintf(f,"%12.6lf  %12.6lf  %d\n", pw->xyzRel(n,1), pw->xyzRel(n,2), pw->atomType(n));

  fprintf(f,"%d\n",pw->NInit);
  for (n=1; n<=pw->NInit; n++) 
    fprintf(f,"%12.6lf\n", pw->xyzInit(n,3));
  for (n=1; n<=pw->NInit; n++)
    fprintf(f,"%12.6lf  %12.6lf\n", pw->xyzInit(n,1), pw->xyzInit(n,2));

  fprintf(f,"%d\n",0);
  fprintf(f,"%12.6f\n", pw->period(1));
  fprintf(f,"%12.6f\n", pw->period(2));
  fprintf(f,"%12.6f\n", pw->period(3));
  fprintf(f,"CORE { %lf;%lf }\n", pw->xCore, pw->yCore);

  fclose(f);
}


void AppWin::exportXYZ()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];
  QString aname, xyzfile, msg;
  FILE *f;
  int n,atype;

  xyzfile = CreateFName(pw->FName,".xyz");
  QString types( "XYZ file (*.xyz)");
  xyzfile = QFileDialog::getSaveFileName(
    this,
    "Export to XYZ format (JMol/RasMol)",
    xyzfile,
    types);
  if (xyzfile=="") return;

  f = fopen(xyzfile.toLatin1().data(), "w+");

  fprintf(f,"%d\n",pw->NInit);
  fprintf(f,"Unrelaxed positions of atoms below...\n");
  for (n=1; n<=pw->NInit; n++) {
    atype = pw->atomType(n);
    aname = ATOM_NAME[atype];
    fprintf(f,"%s  %12.6lf  %12.6lf  %12.6lf\n",
            aname.toLatin1().data(), pw->xyzInit(n,1), pw->xyzInit(n,2), pw->xyzInit(n,3));
  }

  fprintf(f,"%d\n",pw->NRel);
  fprintf(f,"Relaxed positions of atoms below...\n");
  for (n=1; n<=pw->NRel; n++) {
    atype = pw->atomType(n);
    aname = ATOM_NAME[atype];
    fprintf(f,"%s  %12.6lf  %12.6lf  %12.6lf\n",
	    aname.toLatin1().data(), pw->xyzRel(n,1), pw->xyzRel(n,2), pw->xyzRel(n,3));
  }

  fprintf(f, "BVECT %0.4lf %0.4lf %0.4lf\n", pw->Bvect(1), pw->Bvect(2), pw->Bvect(3));

  fclose(f);
}


/*
  Create an unstructured grid, where the nodes are midpoints between each pair of neighbor and
  assign the relative displacements of these two neighbors separately. These are stored as two
  separate fields, where the component parallel to the Burgers vector is saved as a scalar field
  and the component perpendicular to the Burgers vector as a vector field. 
*/

void AppWin::exportVTK()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];
  FILE *f;
  QString vtkfile;
  DMatrix disp, rmid, cs(3,3), cs0(3,3), mat(3,3);
  DVector dpar, b(3), zpar(3), xperp(3), yperp(3), dperp;
  double blen, s, t, mod, xproj, yproj;
  int icen, ineigh, idx, d, npts, i;

  // dialog for export to VTK

  Ui::DSetupExportVTK ui;
  QDialog *dialog = new QDialog;
  ui.setupUi(dialog);

  cs0(1,1) = 1.0; cs0(1,2) = 0.0; cs0(1,3) = 0.0;
  cs0(2,1) = 0.0; cs0(2,2) = 1.0; cs0(2,3) = 0.0;
  cs0(3,1) = 0.0; cs0(3,2) = 0.0; cs0(3,3) = 1.0;
  mat = mtransf(pw->csys, cs0);
  b = mvmult(mat, pw->Bvect);

  ui.x1->setText(QString::number(pw->csys(1,1)));
  ui.x2->setText(QString::number(pw->csys(1,2)));
  ui.x3->setText(QString::number(pw->csys(1,3)));
  ui.y1->setText(QString::number(pw->csys(2,1)));
  ui.y2->setText(QString::number(pw->csys(2,2)));
  ui.y3->setText(QString::number(pw->csys(2,3)));
  ui.z1->setText(QString::number(pw->csys(3,1)));
  ui.z2->setText(QString::number(pw->csys(3,2)));
  ui.z3->setText(QString::number(pw->csys(3,3)));
  ui.b1->setText(QString::number(b(1)));
  ui.b2->setText(QString::number(b(2)));
  ui.b3->setText(QString::number(b(3)));
  ui.zpar1->setText(QString::number(b(1)));
  ui.zpar2->setText(QString::number(b(2)));
  ui.zpar3->setText(QString::number(b(3)));
  ui.xperp1->setText(QString::number(pw->csys(1,1)));
  ui.xperp2->setText(QString::number(pw->csys(1,2)));
  ui.xperp3->setText(QString::number(pw->csys(1,3)));
  ui.yperp1->setText(QString::number(pw->csys(2,1)));
  ui.yperp2->setText(QString::number(pw->csys(2,2)));
  ui.yperp3->setText(QString::number(pw->csys(2,3)));
  
  dialog->exec();
  if (dialog->result() != QDialog::Accepted) return;

  cs(1,1) = ui.x1->text().toDouble();
  cs(1,2) = ui.x2->text().toDouble();
  cs(1,3) = ui.x3->text().toDouble();
  cs(2,1) = ui.y1->text().toDouble();
  cs(2,2) = ui.y2->text().toDouble();
  cs(2,3) = ui.y3->text().toDouble();
  cs(3,1) = ui.z1->text().toDouble();
  cs(3,2) = ui.z2->text().toDouble();
  cs(3,3) = ui.z3->text().toDouble();
  b(1) = ui.b1->text().toDouble();
  b(2) = ui.b2->text().toDouble();
  b(3) = ui.b3->text().toDouble();
  zpar(1) = ui.zpar1->text().toDouble();
  zpar(2) = ui.zpar2->text().toDouble();
  zpar(3) = ui.zpar3->text().toDouble();
  xperp(1) = ui.xperp1->text().toDouble();
  xperp(2) = ui.xperp2->text().toDouble();
  xperp(3) = ui.xperp3->text().toDouble();
  yperp(1) = ui.yperp1->text().toDouble();
  yperp(2) = ui.yperp2->text().toDouble();
  yperp(3) = ui.yperp3->text().toDouble();
  
  // transformations from the coordinate system cs to the orientation of the block

  mat = mtransf(cs0, cs);
  b = mvmult(mat, b);
  zpar = mvmult(mat, zpar);
  zpar /= sqrt(pow(zpar(1),2)+pow(zpar(2),2)+pow(zpar(3),2));
  xperp = mvmult(mat, xperp);
  xperp /= sqrt(pow(xperp(1),2)+pow(xperp(2),2)+pow(xperp(3),2));
  yperp = mvmult(mat, yperp);
  yperp /= sqrt(pow(yperp(1),2)+pow(yperp(2),2)+pow(yperp(3),2));

  // file save dialog
  
  vtkfile = CreateFName(pw->FName,".vtk");
  QString types( "VTK file (*.vtk)");
  vtkfile = QFileDialog::getSaveFileName(
    this,
    "Export to VTK format (ParaView)",
    vtkfile,
    types);
  if (vtkfile=="") return;
  
  // export to VTK
  
  disp.Allocate(pw->nnpairs,3);
  rmid.Allocate(pw->nnpairs,3);
  dperp.Allocate(pw->nnpairs);
  dpar.Allocate(pw->nnpairs);

  blen = vlength(pw->Bvect);
  b = pw->Bvect/blen;

  i = npts = 0;
  for (icen=1; icen<=pw->NInit; icen++) {
    if (!pw->zLayerSel(pw->zLayer(icen))) continue;
    idx = 1;    
    while (pw->NeighListInit(icen,idx)!=END_OF_LIST) {
      ineigh = pw->NeighListInit(icen,idx);
      if (!pw->zLayerSel(pw->zLayer(ineigh))) {
	idx++;
	continue;
      }

      i++;
      for (d=1; d<=3; d++) {
	rmid(i,d) = 0.5*(pw->xyzInit(icen,d)+pw->xyzInit(ineigh,d));
	disp(i,d) = (pw->xyzRel(ineigh,d)-pw->xyzRel(icen,d)) - (pw->xyzInit(ineigh,d)-pw->xyzInit(icen,d));
      }
      s = fabs(disp(i,1)*b(1)+disp(i,2)*b(2)+disp(i,3)*b(3));
      mod = s - floor(s/blen)*blen;
      t = mod/s;
      for (d=1; d<=3; d++) disp(i,d) *= t;

      dpar(i) = disp(i,1)*zpar(1) + disp(i,2)*zpar(2) + disp(i,3)*zpar(3);
      if (fabs(dpar(i)) >= blen/2.0) dpar(i) -= dpar(i)/fabs(dpar(i))*blen;
      dpar(i) = fabs(dpar(i));

      for (d=1; d<=3; d++) {
	xproj = disp(i,1)*xperp(1) + disp(i,2)*xperp(2) + disp(i,3)*xperp(3);
	yproj = disp(i,1)*yperp(1) + disp(i,2)*yperp(2) + disp(i,3)*yperp(3);
	//	dperp(i,1) = xproj; dperp(i,2) = yproj;
	dperp(i) = sqrt(pow(xproj,2)+pow(yproj,2));
      }
	
      idx++;
    }
  }
  npts = i;

  f = fopen(vtkfile.toLatin1().data(), "w+");
  fprintf(f, "# vtk DataFile Version 2.0 (viewable by paraview)\n");
  fprintf(f, "# created using ddplot\n");
  fprintf(f, "ASCII\n");
  fprintf(f, "DATASET UNSTRUCTURED_GRID\n");
  fprintf(f, "POINTS %d float\n", npts);
  for (i=1; i<=npts; i++)
    fprintf(f, "%f %f %f\n", rmid(i,1), rmid(i,2), rmid(i,3));
  fprintf(f, "CELLS %d %d\n", npts, 2*npts);
  for (i=1; i<=npts; i++)
    fprintf(f, "1 1\n");
  fprintf(f, "CELL_TYPES %d\n", npts);
  for (i=1; i<=npts; i++)
    fprintf(f, "1\n");
  fprintf(f, "POINT_DATA %d\n", npts);
  fprintf(f, "SCALARS dpar float 1\n");
  fprintf(f, "LOOKUP_TABLE default\n");
  for (i=1; i<=npts; i++)
    fprintf(f, "%f\n", dpar(i));
  fprintf(f, "SCALARS dperp float 1\n");
  fprintf(f, "LOOKUP_TABLE default\n");
  for (i=1; i<=npts; i++)
    fprintf(f, "%f\n", dperp(i));
  //  fprintf(f, "VECTORS dperp float\n");
  //  for (i=1; i<=npts; i++)
  //    fprintf(f, "%f %f 0.0\n", dperp(i,1), dperp(i,2));
  fclose(f);

  f = fopen("grid.vtk", "w+");
  fprintf(f, "# vtk DataFile Version 2.0 (viewable by paraview)\n");
  fprintf(f, "# created using ddplot\n");
  fprintf(f, "ASCII\n");
  fprintf(f, "DATASET UNSTRUCTURED_GRID\n");
  fprintf(f, "POINTS %d float\n", pw->NInit);
  for (i=1; i<=pw->NInit; i++)
    fprintf(f, "%f %f %f\n", pw->xyzInit(i,1), pw->xyzInit(i,2), pw->xyzInit(i,3));
  fprintf(f, "CELLS %d %d\n", npts, 2*npts);
  for (i=1; i<=npts; i++)
    fprintf(f, "1 1\n");
  fprintf(f, "CELL_TYPES %d\n", npts);
  for (i=1; i<=npts; i++)
    fprintf(f, "1\n");
  fclose(f);

  disp.Free();
  rmid.Free();
  dperp.Free();
  dpar.Free();
}


void AppWin::open()
{
  QString types( "usual plot files (*.plt *.bl *.dd *.xyz);;"
		 "ddplot file (*.dd);;"
		 "JMol .xyz file (*.xyz);;"
		 "plot files (*.plt *.dat);;"
		 "BOP block files (*.bl *.block block.out block.interm st.interm.*);;"
		 "any file (*)");
  bool ok;

  QStringList files = QFileDialog::getOpenFileNames(
    this,
    "Select one or more files to open",
    "./",
    types);

  if (files.isEmpty()) return;

  QProgressDialog pdlg( "Loading plot files...", "&Abort", 0, files.count(),
			this );

  ok = loadFile(files);
}


void AppWin::print()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];

  QPrintDialog printDialog(printer, this);
  if (printDialog.exec() == QDialog::Accepted)
    pw->printIt(printer);
}


void AppWin::quit()
{
  //  if (msgWarning( warn_ReallyQuit ))
  exit(0);
}


// Saves the current plot in the .dd format of ddplot. If we are
// saving a file which has been read in the .dd format, the coordinate
// system is not requested because it was read from the .dd file. If
// the original file is .plt, the system asks for the coordinate
// system of the block.
void AppWin::saveDD()
{
  PltWin *pw = plotWidget[ tabWidget->currentIndex() ];
  QString fname, msg;
  FILE *fdd;
  int i;

  fname = CreateFName(pw->FName,".dd");
  QString types("ddplot's internal (*.dd)");
  fname = QFileDialog::getSaveFileName(
    this,
    "Save the plot as .dd file",
    fname,
    types);
  if (fname=="") return;

  fdd = fopen(fname.toLatin1().data(),"w+");

  // header
  fprintf(fdd,"#\n");
  fprintf(fdd,"#  To view the atomic structure stored in this file, use ddplot\n");
  fprintf(fdd,"#  which is downloadable from http://groger.ipm.cz\n");
  fprintf(fdd,"#\n\n");

  // coordinate system of the block
  fprintf(fdd,"CSYS\n");
  for (i=1; i<=3; i++) {
    fprintf(fdd,"  %10.6lf  %10.6lf  %10.6lf\n",
	    pw->csys(i,1), pw->csys(i,2), pw->csys(i,3)); 
  }
  fprintf(fdd,"\n");

  // periodicity along x,y,z
  fprintf(fdd,"PERIOD\n");
  fprintf(fdd,"  %10.6lf  %10.6lf  %10.6lf\n\n",
	  pw->period(1), pw->period(2), pw->period(3));

  // dislocation center
  fprintf(fdd,"DISLO_CENTER\n");
  fprintf(fdd,"  %10.6lf  %10.6lf\n\n", pw->xCore, pw->yCore);

  // unrelaxed coordinates
  fprintf(fdd,"NUM_UNREL\n  %d\n\n",pw->NInit);
  fprintf(fdd,"COOR_UNREL\n");
  for (i=1; i<=pw->NInit; i++) {
    fprintf(fdd,"  %10.6lf  %10.6lf  %10.6lf  %2d\n",
	    pw->xyzInit(i,1), pw->xyzInit(i,2), pw->xyzInit(i,3), pw->atomType(i));
  }
  fprintf(fdd,"\n");

  // relaxed coordinates
  fprintf(fdd,"NUM_REL\n  %d\n\n",pw->NRel);
  fprintf(fdd,"COOR_REL\n");
  for (i=1; i<=pw->NRel; i++) {
    fprintf(fdd,"  %10.6lf  %10.6lf  %10.6lf\n",
	    pw->xyzRel(i,1), pw->xyzRel(i,2), pw->xyzRel(i,3));
  }
  fprintf(fdd,"\n");

  // details of the relaxation
  if (pw->tail!=NULL) {
    fprintf(fdd,"RELAX_DETAILS\n");
    fwrite(pw->tail, 1, pw->TailSize-1, fdd);
  }
  
  fclose(fdd);

  // change the name of the widget
  fname = QDir::current().relativeFilePath(fname);
  tabWidget->setTabText(tabWidget->currentIndex(), fname);
}
