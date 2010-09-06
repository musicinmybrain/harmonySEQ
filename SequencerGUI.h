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


#ifndef SEQUENCERGUI_H
#define	SEQUENCERGUI_H
#include "global.h"
//#include "Sequencer.h"
class Sequencer;

class SequencerWindow : public Gtk::Window {
public:
    SequencerWindow(Sequencer* prt);
    virtual ~SequencerWindow();
    void UpdateValues();
    Gtk::VBox box_of_sliders;
    Gtk::HBox box_of_notes;
    Gtk::HScale *sequence_scales[8];
    Gtk::SpinButton* note_buttons[6];
    Gtk::SpinButton channel_button;
    Gtk::Label channellabel;
    Gtk::HBox low_hbox;
    Gtk::VBox toggle_vbox;
    Gtk::CheckButton tgl_apply_mainnote, tgl_mute;
private:
    void OnNotesChanged(int note);
    void OnSequenceChanged(int seq);
    void OnChannelChanged();
    void OnToggleMuteToggled();
    void OnToggleApplyMainNoteToggled();
public:
    Sequencer *parent;

};

#endif	/* SEQUENCERGUI_H */

