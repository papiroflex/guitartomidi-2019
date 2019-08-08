#define TSF_IMPLEMENTATION
//#include "/home/oriol/sdcard/Projectes/extern/tinysoundfont/TinySoundFont/tsf.h"
#include <QObject>
#include <QAudioFormat>
#include "tsfmidiout.h"
#include "qt5audioinputinterface.h"
#include <algorithm>

const static unsigned char MinimalSoundFont[] =
{
    #define TEN0 0,0,0,0,0,0,0,0,0,0
    'R','I','F','F',220,1,0,0,'s','f','b','k',
    'L','I','S','T',88,1,0,0,'p','d','t','a',
    'p','h','d','r',76,TEN0,TEN0,TEN0,TEN0,0,0,0,0,TEN0,0,0,0,0,0,0,0,255,0,255,0,1,TEN0,0,0,0,
    'p','b','a','g',8,0,0,0,0,0,0,0,1,0,0,0,'p','m','o','d',10,TEN0,0,0,0,'p','g','e','n',8,0,0,0,41,0,0,0,0,0,0,0,
    'i','n','s','t',44,TEN0,TEN0,0,0,0,0,0,0,0,0,TEN0,0,0,0,0,0,0,0,1,0,
    'i','b','a','g',8,0,0,0,0,0,0,0,2,0,0,0,'i','m','o','d',10,TEN0,0,0,0,
    'i','g','e','n',12,0,0,0,54,0,1,0,53,0,0,0,0,0,0,0,
    's','h','d','r',92,TEN0,TEN0,0,0,0,0,0,0,0,50,0,0,0,0,0,0,0,49,0,0,0,34,86,0,0,60,0,0,0,1,TEN0,TEN0,TEN0,TEN0,0,0,0,0,0,0,0,
    'L','I','S','T',112,0,0,0,'s','d','t','a','s','m','p','l',100,0,0,0,86,0,119,3,31,7,147,10,43,14,169,17,58,21,189,24,73,28,204,31,73,35,249,38,46,42,71,46,250,48,150,53,242,55,126,60,151,63,108,66,126,72,207,
        70,86,83,100,72,74,100,163,39,241,163,59,175,59,179,9,179,134,187,6,186,2,194,5,194,15,200,6,202,96,206,159,209,35,213,213,216,45,220,221,223,76,227,221,230,91,234,242,237,105,241,8,245,118,248,32,252




};

/*void TSFMidiOut::Notify()
{
    std::cout<<"n";
    rendernwriteaudio();
}*/

void TSFMidiOut::Init(QIODevice& audiodev)
{
#if 0
    QAudioFormat format;
    format.setSampleRate(AudioBuff::SAMPLINGRATE);
    format.setChannelCount(1);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);
//    audioout=new ;
//    audioout=new QAudioOutput(format);
    //audioout->setBufferSize(AudioBuff::BUFF_SIZE*2);



    //audioout->start(&audiodev);
    //audioout->setNotifyInterval(20);
    //connect(audioout,SIGNAL(notify()),this,SLOT(rendernwriteaudio()));
//    timer=new QTimer();
//    timer->setInterval(20);
 //   connect(audiodev,SIGNAL(bytesWritten(AudioBuff::BUFF_SIZE*2)),this,SLOT(rendernwriteaudio()));
//    connect(this,SIGNAL(Notify),this,SLOT(rendernwriteaudio));
#endif
};


TSFMidiOut::TSFMidiOut()
{
    timer=nullptr;
    nSample=0   ;
    nBuffer=0;
//    audiodev=nullptr;
//    audioout=nullptr;
    MaxPolyNotes=24;
    //tsf* TinySoundFont = tsf_load_filename("soundfont.sf2");
    //g_TinySoundFont = tsf_load_memory(MinimalSoundFont, sizeof(MinimalSoundFont));
    g_TinySoundFont=tsf_load_filename("/usr/share/sounds/sf2/FluidR3_GM.sf2");
    //g_TinySoundFont=tsf_load_filename("/usr/share/sounds/sf2/FluidR3_GS.sf2");
//    g_TinySoundFont=tsf_load_filename("/home/oriol/Baixades/florestan-subset.sf2");


//    audioout=this;
    if (!g_TinySoundFont)
        {
            std::cerr<<"Could not load soundfont\n";
            exit(-1);
            return;
        }
    ;
    tsf_set_output(g_TinySoundFont, TSF_MONO, AudioBuff::SAMPLINGRATE, 0); //sample rate
    tsf_channel_set_presetindex(g_TinySoundFont,0,29);
}
void TSFMidiOut::Notify(FreqEvent e)
{
    SystemNotifyFreqs::Notify(e);

    switch (e.eventtype) {
        case FreqEvent::NoteOn:
            tsf_note_on(g_TinySoundFont,29,e.note,(double)std::min(0.05,(double)e.velocity/(double)128.0));



        break;
        case FreqEvent::NoteOff:
          tsf_note_off(g_TinySoundFont,29,e.note);

        break;


    }
}
#if 0
void TSFMidiOut::ProcessAudio()
{

    bool cont=true;
    audiodev->waitForBytesWritten(20);
    while(cont)
    {
        renderaudio();

    }
    cont=false;
}
#endif
int TSFMidiOut::renderaudio(int maxlen)
{

    short buffer[AudioBuff::BUFF_SIZE];
    maxlen=std::min((int)maxlen,(int)AudioBuff::BUFF_SIZE*2);
    tsf_render_short(g_TinySoundFont, buffer, maxlen/2, 0);
//    std::cout<<"w";
//    std::cout.flush();
    qint16 max=0;
    for(int nsh=0;nsh<AudioBuff::BUFF_SIZE;nsh++)
    {
        towrite[nsh]=buffer[nsh];
        max=std::max(max,buffer[nsh]);
    }
    if(max>26000)
        std::cout<<"Max:"<<max;
//    audiodev->write((char *)towrite,AudioBuff::BUFF_SIZE*2);


    nBuffer++;
    //cont=false;
    //cont=!audiodev->waitForBytesWritten(20);
    return maxlen;
}


TSFMidiOut::~TSFMidiOut()
{

};

/*void TSFMidiOut::Notify()
{

}*/
