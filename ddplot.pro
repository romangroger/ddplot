message("Compiling ddplot...")
unix { message("platform: Unix") }
linux-g++-32 { message("platform: linux i386 with g++") }
linux-g++-64 { message("platform: Linux x86_64 with g++") }
win32-g++ { message("platform: Windows 32bit with g++") }
win64-g++ { message("platform: Windows 64bit with g++") }
macx { message("platform: Mac OS X") }

TEMPLATE = app
VPATH = src
OBJECTS_DIR = src
MOC_DIR = src
RCC_DIR = src
UI_DIR = src
DESTDIR =
TARGET = ddplot
CONFIG += qt warn_off #release
DEFINES += #EXPERIMENTAL
QT = core gui
QT += widgets printsupport
#QMAKE_LFLAGS += -Xlinker -Bstatic -L/usr/local/Qt-5.14.2/lib -L/usr/lib/gcc/x86_64-linux-gnu/5/libgcc_s -lgcc_s
RESOURCES = ddplot.qrc

#win32 {
#  CONFIG += console
#  RC_FILE = ddplot.rc
#}
#
#macx {
#  ICON = ddplot.icns
#}

#----------------------------------------------------------------------------------------------------

HEADERS = animate.h atomstyle.h bvect.h colormap.h cutpoly.h ddplot.h dpos_decimate.h dpos_peierlsnabarro.h	\
	fformat.h getatomchain.h messages.h appwin.h dpospnresults.h lagrpoly.h matinv.h misc.h		\
	neighlist.h pltwin.h rdf.h qcustomplot.h scalepos.h selectplots.h selectzlayers.h calcs.h     \
	cline.h darray.h lookandfeel.h probeatomdispl.h rdfplot.h test.h

SOURCES = animate.cpp atomstyle.cpp bvect.cpp colormap.cpp cutpoly.cpp ddplot.cpp	\
	dpos_decimate.cpp dpos_peierlsnabarro.cpp dpospnresults.cpp fformat.cpp getatomchain.cpp lagrpoly.cpp \
	messages.cpp appwin.cpp appwin-init.cpp appwin-misc.cpp appwin-menu.cpp appwin-file.cpp appwin-plot.cpp appwin-view.cpp appwin-layers.cpp \
        appwin-transforms.cpp appwin-calculations.cpp appwin-specials.cpp appwin-appearance.cpp appwin-help.cpp matinv.cpp misc.cpp neighlist.cpp pltwin.cpp	\
	pltwin-calc.cpp pltwin-geom.cpp pltwin-func.cpp pltwin-load.cpp pltwin-misc.cpp pltwin-render.cpp rdf.cpp qcustomplot.cpp scalepos.cpp	\
	selectplots.cpp selectzlayers.cpp calcs.cpp cline.cpp darray.cpp lookandfeel.cpp rdfplot.cpp test.cpp probeatomdispl.cpp

FORMS =  ui/dabout.ui ui/appwin.ui ui/dburgvect.ui ui/dcalcbvector.ui ui/ddposburgcircuitresults.ui ui/dfinddposdecimate.ui ui/drdfplot.ui ui/ddefinebcircuit.ui	        \
	ui/ddpospnresults.ui ui/ddefdislopos.ui	\
	ui/dfinddpospeierlsnabarro.ui ui/dprojvector.ui ui/dscalearrow.ui ui/datomstyle.ui		\
	ui/darrowstyle.ui ui/dselectplots.ui ui/danimate.ui ui/dshowplanes.ui ui/darrneighbors.ui	\
	ui/dcoordsys.ui ui/dlookandfeel.ui ui/dtransform.ui ui/drelaxinfo.ui ui/dscalepos.ui		\
	ui/dselectzlayers.ui ui/dsetupexportvtk.ui ui/dnewcutoff.ui ui/dcomparedisp.ui ui/dcoordatoms.ui			\
	ui/dztolerance.ui ui/dsetupprobeatomdispl.ui ui/dprobeatomdispl.ui
