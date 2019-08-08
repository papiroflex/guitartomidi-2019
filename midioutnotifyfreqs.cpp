#include "midioutnotifyfreqs.h"
#include <iostream>
#include <algorithm>

MidiOutNotifyFreqs::MidiOutNotifyFreqs()
{/*
   QMap<QString,QString> dev=midisynth.devices();
   QMap<QString,QString>::iterator it=dev.begin();
   std::cout<<"Midi Devices:"<<std::endl;
   QMidiEvent ev;*/
   /*   QString s;
   while(it!=dev.end())
   {
       s=(*(*it).begin());
       std::cout<<(*it).begin()<<std::endl;
       it++;
   }*/
//   midisynth.connect("localhost:0");
  //  midisynth.connect("129:0");
   //midisynth.connect(s);
    //midisynth.setInstrument(0,1);
//    midisynth.
 //   std::cout<<"connected to midi device : "<<"hw:PCH"<<std::endl;

}
void MidiOutNotifyFreqs::Notify(FreqEvent e)
{
    SystemNotifyFreqs::Notify(e);

    switch (e.eventtype) {
        case FreqEvent::NoteOn:
            std::cout<<std::endl<<"NoteOn:";
            midisynth.noteOn(e.note,1,std::max((unsigned)64,(unsigned)e.velocity));
        break;
        case FreqEvent::NoteOff:
            midisynth.noteOff(e.note,0);
        break;
    }
};
