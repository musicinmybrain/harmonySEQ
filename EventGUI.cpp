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

    set_title(parent->GetLabel());
    set_border_width(5);
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

    label_type.set_text(_("Type:"));
    label_channel.set_text(_("Channel:"));
    label_controller.set_text(_("Controller:"));
    label_key.set_text(_("Key:"));
    label_note.set_text(_("Note:"));

    Types_combo.pack_start(m_columns_event_types.label);
    Types_combo.set_active(parent->type);
    Types_combo.signal_changed().connect(mem_fun(*this,&EventGUI::TypeChanged));
    Keys_combo.pack_start(m_columns_key_codes.label);
    Channels_combo.pack_start(m_columns_channels.label);

    main_box.pack_start(ok_button,Gtk::PACK_SHRINK);
    ok_button.set_label(_("OK"));
    ok_button.signal_clicked().connect(mem_fun(*this,&EventGUI::OnOKClicked));

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
        case Event::EVENT_TYPE_NONE:

            break;
        case Event::EVENT_TYPE_CONTROLLER:
            line_controller.show();
            line_channel.show();
            break;
        case Event::EVENT_TYPE_NOTE:
            line_note.show();
            line_channel.show();
            break;
        case Event::EVENT_TYPE_KEYBOARD:
            line_key.show();
            break;

    }
    resize(2,2);
}

void EventGUI::OnOKClicked(){
    Gtk::TreeModel::Row row = *(Types_combo.get_active());
    int type = row[m_columns_event_types.type];
    parent->type = type;
    switch (type){
        case Event::EVENT_TYPE_NONE:

            break;
        case Event::EVENT_TYPE_KEYBOARD:
            parent->arg1 = (*(Keys_combo.get_active()))[m_columns_key_codes.keycode];
            break;
        case Event::EVENT_TYPE_NOTE:
            parent->arg1 = note_spinbutton.get_value();
            parent->arg2 = (*(Channels_combo.get_active()))[m_columns_channels.ch];
            break;
        case Event::EVENT_TYPE_CONTROLLER:
            parent->arg1 =ctrl_spinbutton.get_value();
            parent->arg2 = (*(Channels_combo.get_active()))[m_columns_channels.ch];
            break;
    }
    
    eventswindow->UpdateRow(parent->row_in_event_window);
    hide();
}

void EventGUI::UpdateValues(){


}

//===============================================


void InitEventTypesTreeModel(){

    m_refTreeModel_EventTypes = Gtk::ListStore::create(m_columns_event_types);
    Gtk::TreeModel::Row row = *(m_refTreeModel_EventTypes->append());
    row[m_columns_event_types.type] = Event::EVENT_TYPE_NONE;
    row[m_columns_event_types.label] = _("Empty");
    row = *(m_refTreeModel_EventTypes->append());
    row[m_columns_event_types.type] = Event::EVENT_TYPE_KEYBOARD;
    row[m_columns_event_types.label] = _("Keyboard");
    row = *(m_refTreeModel_EventTypes->append());
    row[m_columns_event_types.type] = Event::EVENT_TYPE_NOTE;
    row[m_columns_event_types.label] = _("Note");
    row = *(m_refTreeModel_EventTypes->append());
    row[m_columns_event_types.type] = Event::EVENT_TYPE_CONTROLLER;
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
