/*
    Copyright (C) 2010 Rafał Cieślak

    This file is part of harmonySEQ.

    HarmonySEQ is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    HarmonySEQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with HarmonySEQ.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "Chord.h"

#include "messages.h"
extern debug *dbg;
int STANDARD_GUITAR_TUNING[6] =
                                    {0,5,10,15,19,24};

int GUITAR_TABS_MAJOR[12][6] = {
    //here we go!
    {0,3,2,0,1,0}, //C
    {4,4,6,6,6,4}, //C#
    {0,0,0,2,3,2}, //D
    {6,6,8,8,8,6}, //D#
    {0,2,2,1,0,0}, //E
    {1,3,3,2,1,1}, //F
    {2,4,4,3,2,2}, //F#
    {3,2,0,0,0,3}, //G
    {4,6,6,5,4,4}, //G#
    {0,0,2,2,2,0}, //A
    {1,1,3,3,3,1}, //A#
    {2,2,4,4,4,2}  //H

};

int GUITAR_TABS_MINOR[12][6] = {
    //here we go!
    {3,3,5,5,4,3}, //C
    {4,4,6,6,5,4}, //C#
    {0,0,0,2,3,1}, //D
    {6,6,8,8,7,6}, //D#
    {0,2,2,0,0,0}, //E
    {1,3,3,1,1,1}, //F
    {2,4,4,2,2,2}, //F#
    {3,5,5,3,3,3}, //G
    {4,6,6,4,4,4}, //G#
    {0,0,2,2,1,0}, //A
    {1,1,3,3,2,1}, //A#
    {2,2,4,4,3,2}  //H

};

Chord::Chord(){
    type = CHORD_TYPE_TRIAD;
    inversion = 0;
    root = 0;
    mode_guitar = CHORD_GUITAR_MODE_MAJOR;
    mode_triad = CHORD_TRIAD_MODE_MAJOR;
    base_octave = 0;
    base_note=0;
    NoteAndOctaveToBase();
}

Chord::~Chord(){
}

void Chord::RecalcNotes(){
    *dbg<<"recalculating chord\n";
    int n1,n2,n3;
    switch (type){
        case CHORD_TYPE_CUSTOM:

            //well, in this case nothing is to be recalculated.
            break;
        case CHORD_TYPE_GUITAR:
            if (mode_guitar == CHORD_GUITAR_MODE_MAJOR){
                for(int x = 0; x < 6; x++)
                    notes[x]=STANDARD_GUITAR_TUNING[x] + GUITAR_TABS_MAJOR[root][x];
            }else if (mode_guitar == CHORD_GUITAR_MODE_MINOR){
                for(int x = 0; x < 6; x++)
                    notes[x]=STANDARD_GUITAR_TUNING[x] + GUITAR_TABS_MINOR[root][x];
            }
            break;
        case CHORD_TYPE_TRIAD:
            n1 =root;
            *dbg << "    base+triad_root  = " << n1 << ENDL;
            if (mode_triad == CHORD_TRIAD_MODE_MAJOR || mode_triad == CHORD_TRIAD_MODE_AUGMENTED) n2 = n1+4;
            else n2 = n1+3;
            if(mode_triad == CHORD_TRIAD_MODE_MINOR || mode_triad == CHORD_TRIAD_MODE_AUGMENTED) n3 = n2+4;
            else n3 = n2+3;



            if (inversion == 0) {
                notes[0] = n1;
                notes[1] = n2;
                notes[2] = n3;
                notes[3] = n1 + 12;
                notes[4] = n2 + 12;
                notes[5] = n3 + 12;
            }else if (inversion == 1){
                notes[0] = n3 - 12;
                notes[1] = n1;
                notes[2] = n2;
                notes[3] = n3;
                notes[4] = n1 + 12;
                notes[5] = n2 + 12;
            }else if (inversion == 2){
                notes[0] = n2 - 12;
                notes[1] = n3 - 12;
                notes[2] = n1;
                notes[3] = n2;
                notes[4] = n3;
                notes[5] = n1 + 12;
            }

            break;
    }

}

int Chord::GetNotePlusBasenote(int n){
    if (n > 5 || n < 0) return 0;
    if (base_use)
        return notes[n] + base;
    else
        return notes[n];
}

int Chord::GetNote(int n){
    if (n > 5 || n < 0) return 0;
    return notes[n];
}

void Chord::SetNote(int note, int pitch){
    if (note > 5 || note < 0) return;
    type = CHORD_TYPE_CUSTOM;
    notes[note] = pitch;
    //RecalcNotes(); //well, in fact: not needed;
}

void Chord::SetRoot(int pitch){
    root = pitch%12;
    RecalcNotes();
}

int Chord::GetRoot(){
    return root;
}


void Chord::SetType(int n){
    type = n;
    if(type == CHORD_TYPE_GUITAR || type == CHORD_TYPE_TRIAD) base_use =1;
    if(type == CHORD_TYPE_GUITAR) base_note = 4;
    if(type == CHORD_TYPE_TRIAD) base_note = 0;
    RecalcNotes();

}

int Chord::GetType(){
    return type;
}

void Chord::SetTriadMode(int n){
    mode_triad = n;
    RecalcNotes();
}

int Chord::GetTriadMode(){
    return mode_triad;
}

void Chord::SetGuitarMode(int n){
    mode_guitar = n;
    RecalcNotes();
}

int Chord::GetGuitarMode(){
    return mode_guitar;
}

void Chord::SetInversion(int n){
    inversion = n;
    RecalcNotes();
}

int Chord::GetInversion(){
    return inversion;
}

void Chord::SetBaseOctave(int n){
    base_octave = n;
    NoteAndOctaveToBase();
    RecalcNotes();
}

int Chord::GetBaseOctave(){
    return base_octave;
}

void Chord::SetBaseNote(int n){
    base_note = n;
    NoteAndOctaveToBase();
    RecalcNotes();
}
int Chord::GetBaseNote(){
    return base_note;
}

void Chord::SetBase(int n){
    base = n;
    BaseToOctaveAndNote();
    RecalcNotes();
}
int Chord::GetBase(){
    return base;
}

void Chord::SetBaseUse(bool use){
    base_use = use;
    //no need to recalc. base_use is checked on GetNote();
}
bool Chord::GetBaseUse(){
    return base_use;
}

void Chord::BaseToOctaveAndNote(){
    int oct = base/12;
    base_octave = oct-5;
    base_note = base - (base_octave+5)*12;
}

void Chord::NoteAndOctaveToBase(){
    base = (base_octave+5)*12+base_note;
}

void Chord::Set(const Chord& other){
    *dbg << "copying chord." << ENDL;
    type = other.type;
    if (type == CHORD_TYPE_CUSTOM) for (int x = 0 ; x < 6; x++) notes[x] = other.notes[x];
    base = other.base;
    base_use = other.base_use;
    BaseToOctaveAndNote();
    mode_triad = other.mode_triad;
    mode_guitar = other.mode_guitar;
    root = other.root;
    inversion = other.inversion;
    RecalcNotes();
}


Glib::ustring Chord::GetName(){
    char temp[100];
    Glib::ustring a;
    switch (type){
        case CHORD_TYPE_CUSTOM:
            sprintf(temp,_("Custom: %d, %d, %d, %d, %d, %d"),notes[0],notes[1],notes[2],notes[3],notes[4],notes[5]);
            break;
        case CHORD_TYPE_GUITAR:
            if (mode_guitar == CHORD_GUITAR_MODE_MAJOR){
                sprintf(temp,_("Guitar: %s major"),notemap.find(root)->second.c_str());
            }else if (mode_guitar == CHORD_GUITAR_MODE_MINOR){
                sprintf(temp,_("Guitar: %s minor"),notemap.find(root)->second.c_str());
            }
            break;
        case CHORD_TYPE_TRIAD:
            if (mode_triad ==  CHORD_TRIAD_MODE_MAJOR){
                sprintf(temp,_("Triad: %s major"),notemap.find(root)->second.c_str());
            } else
            if (mode_triad == CHORD_TRIAD_MODE_MINOR){
                sprintf(temp,_("Triad: %s minor"),notemap.find(root)->second.c_str());
            } else
            if (mode_triad ==  CHORD_TRIAD_MODE_AUGMENTED){
                sprintf(temp,_("Triad: %s augumented"),notemap.find(root)->second.c_str());
            } else
            if (mode_triad ==  CHORD_TRIAD_MODE_DIMINICHED){
                sprintf(temp,_("Triad: %s diminished"),notemap.find(root)->second.c_str());
            }
            break;
    }

    a = temp;
    return a;

}

std::vector<int> Chord::SaveToVector(){
   /**Should have following format: type, root, guitar_mode, triad_mode,inversion,base,use_base, notes(6)(if custom)*/
    std::vector<int> V;
    V.push_back(type);
    V.push_back(root);
    V.push_back(mode_guitar);
    V.push_back(mode_triad);
    V.push_back(inversion);
    V.push_back(base);
    V.push_back(base_use);
    if (type == CHORD_TYPE_CUSTOM) for (int x = 0; x < 6; x++) V.push_back(notes[x]);
    return V;
}

void Chord::SetFromVector(std::vector<int>& V){
    if (V.size() < 7) {
        *err << "ERROR: chord vector too small\n";
        return;
    }
    type = V[0];
    root = V[1];
    mode_guitar = V[2];
    mode_triad = V[3];
    inversion = V[4];
    base = V[5];
    base_use = V[6];
    BaseToOctaveAndNote();
    if(type == CHORD_TYPE_CUSTOM)for (int x = 0; x < 6; x++) notes[x] = V[7+x];
    RecalcNotes();
}


   void Chord::SetFromVector_OLD_FILE_PRE_0_14(std::vector<int> &V){
   /**Old files have following format: mode, guitar_root, guitar_note, triad_root,triad_note,octave,inversion,notes(6)(if custom)*/
    if (V.size() < 7) {
        *err << "ERROR: chord vector too small\n";
        return;
    }
    type = V[0];
    if (type == CHORD_TYPE_GUITAR) root = V[1];
    else root = V[3];

    mode_guitar = V[2];
    mode_triad = V[4];
    base_octave = V[5];
    NoteAndOctaveToBase();
    inversion = V[6];
    if(type == CHORD_TYPE_CUSTOM)for (int x = 0; x < 6; x++) notes[x] = V[7+x];
    else RecalcNotes();
   }