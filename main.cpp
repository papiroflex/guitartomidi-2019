#include <QApplication>
#include <QObject>
#include <QMediaPlayerControl>
#include "qt5audioinputinterface.h"
#include "guitarfreqanalysis.h"
//#include <pseudosynth.h>
#include <ios>
//#include "midioutnotifyfreqs.h"
#include "tsfmidiout.h"
#include "gtmframe.h"
#include "../ObjectVisualiser/audiobuffvisualiser.h"
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
using namespace std;

int main(int argc, char *argv[])
{


    //QCoreApplication a(argc, argv);
    QApplication a(argc, argv);
    //QTimer actualitzar(&a);
    //actualitzar.setInterval(20);

    TSFMidiOut mof;
    //    PseudoSynth ps;

    cout<<"constructing Frequency analizer"<<endl;

    Qt5AudioInputInterface ai(&mof);

    gtmFrame f(nullptr,&ai);
    Qtg2mSystem sysinter(&f);
    sysinter.SetStatus("Starting...");

     f.show();
    QtCharts::QChart vchart;
    QtCharts::QChartView chv(&vchart);
    chv.size().rwidth()=800;
    chv.size().rheight()=450;
    sysinter.SetStatus("starting Guitar Freq Analyzer");
    GuitarFreqAnalysis gfa((AudioInputInterf *)&ai,&mof);
    ai.gfa=&gfa;
    sysinter.SetStatus("Initializing charts");
    AudioBuffVisualiser visu(&f,ai.gfa->getAudioBuff(),&vchart,gfa.NoteStatus,&gfa);
    chv.show();
    vchart.show();
    //gtmFrame::connect(f,SIGNAL(clicked()),&visu,SLOT(UpdateShow()));
#define GTM_CHART
#ifdef GTM_CHART
    GuitarFreqAnalysis::connect(&gfa,SIGNAL(NewEventsnPause()),&visu,SLOT(UpdateShow()));

    GuitarFreqAnalysis::connect(&gfa,SIGNAL(NewEventsnPause()),&ai,SLOT(Pause()));
    GuitarFreqAnalysis::connect(&gfa,SIGNAL(Pause()),&visu,SLOT(UpdateShow()));
    GuitarFreqAnalysis::connect(&gfa,SIGNAL(Pause()),&ai,SLOT(Pause()));
    //GuitarFreqAnalysis::connect(&gfa,SIGNAL(Pause()),&visu,SLOT(UpdateShow()));

  //  GuitarFreqAnalysis::connect(&gfa,SIGNAL(BuffProcessed()),&visu,SLOT(UpdateShow()));
//      GuitarFreqAnalysis::connect(&gfa,SIGNAL(NewEventsnPause()),&visu,SLOT(UpdateShow()));
//    GuitarFreqAnalysis::connect(&gfa,SIGNAL(NewEvents()),&visu,SLOT(UpdateShow()));
    sysinter.SetStatus("Starting audio play and record...");
    std::cout<<"starting audio recorder"<<endl;
//    mof.start();
#endif
     //(gfa.Start("/home/oriol/Documents/guitarrasolagiu2mid.wav");
    gfa.Start("/home/oriol/Documents/prova-aire-gtm-guitarra.wav");
    ai.Pause();

    return a.exec();
}
//#include "main.moc"
