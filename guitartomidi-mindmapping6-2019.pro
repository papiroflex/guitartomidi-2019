QT += gui
QT += multimedia
QT += core
QT += widgets
QT += charts
QT += sql
CONFIG += c++11 console
CONFIG += app_bundle

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        ../../../../extern/QMidi-master/src/QMidiFile.cpp \
        ../../../../extern/QMidi-master/src/QMidiOut.cpp \
        ../../../../extern/QMidi/QMidi/src/OS/QMidi_ALSA.cpp \
        ../ObjectVisualiser/audiobuffvisualiser.cpp \
        ../ObjectVisualiser/basetypeobject.cpp \
        audioinputinterf.cpp \
        gtmframe.cpp \
        gtmtoolbox.cpp \
        guitarfreqanalysis.cpp \
        main.cpp \
        midioutnotifyfreqs.cpp \
        pseudosynth.cpp \
        qt5audioinputinterface.cpp \
        qt5mediaplayercontrolgtm.cpp \
        qtsqlmanager.cpp \
        systemabstract.cpp \
        tsfmidiout.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    ../../../../../../Baixades/Guitar to midi.mind \
    ../../../../../../Baixades/Guitar to midi.pdf \
    ../../../../../../Baixades/Guitar to midi.pdf \
    ../../../../../../Baixades/guitartomidi 2, freq analysis.mind \
    ../../../../../../Baixades/guitartomidi 2, freq analysis.mind \
    ../../../../../../Documents/guitarrasolagiu2mid.wav \
    gtmcreatedb.sqlite3

HEADERS += \
    ../../../../extern/QMidi-master/src/QMidiFile.h \
    ../../../../extern/QMidi-master/src/QMidiOut.h \
    ../ObjectVisualiser/audiobuffvisualiser.h \
    ../ObjectVisualiser/basetypeobject.h \
    audioinputinterf.h \
    gtmframe.h \
    gtmtoolbox.h \
    guitarfreqanalysis.h \
    midioutnotifyfreqs.h \
    pseudosynth.h \
    qt5audioinputinterface.h \
    qt5mediaplayercontrolgtm.h \
    qtsqlmanager.h \
    systemabstract.h \
    tsfmidiout.h

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += alsa

FORMS += \
    gtmframe.ui \
    gtmtoolbox.ui
