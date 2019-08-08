#ifndef MIDIOUTNOTIFYFREQS_H
#define MIDIOUTNOTIFYFREQS_H
#include "guitarfreqanalysis.h"
#include <../../../../extern/QMidi-master/src/QMidiOut.h>
#include <../../../../extern/QMidi-master/src/QMidiFile.h>


class MidiOutNotifyFreqs : public SystemNotifyFreqs
{
public:
    QMidiOut midisynth;
    MidiOutNotifyFreqs();
    virtual void Notify(FreqEvent e);
};

#endif // MIDIOUTNOTIFYFREQS_H
