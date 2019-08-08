#ifndef AUDIOINPUTINTERF_H
#define AUDIOINPUTINTERF_H

//#include "guitarfreqanalysis.h"
#include <ctype.h>
#include <stdint.h>

typedef int16_t orint16;

typedef long long unifiedint;


struct FreqVal
{
    double xc;
    double ys;
    double modul;
    double nfactors;//normalment 48 però pot ser menys
    double max;
    double absdiff;
    double alphacos;
};




class SegmentDeFrequencies
{
public:
    static const unsigned NumFactorsPreCalc=96;
    static const int FreqsPerNota=4;
    static const int NotesOctava=12;
    static const int arraylongona=(int)NumFactorsPreCalc/4;
    constexpr static const double MinProporcioModulMax=1.0/2.0;
    constexpr static const double arrayfreq=1.0/(double)arraylongona;
    double MaximModul=0;
    double stddev=1;
    double Mitja=0;
    int16_t NumFreqs=1;
    static double PreCalcSin482f[NumFactorsPreCalc];
    static double PreCalcCos482f[NumFactorsPreCalc];
    static void DoPreCalcsInit();
    FreqVal Segment[FreqsPerNota*128];
    SegmentDeFrequencies& operator -=(const SegmentDeFrequencies& ant);
    SegmentDeFrequencies& operator=(const SegmentDeFrequencies& seg);
    void ZeroBuff();
private:

};

class AudioBuff //Default format PCM_INT16_48000_?MONO_LSB?
{
public:
    constexpr static const double MinStdDevDivN=16;
    static const unifiedint BUFF_SIZE=960;
    static const unifiedint  MIN_NNOTE=40;
    static const unifiedint MAX_NNOTE=84;
    static const orint16    MinFactorsFreq=48;
    constexpr static const unifiedint SAMPLINGRATE=48000;

    char data[BUFF_SIZE*2];
    orint16 int16array[BUFF_SIZE];
//protected:
    unifiedint nBuffer=-1;
    double NormalizedData[BUFF_SIZE];
    unifiedint nSamples;
    double stddev;
    double stddevdivn;
    double mitja;
    double suma;
    bool Full();
    AudioBuff();
    void ZeroBuff();
    orint16 * GetInt16Array(){return int16array;}
    void CalcVariables();
    friend class SegmentDeFrequencies;
    double NSamplesFreq(double freq)
    {
        return AudioBuff::SAMPLINGRATE/freq;
    }

    SegmentDeFrequencies prevseg;
    SegmentDeFrequencies currentseg;
//    SegmentDeFrequencies substract;

    void CalcCurrentFreqSeg();
protected:
    inline FreqVal CalciFreq(unsigned i);
};

class GuitarFreqAnalysis;
class AudioInputInterf
{

public:
    bool OneStep=false;

public:

    GuitarFreqAnalysis *gfa=nullptr;
    AudioInputInterf();
    virtual ~AudioInputInterf(){}
    virtual void Start(const char *src=nullptr)=0;
    virtual int ReadBuff(AudioBuff& buff)=0;
    virtual bool BufferReady()=0;

};

#endif // AUDIOINPUTINTERF_H
