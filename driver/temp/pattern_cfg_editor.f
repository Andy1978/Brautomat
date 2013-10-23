# data file for the Fltk User Interface Designer (fluid)
version 1.0300
header_name {.h}
code_name {.cxx}
decl {\#include <FL/Fl_File_Chooser.H>} {public global
}

decl {\#include <libconfig.h++>} {public global
}

decl {\#include <iostream>} {public global
}

decl {using namespace libconfig;} {public global
}

decl {using namespace std;} {public global
}

decl {\#include "table.h"} {public global
}

Function {} {open
} {
  Fl_Window mainwin {
    label {pattern config editor} open
    xywh {644 404 635 450} type Double visible
  } {
    Fl_Output to_filename {
      label {filename:}
      xywh {85 20 470 25}
    }
    Fl_Input ti_version {
      label {version:}
      xywh {85 55 185 25}
    }
    Fl_Button {} {
      label {@+3fileopen}
      callback {// Create the file chooser, and show it
    Fl_File_Chooser chooser(".",                        // directory
                            "*",                        // filter
                            Fl_File_Chooser::MULTI,     // chooser type
                            "Choose pattern file");        // title
    chooser.show();

    // Block until user picks something.
    //     (The other way to do this is to use a callback())
    //
    while(chooser.shown())
        { Fl::wait(); }

    // User hit cancel?
    if ( chooser.value() == NULL )
        { fprintf(stderr, "(User hit 'Cancel')\\\\n"); return; }

    to_filename->value(chooser.value());

    // Print what the user picked
    fprintf(stderr, "--------------------\\\\n");
    fprintf(stderr, "DIRECTORY: '%s'\\\\n", chooser.directory());
    fprintf(stderr, "    VALUE: '%s'\\\\n", chooser.value());
    fprintf(stderr, "    COUNT: %d files selected\\\\n", chooser.count());

    // Multiple files? Show all of them
    if ( chooser.count() > 1 ) {
        for ( int t=1; t<=chooser.count(); t++ ) {
            fprintf(stderr, " VALUE[%d]: '%s'\\\\n", t, chooser.value(t));
        }
       }


   //hier weiter öffnen, parsen usw.

  Config cfg;
  //try to read the file
  try
  {
    cfg.readFile(chooser.value());
    string version = cfg.lookup("version");
    string comment = cfg.lookup("comment");
    string author  = cfg.lookup("author");

    ti_version->value(version.c_str());
    ti_comment->value(comment.c_str());
    ti_author->value(author.c_str());

    Setting &period = cfg.lookup("pattern.period");
    Setting &angle  = cfg.lookup("pattern.angle");
    Setting &phase  = cfg.lookup("pattern.phase");
    if(    (period.getLength() != angle.getLength())
        || (phase.getLength()  != angle.getLength())
        || (angle.getLength()  != phase.getLength()))
    {
      fprintf(stderr, "error: length of period, angle and phase have to be equal\\n");
      return;
    }

    int cnt = period.getLength();
    cout << cnt << endl;
    for (int i=0; i<cnt; ++i)
    {
      table->set_value(i,0, double(period[i]));
      table->set_value(i,1, double(angle[i]));
      table->set_value(i,2, double(phase[i]));
    }

  }
  catch (FileIOException) //file does not exist, create one
  {


  }}
      xywh {580 21 40 40}
    }
    Fl_Input ti_comment {
      label {comment:}
      xywh {85 90 470 25}
    }
    Fl_Input ti_author {
      label {author:}
      xywh {370 55 185 25}
    }
    Fl_Table table {open
      xywh {10 125 365 315}
      class Spreadsheet
    } {}
    Fl_Group {} {
      label {auto settings} open selected
      xywh {385 140 240 300} box PLASTIC_UP_BOX align 5
    } {
      Fl_Value_Input vi_num_angles {
        label {num angles}
        xywh {500 190 45 25} maximum 20 step 1 value 3
      }
      Fl_Button {} {
        label {calc table}
        xywh {405 385 200 35}
      }
      Fl_Value_Input vi_period {
        label {period [px]}
        xywh {500 155 45 25} maximum 1000 step 10 value 100
      }
      Fl_Value_Input vi_num_phases {
        label {num phases}
        xywh {500 225 45 25} maximum 20 step 1 value 1
      }
    }
    Fl_Button {} {
      label {@+3filesave}
      callback {// Create the file chooser, and show it
    Fl_File_Chooser chooser(".",                        // directory
                            "*",                        // filter
                            Fl_File_Chooser::MULTI,     // chooser type
                            "Choose pattern file");        // title
    chooser.show();

    // Block until user picks something.
    //     (The other way to do this is to use a callback())
    //
    while(chooser.shown())
        { Fl::wait(); }

    // User hit cancel?
    if ( chooser.value() == NULL )
        { fprintf(stderr, "(User hit 'Cancel')\\\\n"); return; }

    to_filename->value(chooser.value());

    // Print what the user picked
    fprintf(stderr, "--------------------\\\\n");
    fprintf(stderr, "DIRECTORY: '%s'\\\\n", chooser.directory());
    fprintf(stderr, "    VALUE: '%s'\\\\n", chooser.value());
    fprintf(stderr, "    COUNT: %d files selected\\\\n", chooser.count());

    // Multiple files? Show all of them
    if ( chooser.count() > 1 ) {
        for ( int t=1; t<=chooser.count(); t++ ) {
            fprintf(stderr, " VALUE[%d]: '%s'\\\\n", t, chooser.value(t));
        }
       }


   //hier weiter öffnen, parsen usw.}
      xywh {580 73 40 40}
    }
  }
  code {// Table rows
  table->row_header(1);
  table->row_header_width(70);
  table->row_resize(1);
  table->rows(5);
  table->row_height_all(25);
  // Table cols
  table->col_header(1);
  table->col_header_height(25);
  table->col_resize(1);
  table->cols(MAX_COLS);
  table->col_width_all(90);} {}
}
