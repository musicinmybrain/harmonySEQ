/*
    Copyright (C) 2010-2012, 2020 Rafał Cieślak

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

#include "MainWindow.hpp"

#include "Configuration.hpp"
#include "ControlSequencer.hpp"
#include "Event.hpp"
#include "Files.hpp"
#include "MidiDriver.hpp"
#include "NoteSequencer.hpp"
#include "SequencerManager.hpp"
#include "SettingsWindow.hpp"
#include "TreeModels.hpp"
#include "config.hpp"
#include "messages.hpp"
#include "resources.hpp"
#include "shared.hpp"


extern MidiDriver* midi;
extern SettingsWindow* settingswindow;

// Time (ms) how long does the tempo button blink on beat.
#define FLASH_INTERVAL 25

#define TREEVIEW_COLOR_ON Color(0.0, 1.0, 0.0, 0.5)
#define TREEVIEW_COLOR_OFF Color(0.0, 0.0, 0.0, 0.0)
#define TREEVIEW_COLOR_PRE_P1 Color(1.0, 0.87, 0.0, 0.5)
#define TREEVIEW_COLOR_P1 Color(1.0, 1.0, 0.0, 0.5)

bool CtrlKeyDown;
bool ShiftKeyDown;


Gtk::TreeModel::iterator row_inserted_by_drag;
bool seq_list_drag_in_progress;

MainWindow::MainWindow()
{
    set_name("mainwindow");

    set_border_width(0);
    set_default_size(950, 600);
    set_size_request(900, 450); //minimum size
    UpdateTitle();

    wTempoLabel.set_text(_("Tempo:"));
    add(wMainVBox);

    m_refActionGroup = Gtk::ActionGroup::create();

    m_refActionGroup->add(Gtk::Action::create("MenuFile",_("File")));
    m_refActionGroup->add(Gtk::Action::create("MenuHelp",_( "Help")));
    m_refActionGroup->add(Gtk::Action::create("MenuTools",_( "Tools")));
    m_refActionGroup->add(Gtk::Action::create("FileNew", Gtk::Stock::NEW,_("New"),_("Creates a new file.")), std::bind(&MainWindow::OnMenuNewClicked, this));
    m_refActionGroup->add(Gtk::Action::create("FileOpen", Gtk::Stock::OPEN,_("Open"),_("Opens a file.")), std::bind(&MainWindow::OnMenuOpenClicked, this));
    m_refActionGroup->add(Gtk::Action::create("FileSave", Gtk::Stock::SAVE,_("Save"),_("Saves the current file.")), std::bind(&MainWindow::OnMenuSaveClicked, this));
    m_refActionGroup->add(Gtk::Action::create("FileSaveAs", Gtk::Stock::SAVE_AS,_("Save as..."),_("Saves the current file with a different name.")), std::bind(&MainWindow::OnMenuSaveAsClicked, this));
    m_refActionGroup->add(Gtk::Action::create("FileQuit", Gtk::Stock::QUIT,_("Quit"),_("Quits harmonySEQ.")), std::bind(&MainWindow::OnMenuQuitClicked, this));
    m_refActionGroup->add(Gtk::Action::create("Preferences", Gtk::Stock::PREFERENCES,_("Preferences"),_("harmonySEQ configuration.")), std::bind(&MainWindow::OnPreferencesClicked, this));
    m_refActionGroup->add(Gtk::Action::create("AddNoteSeq", _("Add note sequencer"),_("Adds a new note seqencer. Note sequencers store melodies and output them as MIDI notes.")), std::bind(&MainWindow::OnAddNoteSeqClicked, this));
    m_refActionGroup->add(Gtk::Action::create("AddCtrlSeq", _("Add control sequencer"),_("Adds a new control seqencer. Control sequencers store a graph of a particular setting, and output it as MIDI control messages.")), std::bind(&MainWindow::OnAddControlSeqClicked, this));
    m_refActionGroup->add(Gtk::Action::create("RemoveSeq", Gtk::Stock::REMOVE, _("Remove"),_("Removes selected sequencer")), std::bind(&MainWindow::OnRemoveClicked, this));
    m_refActionGroup->add(Gtk::Action::create("DuplicateSeq", Gtk::Stock::CONVERT, _("Duplicate"), _("Duplicates selected sequencer")), std::bind(&MainWindow::OnCloneClicked, this));
    m_refActionGroup->add(Gtk::Action::create("About", Gtk::Stock::ABOUT), std::bind(&MainWindow::OnAboutMenuClicked, this));
    m_refActionGroup->add(Gtk::ToggleAction::create("MIDIClock", _("MIDIClock"),_("Toggle MIDI clock and start/stop messages output")));
    m_refActionGroup->add(Gtk::Action::create("Sync", Gtk::Stock::MEDIA_PLAY, _("Sync"),_("Toggle MIDI clock and start/stop messages output")));
    m_refActionGroup->add(Gtk::ToggleAction::create("Metronome", _("Metronome"),_("Toggle metronome on/off")), std::bind(&MainWindow::OnMetronomeToggleClicked, this));
    m_refActionGroup->add(Gtk::Action::create("Tap", Gtk::Stock::MEDIA_PLAY, _("Tap"),_("Tap multiple times to calculate and set BPM")));
    m_refActionGroup->add(Gtk::Action::create("PlayPause", Gtk::Stock::MEDIA_PAUSE, _("Play/Pause"),_("Toggle play/pause")), std::bind(&MainWindow::OnPlayPauseClicked, this));
    m_refActionGroup->add(Gtk::ToggleAction::create("PassMidiEvents", _("Pass MIDI events"),_("States whether MIDI events are passed-through harmonySEQ.")), std::bind(&MainWindow::OnPassToggleClicked, this));
    //m_refActionGroup->add(Gtk::ToggleAction::create("PlayOnEdit", _("Play on edit"),_("If on, harmonySEQ will play a brief preview of note, when it's added, or changed manually in chord.")), std::bind(&MainWindow::OnPlayOnEditClicked, this));
    m_refActionGroup->add(Gtk::Action::create("seq/PlayOnce", Gtk::Stock::MEDIA_NEXT, _("Play once"), _("Plays the sequence once.")), std::bind(&MainWindow::OnPopupPlayOnce, this));
    m_refActionGroup->add(Gtk::Action::create("seq/Remove", Gtk::Stock::REMOVE, _("Remove"), _("Removes the sequencer.")), std::bind(&MainWindow::OnPopupRemove, this));
    m_refActionGroup->add(Gtk::Action::create("seq/Duplicate", Gtk::Stock::CONVERT, _("Duplicate"), _("Duplicates the sequencer")), std::bind(&MainWindow::OnPopupDuplicate, this));

    m_refActionGroup->add(Gtk::Action::create("Empty"));

    m_refUIManager = Gtk::UIManager::create();
    m_refUIManager->insert_action_group(m_refActionGroup);
    add_accel_group(m_refUIManager->get_accel_group());

    Glib::ustring ui_info =
            "<ui>"
            "  <menubar name='MenuBar'>"
            "    <menu action='MenuFile'>"
            "      <menuitem action='FileNew'/>"
            "      <separator/>"
            "      <menuitem action='FileOpen'/>"
            "      <menuitem action='FileSave'/>"
            "      <menuitem action='FileSaveAs'/>"
            "      <separator/>"
            "      <menuitem action='FileQuit'/>"
            "    </menu>"
            "    <menu action='MenuTools'>"
            "      <menuitem action='PassMidiEvents'/>"
            "      <separator/>"
            "      <menuitem action='Preferences'/>"
            "    </menu>"
            "    <menu action='MenuHelp'>"
            "      <menuitem action='About'/>"
            "    </menu>"
            "  </menubar>"

            "  <toolbar name='ToolBar'>"
            "   <toolitem action='FileNew'/>"
            "   <toolitem action='FileOpen'/>"
            "   <toolitem action='FileSave'/>"
            "   <toolitem action='FileSaveAs'/>"
            "   <separator/>"
            "   <toolitem action='AddNoteSeq'/>"
            "   <toolitem action='AddCtrlSeq'/>"
            "   <toolitem name='RemoveTool' action='RemoveSeq'/>"
            "   <toolitem name='DuplicateTool' action='DuplicateSeq'/>"
            "   <separator expand='true'/>"
            "   <toolitem name='MIDIClock' action='MIDIClock'/>"
            "   <toolitem name='Sync' action='Sync'/>"
            "   <toolitem name='Metronome' action='Metronome'/>"
            "   <toolitem name='TempoLabel' action='Empty'/>"
            "   <toolitem name='Tempo' action='Empty'/>"
            "   <toolitem name='Tap' action='Tap'/>"
            "   <toolitem name='PlayPauseTool' action='PlayPause'/>"
            "  </toolbar>"

            "  <popup name='Popup'>"
            "   <menuitem action='seq/PlayOnce'/>"
            "   <separator/>"
            "   <menuitem action='seq/Duplicate'/>"
            "   <menuitem action='seq/Remove'/>"
            "  </popup>"
            "</ui>";
#ifdef GLIBMM_EXCEPTIONS_ENABLED
    try {
        m_refUIManager->add_ui_from_string(ui_info);
    } catch (const Glib::Error& ex) {
        *err << _("ERROR - error while building menus: ") << ex.what();
    }
#else
    std::auto_ptr<Glib::Error> ex;
    m_refUIManager->add_ui_from_string(ui_info, ex);
    if (ex.get()) {
        *err << _("ERROR - error while building menus: ") << ex->what();
    }
#endif //GLIBMM_EXCEPTIONS_ENABLED

    Gtk::Widget* pMenubar = m_refUIManager->get_widget("/MenuBar");
    Gtk::Widget* pToolbar = m_refUIManager->get_widget("/ToolBar");
    Gtk::Widget* pPopup = m_refUIManager->get_widget("/Popup");

    Gtk::Menu& Popup = dynamic_cast<Gtk::Menu&>(*pPopup);
    wPopupMenu = &Popup;
    wPopupMenu->accelerate(*this);

    Gtk::Toolbar& Toolbar = dynamic_cast<Gtk::Toolbar&> (*pToolbar);
    Toolbar.set_toolbar_style(Gtk::TOOLBAR_BOTH_HORIZ);
    Toolbar.set_border_width(0);

    Gtk::Widget* pRemoveTool = m_refUIManager->get_widget("/ToolBar/RemoveTool");
    pRemoveTool->set_sensitive(0);
    Gtk::Widget* pDuplicateTool = m_refUIManager->get_widget("/ToolBar/DuplicateTool");
    pDuplicateTool->set_sensitive(0);
    Gtk::Widget* pPlayPauseTool = m_refUIManager->get_widget("/ToolBar/PlayPauseTool");
    Gtk::ToolItem& PlayPauseTool = dynamic_cast<Gtk::ToolItem&> (*pPlayPauseTool);
    PlayPauseTool.set_is_important(1); // will display text text to the icon
    Gtk::Widget* pMetronome = m_refUIManager->get_widget("/ToolBar/Metronome");
    Gtk::ToggleToolButton& MetronomeTool = dynamic_cast<Gtk::ToggleToolButton&> (*pMetronome);
    MetronomeTool.set_active(false);
    Gtk::Widget* pMidiClock = m_refUIManager->get_widget("/ToolBar/MIDIClock");
    Gtk::ToggleToolButton& MidiClockTool = dynamic_cast<Gtk::ToggleToolButton&> (*pMidiClock);
    Gtk::Widget* pSync = m_refUIManager->get_widget("/ToolBar/Sync");
    Gtk::ToolButton& SyncTool = dynamic_cast<Gtk::ToolButton&> (*pSync);
    Gtk::Widget* pTap = m_refUIManager->get_widget("/ToolBar/Tap");
    Gtk::ToolButton& TapTool = dynamic_cast<Gtk::ToolButton&> (*pTap);
    Gtk::Widget* pAddCtrl = m_refUIManager->get_widget("/ToolBar/AddCtrlSeq");
    Gtk::ToolButton& AddCtrlTool = dynamic_cast<Gtk::ToolButton&> (*pAddCtrl);
    Gtk::Widget* pAddNote = m_refUIManager->get_widget("/ToolBar/AddNoteSeq");
    Gtk::ToolButton& AddNoteTool = dynamic_cast<Gtk::ToolButton&> (*pAddNote);
    Gtk::Widget* pTempo = m_refUIManager->get_widget("/ToolBar/Tempo");
    Gtk::ToolItem& TempoTool = dynamic_cast<Gtk::ToolItem&> (*pTempo);
    Gtk::Widget* pTempoLabelTool = m_refUIManager->get_widget("/ToolBar/TempoLabel");
    Gtk::ToolItem& TempoLabelTool = dynamic_cast<Gtk::ToolItem&> (*pTempoLabelTool);

    wMainVBox.pack_start(*pMenubar,Gtk::PACK_SHRINK);
    wMainVBox.pack_start(Toolbar,Gtk::PACK_SHRINK);
    wMainVBox.pack_start(wVBox1);
    wVBox1.pack_end(wFrame,Gtk::PACK_SHRINK);
    wFrame.add(wFrameNotebook);
    wFrame.set_border_width(1);
    wFrame.set_label_align(0.2,0.5);
    wFrame.set_label(_("Sequencer properties"));

    wFrameNotebook.append_page(wNoSeqSelected);
    wFrameNotebook.append_page(seqWidget);
    wFrameNotebook.set_current_page(0);
    wFrameNotebook.set_show_tabs(0);
    wFrameNotebook.set_show_border(0);

    wNoSeqSelected.set_text(_("(No sequencer selected)"));
    wNoSeqSelected.set_tooltip_markup(_("<b>There is no sequencer selected</b>, so it's properties cannot be displayed.\n\nSelect one from the list above, or add a new one."));
    wNoSeqSelected.set_sensitive(0);

    TempoTool.remove();
    TempoTool.add(tempo_button);
    TempoTool.set_homogeneous(0);
    TempoLabelTool.remove();
    TempoLabelTool.add(wTempoLabel);
    TempoLabelTool.set_homogeneous(0);
    tempo_button.set_name("tempo");
    tempo_button.set_range(2.0, 1000.0);
    tempo_button.set_tooltip_markup(_("Sets the <b>tempo</b> applied to all sequencers."));
    tempo_button.set_increments(1.0, 5.0);
    tempo_button.set_digits(1);
    tempo_button.set_width_chars(5);
    tempo_button.set_value(midi->GetTempo());
    tempo_button.signal_value_changed().connect(std::bind(&MainWindow::TempoChanged, this));

    MidiClockTool.remove();
    MidiClockTool.add(midi_clock_button);
    MidiClockTool.set_homogeneous(0);
    midi_clock_button.set_label(_("MIDI Clock"));
    midi_clock_button.set_active(midi->GetMidiClockEnabled());
    midi_clock_button.set_tooltip_markup(_("Toggles MIDI clock and start/stop messages output."));
    midi_clock_button.signal_toggled().connect(std::bind(&MainWindow::OnMIDIClockToggled, this));

    SyncTool.remove();
    SyncTool.add(sync_button);
    SyncTool.set_homogeneous(0);
    sync_button.set_label(_("Sync"));
    sync_button.set_tooltip_markup(_("Synchronizes external devices to harmonySEQs output buffer."));
    sync_button.signal_clicked().connect(std::bind(&MainWindow::OnSyncClicked, this));

    TapTool.remove();
    TapTool.add(tap_button);
    TapTool.set_homogeneous(0);
    tap_button.set_label(_("Tap"));
    tap_button.set_tooltip_markup(_("Tap multiple times to calculate BPM and set tempo."));
    tap_button.signal_clicked().connect(std::bind(&MainWindow::OnTapTempoClicked, this));

    UpdatePlayPauseTool();
    UpdatePassMidiToggle(); //sometimes we pass midi by default.
    //UpdatePlayOnEditToggle(); //as above

    wScrolledWindow.add(wTreeView);
    wScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC,Gtk::POLICY_AUTOMATIC); //The sliders should be shown only when needed
    wVBox1.pack_start(wHPaned); //will expand, no shrinking
    wHPaned.pack1(wScrolledWindow,1,1);
    wHPaned.pack2(eventsWidget,0,0);
    wHPaned.set_position(625);


    // <editor-fold defaultstate="collapsed" desc="tree">
    { //creating the tree model
        TreeModel_sequencers = Gtk::ListStore::create(m_columns_sequencers);

        // wTreeView.append_column(_("Handle"), m_columns_sequencers.col_handle);

        int col_count = wTreeView.append_column_editable(_("Name"), m_columns_sequencers.col_name);
        Gtk::CellRenderer* cell = wTreeView.get_column_cell_renderer(col_count - 1);
        Gtk::TreeViewColumn * column = wTreeView.get_column(col_count-1);
        Gtk::CellRendererText& txt = dynamic_cast<Gtk::CellRendererText&> (*cell);
        txt.signal_edited().connect(std::bind(&MainWindow::OnNameEdited, this, std::placeholders::_1, std::placeholders::_2));

        col_count = wTreeView.append_column_editable(_("On"), m_columns_sequencers.col_muted);
        column = wTreeView.get_column(col_count-1);
        cell = wTreeView.get_column_cell_renderer(col_count - 1);
        column->add_attribute(cell->property_cell_background_rgba(),m_columns_sequencers.col_colour);
        column->set_min_width(32);
        Gtk::CellRendererToggle& tgl = dynamic_cast<Gtk::CellRendererToggle&> (*cell);
        tgl.signal_toggled().connect(std::bind(&MainWindow::OnMutedToggleToggled, this, std::placeholders::_1));

        col_count = wTreeView.append_column(_("Channel"), m_columns_sequencers.col_channel);
        col_count = wTreeView.append_column(_("Pattern"), m_columns_sequencers.col_pattern);
        col_count = wTreeView.append_column(_("Resolution"), m_columns_sequencers.col_res);
        col_count = wTreeView.append_column_numeric(_("Length"), m_columns_sequencers.col_len,"%g");
        col_count = wTreeView.append_column(_("Chord (notes) / Ctrl. No."), m_columns_sequencers.col_chord);

        //drag and drop enabling
        wTreeView.enable_model_drag_source();
        wTreeView.enable_model_drag_dest();

        wTreeView.signal_drag_begin().connect(std::bind(&MainWindow::OnTreeviewDragBegin, this, std::placeholders::_1));
        wTreeView.signal_drag_end().connect(std::bind(&MainWindow::OnTreeviewDragEnd, this, std::placeholders::_1));
        TreeModel_sequencers->signal_row_deleted().connect(std::bind(&MainWindow::OnTreeModelRowDeleted, this, std::placeholders::_1));
        TreeModel_sequencers->signal_row_inserted().connect(std::bind(&MainWindow::OnTreeModelRowInserted, this, std::placeholders::_1, std::placeholders::_2));

        //forbids to typesearch
        wTreeView.set_enable_search(0);

        //click signal (for popup)
        wTreeView.signal_button_press_event().connect([=](GdkEventButton* ev){return OnTreviewButtonPress(ev);});

        //react on selection change
        Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = wTreeView.get_selection();
        refTreeSelection->signal_changed().connect(std::bind(&MainWindow::OnSelectionChanged, this));

        wTreeView.set_model(TreeModel_sequencers);

        UpdateVisibleColumns();

    }// </editor-fold>

    add_events(Gdk::KEY_PRESS_MASK);
    signal_key_press_event().connect([=](GdkEventKey* k){return OnKeyPress(k);});
    signal_key_release_event().connect([=](GdkEventKey* k){return OnKeyRelease(k);});

    //icons settings
    if (harmonySEQ_logo_48) set_icon(harmonySEQ_logo_48);
    if (metronome_icon_24){
        metronometool_icon.set(metronome_icon_24);
        add_ctrl_seq_icon.set(icon_add_ctrl_seq);
        add_note_seq_icon.set(icon_add_note_seq);
        MetronomeTool.set_icon_widget(metronometool_icon);
        AddCtrlTool.set_icon_widget(add_ctrl_seq_icon);
        AddNoteTool.set_icon_widget(add_note_seq_icon);
    }

    // Subscribe to engine signals. They are emitted by the engine
    // thread, so we refer them to the idle timeout, which is
    // processed by the GTK thread.

    midi->on_beat.connect(
        [=](){ DeferWorkToUIThread(
            [=](){ FlashTempo(); });});

    midi->on_paused.connect(
        [=](){ DeferWorkToUIThread(
            [=](){ UpdatePlayPauseTool(); });});

    midi->on_unpaused.connect(
        [=](){ DeferWorkToUIThread(
            [=](){ UpdatePlayPauseTool(); });});

    midi->on_diode.connect(
        [=](auto dev){ DeferWorkToUIThread(
            [=](){ OnDiodeEvent(dev); });});

    midi->on_tempo_changed.connect(
        [=](){ DeferWorkToUIThread(
            [=](){ UpdateTempo(); });});

    Config::on_changed.connect(
        [=](){ DeferWorkToUIThread(
            [=](){ UpdateVisibleColumns(); });});

    Files::on_file_loaded.connect(
        [=](){ DeferWorkToUIThread(
            [=](){
                InitTreeData();
                UpdateTempo();
                UpdateTitle();
            });});

    Files::on_file_saved.connect(
        [=](){ DeferWorkToUIThread(
            [=](){
                UpdateTitle();
            });});

    Files::on_file_modified.connect(
        [=](){ DeferWorkToUIThread(
            [=](){
                UpdateTitle();
            });});

    show_all_children(1);
}

MainWindow::~MainWindow()
{

    delete wPopupMenu;
}

void MainWindow::UpdateTitle(){
    char temp[300];
    if (Files::file_name == ""){
        if (Files::file_modified)
           sprintf(temp, "harmonySEQ %s - %s [*]", VERSION,_("Untitled")) ;
        else
           sprintf(temp, "harmonySEQ %s - %s", VERSION,_("Untitled")) ;
    }else{
        if (Files::file_modified)
           sprintf(temp, "harmonySEQ %s - %s [*]", VERSION,Files::file_name.c_str()) ;
        else
            sprintf(temp, "harmonySEQ %s - %s", VERSION,Files::file_name.c_str());
    }
    set_title(temp);
}

bool
MainWindow::on_delete_event(GdkEventAny* event)
{
    *dbg << "user clicked X\n";
    if(Files::file_modified)
        if(!Ask(this, _("The file has unsaved changes."),_("Are sure you want to quit?")))
          return 1;

    on_quit_request();
    return 0;
}

void
MainWindow::TempoChanged()
{
    double tempo = tempo_button.get_value();
    if(midi->GetTempo() != tempo){
        midi->SetTempo(tempo);
        Files::SetFileModified(1);
    }
}

void MainWindow::UpdateTempo(){
    double tempo = midi->GetTempo();
    if(tempo != tempo_button.get_value())
        tempo_button.set_value(tempo);
}

std::shared_ptr<Sequencer> MainWindow::GetSelectedSequencer(){
    Gtk::TreeModel::iterator iter = GetSelectedSequencerIter();
    if(!iter) return nullptr;
    Gtk::TreeModel::Row row = *iter;
    return row[m_columns_sequencers.col_seq];
}

Gtk::TreeModel::iterator MainWindow::GetSelectedSequencerIter(){
    return *(wTreeView.get_selection())->get_selected();
}

void
MainWindow::OnMutedToggleToggled(const Glib::ustring& path)
{

    Gtk::TreeModel::iterator iter = TreeModel_sequencers->get_iter(path);
    if (!iter) return;
    Gtk::TreeModel::Row row = *iter;
    std::shared_ptr<Sequencer> seq = row[m_columns_sequencers.col_seq];

    seq->SetOn(!row[m_columns_sequencers.col_muted]);

    if(seqWidget.selectedSeq == seq) seqWidget.UpdateOnOff();

    RefreshRow(row);

    //Files::SetFileModified(1); do not detect mutes
}

void
MainWindow::OnNameEdited(const Glib::ustring& path, const Glib::ustring& newtext)
{

    Gtk::TreeModel::iterator iter = TreeModel_sequencers->get_iter(path);
    if (!iter) return;
    Gtk::TreeModel::Row row = *iter;
    std::shared_ptr<Sequencer> seq = row[m_columns_sequencers.col_seq];
    seq->SetName(newtext);

    if(seqWidget.selectedSeq == seq) seqWidget.UpdateName();

    /* TODO: Ideally this signal would never get triggered outside of
     * SequencerManager. */
    SequencerManager::on_sequencer_list_changed();

    RefreshRow(row);

    Files::SetFileModified(1);
}

void MainWindow::AddSequencer(std::shared_ptr<Sequencer> seq)
{
    SequencerManager::Register(seq);

    Gtk::TreeModel::Row row = AddSequencerRow(seq);

    RefreshRow(row);
    wTreeView.get_selection()->select(row);

    Files::SetFileModified(1);
}

Gtk::TreeModel::Row MainWindow::AddSequencerRow(std::shared_ptr<Sequencer> seq)
{
    Gtk::TreeModel::iterator iter = TreeModel_sequencers->append();
    Gtk::TreeModel::Row row = *(iter);

    // TODO: Is it okay to store the shared pointer? Maybe we need to use a weak pointer instead.
    row[m_columns_sequencers.col_seq] = seq;

    // We store these connections it the row so that we can disconnect
    // slots when the row is removed.
    std::vector<bs2::connection> connections;
    connections.push_back(
        seq->on_playstate_change.connect(
            [=](){ DeferWorkToUIThread(
                [=](){ RefreshRow(row); });})
        );

    connections.push_back(
        seq->on_parameter_change.connect(
            [=](){ DeferWorkToUIThread(
                    [=](){ RefreshRow(row); });})
        );

    if(seq->GetType() == SEQ_TYPE_NOTE){
        auto noteseq = std::dynamic_pointer_cast<NoteSequencer>(seq);

        connections.push_back(
            noteseq->on_chord_change.connect(
                [=](){ DeferWorkToUIThread(
                        [=](){ RefreshRow(row); });})
            );
    }
    row[m_columns_sequencers.col_connections_using_this_row] = connections;

    return row;
}

void MainWindow::InitTreeData(){
    // Disconnect all signal handlers registered for all rows.
    for (auto iter = TreeModel_sequencers->children().begin();
         iter != TreeModel_sequencers->children().end(); ++iter){
        std::vector<bs2::connection> conns = (*iter)[m_columns_sequencers.col_connections_using_this_row];
        for (auto &conn : conns)
            conn.disconnect();
    }

    TreeModel_sequencers->clear();

    for (auto seq : SequencerManager::GetAll()){
        auto row = AddSequencerRow(seq);
        RefreshRow(row);
    }
}

void MainWindow::RefreshRow(Gtk::TreeRowReference rowref){
    Gtk::TreeModel::Row row = *(TreeModel_sequencers->get_iter(rowref.get_path()));
    RefreshRow(row);
}

void MainWindow::RefreshRow(Gtk::TreeRow row){
    if(!row) return;
    std::shared_ptr<Sequencer> seq = row[m_columns_sequencers.col_seq];

    row[m_columns_sequencers.col_muted] = seq->GetOn();
    row[m_columns_sequencers.col_name] = seq->GetName();
    row[m_columns_sequencers.col_channel] = seq->GetChannel();
    row[m_columns_sequencers.col_res] = seq->GetResolution();
    row[m_columns_sequencers.col_pattern] = seq->GetActivePatternNumber();
    row[m_columns_sequencers.col_len] = seq->GetLength();

    if(seq->GetType() == SEQ_TYPE_NOTE){
        auto noteseq = std::dynamic_pointer_cast<NoteSequencer>(seq);
        row[m_columns_sequencers.col_chord] = noteseq->chord.GetName();
    } else if (seq->GetType() == SEQ_TYPE_CONTROL){
        auto ctrlseq = std::dynamic_pointer_cast<ControlSequencer>(seq);
        char temp[20];
        sprintf(temp,_("Ctrl %d"),ctrlseq->GetControllerNumber());
        row[m_columns_sequencers.col_chord] = temp;
    }

    if(seq->GetOn()){
        row[m_columns_sequencers.col_colour] = TREEVIEW_COLOR_ON;
    }else if (seq->GetPlayOncePhase() == 2 || seq->GetPlayOncePhase() == 3){
        row[m_columns_sequencers.col_colour] = TREEVIEW_COLOR_P1;
    }else if(seq->GetPlayOncePhase()== 1){
        row[m_columns_sequencers.col_colour] = TREEVIEW_COLOR_PRE_P1;
    }else{
        row[m_columns_sequencers.col_colour] = TREEVIEW_COLOR_OFF;
    }
}

void MainWindow::OnRemoveClicked(){
    std::shared_ptr<Sequencer> seq = GetSelectedSequencer();
    Gtk::TreeModel::iterator iter = GetSelectedSequencerIter();

    //removing the row
    seq_list_drag_in_progress = 0; //important


    std::vector<bs2::connection> conns = (*iter)[m_columns_sequencers.col_connections_using_this_row];
    for (auto &conn : conns)
        conn.disconnect();

    TreeModel_sequencers->erase(iter);

    //and the corresponding sequencer
    SequencerManager::Unregister(seq);

    Files::SetFileModified(1);
}

void MainWindow::OnAddNoteSeqClicked(){
    seq_list_drag_in_progress = 0; //important

    int n = SequencerManager::GetCount();
    char temp[20];
    snprintf(temp, 20, _("seq %d"), n+1);
    auto seq = std::make_shared<NoteSequencer>(temp);

    AddSequencer(seq);
}

void MainWindow::OnAddControlSeqClicked(){
    seq_list_drag_in_progress = 0; //important

    int n = SequencerManager::GetCount();
    char temp[20];
    snprintf(temp, 20, _("seq %d"), n+1);
    auto seq = std::make_shared<ControlSequencer>(temp);

    AddSequencer(seq);
}

void MainWindow::OnCloneClicked(){
    Gtk::TreeModel::iterator iter = *(wTreeView.get_selection())->get_selected();
    if(!iter) return;
    Gtk::TreeModel::Row row = *iter;

    std::shared_ptr<Sequencer> seq = row[m_columns_sequencers.col_seq];
    std::shared_ptr<Sequencer> new_seq = seq->Clone();
    new_seq->SetOn(0);
    AddSequencer(new_seq);
}

void MainWindow::FlashTempo(){
    auto sc = tempo_button.get_style_context();

    /* For some reason, CSS transitions do not work, so we do a custom animation. */
    sc->add_class("lit");
    Glib::signal_timeout().connect(
        [=](){
            sc->remove_class("lit");
            return false;
        }, FLASH_INTERVAL, Glib::PRIORITY_DEFAULT_IDLE);
}

void MainWindow::OnPassToggleClicked(){
    Gtk::Widget* pPassToggle = m_refUIManager->get_widget("/MenuBar/MenuTools/PassMidiEvents");
    Gtk::CheckMenuItem& PassToggle = dynamic_cast<Gtk::CheckMenuItem&> (*pPassToggle);
    midi->SetPassMidiEvents(PassToggle.get_active());
}

void MainWindow::UpdatePassMidiToggle(){
    Gtk::Widget* pPassToggle = m_refUIManager->get_widget("/MenuBar/MenuTools/PassMidiEvents");
    Gtk::CheckMenuItem& PassToggle = dynamic_cast<Gtk::CheckMenuItem&> (*pPassToggle);
    PassToggle.set_active(midi->GetPassMidiEvents());
}

bool MainWindow::OnKeyPress(GdkEventKey* event){
    //*dbg << "triggered " << event->keyval << "\n";
    std::map<int,std::string>::iterator iter;
    iter = keymap_itos.find(event->keyval);
    if(iter != keymap_itos.end()){
        *dbg << "Pressed key '" << iter->second << "'.\n";

    }else
        *dbg << "Unknown key pressed\n";

    if (event->keyval == 65507){ //Ctrl (left)
        if (!CtrlKeyDown) CtrlKeyDown = true;
    } else if (event->keyval == 65505){ // Shift(left)
        if (!ShiftKeyDown) ShiftKeyDown = true;
    }

    FindAndProcessEvents(Event::KEYBOARD,event->keyval);

    return 1;
}
bool MainWindow::OnKeyRelease(GdkEventKey* event){

    if (event->keyval == 65507){ //Ctrl (left)
        if (CtrlKeyDown) CtrlKeyDown = false;
    } else if (event->keyval == 65505){ // Shift(left)
        if (ShiftKeyDown) ShiftKeyDown = false;
    }
    return 1;
}
void MainWindow::OnSelectionChanged(){
    Gtk::TreeModel::iterator iter = wTreeView.get_selection()->get_selected();
    if(iter){
        //something is selected
        Gtk::Widget* pRemoveTool = m_refUIManager->get_widget("/ToolBar/RemoveTool");
        pRemoveTool->set_sensitive(1);
        Gtk::Widget* pDuplicateTool = m_refUIManager->get_widget("/ToolBar/DuplicateTool");
        pDuplicateTool->set_sensitive(1);

        std::shared_ptr<Sequencer> seq = (*iter)[m_columns_sequencers.col_seq];
        seqWidget.SelectSeq(seq);
        wFrameNotebook.set_current_page(1);
    } else {
        //selection is empty
        Gtk::Widget* pRemoveTool = m_refUIManager->get_widget("/ToolBar/RemoveTool");
        pRemoveTool->set_sensitive(0);
        Gtk::Widget* pDuplicateTool = m_refUIManager->get_widget("/ToolBar/DuplicateTool");
        pDuplicateTool->set_sensitive(0);

        seqWidget.SelectNothing();
        wFrameNotebook.set_current_page(0);
    }

}

void MainWindow::UpdatePlayPauseTool(){
    Gtk::Widget* pPlayPauseTool = m_refUIManager->get_widget("/ToolBar/PlayPauseTool");
    Gtk::ToolButton& PlayPauseTool = dynamic_cast<Gtk::ToolButton&> (*pPlayPauseTool);
    if (midi->GetPaused()){
        PlayPauseTool.set_label(_("Play"));
        PlayPauseTool.set_stock_id(Gtk::Stock::MEDIA_PLAY);
    }else{
        PlayPauseTool.set_label(_("Pause"));
        PlayPauseTool.set_stock_id(Gtk::Stock::MEDIA_PAUSE);
    }
}

void MainWindow::OnPlayPauseClicked(){
    if (midi->GetPaused())
        midi->Unpause();
    else
        midi->PauseImmediately();
}

void MainWindow::OnDiodeEvent(DiodeMidiEvent dev){
    // TODO: Move this logic to sequencer widget
    if (seqWidget.selectedSeq == dev.target.lock())
        seqWidget.ActivateDiode(dev);
}

void MainWindow::OnAboutMenuClicked(){
    Gtk::AboutDialog aboutbox;
    aboutbox.set_transient_for(*this);
    aboutbox.set_program_name("harmonySEQ");
    aboutbox.set_version(VERSION);
    aboutbox.set_logo(harmonySEQ_logo_48);
    aboutbox.set_copyright("Copyright © 2010-2011, 2020 Rafał Cieślak");
    aboutbox.set_comments(_("A MIDI sequencing application helpful for music composers and live artists."));
    /* TRANSLATORS:The GNU GPL v.3*/
     aboutbox.set_license(_("HarmonySEQ is free software: you can redistribute it and/or modify\nit under the terms of the GNU General Public License as published by\nthe Free Software Foundation, either version 3 of the License, or\n"
                                                           "(at your option) any later version.\n\nHarmonySEQ is distributed in the hope that it will be useful,\nbut WITHOUT ANY WARRANTY; without even the implied warranty of\nMERCHANTABILITY or FITNESS FOR A P"
                                                            "ARTICULAR PURPOSE.  See the\nGNU General Public License for more details.\n\nYou should have received a copy of the GNU General Public License\nalong with HarmonySEQ.  If not, see <http://www.gnu.org/licenses/>." ));
    aboutbox.set_website("http://harmonyseq.wordpress.com");
    aboutbox.set_website_label(_("harmonySEQ website"));
    std::vector<Glib::ustring> authors;
    /* TRANSLATORS:First caption in authors list */
    authors.push_back(_("Main author:"));
    authors.push_back("   Rafał Cieślak <rafalcieslak256@gmail.com>");
    authors.push_back("");
    authors.push_back(_("Special thanks to:"));
    authors.push_back("   Louigi Verona <http://www.louigiverona.ru>");
    authors.push_back("   Joanna Łopuch");
    authors.push_back("   Krzysztof Platis");
    authors.push_back("   Nel Pogorzelska");
    aboutbox.set_authors(authors);
    /* TRANSLATORS: The list of translators to be placed in about-box */
    aboutbox.set_translator_credits(_("translator-credits"));

    aboutbox.run();

}

void MainWindow::OnMenuQuitClicked(){
    if(Files::file_modified)
        if(!Ask(this, _("The file has unsaved changes."),_("Are sure you want to quit?")))
            return;

    on_quit_request();
}

void MainWindow::OnMenuNewClicked(){

    if (Files::file_modified)
        if (!Ask(this, _("The file has unsaved changes."), _("Are sure you want to loose them?"))) {
            return;
        }

    Files::file_name = "";


    //clear everything.
    ClearEvents();
    SequencerManager::Clear();
    InitTreeData();

    midi->SetTempo(DEFAULT_TEMPO);

    UpdateTitle();

    Files::SetFileModified(0);
}

void MainWindow::OnMenuOpenClicked(){
    if(Files::file_modified)
        if(!Ask(this, _("The file has unsaved changes."),_("Are sure you want to loose them and open another file?")))
        {
            return;
        }
    Files::LoadFileDialog(this);
}

void MainWindow::OnMenuSaveClicked(){
    if (Files::file_name == ""){ // if it's the first save, behave just if we were saving as
        OnMenuSaveAsClicked();
        return;
    }
    Files::SaveToFile(Files::file_dir + Files::file_name, this);
}

void MainWindow::OnMenuSaveAsClicked(){
    Files::SaveFileDialog(this);
}

bool MainWindow::OnTreviewButtonPress(GdkEventButton* event){
   Gtk::TreePath path;
   wTreeView.get_path_at_pos(event->x,event->y,path);

   if (path){
       // right-clicked on a seq, not on the empty space
       if( (event->type == GDK_BUTTON_PRESS) && (event->button == 3) )
           {
               wPopupMenu->popup(event->button, event->time);
           }
   }

  return false;
}

void MainWindow::OnPopupRemove(){
    //should do same action as remove tool
    OnRemoveClicked();
}

void MainWindow::OnPopupDuplicate(){
    //should do same action as duplicate tool
    OnCloneClicked();
}

void MainWindow::OnPopupPlayOnce(){
    auto seq = GetSelectedSequencer();
    seq->SetPlayOncePhase(1);
    Gtk::TreeModel::iterator iter = GetSelectedSequencerIter();
    RefreshRow(*iter);
    if (seqWidget.selectedSeq == seq) seqWidget.UpdateOnOffColour();

}


void MainWindow::OnPreferencesClicked(){
    settingswindow->present();
}

void MainWindow::UpdateVisibleColumns(){
        Gtk::TreeView::Column* pColumn;
        int col_iter = 0;
        //pColumn = wTreeView.get_column(col_iter); //Handle
        //pColumn->set_visible(debugging);
        //col_iter++;
        pColumn = wTreeView.get_column(col_iter); //Name
        pColumn->set_visible(1);
        col_iter++;
        pColumn = wTreeView.get_column(col_iter); //ON/OFF
        pColumn->set_visible(1);
        col_iter++;
        pColumn = wTreeView.get_column(col_iter); //Channel
        pColumn->set_visible(Config::VisibleColumns::Channel);
        col_iter++;
        pColumn = wTreeView.get_column(col_iter); //Pattern
        pColumn->set_visible(Config::VisibleColumns::Pattern);
        col_iter++;
        pColumn = wTreeView.get_column(col_iter); //Res
        pColumn->set_visible(Config::VisibleColumns::Resolution);
        col_iter++;
        pColumn = wTreeView.get_column(col_iter); //Len
        pColumn->set_visible(Config::VisibleColumns::Length);
        col_iter++;
        pColumn = wTreeView.get_column(col_iter); //Chord
        pColumn->set_visible(Config::VisibleColumns::ChordAndCtrlNo);
}

void MainWindow::OnMIDIClockToggled(){
    midi->SetMidiClockEnabled(midi_clock_button.get_active());
}

void MainWindow::OnSyncClicked(){
    midi->Sync();
}

void MainWindow::OnMetronomeToggleClicked(){
    Gtk::Widget* pMetronome = m_refUIManager->get_widget("/ToolBar/Metronome");
    Gtk::ToggleToolButton& Metronome = dynamic_cast<Gtk::ToggleToolButton&> (*pMetronome);
    midi->SetMetronome(Metronome.get_active());
}

void MainWindow::OnTapTempoClicked(){
    midi->TapTempo();
    tempo_button.set_value(midi->GetTempo());
}

void MainWindow::OnTreeviewDragBegin(const Glib::RefPtr<Gdk::DragContext>& ct){
    seq_list_drag_in_progress= 1;

}

void MainWindow::OnTreeviewDragEnd(const Glib::RefPtr<Gdk::DragContext>& ct){
    seq_list_drag_in_progress = 0;

}

void MainWindow::OnTreeModelRowInserted(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter){
    if (seq_list_drag_in_progress == 1){
        //great! drag'n'drop inserted a row!
        //the point is that it first inserts a row, and then deletes it.
        row_inserted_by_drag  = iter;
    }
}

void MainWindow::OnTreeModelRowDeleted(const Gtk::TreeModel::Path& path){
    if (seq_list_drag_in_progress == 1){
        //great! drag'n'drop removed a row!

/* NOTE: Sequencer reordering is disabled while we port handles to sequencer manager.


        //if a row was deleted, then we need to update the moved sequencer's row entry.
        int h = (*row_inserted_by_drag)[m_columns_sequencers.col_handle];
        seqH(h)->my_row = *row_inserted_by_drag;

        //also, we need to REORDER sequencers in the window
        //The ID of the moved sequencer
        int ID = HandleToID(h);
        //The position it was moved to:
        int ID2 = 0;
        //Here we compare the handles assigned to the inserted row, and the 0th row. If equal, it means it was inserted at the beggining, otherwise we cal calculate the position by counting id of above sequencer.
        if (h != (int)(*TreeModel_sequencers->get_iter("0"))[m_columns_sequencers.col_handle]){
            //Get the id of sequencer with the row above
            row_inserted_by_drag--;
            ID2 = HandleToID ((*row_inserted_by_drag)[m_columns_sequencers.col_handle]);
            //The point in the line below, is the fact that when we move a sequencer downwards, the position isn't equal to the above's id, as one of the above (the one we moved) was removed.z
            if (ID > ID2) ID2++;
        }else{
            ID2 = 0;
        }
        *dbg << "Moved " << ID << "-" << ID2 << ENDL;

        //OK, now we know where from and to we moved a seq, we can switch the seq's in vector.
        if (ID == ID2) return;
        if (ID < ID2) //moved downwards
        {
            Sequencer* temp;
            temp = seqVector[ID];
            for (int i = ID; i <= ID2; i++){
                if (i != ID2) //not the last one, so copy from next
                    seqVector[i] = seqVector[i+1];
                else
                    seqVector[i] = temp;
            }

        }else{ //moved upwards
            Sequencer* temp;
            temp = seqVector[ID];
            for(int i = ID; i >= ID2; i--){
                if (i != ID2)//not the last one, so copy from prevoius
                    seqVector[i] = seqVector[i-1];
                else
                    seqVector[i] = temp;
            }

        }

        //Finally, update seqHandles
        UpdateSeqHandlesAfterMoving(ID,ID2);
        RefreshRow(seqH(h)->my_row);

*/

    }
}
