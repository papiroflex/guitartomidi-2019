#ifndef GUITARFREQANALYSIS_H
#define GUITARFREQANALYSIS_H

#include "audioinputinterf.h"
#include <time.h>
#include <iostream>
#include <vector>
#include <QObject>
class AndCondition;
class OrCondition;
class  Condition
{

public:
    OrCondition& operator || (Condition& other);
    AndCondition& operator && (Condition& other);
    virtual bool IsTrue()=0;
    operator bool(){return IsTrue();}
    virtual ~Condition(){}
};
class AndCondition:public Condition
{
    Condition *a,*b;
public:
    AndCondition(Condition *ca,Condition *cb){a=ca;b=cb;}

    bool IsTrue()  override;
    ~AndCondition() {}

};
class OrCondition:public Condition
{
    Condition *a,*b;
public:
    OrCondition(Condition *ac,Condition *bc){a=ac;b=bc;}
    bool IsTrue() override;

};

class NoteCondition :public Condition{
public:

    NoteCondition(GuitarFreqAnalysis& gfa,SegmentDeFrequencies* cursg,SegmentDeFrequencies *prevsg,int freq,double velocity,int note,bool Precis);
    GuitarFreqAnalysis* gfa;
    SegmentDeFrequencies* curseg;
    SegmentDeFrequencies* prevseg;
    int nfreq;
    double velocity;
    unsigned nnote;
    bool Precis=false;
    //virtual bool IsTrue() override;
    Condition *rootCondition=nullptr;
};
class IsPreciseMaxCondition:public NoteCondition
{
public:
    IsPreciseMaxCondition(GuitarFreqAnalysis& gfa,SegmentDeFrequencies* cursg,SegmentDeFrequencies *prevsg,int freq,double velocity,int note,bool Precis);
    bool IsTrue() override;
};

class NoteOffCondition:public NoteCondition
{
public:
     bool IsTrue() override;
};

class IncreaseModuleCondition:public Condition
{
    double cur,ant,min,max,proporcio;
 public:
    IncreaseModuleCondition(double curabsdiff,double antabsdiff,double MinModul,double maxmodul,double minproporcio)
    {cur=curabsdiff;ant=antabsdiff;min=MinModul;max=maxmodul;proporcio=minproporcio;};
    bool IsTrue() override
    {
        return (cur-ant)>min && cur>max*proporcio;
    }
};

class IsMaxFreqCondition:public Condition
{
public:
    int nfreq;
    SegmentDeFrequencies *currentseg;
    GuitarFreqAnalysis *gfa;
    int addsubst=SegmentDeFrequencies::FreqsPerNota/2;
    static const int ModulCond=1,AbsDiffCond=2,MaxValueCond=4;
    int TypeCond=AbsDiffCond;
    IsMaxFreqCondition(int nfr,SegmentDeFrequencies *cs,GuitarFreqAnalysis *ga,int as=0,int type=AbsDiffCond);

    bool IsTrue()override;
};

class IsMaxCenterCondition:public IsMaxFreqCondition
{
public:
    IsMaxCenterCondition(int nfr,SegmentDeFrequencies *cs,GuitarFreqAnalysis *ga,int as=0,int type=AbsDiffCond);
    bool IsTrue() override;
};

class NoteOnCondition:public NoteCondition{
protected:
    bool SecondSel;
public:

    NoteOnCondition(GuitarFreqAnalysis& gfa,SegmentDeFrequencies* cursg,SegmentDeFrequencies *prevsg,int freq,double velocity,int note,bool Precis,bool prev);

    bool IsTrue() override;
};

template<class EL>class ContinuousBuffer
{
public:
    const EL *buff=nullptr;
    size_t Num=0;
    ContinuousBuffer(const EL *array,size_t numels);
    double operator[] (double contindex);
};


struct FreqEvent
{

    static const short NoteOn=1;
    static const short NoteOff=2;
    short eventtype=-1;//{NoteOn=1,NoteOff=2};
    unsigned long nBuffer=0;
    unsigned short note;
    double velocity;
    clock_t time;
    unsigned short nFreq;
    double hz;
    FreqEvent(){}
    FreqEvent(const FreqEvent& altre)
    {
        eventtype=altre.eventtype;
        nBuffer=altre.nBuffer;
        note=altre.note;
        velocity=altre.velocity;
        time=altre.time;
        nFreq=altre.nFreq;
        hz=altre.hz;
    }
    FreqEvent& operator=(const FreqEvent& altre)
    {
        eventtype=altre.eventtype;
        nBuffer=altre.nBuffer;
        note=altre.note;
        velocity=altre.velocity;
        time=altre.time;
        nFreq=altre.nFreq;
        hz=altre.hz;
        return *this;
    }

    void evNoteOn(unsigned short nnote,unsigned short vel,unsigned short nfreq,unsigned long nbuffer,clock_t tm);
    void evNoteOff(unsigned short nnote,unsigned long nbuffer,clock_t tm);



};

char *EnglishNoteFromMidiNNote(unsigned short nnote,char *Here);
class SystemNotifyFreqs
{
public:
    virtual void Notify(FreqEvent f);
    virtual ~SystemNotifyFreqs(){}
};

class GuitarFreqAnalysis: public QObject
{
    Q_OBJECT
    typedef short FreqNumber_t;
    typedef short NoteNumber_t;

    AudioInputInterf *audioInterface=nullptr;
    SystemNotifyFreqs *noti=nullptr;
public:
    static const int MaxNotesPoly=6; //24....6 strings

    constexpr static const double MinModulNota=256;
    constexpr static const double MinModulNoteOff=92;

    unsigned long nBuffer=0;



    AudioBuff currentaudiobuf;

public:
    bool NewEventsPause=false;
    std::vector<FreqEvent> NotesOn;
    std::list<FreqEvent> PrevSelNotes;

    unsigned long getNBuffer(){return nBuffer;}
    short NoteStatus[128];//NoteOn o NoteOff;
    static const short NoteStatusOn=1,NoteStatusOff=0;

    static short NoteFromFreqNote(short freq);
    static short FreqNoteFromNote(short note);


    AudioBuff* getAudioBuff(){return &currentaudiobuf;};
    bool ChangedEvents=false;
    static double HzFromnFreq(double nfreq,double freqsoctava);
    GuitarFreqAnalysis(AudioInputInterf *interf,SystemNotifyFreqs *   sn);
    bool Start(const char *src=nullptr);
    bool IsNoteOn(SegmentDeFrequencies& curseg,SegmentDeFrequencies& prevseg,int nfreq,double& velocity,unsigned nnote,bool Precis=false);
    void NoteOn(unsigned short nnote,unsigned short velocity,unsigned short nfreq,unsigned long nbuffer,clock_t time=0);
    void NoteOff(unsigned short nnote,unsigned long nbuffer,clock_t time=0);
    void AllNotesOff();
    FreqVal CalcModulPrecisNota(int nfreq,unsigned nnota,SegmentDeFrequencies& curseg,unsigned NumFactorsMax=96);
    std::list<FreqEvent> PrioritySort(std::list<FreqEvent> notes);
    static std::list<FreqEvent> Intersect(std::list<FreqEvent>& SelNotes,std::list<FreqEvent>& prevnotes);

    virtual ~GuitarFreqAnalysis();
    bool OneStep=false;
signals:
    void BuffProcessed();
    void NewEvents();
    void NewEventsnPause();
    void Pause();
public slots:
    void Step();
//    void NewEventsPause();


};
class EsHarmonicCond:public Condition
{
    FreqEvent b;
    FreqEvent h;
public:
    EsHarmonicCond(FreqEvent base,FreqEvent harm);
    bool IsTrue() override;

};
class ApproxCondition:public Condition
{
    double a;
    double b;
    double pcerr;
public:
    ApproxCondition(double v,double w,double err)
    {
        a=v;
        b=w;
        pcerr=err;

    }
    bool IsTrue() override;

};

#endif // GUITARFREQANALYSIS_H
