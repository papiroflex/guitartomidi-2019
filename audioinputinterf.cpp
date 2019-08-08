#include "audioinputinterf.h"
#include "guitarfreqanalysis.h"
#include <memory.h>
#include <algorithm>
#include <math.h>
#include <assert.h>
#include <ios>
#include <iostream>
#include "systemabstract.h"





double SegmentDeFrequencies::PreCalcSin482f[SegmentDeFrequencies::NumFactorsPreCalc];
double SegmentDeFrequencies::PreCalcCos482f[SegmentDeFrequencies::NumFactorsPreCalc];



void AudioBuff::CalcCurrentFreqSeg()
{
    assert(sizeof(currentseg.Segment)==SegmentDeFrequencies::FreqsPerNota*128*sizeof(currentseg.Segment[0]));

    currentseg.ZeroBuff();
    currentseg.MaximModul=0;
    currentseg.Mitja=0;
    currentseg.stddev=0;
    currentseg.NumFreqs=MAX_NNOTE*SegmentDeFrequencies::FreqsPerNota-MIN_NNOTE*SegmentDeFrequencies::FreqsPerNota;
    for(unsigned int i=MIN_NNOTE*SegmentDeFrequencies::FreqsPerNota-8;i<MAX_NNOTE*SegmentDeFrequencies::FreqsPerNota;i++)
    {

        currentseg.Segment[i]=CalciFreq(i);
        currentseg.MaximModul=std::max(currentseg.MaximModul,currentseg.Segment[i].modul);
        currentseg.Mitja+=currentseg.Segment[i].modul;
        currentseg.stddev+=pow(currentseg.Segment[i].modul,2);

    }
    currentseg.Mitja=currentseg.Mitja/currentseg.NumFreqs;
    currentseg.stddev=sqrt(currentseg.stddev);
};

FreqVal AudioBuff::CalciFreq(unsigned i)
{
    int16_t *buff=GetInt16Array();//NormalizedData;//normalizedData/nsamples
    ContinuousBuffer<int16_t> contbuff(buff,sizeof (this->int16array)/sizeof(int16_t));
    FreqVal ret;
    ret.xc=0;
    ret.ys=0;
    ret.modul=0;
    ret.nfactors=1;
    ret.absdiff=0;
    ret.max=0;
    double curfreqhz=GuitarFreqAnalysis::HzFromnFreq(i,SegmentDeFrequencies::FreqsPerNota*SegmentDeFrequencies::NotesOctava);//440.0*(double)pow((double)2.0,((double)i/(double)SegmentDeFrequencies::FreqsPerNota-(double)69.0)/(double)SegmentDeFrequencies::NotesOctava);


    double longona=1.0/(/*2.0*M_PI**/curfreqhz);
    double longonasamples=longona*AudioBuff::SAMPLINGRATE;
    double longonax4=longonasamples*2;//SegmentDeFrequencies::FreqsPerNota;

    unifiedint n=0;
    /*if(longonax4<SegmentDeFrequencies::NumFactorsPreCalc)
        return ret;*/

    for(n=0;n<SegmentDeFrequencies::NumFactorsPreCalc;n++)
    {


        double dbuffcontidx=((double)longonax4*(double)n/(double)SegmentDeFrequencies::NumFactorsPreCalc);

        //unifiedint ibuffidx=(unifiedint)std::round(((double)longonax4*(double)n/(double)SegmentDeFrequencies::NumFactorsPreCalc));
        if(dbuffcontidx>=AudioBuff::BUFF_SIZE)
        {

            break;

        }

        ret.ys+=SegmentDeFrequencies::PreCalcSin482f[n]*(double)contbuff[dbuffcontidx];
        ret.xc+=SegmentDeFrequencies::PreCalcCos482f[n]*(double)contbuff[dbuffcontidx];

    }

    ret.nfactors=n;
    if(ret.nfactors<MinFactorsFreq)
        return ret;
    ret.xc=ret.xc/(double)ret.nfactors;
    ret.ys=ret.ys/(double)ret.nfactors;
    ret.modul=sqrt(pow(ret.xc,2)+pow(ret.ys,2))/**(double)48*/;
    ret.max=std::max(abs(ret.xc),abs(ret.ys));
    ret.absdiff=abs(ret.xc-ret.ys);
    ret.alphacos=acos(ret.xc/ret.modul);
    return ret;

}
AudioInputInterf::AudioInputInterf()
{
    SegmentDeFrequencies::DoPreCalcsInit();
}
void SegmentDeFrequencies::DoPreCalcsInit()
{
    double longona=SegmentDeFrequencies::arraylongona;
    for(unsigned int i=0;i<NumFactorsPreCalc;i++)
    {
        double di=(double)i;


        SegmentDeFrequencies::PreCalcSin482f[i]=sin(2.0*M_PI*di/longona);
        SegmentDeFrequencies::PreCalcCos482f[i]=cos(2.0*M_PI*di/longona);
    }
};
SegmentDeFrequencies& SegmentDeFrequencies::operator-=(const SegmentDeFrequencies& ant)
{
    for(size_t i=0;i<FreqsPerNota*128;i++)
    {
        Segment[i].modul=(double)/*std::max(0LL,(unifiedint)*/(Segment[i].modul-ant.Segment[i].modul);
        Segment[i].xc=Segment[i].xc-ant.Segment[i].xc;
        Segment[i].ys=Segment[i].ys-ant.Segment[i].ys;

    }
    return *this;
}
void SegmentDeFrequencies::ZeroBuff()
{
    MaximModul=0;
    stddev=1;
    Mitja=0;
    for(size_t i=0;i<sizeof(Segment)/sizeof(Segment[0]);i++)
    {

        Segment[i].xc=0;
        Segment[i].ys=0;
        Segment[i].modul=0;
        Segment[i].nfactors=0;
        Segment[i].max=0;
        Segment[i].absdiff=0;

    }

}

bool AudioBuff::Full()
{
    assert(nSamples<=BUFF_SIZE);
    return nSamples==BUFF_SIZE;
}
AudioBuff::AudioBuff()
{
    stddev=1;
    mitja=0;
    suma=0;
    nSamples=0;
    currentseg.ZeroBuff();
    prevseg.ZeroBuff();
  //  substract.ZeroBuff();

}
void AudioBuff::ZeroBuff()
{
    memset(data,0,sizeof(data));


};
void AudioBuff::CalcVariables()
{
    suma=0;
    mitja=0;
    stddev=0;
    for(unsigned i=0;i<nSamples;i++)
    {
        suma+=int16array[i];
        stddev+=pow(int16array[i],2);


    }
    stddev=sqrt(stddev);
    stddevdivn=stddev/nSamples;
    mitja=suma/nSamples;
    for(unsigned i=0;i<nSamples;i++)
    {
        this->NormalizedData[i]=(int16array[i]-mitja)/stddev;

    }
}
SegmentDeFrequencies& SegmentDeFrequencies::operator=(const SegmentDeFrequencies& seg)
{

    memcpy(Segment,seg.Segment,128*FreqsPerNota*sizeof(FreqVal));
    this->Mitja=seg.Mitja;
    this->stddev=seg.stddev;
    this->NumFreqs=seg.NumFreqs;
    this->MaximModul=seg.MaximModul;
    return *this;
}
bool IsNoteOn(SegmentDeFrequencies& curseg,SegmentDeFrequencies& prevseg,unsigned nfreq);
