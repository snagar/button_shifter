//
// Created by Saar.Nagar on 09/11/2025.
//

#ifndef BUTTON_SHIFTER_STATE_H
#define BUTTON_SHIFTER_STATE_H
#include "XPLMMenus.h"
#include "utils.h"

namespace shifter
{

class state
{
private:
public:
  state();
  ~state();

  XPLMMenuCheck                 check_state_{ xplm_Menu_Unchecked };
  int                           menu_file_item_{ 0 }; // XPLMAppendMenuItem() returns integer and not XPLMMenuID

  // stores all commands. The key represents the "#" option.
  // The struct: "st_custom_commands" will hold the optional "#" command in key: "100".
  std::map<intptr_t, shifter::strct::st_custom_commands_set> map_command_sets_;


  shifter::strct::strct_state_file strct_file_metadata_{ -1, "", "", "" };

  // Return true if dataref parsed correctly
  // bool parse_dataref (shifter::strct::st_command_ref & inout_command_strct);

  // parse the file that is in "strct_state_file_.full_file_path"
  bool parse_config_file ();

};

} // shifter

#endif // BUTTON_SHIFTER_STATE_H
