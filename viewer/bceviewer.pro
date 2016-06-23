QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.11
QMAKE_MAC_SDK = macosx10.11

QT += core gui printsupport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += release
CONFIG += -std=gnu++11
CONFIG += WARN_OFF
CONFIG += debug

QMAKE_CXXFLAGS += -std=gnu++11
QMAKE_CXXFLAGS += -m64 -lm -lpthread  -O0 -O -fPIC -fexceptions -DNDEBUG -DIL_STD 
QMAKE_LFLAGS += -m64 -lm -lpthread  -m64 -O -fPIC -fexceptions -DNDEBUG -DIL_STD

INCLUDEPATH += /usr/local/include \
/usr/local/lib \
../src/cpp \
../src/hpp \
../viewer/cpp \
../viewer/hpp \
../examples/hpp \
/usr/include/boost

VPATH += ../src/cpp \
../src/hpp \
/usr/local/include \
/usr/local/lib \
/usr/lib/x86_64-linux-gnu \
../viewer/cpp \
../viewer/hpp \
/usr/include/boost

HEADERS += bcedatastate.hpp \
bceplothandler.hpp \
qcustomplot.h \
bcesolution.hpp \
bcegame.hpp \
bce.hpp \
bcedistr.hpp \
bceabstractgame.hpp \
bceslider.hpp \
bcelineedit.hpp \
bcevaluesetplot.hpp \
bceenumeration.hpp \
bcelabel.hpp \
bcegamehandler.hpp \
bcepayofftablemodel.hpp \
bcetableview.hpp \
bcetablemodel.hpp \
bcepriortablemodel.hpp \
bceconditionaltablemodel.hpp \
bcepushbutton.hpp \
bcewindow.hpp \
bcelogstream.hpp \
bceloghandler.hpp \
bceobjweightstablemodel.hpp \
bcesolverworker.hpp \
bcegurobicallback.hpp \
bceplottitle.hpp

SOURCES += main.cpp \
bceplothandler.cpp \
qcustomplot.cpp \
bcedatastate.cpp \
bcegamehandler.cpp \
bcepayofftablemodel.cpp \
bcepriortablemodel.cpp \
bceconditionaltablemodel.cpp \
bcewindow.cpp \
bceobjweightstablemodel.cpp \
bcesolver.cpp \
bcelabel.cpp \
bceplottitle.cpp

LIBS += ../lib/libbce.a

include(../localviewersettings.mk)

linux-g++ {
LIBS +=  -Bstatic -L../lib -Wl,-Bstatic  -L/usr/lib/x86_64-linux-gnu/ -lboost_serialization -lboost_system -Wl,-Bdynamic
}
macx {
LIBS +=
}

LIBS += -L/usr/lib/x86_64-linux-gnu/ -L/usr/local/lib -lboost_serialization -lboost_system

LIBS += -L../lib/ -L/usr/local/lib/ -L/usr/lib/x86_64-linux-gnu


MOC_DIR = ./tmp
OBJECTS_DIR = ./tmp

TEMPLATE = app
TARGET = bceviewer

