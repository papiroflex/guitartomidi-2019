#ifndef QT5AUDIOINPUTINTERFACE_H
#define QT5AUDIOINPUTINTERFACE_H
#include <list>

#include <QObject>
#include <QAudioInput>
#include <QAudioOutput>
#include <QIODevice>
#include <QMediaPlayer>
#include <QAudioProbe>
#include "tsfmidiout.h"

#include "audioinputinterf.h"
//#include "guitarfreqanalysis.h"

class Qt5AudioInputInterface;


class Qt5AudioInputDevice: public QIODevice
{
//    unsigned nBuffers
    Q_OBJECT
protected:
    friend class Qt5AudioInputInterface;

    unifiedint iBuff=0;
    static const unifiedint MAX_BUFFS=8;
    AudioBuff *curbuff=nullptr;
//    bool BuffReaded=false;
//    char DataWritten[AudioBuff::BUFF_SIZE*2*MAX_BUFFS];
//    GuitarFreqAnalysis *gfa=nullptr;
    public:
    Qt5AudioInputInterface *interf=nullptr;

    std::list<AudioBuff *> buffswritten;

    Qt5AudioInputDevice(const QAudioFormat &format);

    void start();
    void stop();

    qreal level() const { return m_level; }

    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;

private:
    void generateData(const QAudioFormat &format, qint64 durationUs, int sampleRate);
    QAudioFormat m_format;
    quint32 m_maxAmplitude = 0;
    qreal m_level = 0.5; // 0.0 <= m_level <= 1.0


signals:
    void update();
public slots :
//public slots:



};








class Qt5AudioInputInterface : public QObject,public AudioInputInterf
{

    Q_OBJECT
public:
    QIODevice *mediadev=nullptr;
    QAudioProbe playmonitor;
    QTimer timer20;
    Qt5AudioInputDevice *dev=nullptr;
    QAudioInput *audioin=nullptr;
    QAudioOutput *audioout=nullptr;
    TSFMidiOut *midiout;
    QMediaPlayer *mediaplay;
    const QIODevice *mediastream;
    bool AudioInputMedia(QUrl ur);
//   GuitarFreqAnalysis *gfa=nullptr;
public:
    friend class Qt5AudioInputDevice;
    Qt5AudioInputInterface(TSFMidiOut *midiout);
    virtual void Start(const char* src) override;

    friend class AudioBuff;
    virtual int ReadBuff(AudioBuff& ret) override;
    virtual bool BufferReady() override;
    virtual ~Qt5AudioInputInterface() override;
    bool isRunning(){return dev!=nullptr;}
    QMediaPlayer& getMediaPlayer(){return *mediaplay;}

    public slots:
    void MediaPlay(bool avail);
    void processBuffer(const QAudioBuffer& buff);

    void mediaStateChanged(QMediaPlayer::State state);

    void positionChanged(qint64 nwpos);
    void Timer20ms();


public slots:
    void Pause();
    void Play();
    void DoOneStep();

};







#endif // QT5AUDIOINPUTINTERFACE_H
