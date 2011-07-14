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
#include "PatternWidget.h"
#include "cairomm/context.h"
#include "global.h"
#include "messages.h"
#include "AtomContainer.h"
#include "NoteAtom.h"
#include "Sequencer.h"
PatternWidget::PatternWidget(){
    internal_height=50; //random guess. will be reset soon anyway by the SequencerWidget, but better protect from 0-like values.
    vert_size = 450.0; //adjust for better default size
    add_events(Gdk::BUTTON_PRESS_MASK);
}

PatternWidget::~PatternWidget(){
}

void PatternWidget::SetInternalHeight(int h){
    internal_height = h;
    UpdateSizeRequest();
}

void PatternWidget::Redraw(){
    queue_draw();
}

void PatternWidget::UpdateSizeRequest(){
    //*dbg << "sizerequest " << vert_size << " " << internal_height+20 << ENDL;
    set_size_request(vert_size,internal_height+20);
}

void PatternWidget::ZoomIn(){
    vert_size = vert_size/1.2;
    UpdateSizeRequest();
}

void PatternWidget::ZoomOut(){
    vert_size = vert_size*1.2;
    UpdateSizeRequest();
}

void PatternWidget::AssignPattern(AtomContainer* cont){
    *dbg << "assigning pattern \n";
    container = cont;
    selection.clear();
    Redraw();
}

void PatternWidget::ClearSelection(){
    selection.clear();
    Redraw();
}

bool PatternWidget::on_button_press_event(GdkEventButton* event){
    if(event->button == 1) //LMB
    {
        Gtk::Allocation allocation = get_allocation();
        const int width = allocation.get_width();
        const int height = allocation.get_height();
        //determine line:
        if(event->y <= internal_height){
            int line = 6-event->y/(internal_height/6);
            *err << line << ENDL;
            double time = (double)event->x/(double)width;
            *err << time <<ENDL;
            int found = -1;
            int size = container->GetSize();
            for(int x = 0; x <size;x++){
                NoteAtom* note = dynamic_cast<NoteAtom*>((*container)[x]);
                if(note->pitch == line &&note->time < time && time < note->time+note->length){
                    found = x;
                    break;
                }
            }
            if(found == -1){
                //clicked empty space, clear selection.
                if (event->state & (1 << 0)) {//shift key was pressed...
                    //Do nothing, do not clear selection.
                } else {
                    //Empty space with no shift... clear selection.
                    selection.clear();
                }
            }else{
                //clicked a note.
                if (event->state & (1 << 0)) {//shift key was pressed...
                    //we'll add the note to selection, unless it's already selecte, then we de-select it.
                    std::set<int>::iterator it= selection.find(found);
                     if(it != selection.end()) 
                         //it's already selected, then:
                         selection.erase(it);
                     else
                         //it was not selected, select it.
                         selection.insert(found);
                } else {
                    //we'll make a new selection
                    selection.clear();
                    selection.insert(found);
                }
            }
            
            Redraw();
        } //(event->y <= internal_height)
    }
}

void PatternWidget::on_drag_begin(const Glib::RefPtr<Gdk::DragContext>& context){
    
}

  bool PatternWidget::on_expose_event(GdkEventExpose* event){
   cairo_t * c_t = gdk_cairo_create(event->window);
   Cairo::Context ct(c_t);
   //Cairo::RefPtr<Cairo::Context> cr(&ct);
    
   Gtk::Allocation allocation = get_allocation();
  const int width = allocation.get_width();
  const int height = allocation.get_height();
  
  int resolution = container->owner->resolution;
  
  //The +0.5 that often appears below in coordinates it to prevent cairo from antyaliasing lines.
    
  ct.set_line_width(3.0);
  ct.set_line_join(Cairo::LINE_JOIN_ROUND);
  if (container) //just in case it's NULL...
  for (int x = 0; x < container->GetSize(); x++){
      Atom* atm = (*container)[x];
      NoteAtom* note = dynamic_cast<NoteAtom*>(atm);
      double y1 = (5-note->pitch)*internal_height/6;
      double h = internal_height/6;
      double x1 = note->time*width;
      double w = note->length*width;
      ;y1++; // This is because the very first 1px line is the upper border.
    //*dbg << "drawing note... "<< x1 << " " << y1 << " " << w << " " <<  h << "\n";
      //Check if note is in selection.
      bool selected = false;
      if(selection.find(x) != selection.end()) selected = true;
      
      ct.rectangle(x1+1.5,y1+1.5,w-3,h-3);
      if(!selected) ct.set_source_rgb(0.0,0.0,0.8);
      else ct.set_source_rgb(0.8,0.0,0.0);
      ct.fill_preserve();
      if(!selected) ct.set_source_rgb(0.0,0.0,0.4);
      else ct.set_source_rgb(0.4,0.0,0.0);
      ct.stroke();
  }
  
  //horizontal grid
  ct.set_line_width(1);
  ct.set_source_rgb(0.0,0.0,0.0);
  for(int x = 0; x <= 6; x++){
        ct.move_to(0,x*internal_height/6+0.5);
        ct.line_to(width,x*internal_height/6+0.5);
        ct.stroke();
  }
  
  //vertical grid
  ct.set_line_width(1);
  ct.set_source_rgb(0.3,0.3,0.2);
  for(int x = 0; x <= resolution; x++){
        if (x!=resolution){
                ct.move_to((int)((double)x*(double)width/resolution) + 0.5,0);
                ct.line_to((int)((double)x*(double)width/resolution) + 0.5,internal_height);
        }else{
                ct.move_to((int)((double)x*(double)width/resolution) - 0.5,0);
                ct.line_to((int)((double)x*(double)width/resolution) - 0.5,internal_height); //the last one must be in drawing area, so let's put it a 1 px before
        }  
        ct.stroke();
  }
  
  return true;
      
  }