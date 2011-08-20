/*
    Copyright (C)  2011 Rafał Cieślak

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

#ifndef PATTERNWIDGET_H
#define	PATTERNWIDGET_H
#include "gtkmm.h"
#include <set>
#include "Sequencer.h"
#include "ControllerAtom.h"

class AtomContainer;

/**PatternWidget is basically a GUI for AtomContainer, that uses a DrawingArea to display a piano-roll interface.*/
class PatternWidget : public Gtk::DrawingArea {
public:
    PatternWidget();
    virtual ~PatternWidget();
    void SetInternalHeight(int h);
    
    /**Redraws all atoms. Call when any of them was moved/added/removed/etc.*/
    void RedrawAtoms();
    /**Redraws grid. Call when resolution was changed.*/
    void RedrawGrid();
    /**Redraws both grid and atoms*/
    bool RedrawEverything();
    
    /**Assigns a pattern to draw.
     * @parram cont Adress of an AtomContainer that has to be drawn in this Pattern Widget. Note that PatternWidget <b>WILL</b> write data to it, basing on user input!
     * @parram type Sequencer type (note/control)*/
    void AssignPattern(AtomContainer* cont, SeqType_t type);
    
    /**Resizes this PatternWidget to zoom-in*/
    void ZoomIn();
    /**Resizes this PatternWidget to zoom-out*/
    void ZoomOut();
    
    /**Starts adding atoms with LMB*/
    void EnterAddMode();
    /**Stops adding notes with LMB*/
    void LeaveAddMode();
    /**Returns whether AddMode is on*/
    bool GetAddMode();
    
    /**Removes selected notes from container.*/
    void DeleteSelected();
    
    /**Selects nothing.*/
    void ClearSelection();
    
    /**Turns snap-to-grid on/off.*/
    void SetSnap(bool s);
    /**Tells whether snap-to-grid is on.*/
    bool GetSnap();
    
    /*Calculates average velocity of selection.*/
    int GetSelectionVelocity();
    /*Sets all selected notes velocity to v.*/
    void SetSelectionVelocity(int v);
    /*Calcucates average value of selection.*/
    int GetSelectionValue();
    /*Sets all selectes notes value to v.*/
    void SetSelectionValue(int v);
    
    /**Sets this ControllerAtom's slope-type to s.*/
    void SetSlopeType(SlopeType s);
    /**Returns selected ControllerAtom's slope-type. If there are different, returns SLOPE_TYPE_NONE*/
    SlopeType GetSlopeType();
    
    /**Emitted when selection is changed. Provides an argument that is equal to number of notes in selection.*/
    sigc::signal<void,int> on_selection_changed;
    
    /**Emmited when patter changed add_mode and parent widget needs to update button*/
    sigc::signal<void> on_add_mode_changed;
    
    sigc::signal<void> on_slope_type_needs_additional_refreshing;
    
protected:
    //Override default signal handler:
   virtual bool on_expose_event(GdkEventExpose* event);
    
   virtual bool on_button_press_event(GdkEventButton* event);
   virtual bool on_button_release_event(GdkEventButton* event);
   
   virtual bool on_motion_notify_event(GdkEventMotion* event);
   virtual bool on_leave_notify_event(GdkEventCrossing* event);
   
   virtual bool on_key_press_event(GdkEventKey* event);
private:
    
    void Redraw();
    bool TimeLockAtomsCompleted();
    bool TimeLockGridCompleted();
    Cairo::RefPtr<Cairo::ImageSurface> cr_buffer_surface;
    Cairo::RefPtr<Cairo::Context> cr_buffer_context;
    Cairo::RefPtr<Cairo::ImageSurface> cr_atoms_surface;
    Cairo::RefPtr<Cairo::Context> cr_atoms_context;
    Cairo::RefPtr<Cairo::ImageSurface> cr_grid_surface;
    Cairo::RefPtr<Cairo::Context> cr_grid_context;
    bool atoms_lock;
    bool grid_lock;
    bool man_i_wanted_to_redraw_atoms_but_it_was_locked_could_you_please_do_it_later_for_me;
    bool man_i_wanted_to_redraw_grid_but_it_was_locked_could_you_please_do_it_later_for_me;
    
    int last_drawn_width;
    int last_drawn_height;
    
    bool add_mode;
    SlopeType add_slope_type;
    
    std::set<Atom *,AtomComparingClass> selection;
    
    bool snap;
    std::set<Atom *,AtomComparingClass> drag_temporary_selection;
    int drag_beggining_x, drag_beggining_y;
    int drag_beggining_line;
    int drag_beggining_value;
    double drag_beggining_time;
    int drag_current_x, drag_current_y;
    int drag_current_line;
    double drag_current_time;
    int drag_current_value;
    bool drag_in_progress;
    int drag_mode;
    Atom* drag_note_dragged;
    double drag_time_offset_to_dragged_note;
    enum DragModes{
        DRAG_MODE_MOVE_SELECTION,
        DRAG_MODE_SELECT_AREA,
        DRAG_MODE_RESIZE
    };
    
    SlopeType GetSelectionSlopeType();
    
    void UpdateSizeRequest();
    double Snap(double t);
    double SnapDown(double t);
    void DeleteNth(int n);
    int internal_height;
    double horiz_size; //used to controll zooming
    
    AtomContainer* container;
    SeqType_t seq_type;
    
    void InitDrag();
    void ProcessDrag(double x, double y,bool shift_key=false);
    
    void MoveSelectionUp();
    void MoveSelectionDown();
    void MoveSelectionLeft();
    void MoveSelectionRight();
    void IncreaseSelectionVelocity(int amount);
    void DecreaseSelectionVelocity(int amount);
    void IncreaseSelectionValue(int amount);
    void DecreaseSelectionValue(int amount);
    
};

#endif	/* PATTERNWIDGET_H */

