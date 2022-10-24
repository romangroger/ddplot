#include "appwin.h"


/*----------------------------------------------------------------------------------------------------
  Set the visibility of the individual items in the main menu and submenus.
----------------------------------------------------------------------------------------------------*/

void AppWin::enableMenuItems( bool flag )
{
  ui->mfile->setEnabled(true);
  ui->mview->setEnabled(flag);
  ui->mplot->setEnabled(flag);
  ui->mlayers->setEnabled(flag);
  ui->mcalcs->setEnabled(flag);
  ui->mappear->setEnabled(flag);
  ui->mtran->setEnabled(flag);
  ui->mspecials->setEnabled(flag);
  ui->mhelp->setEnabled(true);

  ui->actSave->setEnabled(flag);
  ui->actClose->setEnabled(flag);
  ui->actCloseAll->setEnabled(flag);
  ui->actPrint->setEnabled(flag);
  ui->mexport->setEnabled(flag);
  ui->actExpEPS->setEnabled(flag);
  ui->actExpXYZ->setEnabled(flag);
  ui->actExpPLT->setEnabled(flag);
  ui->actExpCFG->setEnabled(flag);
  ui->actExpVTK->setEnabled(flag);
  ui->actAnimate->setEnabled(flag);

  ui->actZoomIn->setEnabled(flag);
  ui->actZoomOut->setEnabled(flag);
  ui->actFitWin->setEnabled(flag);
  ui->actPanRight->setEnabled(flag);
  ui->actPanLeft->setEnabled(flag);
  ui->actPanUp->setEnabled(flag);
  ui->actPanDown->setEnabled(flag);
  ui->actPanFRight->setEnabled(flag);
  ui->actPanFLeft->setEnabled(flag);
  ui->actPanFUp->setEnabled(flag);
  ui->actPanFDown->setEnabled(flag);

  ui->actFirstPlot->setEnabled(flag);
  ui->actPrevPlot->setEnabled(flag);
  ui->actNextPlot->setEnabled(flag);
  ui->actLastPlot->setEnabled(flag);
  ui->actSelPlots->setEnabled(flag);
  ui->actUnrelView->setEnabled(flag);
  ui->actRelView->setEnabled(flag);
  ui->actCompView->setEnabled(flag);
  ui->actMagDisp->setEnabled(flag);
  ui->actNumAtom->setEnabled(flag);
  ui->actNoComp->setEnabled(flag);
  ui->actEdgeComp->setEnabled(flag);
  ui->actScrewComp->setEnabled(flag);
  ui->actDAtLayers->setEnabled(flag);
  ui->actDAtTypes->setEnabled(flag);
  ui->actDAtNeighbors->setEnabled(flag);
  ui->actInertAtoms->setEnabled(flag);
  ui->actCalcRDF->setEnabled(flag);
  ui->actProbeDispl->setEnabled(flag);
  ui->actBurgVect->setEnabled(flag);
  ui->actDCenter->setEnabled(flag);
  ui->actSPlanes->setEnabled(flag);
  ui->actShowCSys->setEnabled(flag);
  ui->actShowCells->setEnabled(flag);
  ui->actShowGb->setEnabled(flag);
  ui->actRelaxInfo->setEnabled(flag);

  ui->actShowZLay->setEnabled(flag);
  ui->actSetZLay->setEnabled(flag);
  ui->actZLayUp->setEnabled(flag);
  ui->actZLayDown->setEnabled(flag);
  ui->actCoordAt->setEnabled(flag);

  ui->actCoordSys->setEnabled(flag);
  ui->actXZReflect->setEnabled(flag);
  ui->actYZReflect->setEnabled(flag);
  ui->actXYReflect->setEnabled(flag);
  ui->actXRot180->setEnabled(flag);
  ui->actYRot180->setEnabled(flag);
  ui->actZRot180->setEnabled(flag);
  ui->actXRot90->setEnabled(flag);
  ui->actYRot90->setEnabled(flag);
  ui->actZRot90->setEnabled(flag);
  ui->actTransform->setEnabled(flag);
  ui->actProjVect->setEnabled(flag);
  ui->actCompDisp->setEnabled(flag);

  ui->actLookAndFeel->setEnabled(flag);
  ui->actCrystPlot->setEnabled(flag);
  ui->actArrPlot->setEnabled(flag);
  ui->actScalePos->setEnabled(flag);
  ui->actScaleArr->setEnabled(flag);
  ui->actArrNeigh->setEnabled(flag);
  ui->actSetZTol->setEnabled(flag);

  ui->actBurgCircuit->setEnabled(flag);
  ui->actDPosPeierlsNabarro->setEnabled(flag);
  ui->actFindDisloSites->setEnabled(flag);
  ui->actDPosDecimate->setEnabled(flag);

  ui->actApplyYDiad->setEnabled(flag);
  ui->actCorrEdgeBCC->setEnabled(flag);

  #ifndef EXPERIMENTAL
    ui->actDPosDecimate->setVisible(false);
  #endif
}
