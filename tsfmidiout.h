#ifndef TSFMIDIOUT_H
#define TSFMIDIOUT_H
#include <QAudioOutput>
#include <QIODevice>
#include <guitarfreqanalysis.h>
#include "/home/oriol/sdcard/Projectes/extern/tinysoundfont/TinySoundFont/tsf.h"
#include <QObject>
#include <QTimer>

class TSFMidiOut : public QObject,public SystemNotifyFreqs
{
public:
    qint16 towrite[AudioBuff::BUFF_SIZE];
protected:

    QTimer *timer;
    tsf* g_TinySoundFont;
    unifiedint nSample   ;
    unifiedint nBuffer;
//    qint16 buffer[AudioBuff::BUFF_SIZE];

 //   QIODevice *   audiodev;

    int MaxPolyNotes;

Q_OBJECT

public:
    TSFMidiOut();
    virtual ~TSFMidiOut() override;
    void Init(QIODevice& dev);
    virtual void Notify(FreqEvent e) override;
 //   void ProcessAudio();

//public slots:
   int renderaudio(int maxlen);

};
#endif // TSFMIDIOUT_H
