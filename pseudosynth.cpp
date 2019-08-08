#include "pseudosynth.h"
#include <QAudioDeviceInfo>
#include <QAudioOutput>
#include <QIODevice>
#include <QDebug>
#include <math.h>
//#include <QVBoxLayout>
//#include <../../../../extern/QMidi-master/src/QMidiOut.h>

//static QMidiOut midio;
PseudoSynth::PseudoSynth()
{


}
void PseudoSynth::processaudio()
{
    qint16 buffer[AudioBuff::BUFF_SIZE];
    bool cont=true;
    while(cont)
    {

        for(nSample=0;nSample<AudioBuff::BUFF_SIZE;nSample++)
        {
            buffer[nSample]=CalcSample(nSample,nBuffer);
        }
//        audiodev->waitForReadyRead(20);
        audiodev->write((char *)buffer,nSample*2);


        nBuffer++;
        cont=false;
     }
    cont=audiodev->waitForBytesWritten(20);
};

void PseudoSynth::startSynth()
{
    QThread::start();

};
 void PseudoSynth::Init()
 {
     QAudioFormat format;
     format.setSampleRate(AudioBuff::SAMPLINGRATE);
     format.setChannelCount(1);
     format.setSampleSize(16);
     format.setCodec("audio/pcm");
     format.setByteOrder(QAudioFormat::LittleEndian);
     format.setSampleType(QAudioFormat::SignedInt);

     audioout=new QAudioOutput(format);
     audioout->setBufferSize(AudioBuff::BUFF_SIZE*2);
     audiodev=audioout->start();
 };

void PseudoSynth::run()
{
    Init();
    while (true) {
        processaudio();

    }




}
qint16 PseudoSynth::CalcSample(unifiedint nSample,unifiedint nBuffer)
{
    //unsigned long long TotalnSample=nBuffer*AudioBuff::BUFF_SIZE;
    //unsigned long long nSampleRel=

    std::vector<FreqEvent>::iterator pn=NotesOn.begin();
    qint16 sample=0;
    double dsum=0;
    while(!(pn==NotesOn.end()))
    {
        if((*pn).nBuffer>nBuffer)
            continue;
        double nsamplenota=((double)nBuffer-(double)pn->nBuffer)*(double)AudioBuff::BUFF_SIZE+(double)nSample;
        double nsec=(double)nsamplenota/(double)AudioBuff::SAMPLINGRATE;
        double hznota=440*(double)pow(2.0,(69.0-(double)(*pn).note)/12.0);

        double vnota=(double)128.0*(double)pn->velocity*(double)sin(hznota*nsec);
        dsum+=vnota;
        pn++;
    };
    sample=std::round(dsum);
    return sample;

}

void PseudoSynth::Notify(FreqEvent f)
{
   SystemNotifyFreqs::Notify(f);

   switch(f.eventtype)
   {
       case FreqEvent::NoteOn:
        if(NotesOn.size()<6)
           NotesOn.push_back(f);
          break;
       case FreqEvent::NoteOff:
            std::vector<FreqEvent>::iterator p=NotesOn.begin();
            while(p!=NotesOn.end())
            {
                if((*p).eventtype==FreqEvent::NoteOn && (*p).note==f.note)
                {
                    NotesOn.erase(p);
                    break;
                }
            p++;
            }
            break;
    }


}
