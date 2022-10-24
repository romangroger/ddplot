#ifndef MESSAGES_H
#define MESSAGES_H

#include <qstring.h>

// Critical errors

#define crit_MAX_NEIGHBORS_Exceeded \
  "Maximum number of nearest neighbors exceeded. Increase MAX_NEIGHBORS.\n" \
  "This error can also be caused by wrong periodicity given at the end\n" \
  "of your plot file or by large cut-off radius for the neighbor list.\n" \
  "In the latter case, enter manually NeighRCut in .ddplot file to avoid\n" \
  "this message."

#define crit_OutOfMemory \
  "Out of memory signal received when trying to\n" \
  "allocate space for data."


// Errors

#define err_ProjVectIsString \
  "Projection vector must be defined by three numbers.\n" \
  "Characters and special symbols are not permitted."

#define err_ScaleArrowIsString \
  "Scaling vector must be a number.\n" \
  "Characters and special symbols are not permitted."

#define err_ScalePosIsString \
  "Magnification factor must be a number.\n" \
  "Characters and special symbols are not permitted."

#define err_CircuitNotDefined \
  "Burgers circuit is not uniquely defined.\n" \
  "Pick at least three atoms along the path."

#define err_No_bbox_add \
  "The program bbox_add.pl is not accessible.\n" \
  "The figure will not have a proper bounding\n" \
  "box."

#define err_NoImageMagick \
  "Unable to find ImageMagick which is required to produce animations.\n" \
  "If you have it, please set the PATH variable to the directory where\n" \
  "<convert> resides."

#define err_TooManyPlots \
  "Number of plots simultaneously present in the program exceeded.\n" \
  "Change the value of MAX_PLOTS to get rid of this message."

#define err_InputFmtUnknown \
  "The format of the input file was not recognized."

#define err_AtomNotInCell \
  "An atom found which does not belong to any cell.\n" \
  "Check the construction of the linked neighbor list."

#define err_CannotComparePlots \
  "The dislocation displacement maps in the two plots are not\n" \
  "comparable. The two plots must have:\n" \
  "  * the same number of atoms\n" \
  "  * the same order of atoms\n" \
  "  * identical neighbor lists\n"

#define err_NoUnrelPos \
  "The file does not contain unrelaxed positions of atoms.\n" \
  "No way to draw differential displacements."

#define err_ZToleranceIsString \
  "The Z-tolerance must be a number.\n" \
  "Characters and special symbols are not permitted."

#define err_DPosPeierlsNabarro \
  "It was impossible to identify the dislocation position\n" \
  "using the Peierls-Nabarro concept."

#define err_DPosParallelLines \
  "No intersection of two parallel lines exists."

#define err_DPosNoIntersection \
  "Could not find the intersection of two lines when calculating\n" \
  "the dislocation position using the Peierls-Nabarro model."

#define err_DPosCannotFind3rdNode \
  "Cannot determine the 3rd node for interpolation of the dislocation\n" \
  "position using the Lagrange polynomial."

#define err_ProjVectNotValid \
  "The projection vector is not valid because its dot product with the\n" \
  "Burgers vector is zero."

// Questions

#define quest_AnimateAll \
  "This choice will create an animated GIF from all " \
  "widgets currently loaded. This can be a slow " \
  "operation if many widgets are loaded. \n" \
  "Are you sure you want this ?"


// Warnings

#define warn_ReallyQuit \
  "You are about to quit the program. " \
  "Do you really know what you are going to do ?", \
  "Retry", "Quit", 0, 0, 1


// Information

#define info_SettingsSaved \
  "Settings was successfully saved."

#define info_CalcBurgers \
  "You are about to define the Burgers circuit encompassing a dislocation. " \
  "Use your mouse to successively pick the atoms along the circuit. " \
  "! First and last atom are two different atoms (program automatically " \
  "  closes the curcuit) ! \n" \
  "LEFT BUTTON chooses the nearest atom on any plane.\n" \
  "RIGHT BUTTON closes the circuit and calculates the total Burgers vector."

#define info_AnimationDone \
  "Animation successfully generated.\nThe movie is stored in file %s."

#define info_MagPartialsSaved \
  "Magnitudes of the 6 screw fractionals surrounding the 3 screw " \
  "partials have been saved to file DDCORE.DAT"

#define info_GiveCoordBlock \
  "Coordinate system of the currently displayed block is unknown. " \
  "In order to make transformations, you must specify the orientation " \
  "of the block in [Transformations]."

#define info_CompareSamePlots \
  "You chose identical plots => no comparison needed."

void msgCritical(const char *msg);
void msgError(const char *msg);
int msgQuestion(const char *title, const char *msg);
int msgWarning(const char *msg, char *button1, char *button2, char *button3, int par1, int par2);
void msgInfo(QString msg);

#endif
