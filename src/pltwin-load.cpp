#include <qstring.h>

#include "messages.h"
#include "neighlist.h"
#include "pltwin.h"


/* Reading the BOP block file (.bl, .block) */

bool PltWin::LoadBOP(QString fname)
{
  FILE *f;
  QString rec, elem;
  char *buf;
  int n, idx, d;
  bool eof, ok=true;

  FName = fname;
  f = fopen(fname.toLatin1(), "r");
  if (f == NULL) return false;

  buf = (char *) malloc(128);

  // reading the plot file
  while (!feof(f)) {
    eof = (fgets(buf, 128, f) == NULL);
    if (eof) break;

    // throw out comments
    rec = buf;
    idx = rec.indexOf('#');   if (idx>=0) rec.truncate(idx);
    idx = rec.indexOf('\n');  if (idx>=0) rec.truncate(idx);
    idx = rec.indexOf('\r');  if (idx>=0) rec.truncate(idx);  // Linux terminates with \r\n

    // length of the simulation cell (in units of the lattice parameter)
    if (rec == "LEN") {
      fscanf(f, "%lf %lf %lf\n", &period(1), &period(2), &period(3));
      continue;
    }

    // lattice parameter
    if (rec == "LATPAR") {
      fscanf(f, "%lf\n", &latpar);
      continue;
    }

    // relaxed coordinates of atoms
    if (rec == "ND") { 
      for (d=1; d<=3; d++)
	period(d) /= latpar;    // the correct periodicity along the three directions
      Bvect = 0.0;
      Bvect(3) = period(3);

      fscanf(f, "%d\n", &NRel);  
      fgets(buf, 128, f);       // skip the "D" token

      xyzRel.Allocate(NRel,3);
      atomType.Allocate(NRel);
      atomType = 0;

      for (n=1; n<=NRel; n++) {
	fscanf(f, "%lf %lf %lf %d", &xyzRel(n,1), &xyzRel(n,2), &xyzRel(n,3), &atomType(n));
	atomType(n)++;          // must be from 1
	fgets(buf, 128, f);     // skip the rest, if there is some junk

	for (d=1; d<=3; d++)
	  xyzRel(n,d) *= period(d);  // correct positions of relaxed atoms
      }
      continue;
    }   

    // inert atoms near the surface of the simulated block
    if (rec == "NINERT") {
      fscanf(f, "%d\n", &NInert);
      fgets(buf, 128, f);       // skip DINERT token
      xyzInert.Allocate(NInert,3);

      for (n=1; n<=NInert; n++) {
	fscanf(f, "%lf %lf %lf", &xyzInert(n,1), &xyzInert(n,2), &xyzInert(n,3));
	fgets(buf, 128, f);     // skip the rest, if there is some junk

	for (d=1; d<=3; d++)
	  xyzInert(n,d) *= period(d);  // correct positions of inert atoms
      }

      continue;
    }

     // unrelaxed coordinates of atoms
    if (rec == "UNRLD") {
      NInit = NRel;  // NRel comes from block under D
      xyzInit.Allocate(NInit,3);

      for (n=1; n<=NInit; n++) {
	fscanf(f, "%lf %lf %lf", &xyzInit(n,1), &xyzInit(n,2), &xyzInit(n,3));
	fgets(buf, 128, f);  // skip the rest, if there is some junk
      }
      continue;
     }
  }

  free(buf);
  fclose(f);

  if (xyzInit.data==NULL) {
    msgError(err_NoUnrelPos); 
    return false;
  }

  return ok;
}


//-----------------------------------------------------------------------------
//
//   Reading the block in the DDPlot internal format (.dd)
//
//-----------------------------------------------------------------------------

bool PltWin::LoadDD( QString fname )
{
  FILE *f;
  QString cmd;
  char *buf;
  long int pos;
  int i,idx;
  bool eof, ok=true;

  FName = fname;
  f = fopen(fname.toLatin1(),"r");
  if (f==NULL)
    return false;

  buf = (char *) malloc(128);

  // reading the plot file
  while (!feof(f)) {
    eof = fgets(buf,128,f)==NULL;
    if (eof)
      break;

    // throw out comments
    cmd = buf;

    idx = cmd.indexOf('#');   if (idx>=0) cmd.truncate(idx);
    idx = cmd.indexOf('\n');  if (idx>=0) cmd.truncate(idx);
    idx = cmd.indexOf('\r');  if (idx>=0) cmd.truncate(idx);  // Linux terminates with \r\n
    if (cmd.isEmpty())
      continue;

    cmd = cmd.trimmed();
    cmd = cmd.toUpper();

    // interpretation of commands    
    if (cmd=="CSYS") {
      for (i=1; i<=3; i++)
	fscanf(f, "%lf %lf %lf\n", &csys(i,1), &csys(i,2), &csys(i,3));
      continue;
    }

    if (cmd=="PERIOD") {
      fscanf(f, "%lf %lf %lf\n", &period(1), &period(2), &period(3));
      Bvect = 0.0;
      Bvect(3) = period(3);
      continue;
    }

    if (cmd=="DISLO_CENTER") {
      fscanf(f, "%lf %lf\n", &xCore, &yCore);
      continue;
    }

    if (cmd=="NUM_UNREL") {
      fscanf(f, "%d\n", &NInit);
      continue;
    }

    if (cmd=="COOR_UNREL") {
      xyzInit.Allocate(NInit,3);
      atomType.Allocate(NInit);

      for (i=1; i<=NInit; i++) {
	fscanf(f, "%lf %lf %lf %d\n", 
	       &xyzInit(i,1), &xyzInit(i,2), &xyzInit(i,3), &atomType(i));
        atomType(i)++;   // must be from 1...
      }
      continue;
    }

    if (cmd=="NUM_REL") {
      fscanf(f, "%d\n", &NRel);
      continue;
    }

    if (cmd=="COOR_REL") {
      xyzRel.Allocate(NRel,3);

      for (i=1; i<=NRel; i++)
	fscanf(f, "%lf %lf %lf\n", 
	       &xyzRel(i,1), &xyzRel(i,2), &xyzRel(i,3));
      continue;
    }

    if (cmd=="NUM_INERT") {
      fscanf(f, "%d\n", &NInert);
      continue;
    }

    if (cmd=="COOR_INERT") {
      xyzInert.Allocate(NInert,3);

      for (i=1; i<=NInert; i++)
	fscanf(f, "%lf %lf %lf\n", 
	       &xyzInert(i,1), &xyzInert(i,2), &xyzInert(i,3));
      continue;
    }

    // read the rest of the file - information about the relaxation
    if (cmd=="RELAX_DETAILS") {
      pos = ftell(f);
      fseek(f, 0, SEEK_END);
      TailSize = ftell(f) - pos + 1;
      fseek(f, pos, SEEK_SET);
      
      tail = (char*) malloc(TailSize);
      if (tail==NULL) {
      	msgCritical(crit_OutOfMemory);
      	exit(2);
      }
      
      fread(tail, 1, TailSize, f);
    }

    if (cmd=="BVECT") {
      fscanf(f, "%lf %lf %lf\n", &Bvect(1), &Bvect(2), &Bvect(3));
    }
  }

  free(buf);
  fclose(f);

  return ok;
}


/*--------------------------------------------------------------------

  Reading the block containing the grain boundary data file (.PLT
  format).  The first configuration in the block is regarded as the
  unrelaxed (starting) configuration. The arrows then mean the
  relative displacements between an actual atomic positions and the
  atomic positions in this unrelaxed configuration.

--------------------------------------------------------------------*/

bool PltWin::LoadGB( QString fname, long int *blstart, PltWin *initConf )
{
  FILE *f;
  int dummy, endflag;
  int n;


  // the file read is the grain boundary data file
  isGBfile = true;
  FName = fname;

  f = fopen(fname.toLatin1(), "r");
  if (f==NULL)
    return false;

  // set the position to the start of a given block
  fseek(f, *blstart, SEEK_SET);

  // read one block
  fscanf(f, "%d\n", &NRel);

  // allocate the space for the data
  xyzRel.Allocate(NRel,3);
  atomType.Allocate(NRel);
  gbLayer.Allocate(10);
  gbLayer.EnlargeStep(10);

  // relaxed block - reading coordinates
  for (n=1; n<=NRel; n++)
    fscanf(f, "%lf\n", &xyzRel(n,3));
  
  for (n=1; n<=NRel; n++) {
    fscanf(f, "%lf %lf %d %d %d\n", 
	   &xyzRel(n,1), &xyzRel(n,2), &atomType(n), &gbLayer(n), &dummy);
    if (gbLayer(n)==0)  gbYcoor = xyzRel(n,2);
  }


  // Initial configuration is identical to the relaxed configuration only
  // for the first data set in the file. For all other sets, the unrelaxed
  // configuration is taken from the first block in the file.

  if (*blstart==0) {  
    NInit = NRel;
    xyzInit.Allocate(NInit,3);
    xyzInit = xyzRel;
  } else {
    NInit = initConf->NInit;
    xyzInit.Allocate(NInit,3);
    xyzInit = initConf->xyzInit;  
  }
  
  // skip the trailing zero - what if it is nonzero ?
  fscanf(f, "%d\n", &endflag);  

  // save the position of the start of the subsequent block; if this is the last one,
  // return -1
  if (feof(f))
    *blstart = -1;
  else
    *blstart = ftell(f);

  fclose(f);
  return true;
}


//-----------------------------------------------------------------------------
//
//   Reading the block in the plot format of BOP (.plt)
//
//-----------------------------------------------------------------------------

bool PltWin::LoadPLT( QString fname )
{
  FILE *f;
  QString qtxt;
  char txt[80];
  double unused;
  long int pos;
  int n;

  FName = fname;
  f = fopen(fname.toLatin1(), "r");
  if (f==NULL)
    return false;

  //
  // relaxed block - reading coordinates
  //

  fscanf(f, "%d\n", &NRel);

  xyzRel.Allocate(NRel,3);
  atomType.Allocate(NRel);

  for (n=1; n<=NRel; n++)
    fscanf(f, "%lf\n", &xyzRel(n,3));

  for (n=1; n<=NRel; n++) {
    fscanf(f, "%lf %lf %d\n", &xyzRel(n,1), &xyzRel(n,2), &atomType(n));
    atomType(n)++;  // must be from 1
  }


  //
  // initial block - reading coordinates
  //

  fscanf(f, "%d\n", &NInit);

  xyzInit.Allocate(NInit,3);

  for (n=1; n<=NInit; n++)
    fscanf(f, "%lf\n", &xyzInit(n,3));

  for (n=1; n<=NInit; n++)
    fscanf(f, "%lf %lf\n", &xyzInit(n,1), &xyzInit(n,2));

  // if there is any dislocation info, read it
  fscanf(f, "%lf \n %lf \n %lf \n %lf \n", &unused, &period(1), &period(2), &period(3));
  fscanf(f, "CORE { %lf;%lf }\n", &xCore, &yCore);

  // the Burgers vector is assumed to be parallel to the z-direction
  Bvect = 0.0;
  Bvect(3) = period(3);

  // read the rest of the file - information about the relaxation
  pos = ftell(f);
  qtxt = "";

  // is there any reasonable info (not just spaces) ?
  while (!feof(f)) {
    n = fscanf(f,"%s\n",&txt);
    if (n<0)
      break;

    qtxt = QString(txt).remove(' ');
    if (qtxt!="")
      break;
  }

  if (qtxt!="") {
    fseek(f, 0, SEEK_END);
    TailSize = ftell(f) - pos + 1;
    fseek(f, pos, SEEK_SET);
    
    tail = (char*) malloc(TailSize);
    if (tail==NULL) {
      msgCritical(crit_OutOfMemory);
      exit(2);
    }
  
    fread(tail, 1, TailSize, f);
  }

  fclose(f);

  return true;
}


//-----------------------------------------------------------------------------
//
//   Reading the block in the plot format of JMol (.xyz)
//
//-----------------------------------------------------------------------------

bool PltWin::LoadXYZ( QString fname )
{
  FILE *f;
  QString title, tag, elems[MAX_ELEMENTS];
  char *buf;
  char elem[3];
  double xmin, xmax, ymin, ymax, zmin, zmax;
  int n, i, nelem;

  FName = fname;
  f = fopen(fname.toLatin1(), "r");
  if (f==NULL)
    return false;

  buf = (char *) malloc(128);

  //
  // initial block - reading coordinates
  //

  fscanf(f, "%d\n", &NInit);
  fgets(buf, 128, f);
  title = QString(buf).trimmed();
  xyzInit.Allocate(NInit,3);
  atomType.Allocate(NInit);

  nelem = 0;

  xmin = ymin = zmin = INF;
  xmax = ymax = zmax = -INF;
  for (n=1; n<=NInit; n++) {
    fscanf(f, "%s %lf %lf %lf\n", &elem, &xyzInit(n,1), &xyzInit(n,2), &xyzInit(n,3));
    if (xyzInit(n,1)<xmin) xmin = xyzInit(n,1);
    if (xyzInit(n,1)>xmax) xmax = xyzInit(n,1);
    if (xyzInit(n,2)<ymin) ymin = xyzInit(n,2);
    if (xyzInit(n,2)>ymax) ymax = xyzInit(n,2);
    if (xyzInit(n,3)<zmin) zmin = xyzInit(n,3);
    if (xyzInit(n,3)>zmax) zmax = xyzInit(n,3);

    atomType(n) = 0;
    for (i=1; i<=nelem; i++)
      if (elems[i]==elem) {
	atomType(n) = i;
	break;
      }

    if (atomType(n)==0) {
      nelem++;
      if (nelem>MAX_ELEMENTS) {
	printf("Maximum number of elements reached. Increase MAX_ELEMENTS.\n");
	exit(0);
      }
      atomType(n) = nelem;
      elems[nelem] = elem;
    }
  }

  period(1) = xmax-xmin;
  period(2) = ymax-ymin;
  period(3) = zmax-zmin;

  if (feof(f)) {
    free(buf);
    return false;
  }


  //
  // relaxed block - reading coordinates
  //

  fscanf(f, "%d\n", &NRel);
  fgets(buf, 128, f);
  title = QString(buf).trimmed();
  xyzRel.Allocate(NRel,3);

  for (n=1; n<=NRel; n++)
    fscanf(f, "%s %lf %lf %lf\n", &elem, &xyzRel(n,1), &xyzRel(n,2), &xyzRel(n,3));

  if (!feof(f)) {
    fscanf(f, "%s", buf);
    tag = QString(buf).trimmed();
    if (tag=="BVECT") {
      fscanf(f, "%lf %lf %lf\n", &Bvect(1), &Bvect(2), &Bvect(3));
    }
  }

  free(buf);
  fclose(f);

  return true;
}


