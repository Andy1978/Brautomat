#include <stdio.h>
#include <stdlib.h>
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Table.H>
#include <FL/Fl_Int_Input.H>
#include <FL/fl_draw.H>

const int MAX_COLS = 3;
const int MAX_ROWS = 1000;

class Spreadsheet : public Fl_Table
{
  Fl_Int_Input *input;              // single instance of Fl_Int_Input widget
  double values[MAX_ROWS][MAX_COLS];   // array of data for cells
  int row_edit, col_edit;           // row/col being modified

protected:
  void draw_cell (TableContext context,int=0,int=0,int=0,int=0,int=0,int=0);
  void event_callback2();       // table's event callback (instance)
  static void event_callback(Fl_Widget*,void *v)    // table's event callback (static)
  {
    ((Spreadsheet*)v)->event_callback2();
  }
  static void input_cb(Fl_Widget*,void* v)      // input widget's callback
  {
    ((Spreadsheet*)v)->set_value_hide();
  }

public:
  Spreadsheet(int X,int Y,int W,int H,const char* L=0) : Fl_Table(X,Y,W,H,L)
  {
    callback(&event_callback, (void*)this);
    when(FL_WHEN_NOT_CHANGED|when());
    // Create input widget that we'll use whenever user clicks on a cell
    input = new Fl_Int_Input(W/2,H/2,0,0);
    input->hide();
    input->callback(input_cb, (void*)this);
    input->when(FL_WHEN_ENTER_KEY_ALWAYS);    // callback triggered when user hits Enter
    input->maximum_size(5);

    for (int c = 0; c < MAX_COLS; c++)
      for (int r = 0; r < MAX_ROWS; r++)
        values[r][c] = 0;
    end();
  }
  ~Spreadsheet() { }

  // Apply value from input widget to values[row][col] array and hide (done editing)
  void set_value_hide()
  {
    values[row_edit][col_edit] = atof(input->value());
    input->hide();
    window()->cursor(FL_CURSOR_DEFAULT);    // XXX: if we don't do this, cursor can disappear!
  }
  // Start editing a new cell: move the Fl_Int_Input widget to specified row/column
  //    Preload the widget with the cell's current value,
  //    and make the widget 'appear' at the cell's location.
  //
  void start_editing(int R, int C)
  {
    row_edit = R;         // Now editing this row/col
    col_edit = C;
    int X,Y,W,H;
    find_cell(CONTEXT_CELL, R,C, X,Y,W,H);    // Find X/Y/W/H of cell
    input->resize(X,Y,W,H);       // Move Fl_Input widget there
    char s[30];
    sprintf(s, "%.1f", values[R][C]);   // Load input widget with cell's current value
    input->value(s);
    input->position(0,strlen(s));     // Select entire input field
    input->show();          // Show the input widget, now that we've positioned it
    input->take_focus();
  }
  // Tell the input widget it's done editing, and to 'hide'
  void done_editing()
  {
    if (input->visible())         // input widget visible, ie. edit in progress?
      {
        set_value_hide();         // Transfer its current contents to cell and hide
      }
  }

  void set_value(int R, int C, double value)
  {
    if(R>=0 && R<MAX_ROWS && C>=0 && C<MAX_COLS)
      values[R][C] = value;
    redraw();
  }

};

// Handle drawing all cells in table
void Spreadsheet::draw_cell(TableContext context, int R,int C, int X,int Y,int W,int H)
{
  static char s[30];
  switch ( context )
    {
    case CONTEXT_STARTPAGE:     // table about to redraw
      break;

    case CONTEXT_COL_HEADER:      // table wants us to draw a column heading (C is column)
      fl_font(FL_HELVETICA | FL_BOLD, 14);  // set font for heading to bold
      fl_push_clip(X,Y,W,H);      // clip region for text
      {
        fl_draw_box(FL_THIN_UP_BOX, X,Y,W,H, col_header_color());
        fl_color(FL_BLACK);
        /*
        if (C == cols()-1) {      // Last column? show 'TOTAL'
          fl_draw("TOTAL", X,Y,W,H, FL_ALIGN_CENTER);
        } else {        // Not last column? show column letter
          sprintf(s, "%c", 'A' + C);
          fl_draw(s, X,Y,W,H, FL_ALIGN_CENTER);
        }
        */
        switch (C)
          {
          case 0:
            fl_draw("period", X,Y,W,H, FL_ALIGN_CENTER);
            break;
          case 1:
            fl_draw("angle", X,Y,W,H, FL_ALIGN_CENTER);
            break;
          case 2:
            fl_draw("phase", X,Y,W,H, FL_ALIGN_CENTER);
            break;
          }

      }
      fl_pop_clip();
      return;

    case CONTEXT_ROW_HEADER:      // table wants us to draw a row heading (R is row)
      fl_font(FL_HELVETICA | FL_BOLD, 14);  // set font for row heading to bold
      fl_push_clip(X,Y,W,H);
      {
        fl_draw_box(FL_THIN_UP_BOX, X,Y,W,H, row_header_color());
        fl_color(FL_BLACK);
        /*
        if (R == rows()-1) {      // Last row? Show 'Total'
          fl_draw("TOTAL", X,Y,W,H, FL_ALIGN_CENTER);
        } else {        // Not last row? show row#
        */
        sprintf(s, "%d", R+1);
        fl_draw(s, X,Y,W,H, FL_ALIGN_CENTER);

      }
      fl_pop_clip();
      return;

    case CONTEXT_CELL:        // table wants us to draw a cell
    {
      if (R == row_edit && C == col_edit && input->visible())
        {
          return;         // dont draw for cell with input widget over it
        }
      fl_draw_box(FL_THIN_UP_BOX, X,Y,W,H, FL_WHITE);
      // Text
      fl_push_clip(X+3, Y+3, W-6, H-6);
      {
        fl_color(FL_BLACK);
        /*
        if (C == cols()-1 || R == rows()-1) { // Last row or col? Show total
          fl_font(FL_HELVETICA | FL_BOLD, 14);  // ..in bold font
          if (C == cols()-1 && R == rows()-1) { // Last row+col? Total all cells
            sprintf(s, "%d", sum_all());
          } else if (C == cols()-1) {   // Row subtotal
            sprintf(s, "%d", sum_cols(R));
          } else if (R == rows()-1) {   // Col subtotal
            sprintf(s, "%d", sum_rows(C));
          }
          fl_draw(s, X+3,Y+3,W-6,H-6, FL_ALIGN_RIGHT);
        } else {        // Not last row or col? Show cell contents
        */
        fl_font(FL_HELVETICA, 14);    // ..in regular font
        sprintf(s, "%.1f", values[R][C]);
        fl_draw(s, X+3,Y+3,W-6,H-6, FL_ALIGN_RIGHT);

      }
      fl_pop_clip();
      return;
    }

    default:
      return;
    }
}

// Callback whenever someone clicks on different parts of the table
void Spreadsheet::event_callback2()
{
  int R = callback_row();
  int C = callback_col();
  TableContext context = callback_context();

  switch ( context )
    {
    case CONTEXT_CELL:          // A table event occurred on a cell
    {
      switch (Fl::event())          // see what FLTK event caused it
        {
        case FL_PUSH:         // mouse click?
          done_editing();       // finish editing previous
          //if (R != rows()-1 && C != cols()-1 )    // only edit cells not in total's columns
            start_editing(R,C);       // start new edit
          return;

        case FL_KEYBOARD:       // key press in table?
          if ( Fl::event_key() == FL_Escape ) exit(0);  // ESC closes app
          //if (C == cols()-1 || R == rows()-1) return; // no editing of totals column
          done_editing();       // finish any previous editing
          start_editing(R,C);       // start new edit
          if (Fl::event() == FL_KEYBOARD && Fl::e_text[0] != '\r')
            {
              input->handle(Fl::event());     // pass keypress to input widget
            }
          return;
        }
      return;
    }

    case CONTEXT_TABLE:         // A table event occurred on dead zone in table
    case CONTEXT_ROW_HEADER:        // A table event occurred on row/column header
    case CONTEXT_COL_HEADER:
      done_editing();         // done editing, hide
      return;

    default:
      return;
    }
}
