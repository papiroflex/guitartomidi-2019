#include "qt5audioinputinterface.h"
#include <QAudioFormat>
#include <QDebug>
#include <math.h>
#include <ios>
#include<iostream>
#include <algorithm>
#include <QtEndian>
#include <QObject>
#include <guitarfreqanalysis.h>
#include <qmath.h>
#include <systemabstract.h>


Qt5AudioInputInterface::Qt5AudioInputInterface(TSFMidiOut *mo):AudioInputInterf()
{
   midiout=mo;
}
 void Qt5AudioInputInterface::MediaPlay(bool avail)
{
    if(avail && !dev->isOpen())
        dev->start();
  //  mediaplay->play();

};
void Qt5AudioInputInterface::Start(const char* src)
{

    // Set up the desired format, for example:
    QAudioFormat format;
    format.setSampleRate(AudioBuff::SAMPLINGRATE);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(format)) {
        std::cerr<<"Default format not supported, trying to use the nearest.";
        qWarning() << "Default format not supported, trying to use the nearest.";
        format = info.nearestFormat(format);
        exit(-1);
    }
    //mediadev=new QIODevice();

    dev=new Qt5AudioInputDevice(format);
    dev->interf=this;
    mediaplay=new QMediaPlayer(this,QMediaPlayer::LowLatency);
    //playmonitor.setSource(mediaplay);
    /*if(audio!=nullptr)
    {
        audio->stop();
        delete audio;
    }*/

    audioout=new QAudioOutput(format);


    if(src!=nullptr)
        mediaplay->setMedia(QUrl::fromLocalFile(src));



    //audioout->start(dev);
    if(src!=nullptr && !mediaplay->media().isNull())
    {
         playmonitor.setSource(mediaplay);
         mediaplay->

        connect(mediaplay, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
        connect(mediaplay,SIGNAL(audioAvailableChanged(bool)),this,SLOT(MediaPlay(bool)));
        connect(mediaplay,SIGNAL(stateChanged(QMediaPlayer::State)),this,SLOT(mediaStateChanged(QMediaPlayer::State)));
        connect(&playmonitor,SIGNAL(audioBufferProbed(const QAudioBuffer&)),this,SLOT(processBuffer(const QAudioBuffer&)));

        Play();
     //   mediaplay->play();


    }else
    {
        audioin= new QAudioInput(format);
        audioin->setBufferSize(AudioBuff::BUFF_SIZE*2);

        audioin->start(dev);

    }
    dev->start();
    audioout->start(dev);

 //   connect(dev,SIGNAL(update()),this,SLOT(Step()));

    //timer20.setInterval(20);
    //timer20.start(20);
    //connect(&timer20,SIGNAL(timeout()),this,SLOT(Timer20ms()));
}

void Qt5AudioInputInterface::Timer20ms()
{
//    std::cout<<"t";
//    std::cout.flush();
   /* if(mediaplay->state()!=QMediaPlayer::State::PlayingState)
        mediaplay->play();*/
    //playmonitor.flush();
    //emit playmonitor.flush();
};

void Qt5AudioInputInterface::mediaStateChanged(QMediaPlayer::State state)
{
    if(state!=QMediaPlayer::State::PlayingState)
    {
        std::cout<<"restarting media play"<<endl;

 //       mediaplay->setPosition(0);
 //       mediaplay->play();
    }
    else {
    /*    if(state==QMediaPlayer::State::PlayingState)
           playmonitor.setSource(mediaplay);
      */ // playmonitor.flush();
    }

}

void Qt5AudioInputInterface::positionChanged(qint64 nwpos)
{

    //emit playmonitor.flush();
};




void Qt5AudioInputInterface::processBuffer(const QAudioBuffer& buff)
{
    //std::cout<<'b';
    //std::cout.flush();
    QAudioFormat bf=buff.format();//bf.byteOrder()????
//    std::cout<<"codec:"<<(char *)bf.codec().toUtf8().data()<<" sample rate:"<<bf.sampleRate()<<" samplesize:"<<bf.sampleSize()<<" "<<"signed int:?"<<(bf.sampleType()==QAudioFormat::SignedInt);
//    std::cout<<std::endl;
    assert(buff.isValid() && bf.codec()==QString("audio/pcm") && bf.sampleRate()==48000 && bf.sampleSize()==16 && bf.sampleType()==QAudioFormat::SignedInt && bf.channelCount()==1);
    dev->writeData((char *)(buff.data()),buff.byteCount());


}





bool Qt5AudioInputInterface::BufferReady()
{

//std::cout<<"BufferReady will return "<<(dev->buffswritten.size()>=1);
    return dev->buffswritten.size()>=1;
}
int Qt5AudioInputInterface::ReadBuff(AudioBuff& ret)
{
    if(!BufferReady())
    {
        //emit playmonitor.flush();
        std::cout<<"-";
        std::cout.flush();
        return 0;
    }
    unifiedint readed=0;

    AudioBuff *fifobuf=dev->buffswritten.front();
    dev->buffswritten.pop_front();

    readed=fifobuf->nSamples;
//    ret.ZeroBuff();
    memcpy(ret.data,(*fifobuf).data,(size_t)fifobuf->nSamples*2);
    memcpy(ret.GetInt16Array(),(*fifobuf).data,(size_t)fifobuf->nSamples*2);
    ret.nSamples=fifobuf->nSamples;
    ret.nBuffer=fifobuf->nBuffer;

    //ret.CalcVariables();

    delete fifobuf;

    assert(sizeof(ret.data)==AudioBuff::BUFF_SIZE*2);

    return (int)readed;

}




bool Qt5AudioInputInterface::AudioInputMedia(QUrl ur)
{
    bool ret=true;
    mediaplay->setMedia(ur);
    return ret;
};


Qt5AudioInputInterface::~Qt5AudioInputInterface()
{
    dev->stop();
    if(audioin!=nullptr){
        audioin->stop();
        delete audioin;
    }
    audioout->stop();

    delete audioout;
    delete dev;

};


void Qt5AudioInputDevice::start()
{
    bool ReadOnly=!interf->mediaplay->media().isNull();
    bool opened;
    if(ReadOnly)
        opened=open(QIODevice::ReadOnly);
    else {
        opened=open(QIODevice::ReadWrite);
    }
    if(!opened)
        std::cerr<<"Error opening audio input";
    connect(this,SIGNAL(update()),interf->gfa,SLOT(Step()));
}

void Qt5AudioInputDevice::stop()
{
    close();
}

qint64 Qt5AudioInputDevice::readData(char *data, qint64 maxlen)
{

    maxlen=interf->midiout->renderaudio(maxlen);
    memcpy(data,interf->midiout->towrite,maxlen);
    std::cout<<"r";
    return maxlen;
}

qint64 Qt5AudioInputDevice::writeData(const char *data, qint64 len)
{
    unifiedint OrigSampleNum16=0;
    unifiedint DestSampleNum16=0;
    size_t DestNCopy=0;
    unifiedint MaxBuffWrite=2*AudioBuff::BUFF_SIZE;


    for(int nbuff=0;nbuff<=len/(2*AudioBuff::BUFF_SIZE);nbuff++)
    {
        if(curbuff==nullptr)
        {
            curbuff=new AudioBuff();
            curbuff->nSamples=0;

            curbuff->ZeroBuff();
            curbuff->nBuffer=iBuff++;
            OrigSampleNum16=nbuff*AudioBuff::BUFF_SIZE;
            DestSampleNum16=0;
            DestNCopy=AudioBuff::BUFF_SIZE*2;
            MaxBuffWrite=std::min((unifiedint)DestNCopy,len-OrigSampleNum16*2);
        }
        else {


            DestSampleNum16=curbuff->nSamples;
            OrigSampleNum16=nbuff*AudioBuff::BUFF_SIZE+curbuff->nSamples;


            DestNCopy=std::min(std::min(AudioBuff::BUFF_SIZE*2,len-nbuff*AudioBuff::BUFF_SIZE*2),AudioBuff::BUFF_SIZE*2-DestSampleNum16*2);
            DestNCopy=std::max((size_t)0,DestNCopy);
            MaxBuffWrite=std::min((unifiedint)DestNCopy+(unifiedint)DestSampleNum16*2,(unifiedint)len-(unifiedint)OrigSampleNum16*2+DestSampleNum16*2);
        }
        if(DestNCopy>0)
            memcpy(curbuff->data+DestSampleNum16*2,data+OrigSampleNum16*2,(size_t)DestNCopy);

        curbuff->nSamples=MaxBuffWrite/2;
        assert(curbuff->nSamples<=AudioBuff::BUFF_SIZE);
        if(curbuff->nSamples==AudioBuff::BUFF_SIZE)
        {
            buffswritten.push_back(curbuff);
            curbuff=nullptr;
        }
     }
    //std::cout<<".";






    std::cout<<"w";
    std::cout.flush();

    if(interf->BufferReady())
        emit update();
    return len;
}




Qt5AudioInputDevice::Qt5AudioInputDevice(const QAudioFormat &format):QIODevice ()
{
    m_format=format;

};

void Qt5AudioInputInterface::Pause()
{
    if((gfa->NewEventsPause && gfa->ChangedEvents) || !gfa->NewEventsPause)
    {
        mediaplay->pause();
        audioout->stop();
        gfa->NewEventsPause=false;
        SystemAbstract::SysI().SetStatus("Paused");

    }

}

void Qt5AudioInputInterface::Play()
{
    mediaplay->play();
    if(!dev->isOpen())
        dev->start();
     audioout->start(dev);
     SystemAbstract::SysI().SetStatus("Playing");
};

void Qt5AudioInputInterface::DoOneStep()
{
    OneStep=true;
    gfa->NewEventsPause=true;

    mediaplay->play();
    if(!dev->isOpen())
        dev->start();
};


