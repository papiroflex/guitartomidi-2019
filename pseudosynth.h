#ifndef PSEUDOSYNTH_H
#define PSEUDOSYNTH_H
#include "guitarfreqanalysis.h"
#include <QAudioOutput>
#include <QThread>




class PseudoSynth : public SystemNotifyFreqs, public QThread
{

    QIODevice *   audiodev=nullptr;
    QAudioOutput *audioout=nullptr;
    int MaxPolyNotes=6;
    unifiedint nSample=0;
    unifiedint nBuffer=0;
    bool MidiOut=true;
//    QMidiOut midio;
public:

    PseudoSynth();
    void Init();
    virtual void Notify(FreqEvent f);
    virtual void startSynth();
    void processaudio();
    std::vector<FreqEvent> NotesOn;
protected:
    virtual void run();
    qint16 CalcSample(unifiedint nSample, unifiedint nBuffer);




};

#endif // PSEUDOSYNTH_H
