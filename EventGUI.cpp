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
#define I_DO_NOT_WANT_EXTERNS_FROM_EVENT_GUI
#include "EventGUI.h"
#undef I_DO_NOT_WANT_EXTERNS_FROM_EVENT_GUI
#include "Event.h"

ModelColumns_EventTypes m_columns_event_types;
Glib::RefPtr<Gtk::ListStore> m_refTreeModel_EventTypes;
ModelColumns_KeyCodes m_columns_key_codes;
Glib::RefPtr<Gtk::ListStore> m_refTreeModel_KeyCodes;
ModelColumns_Channels m_columns_channels;
Glib::RefPtr<Gtk::ListStore> m_refTreeModel_Channels;

EventGUI::EventGUI(Event *prt){
    parent = prt;

    set_title(_("Event"));
    set_border_width(5);
    set_transient_for(*eventswindow);
    set_position(Gtk::WIN_POS_CENTER_ON_PARENT);
    //set_modal(1);
    add(main_box);
    main_box.set_spacing(5);
    //preparing
    Types_combo.set_model(m_refTreeModel_EventTypes);
    Keys_combo.set_model(m_refTreeModel_KeyCodes);
    Keys_combo.set_wrap_width(3); //three columns
    Channels_combo.set_model(m_refTreeModel_Channels);

    main_box.pack_start(line_type);
    main_box.pack_start(line_key);
    main_box.pack_start(line_note);
    main_box.pack_start(line_controller);
    main_box.pack_start(line_channel);
    main_box.pack_start(separator);
    main_box.pack_start(label_preview);
    line_type.pack_start(label_type,Gtk::PACK_SHRINK);
    line_key.pack_start(label_key,Gtk::PACK_SHRINK);
    line_note.pack_start(label_note,Gtk::PACK_SHRINK);
    line_controller.pack_start(label_controller,Gtk::PACK_SHRINK);
    line_channel.pack_start(label_channel,Gtk::PACK_SHRINK);

    line_type.pack_start(Types_combo,Gtk::PACK_SHRINK);
    line_key.pack_start(Keys_combo,Gtk::PACK_SHRINK);
    line_note.pack_start(note_spinbutton,Gtk::PACK_SHRINK);
    line_controller.pack_start(ctrl_spinbutton,Gtk::PACK_SHRINK);
    line_channel.pack_start(Channels_combo,Gtk::PACK_SHRINK);

    note_spinbutton.set_range(0.0,127.0);
    ctrl_spinbutton.set_range(0.0,127.0);
    note_spinbutton.set_increments(1.0,16.0);
    ctrl_spinbutton.set_increments(1.0,16.0);
    note_spinbutton.signal_value_changed().connect(mem_fun(*this,&EventGUI::OnNoteChanged));
    ctrl_spinbutton.signal_value_changed().connect(mem_fun(*this,&EventGUI::OnCtrlChanged));

    label_type.set_text(_("Type:"));
    label_channel.set_text(_("Channel:"));
    label_controller.set_text(_("Controller:"));
    label_key.set_text(_("Key:"));
    label_note.set_text(_("Note:"));

    Types_combo.pack_start(m_columns_event_types.label);
    Types_combo.set_active(parent->type);
    Types_combo.signal_changed().connect(mem_fun(*this,&EventGUI::OnTypeChanged));
    Keys_combo.pack_start(m_columns_key_codes.label);
    Keys_combo.signal_changed().connect(mem_fun(*this,&EventGUI::OnKeyChanged));
    Channels_combo.pack_start(m_columns_channels.label);
    Channels_combo.signal_changed().connect(mem_fun(*this,&EventGUI::OnChannelChanged));

    main_box.pack_start(ok_button,Gtk::PACK_SHRINK);
    ok_button.set_label(_("OK"));
    ok_button.signal_clicked().connect(mem_fun(*this,&EventGUI::OnOKClicked));

    signal_show().connect(mem_fun(*this,&EventGUI::UpdateValues));

    label_preview.set_text(parent->GetLabel());
    show_all_children(1);
    TypeChanged(); // to hide some of widgets according to the type
    hide();
}


EventGUI::~EventGUI(){
}

void EventGUI::TypeChanged(){
    Gtk::TreeModel::Row row = *(Types_combo.get_active());
    int type = row[m_columns_event_types.type];
    *dbg << "type is - " << type << ENDL;
    line_key.hide();
    line_note.hide();
    line_controller.hide();
    line_channel.hide();

    switch (type){
        case Event::NONE:

            break;
        case Event::CONTROLLER:
            line_controller.show();
            line_channel.show();
            break;
        case Event::NOTE:
            line_note.show();
            line_channel.show();
            break;
        case Event::KEYBOARD:
            line_key.show();
            break;

    }
    resize(2,2);
}

void EventGUI::OnTypeChanged(){
    Gtk::TreeModel::Row row = *(Types_combo.get_active());
    int type = row[m_columns_event_types.type];
    parent->type = type;
    TypeChanged();
    switch (parent->type){
        case Event::NONE:

            break;
        case Event::KEYBOARD:
            Keys_combo.set_active(0);
            break;
        case Event::NOTE:
            note_spinbutton.set_value(0.0);
            parent->arg1=0;
            Channels_combo.set_active(0);
            break;
        case Event::CONTROLLER:
            ctrl_spinbutton.set_value(0.0);
            parent->arg1=0;
            Channels_combo.set_active(0);
            break;

    }
    label_preview.set_text(parent->GetLabel());
    eventswindow->UpdateRow(parent->row_in_event_window);
}



void EventGUI::OnChannelChanged(){
    if(parent->type == Event::CONTROLLER || parent->type == Event::NOTE){

            parent->arg2 = (*(Channels_combo.get_active()))[m_columns_channels.ch];
        
    }else *err << _("Error: channel has changed, while event is not MIDI-type.") << ENDL;

    label_preview.set_text(parent->GetLabel());
    eventswindow->UpdateRow(parent->row_in_event_window);
}

void EventGUI::OnKeyChanged(){
    if(parent->type == Event::KEYBOARD){
            parent->arg1 = (*(Keys_combo.get_active()))[m_columns_key_codes.keycode];
    }else *err << _("Error: key has changed, while event is not key-type.") << ENDL;

    label_preview.set_text(parent->GetLabel());
    eventswindow->UpdateRow(parent->row_in_event_window);
}

void EventGUI::OnCtrlChanged(){
    if(parent->type == Event::CONTROLLER){
        parent->arg1 = ctrl_spinbutton.get_value();
    }else *err << _("Error: controller has changed, while event is not ctrl-type.") << ENDL;

    label_preview.set_text(parent->GetLabel());
    eventswindow->UpdateRow(parent->row_in_event_window);

}

void EventGUI::OnNoteChanged(){
    if(parent->type == Event::NOTE){
        parent->arg1 = note_spinbutton.get_value();
    }else *err << _("Error: note has changed, while event is not note-type.") << ENDL;

    label_preview.set_text(parent->GetLabel());
    eventswindow->UpdateRow(parent->row_in_event_window);

}
void EventGUI::OnOKClicked(){
    hide();
}

void EventGUI::UpdateValues(){
    Gtk::TreeModel::iterator it = m_refTreeModel_EventTypes->get_iter("0");
    Gtk::TreeModel::Row row;
    for (;it;it++){
        row = *it;
        if (row[m_columns_event_types.type] == parent->type)
            Types_combo.set_active(it);
    }
    switch (parent->type){
        case Event::NONE:

            break;
        case Event::KEYBOARD:
            it = m_refTreeModel_KeyCodes->get_iter("0");

            for (; it; it++) {
                row = *it;
                if(row[m_columns_key_codes.keycode]==parent->arg1)
                    Keys_combo.set_active(it);
            }

            break;
        case Event::NOTE:
            note_spinbutton.set_value(parent->arg1);
            Channels_combo.set_active(parent->arg2);
            break;
        case Event::CONTROLLER:
            ctrl_spinbutton.set_value(parent->arg1);
            Channels_combo.set_active(parent->arg2);
            break;

    }

    label_preview.set_text(parent->GetLabel());

}

//===============================================


void InitEventTypesTreeModel(){

    m_refTreeModel_EventTypes = Gtk::ListStore::create(m_columns_event_types);
    Gtk::TreeModel::Row row = *(m_refTreeModel_EventTypes->append());
    row[m_columns_event_types.type] = Event::NONE;
    row[m_columns_event_types.label] = _("Empty");
    row = *(m_refTreeModel_EventTypes->append());
    row[m_columns_event_types.type] = Event::KEYBOARD;
    row[m_columns_event_types.label] = _("Keyboard");
    row = *(m_refTreeModel_EventTypes->append());
    row[m_columns_event_types.type] = Event::NOTE;
    row[m_columns_event_types.label] = _("Note");
    row = *(m_refTreeModel_EventTypes->append());
    row[m_columns_event_types.type] = Event::CONTROLLER;
    row[m_columns_event_types.label] = _("Controller");
}

void InitKeyTypesTreeModel(){
    m_refTreeModel_KeyCodes = Gtk::ListStore::create(m_columns_key_codes);
    Gtk::TreeModel::Row row;
    std::map<int,string>::iterator iter = keymap_itos.begin();
    for (;iter != keymap_itos.end();iter++){
        row = *(m_refTreeModel_KeyCodes->append());
        row[m_columns_key_codes.keycode] = iter->first;
        row[m_columns_key_codes.label] = iter->second;
    }

}

void InitChannelsTreeModel(){
    m_refTreeModel_Channels = Gtk::ListStore::create(m_columns_channels);
    Gtk::TreeModel::Row row = *(m_refTreeModel_Channels->append());
    row[m_columns_channels.ch] = 0;
    row[m_columns_channels.label] = _("All");
    char temp[3];
    for (int x = 1; x < 17; x++){
        row = *(m_refTreeModel_Channels->append());
        *dbg << "-----" << x << ENDL;
        sprintf(temp,"%d",x);
        row[m_columns_channels.ch] = x;
        row[m_columns_channels.label] = temp;
    }
}

void InitAllTreeModels(){
    InitEventTypesTreeModel();
    InitKeyTypesTreeModel();
    InitChannelsTreeModel();
}