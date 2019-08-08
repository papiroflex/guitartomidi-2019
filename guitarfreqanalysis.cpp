#include "audioinputinterf.h"
#include "guitarfreqanalysis.h"
#include <ctime>
#include <QThread>
#include <algorithm>

#include <math.h>

#include <cmath>

#include <string.h>

static char EnglishNotes[12][3]={"A","A#","B","C","C#","D","D#","E","F","F#","G","G#"};


char *EnglishNoteFromMidiNNote(unsigned short nnote,char *Here)
{
    char *ret=Here;
//    char ret[6];
    strcpy(ret,EnglishNotes[(nnote+3)%12]);
    short nOctava=(nnote+3)/12;
    char noctchar=(char)(nOctava+'0');
    char nOctStr[3]={noctchar,0,0};
    strcat(ret,nOctStr);
    return Here;
};





void FreqEvent::evNoteOn(unsigned short nnote,unsigned short vel,unsigned short nfreq,unsigned long nbuffer,clock_t tm)
{
    nFreq=nfreq;
    hz=GuitarFreqAnalysis::HzFromnFreq(nFreq,SegmentDeFrequencies::NotesOctava*SegmentDeFrequencies::FreqsPerNota);
    eventtype=NoteOn;
    note=nnote;
    velocity=vel;//0-127
    if(tm==0)
        time=clock();
    else {
        time=tm;
    }
    nBuffer=nbuffer;
}
void FreqEvent::evNoteOff(unsigned short nnote,unsigned long nbuffer,clock_t tm)
{
    eventtype=NoteOff;
    note=nnote;
    velocity=0;
    nBuffer=nbuffer;
    if(tm==0)
        time=clock();
    else {
        time=tm;
    }
}







double GuitarFreqAnalysis::HzFromnFreq(double nfreq,double freqsoctava)
{
    double nnote12=nfreq*(double)12.0/(double)freqsoctava;
    double potencia=(nnote12-(double)69.0)/(double)12.0;
    double hz=((double)440.0)*std::pow((double)2.0,potencia);
    return hz;
}

short GuitarFreqAnalysis::NoteFromFreqNote(short freq)
{
    double dnote=(double)freq/(double)(SegmentDeFrequencies::FreqsPerNota);
    //short ret=(short)std::round(dnote);
    short ret=(short)std::round(dnote);
    return ret;
};
short GuitarFreqAnalysis::FreqNoteFromNote(short note)
{
    short ret=(short)note*SegmentDeFrequencies::FreqsPerNota;
    return ret;
};
bool AndCondition::IsTrue()
{
    return a->IsTrue() && b->IsTrue();
}
bool OrCondition::IsTrue()
{
    return a->IsTrue() || b->IsTrue();
}
OrCondition& Condition::operator || (Condition& other)
{
    return *new OrCondition(this,&other);
}

AndCondition& Condition::operator && (Condition& other)
{
    return *new AndCondition(this,&other);

};

NoteCondition::NoteCondition(GuitarFreqAnalysis& gfa,SegmentDeFrequencies* cursg,SegmentDeFrequencies *prevsg,int freq,double velocity,int note,bool Precis){
    this->gfa=&gfa;
    curseg=cursg;
    prevseg=prevsg;
    this->nfreq=freq;
    this->velocity=velocity;
    this->nnote=note;
    this->Precis=Precis;
}
IsPreciseMaxCondition::IsPreciseMaxCondition(GuitarFreqAnalysis& gfa,SegmentDeFrequencies* cursg,SegmentDeFrequencies *prevsg,int freq,double velocity,int note,bool Precis):NoteCondition(gfa,cursg,prevsg,freq,velocity,note,Precis)
{

}
bool IsPreciseMaxCondition::IsTrue()
{
    static const int NumFreqsMirar=9;
    static const bool MirarCentre=true;
    assert((NumFreqsMirar%2)==1);
    FreqVal events[NumFreqsMirar];
    bool ret=true;
    for(int i=-NumFreqsMirar/2;i<=NumFreqsMirar/2;i++)
    {
        events[i+NumFreqsMirar/2]=this->gfa->CalcModulPrecisNota(this->nfreq+i,nnote,*curseg,192);
    }
    int ievmax=NumFreqsMirar/2;;

    for(int i=0;i<NumFreqsMirar;i++)
    {
        int nfreqcur=nfreq+i-NumFreqsMirar/2;
        //if(events[i].modul>events[ievmax].modul)
        double modulseguent=0;
        if(i<NumFreqsMirar/2 && !MirarCentre)
        {
                modulseguent=events[i+1].modul;
        }
        else if(i>NumFreqsMirar/2 && !MirarCentre)
        {
                modulseguent=events[i-1].modul;

        }else {//i==NumFreqsMirar/2 || MirarCentre
                modulseguent=events[i].modul;
        }
        if(events[i].modul>modulseguent){
            ret=false;
            break;
        }

    }


    return ret;
}
NoteOnCondition::NoteOnCondition(GuitarFreqAnalysis& gfa,SegmentDeFrequencies* cursg,SegmentDeFrequencies *prevsg,int freq,double velocity,int note,bool Precis,bool prevsel):NoteCondition(gfa,cursg,prevsg,freq,velocity,note,Precis)
{
    int nfreq1=nnote*SegmentDeFrequencies::FreqsPerNota-SegmentDeFrequencies::FreqsPerNota/2;
    int nfreq2=nnote*SegmentDeFrequencies::FreqsPerNota+SegmentDeFrequencies::FreqsPerNota/2;
    SecondSel=prevsel;

}
bool NoteOnCondition::IsTrue()
{
    bool ret=false;
//    IncreaseModuleCondition inccdiff(curseg->Segment[nfreq].absdiff,prevseg->Segment[nfreq].absdiff,GuitarFreqAnalysis::MinModulNota);

    IncreaseModuleCondition inccmodul(curseg->Segment[nfreq].modul,prevseg->Segment[nfreq].modul,GuitarFreqAnalysis::MinModulNota,
                                    curseg->MaximModul,curseg->MinProporcioModulMax                                      );

    int rangmirar=1*SegmentDeFrequencies::FreqsPerNota/4;
    if(!inccmodul.IsTrue() && !SecondSel)
        return false;
    ret=false;
    for(int r=-rangmirar;r<=rangmirar && (!ret);r++)
    {
//        IsMaxFreqCondition maxfreq(nfreq+r,curseg,gfa,(int)SegmentDeFrequencies::FreqsPerNota/2.0,IsMaxFreqCondition::ModulCond /*| IsMaxFreqCondition::AbsDiffCond*/);
        IsMaxCenterCondition maxcenter(nfreq+r,curseg,gfa,4*(int)SegmentDeFrequencies::FreqsPerNota/4.0,
                                 /*IsMaxFreqCondition::MaxValueCond*/IsMaxFreqCondition::ModulCond);
        ret=maxcenter.IsTrue();
        if(ret)
        {
            char sNote[5];
            //std::cout<<"MaxCenter: "<<EnglishNoteFromMidiNNote(nnote,sNote)<<std::endl;
            //std::cout<<"Freq:"<<nfreq+r<<"Modul:""

        }
        if(ret && Precis)
        {
            IsPreciseMaxCondition Prcs(*gfa,curseg,prevseg,nfreq+r,2,nnote,true);
            ret=Prcs.IsTrue();
            if(ret)
                nfreq=nfreq+r;

        }
    }

    return ret;
}
bool ApproxCondition::IsTrue()
{
    return fabs(a-b)<fabs(a*pcerr/100.0);
}

EsHarmonicCond::EsHarmonicCond(FreqEvent base,FreqEvent harm)
{
    b=base;
    h=harm;

};
bool EsHarmonicCond::IsTrue()
{
    double maxerrorpercent=2.0/48.0;

    for(int nharm=2;nharm<8;nharm++)
    {
        ApproxCondition appr(b.hz*nharm,h.hz,2);

        if(appr.IsTrue())
            return true;
    }
    return false;
}





IsMaxFreqCondition::IsMaxFreqCondition(int nfr,SegmentDeFrequencies *cs,GuitarFreqAnalysis *ga,int as,int type)
{
    nfreq=nfr;
    currentseg=cs;
    gfa=ga;
    if(as!=0)
        addsubst=as;
    TypeCond=type;


}
bool IsMaxFreqCondition::IsTrue()
{
    bool ret=true;
    if(TypeCond&AbsDiffCond)
    {
        for (int r=-addsubst;r<=-1 && ret;r++)
        {
             ret=ret && currentseg->Segment[nfreq+r].absdiff<currentseg->Segment[nfreq+r+1].absdiff;

        }
        for(int r=1;r<=addsubst && ret;r++)
        {
             ret=ret && currentseg->Segment[nfreq+r].absdiff<currentseg->Segment[nfreq+r-1].absdiff;
        }
    }
    if(TypeCond&ModulCond)
    {
        for (int r=-addsubst;r<=-1 && ret;r++)
        {
             ret=ret && currentseg->Segment[nfreq+r].modul<currentseg->Segment[nfreq+r+1].modul;

        }
        for(int r=1;r<=addsubst && ret;r++)
        {
             ret=ret && currentseg->Segment[nfreq+r].modul<currentseg->Segment[nfreq+r-1].modul;
        }
    }
  //  else assert(false);
    if(TypeCond&MaxValueCond)
    {
        for (int r=-addsubst;r<=-1 && ret;r++)
        {
             ret=ret && currentseg->Segment[nfreq+r].max<currentseg->Segment[nfreq+r+1].max;

        }
        for(int r=1;r<=addsubst && ret;r++)
        {
             ret=ret && currentseg->Segment[nfreq+r].max<currentseg->Segment[nfreq+r-1].max;
        }
    }
    return ret;
}
IsMaxCenterCondition::IsMaxCenterCondition(int nfr,SegmentDeFrequencies *cs,GuitarFreqAnalysis *ga,int as,int type):IsMaxFreqCondition(nfr,cs,ga,as,type)
{


}

bool IsMaxCenterCondition::IsTrue()
{
    bool ret=true;
    if(TypeCond&AbsDiffCond)
    {
        for (int r=-addsubst;r<=-1 && ret;r++)
        {
             ret=ret && currentseg->Segment[nfreq+r].absdiff<currentseg->Segment[nfreq].absdiff;

        }
        for(int r=1;r<=addsubst && ret;r++)
        {
             ret=ret && currentseg->Segment[nfreq+r].absdiff<currentseg->Segment[nfreq].absdiff;
        }
    }
    if(TypeCond&ModulCond)
    {
        for (int r=-addsubst;r<=-1 && ret;r++)
        {
             ret=ret && currentseg->Segment[nfreq+r].modul<currentseg->Segment[nfreq].modul;

        }
        for(int r=1;r<=addsubst && ret;r++)
        {
             ret=ret && currentseg->Segment[nfreq+r].modul<currentseg->Segment[nfreq].modul;
        }
    }
    if(TypeCond&MaxValueCond)
    {
        for (int r=-addsubst;r<=-1 && ret;r++)
        {
             ret=ret && currentseg->Segment[nfreq+r].max<currentseg->Segment[nfreq].max;

        }
        for(int r=1;r<=addsubst && ret;r++)
        {
             ret=ret && currentseg->Segment[nfreq+r].max<currentseg->Segment[nfreq].max;
        }
    }
    return ret;
}
template<class EL>ContinuousBuffer<EL>::ContinuousBuffer(const EL *array,size_t numels)
{
    buff=array;
    Num=numels;
};
template<class EL>double ContinuousBuffer<EL>::operator[] (double contindex)
{
    double flidx=std::floor(contindex);
    double ceilidx=std::ceil(contindex);
    double fractionidx=contindex-flidx;

    double ret1=buff[(size_t)flidx];
    double ret2=buff[(size_t)ceilidx];
    double ret=ret1+(ret2-ret1)*fractionidx;
    return ret;

}


void SystemNotifyFreqs::Notify(FreqEvent f)
{
        //char *EnglishNote=EnglishNotes[((f.note)%12)];
    char EnglishNote[6];
    EnglishNoteFromMidiNNote(f.note,EnglishNote);
    switch(f.eventtype)
    {

        case FreqEvent::NoteOn:
            std::cout<<std::endl<<f.nBuffer<<":note on: "<<EnglishNote<<"note num: "<<f.note<<" Velocity:"<<f.velocity<<" Frequency"<<f.nFreq<<"Hertz:"<<f.hz<< "Hz"<<std::endl;
        break;
        case FreqEvent::NoteOff:
            std::cout<<std::endl<<f.nBuffer<<":note off: "<<EnglishNote<<std::endl;
        break;
    }
}
GuitarFreqAnalysis::GuitarFreqAnalysis(AudioInputInterf *Interf,SystemNotifyFreqs *sn)
{
    audioInterface=Interf;
    noti=sn;

    memset(NoteStatus,NoteStatusOff,sizeof(NoteStatus));
}
bool GuitarFreqAnalysis::Start(const char *src)
{
    currentaudiobuf.ZeroBuff();
    currentaudiobuf.currentseg.ZeroBuff();
    currentaudiobuf.prevseg.ZeroBuff();
    audioInterface->Start(src);
    return true;

}
void GuitarFreqAnalysis::AllNotesOff()
{
   // std::vector<FreqEvent>::iterator pnotes=NotesOn.begin();
    while(NotesOn.size()>0)
    {
        NoteOff(NotesOn.at(0).note,nBuffer);
    }
}
FreqVal GuitarFreqAnalysis::CalcModulPrecisNota(int nfreq,unsigned nnota,SegmentDeFrequencies& curseg,unsigned NumFactorsMax)
{
    int16_t *buff=currentaudiobuf.GetInt16Array();

    ContinuousBuffer<int16_t> buffcont(buff,sizeof(currentaudiobuf.int16array)/sizeof(int16_t));
    FreqVal ret;
    ret.xc=0;
    ret.ys=0;
    ret.modul=0;
    ret.max=0;
    ret.absdiff=0;
    double curfreqhz=HzFromnFreq(nfreq,SegmentDeFrequencies::FreqsPerNota*SegmentDeFrequencies::NotesOctava);//(double)440.0*(double)pow((double)2.0,((double)nfreq/(double)SegmentDeFrequencies::FreqsPerNota-(double)69.0)/(double)SegmentDeFrequencies::NotesOctava);


    double longona=(double)1.0/curfreqhz;
    double longonasamples=longona*(double)AudioBuff::SAMPLINGRATE;
    double longonax4=longonasamples*4;//SegmentDeFrequencies::FreqsPerNota;
    unifiedint n=0;
    double alpha=curseg.Segment[nfreq].alphacos;
    for(n=0;n<NumFactorsMax;n++)
    {



        //unifiedint ibuffidx=(unifiedint)std::round(((double)longonax4*(double)n)/(double)NumFactorsMax);
        double buffidxc=(longonax4*(double)n)/(double)NumFactorsMax;
        if(buffidxc>=AudioBuff::BUFF_SIZE)
        {

            break;

        }
        ;
        ret.ys+=sin(alpha+(double)4.0*(double)n*(double)2.0*(double)M_PI/(double)NumFactorsMax)*(double)buffcont[buffidxc];

        ret.xc+=cos(alpha+(double)4.0*(double)n*(double)2.0*(double)M_PI/(double)NumFactorsMax)*(double)buffcont[buffidxc];

    }
    unifiedint nfactors=n;
    if(nfactors<AudioBuff::MinFactorsFreq)
        return ret;
    ret.xc=ret.xc/nfactors;
    ret.ys=ret.ys/nfactors;
    ret.modul=sqrt(pow(ret.ys,2.0)+pow(ret.xc,2.0));
    ret.max=std::max(ret.ys,ret.xc);
    ret.nfactors=nfactors;
    ret.absdiff=abs(ret.ys-ret.xc);
    ret.alphacos=acos(ret.xc/ret.modul);

    return ret;
}
 bool GuitarFreqAnalysis::IsNoteOn(SegmentDeFrequencies& curseg,SegmentDeFrequencies& prevseg,int nfreq,double& velocity,unsigned nnote,bool Precis)
 {
//#define NOTEONCONDITION
//#ifdef NOTEONCONDITION
//#endif
       // std::max((double)0,(double)currentaudiobuf.substract.Segment[i].modul*16.0/(double)MinMFreq);
    double curdiff=curseg.Segment[nfreq].absdiff;
    double antdiff=curseg.Segment[nfreq-1].absdiff;
    double nextdiff=curseg.Segment[nfreq+1].absdiff;
    double prevdiff=prevseg.Segment[nfreq].absdiff;
    //double curmodul=curseg.Segment[nfreq].modul;
    //double prevmodul=prevseg.Segment[nfreq].modul;
   // double antmodul=(nfreq>0)?(curseg.Segment[nfreq-1].modul):(0.0);
    //double nextmodul=(nfreq<(AudioBuff::MAX_NNOTE*SegmentDeFrequencies::FreqsPerNota-1))?(curseg.Segment[nfreq+1].modul):(0.0);
    bool IsMax=true;//curmodul>antmodul && curmodul>nextmodul;
//    bool NouAmplitud=(curmodul-prevmodul)>0 &&  curmodul>this->MinModulNota && curmodul>curseg.MaximModul*curseg.MinProporcioModulMax;
    bool NouAmplituddiff=(curdiff-prevdiff)>0 && curdiff>this->MinModulNota && curdiff>curseg.MaximModul*curseg.MinProporcioModulMax;
    velocity=(curdiff)*10.0/MinModulNota;
    bool defiar=true;
    int nfreqsvoltantmirar=6.0*SegmentDeFrequencies::FreqsPerNota/4.0;
    /*if(Precis)
        nfreqsvoltantmirar=1;*/
    bool SuffFactors=curseg.Segment[nfreq].nfactors>=AudioBuff::MinFactorsFreq;
    if(nfreqsvoltantmirar>=1 && IsMax && NouAmplituddiff)
    {
        for(int dx=1;(dx<=nfreqsvoltantmirar) && IsMax;dx++)
        {
#ifdef SEG_MODULS
            if(curmodul<curseg.Segment[nfreq-dx].modul){
//              if(curseg.Segment[nfreq-dx+1].modul<curseg.Segment[nfreq-dx].modul){
                IsMax=false;
                break;
            }else// if(curseg.Segment[nfreq+dx-1].modul<curseg.Segment[nfreq+dx].modul){
            if(curmodul<curseg.Segment[nfreq+dx].modul){
                IsMax=false;
            }
#endif
#ifndef DIFFCOMP1
            if(curseg.Segment[nfreq-dx+1].absdiff<curseg.Segment[nfreq-dx].absdiff){
//
                IsMax=false;
                break;
            }else
            if(curseg.Segment[nfreq+dx-1].absdiff<curseg.Segment[nfreq+dx].absdiff){
                IsMax=false;
                break;
            }
#else
            if(curdiff<curseg.Segment[nfreq-dx].absdiff){
//              if(curseg.Segment[nfreq-dx+1].modul<curseg.Segment[nfreq-dx].modul){
                IsMax=false;
                break;
            }else// if(curseg.Segment[nfreq+dx-1].modul<curseg.Segment[nfreq+dx].modul){
            if(curdiff<curseg.Segment[nfreq+dx].absdiff){
                IsMax=false;
                break;
            }
#endif

        }
    }
    if(IsMax && NouAmplituddiff && SuffFactors)
    {
        //double mincoef=((double)curseg.Segment[nfreq].nfactors-1.0)/((double)curseg.Segment[nfreq].nfactors);
        double mincoef=11.9/12.0;
        //if(antmodul>curmodul*mincoef || nextmodul>curmodul*mincoef)
        if(antdiff>curdiff*mincoef || nextdiff>curdiff*mincoef)
        {
            std::cout<<"n";
            std::cout.flush();
            defiar=false;
        }

    }
    return IsMax && NouAmplituddiff && defiar && SuffFactors;
 };
int freqEventCmp(const AudioBuff& buff,FreqEvent note1,FreqEvent note2)
{
    int ret=0;
    if(note1.velocity>note2.velocity)
    {
        ret=1;
    }
    else if(note1.velocity<note2.velocity)
    {
        ret=-1;
    }
    else  {
        ret=-0;
       }
    return ret;
}

std::list<FreqEvent> GuitarFreqAnalysis::PrioritySort( std::list<FreqEvent> notes)
{
    std::list<FreqEvent> ret;

    int i;
    for(int i=0;notes.size()>0  && ((ret.size()+NotesOn.size())<MaxNotesPoly);i++)
    {
        std::list<FreqEvent>::iterator it=notes.begin();
        std::list<FreqEvent>::iterator itmax=it;
        FreqEvent max;
        if(it!=notes.end()){
            max=(*it);
            itmax=it;
        }
        else {
            break;
        }
        it++;
        while(it!=notes.end())
        {
            if(freqEventCmp(currentaudiobuf,max,(*it))<0)
            {
                max=(*it);

                itmax=it;
            }

            it++;
        }
        notes.erase(itmax);
        ret.push_back(max);

    }
    return ret;
}
 std::list<FreqEvent> GuitarFreqAnalysis::Intersect(std::list<FreqEvent>& SelNotes,std::list<FreqEvent>& prevnotes)
{
    std::list<FreqEvent> ret;
    std::list<FreqEvent>::iterator itSelNotes=SelNotes.begin();
    while(itSelNotes!=SelNotes.end())
    {
        std::list<FreqEvent>::iterator itPrevNotes=prevnotes.begin();
        while(itPrevNotes!=prevnotes.end())
        {
            if((*itPrevNotes).note==(*itSelNotes).note)
            {
                ret.push_back((*itSelNotes));
            }
           itPrevNotes++;
        }
        itSelNotes++;



    }
    return ret;
}

#undef INTERSECT_PREVSELNOTES
#ifdef INTERSECT_PREVSELNOTES
static const bool IntersectSelNotes=true;
#else
 static const bool IntersectSelNotes=false;
#endif
void GuitarFreqAnalysis::Step()
{

    ChangedEvents=false;

    SegmentDeFrequencies cursegcopy=currentaudiobuf.currentseg;
    currentaudiobuf.ZeroBuff();
    currentaudiobuf.currentseg.ZeroBuff();
    currentaudiobuf.prevseg.ZeroBuff();
//    currentaudiobuf.substract.ZeroBuff();
    std::cout<<"S";
    std::cout.flush();
   if(audioInterface->ReadBuff(currentaudiobuf)<=0)
    {
        std::cerr<<"Warning!! couldn't read audio data!!";

        return;
    }

   currentaudiobuf.CalcVariables();

   if(currentaudiobuf.stddevdivn<currentaudiobuf.MinStdDevDivN)
   {
       currentaudiobuf.currentseg.ZeroBuff();
       currentaudiobuf.prevseg.ZeroBuff();
       AllNotesOff();
       //std::cout<<"s";
       nBuffer++;
       PrevSelNotes.clear();
       return;
   }

   currentaudiobuf.prevseg=cursegcopy;
   currentaudiobuf.CalcCurrentFreqSeg();

   size_t i=0;
   bool HiHaNotesOff=false;
   while(i<NotesOn.size())
   {
       unsigned short nn=NotesOn[i].note;
       unsigned short nFreq=NotesOn[i].nFreq;
       if(currentaudiobuf.currentseg.Segment[nFreq].modul<MinModulNoteOff)
       {

           NoteOff(nn,nBuffer);
           HiHaNotesOff=true;
           NoteStatus[nn]=NoteStatusOff;
       }
       else
           i++;
   }
   bool HiHaNotesOn=HiHaNotesOff || (currentaudiobuf.currentseg.stddev>currentaudiobuf.prevseg.stddev);


   int ini=AudioBuff::MIN_NNOTE*SegmentDeFrequencies::FreqsPerNota;
   int fi=AudioBuff::MAX_NNOTE*SegmentDeFrequencies::FreqsPerNota;
   std::list<FreqEvent> SelNotes;
#ifdef NONOTEONCONDITION

   for(unifiedint i=ini+1;i<=fi-1;i++)
   {
       short nnote=GuitarFreqAnalysis::NoteFromFreqNote((short)i);
       double velocity=0;
       FreqEvent curev;
       if(NoteStatus[nnote]!=NoteStatusOn  && IsNoteOn(currentaudiobuf.currentseg,currentaudiobuf.prevseg,(unsigned)i,velocity,(unsigned)nnote,false))
       {


//           if(NoteStatus[nnote]!=NoteStatusOn /*&& NotesOn.size()<(size_t)MaxNotesPoly*/)
           {
               bool PrecisNoteOn=true;
#if 0
               FreqVal freqprecispre=CalcModulPrecisNota((int)i-1,(int)nnote,currentaudiobuf.currentseg,192);
               FreqVal freqprecis=CalcModulPrecisNota((int)i,(int)nnote,currentaudiobuf.currentseg,192);
               FreqVal freqprecispost=CalcModulPrecisNota((int)i+1,(int)nnote,currentaudiobuf.currentseg,192);
               //FreqVal freqprecisant=CalcModulPrecisNota((int)i,(int)nnote,currentaudiobuf.prevseg,192);
               currentaudiobuf.currentseg.Segment[i-1]=freqprecispre;
               currentaudiobuf.currentseg.Segment[i]=freqprecis;
               currentaudiobuf.currentseg.Segment[i+1]=freqprecispost;

               PrecisNoteOn=IsNoteOn(currentaudiobuf.currentseg,currentaudiobuf.prevseg,(int)i,velocity,(unsigned)nnote,true);
#endif


                if(velocity==0.0 || !PrecisNoteOn)
                    continue;
                curev.note=(unsigned short)nnote;
                curev.nFreq=(unsigned short)i;
                curev.time=clock();
                curev.nBuffer=nBuffer;
                curev.velocity=velocity;
                curev.eventtype=FreqEvent::NoteOn;

                SelNotes.push_back(curev);
                //
           }

       }


   }
#endif
    bool SecondSel=PrevSelNotes.size()>0 && IntersectSelNotes;
   for(int nnota=AudioBuff::MIN_NNOTE;nnota<AudioBuff::MAX_NNOTE && HiHaNotesOn;nnota++)
   {
       bool precis=!IntersectSelNotes || SecondSel;
       NoteOnCondition noc(*this,&currentaudiobuf.currentseg,&currentaudiobuf.prevseg,(short)FreqNoteFromNote((short)nnota),16.0,(short)nnota,precis,SecondSel);
       FreqEvent curev;
       if(noc.IsTrue())
       {
           curev.hz=HzFromnFreq(noc.nfreq,SegmentDeFrequencies::NotesOctava*SegmentDeFrequencies::FreqsPerNota);
           curev.note=(unsigned short)nnota;
           curev.nFreq=(unsigned short)noc.nfreq;
           curev.time=clock();
           curev.nBuffer=nBuffer;
           curev.velocity=currentaudiobuf.currentseg.Segment[curev.nFreq].modul*2.0/MinModulNota;
           curev.eventtype=FreqEvent::NoteOn;



           SelNotes.push_back(curev);
       }

   }
#ifdef INTERSECT_PREVSELNOTES
   std::list<FreqEvent> intersectNotes;
   if(SecondSel){
        intersectNotes=Intersect(SelNotes,PrevSelNotes);
        PrevSelNotes=SelNotes;
        SelNotes=intersectNotes;
   }else {
       PrevSelNotes=SelNotes;
       SelNotes.clear();
   }
#else
   PrevSelNotes=SelNotes;
#endif
   for(std::list<FreqEvent>::iterator it=SelNotes.begin();it!=SelNotes.end();it++)
   {
       std::list<FreqEvent>::iterator it2=it;
       it2++;
       while(it2!=SelNotes.end())
       {
            double hzprec=(*it).hz;
            double hzh=(*it2).hz;
            if(hzprec>=hzh)
            {
                assert(hzprec>=hzh);
               break;
            }
            EsHarmonicCond es((*it),(*it2));

            if(es.IsTrue())
            {
                it2=SelNotes.erase(it2);

                std::cout<<"H"<<std::endl;
                std::cout.flush();
            }
            else {
                it2++;
            }

       }

   }

   if(HiHaNotesOn)
   {
        SelNotes=PrioritySort(SelNotes);
        std::list<FreqEvent>::iterator ith=SelNotes.begin();
   //for(int i=0;i<SelNotes.size() && NotesOn.size()<MaxNotesPoly;i++)
        while(ith!=SelNotes.end() && NotesOn.size()<MaxNotesPoly)
        {
            if(NoteStatus[(*ith).note]!=NoteStatusOn)
            {
                short nnote=(*ith).note;
                assert(nnote==this->NoteFromFreqNote(ith->nFreq));
                NoteOn((unsigned)nnote,(unsigned short)std::min((short)127,(short)((ith->velocity))),ith->nFreq,ith->nBuffer,ith->time);
                NoteStatus[nnote]=NoteStatusOn;
            }
            ith++;
       //NotesOn.push_back(SelNotes[i]);
        }
   }
   nBuffer++;
   std::cout<<"F";
   std::cout.flush();
   emit BuffProcessed();

   if(audioInterface->OneStep)
   {
       audioInterface->OneStep=false;
        emit Pause();
   }

   if(ChangedEvents)
   {
       emit NewEvents();
       if(NewEventsPause)
       {

           emit NewEventsnPause();
       }
        NewEventsPause=false;

   }






};


void GuitarFreqAnalysis::NoteOn(unsigned short nnote,unsigned short velocity,unsigned short nfreq, unsigned long nbuffer,clock_t time)
{
    FreqEvent evon;
    if(time==0)
        time=clock();
    evon.evNoteOn(nnote,velocity,nfreq,nbuffer,time);
    NoteStatus[nnote]=NoteStatusOn;
    if(noti!=nullptr)
        noti->Notify(evon);
    NotesOn.push_back(evon);
    ChangedEvents=true;
};
void GuitarFreqAnalysis::NoteOff(unsigned short nnote,unsigned long nbuffer,clock_t time)
{
    if(time==0)
        time=clock();

    NoteStatus[nnote]=NoteStatusOff;
    if(noti!=nullptr)
    {
        FreqEvent evoff;
        evoff.evNoteOff(nnote,nbuffer,time);
        noti->Notify(evoff);

    }
    std::vector<FreqEvent>::iterator vi=NotesOn.begin();
    while(vi!=NotesOn.end())
    {
        unsigned short nn=(*vi).note;

        if(nn==nnote)
        {
            NotesOn.erase(vi);
            break;
        }
        vi++;

    }

    //ChangedEvents=true;
};
GuitarFreqAnalysis::~GuitarFreqAnalysis()
{;
}
