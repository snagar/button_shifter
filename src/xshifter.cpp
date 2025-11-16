//
// Created by xplane on 08/11/2025.
//

#include "xshifter.h"

namespace shifter
{

  // ---------- static declaration --------------
int                                             xshifter::active_file_id{ 0 };
intptr_t                                        xshifter::active_set_id{ 0 };
xshifter::strct_menu                            xshifter::menu_;
std::map<size_t, shifter::state>                xshifter::map_states_;
std::map<size_t, shifter::strct::st_state_file> xshifter::map_list_state_files_;

std::map<intptr_t, XPLMCommandRef>    xshifter::mapCommands_;
std::map<intptr_t, XPLMCommandRef>    xshifter::mapCmdCustomCommand_;
std::map<XPLMCommandRef, int>         xshifter::mapSelCmdRef_;

XPLMCommandRef xshifter::s_current_command_ref_;
std::string    xshifter::s_current_command_button_type_;

Timer xshifter::s_hold_timer; // we never reset during the button hold
Timer xshifter::s_hold_timer_last_tick; // hold the last tick that was taken
float xshifter::s_tick_period_every_milli_;

// --------------------

  xshifter::xshifter ()
  {
  }

  // --------------------

  xshifter::~xshifter ()
  {
  }

  // --------------------

  // --------------------

  void
  xshifter::init_states_from_config_files ()
  {
    shifter::xshifter::map_states_.clear ();
    const auto plugin_path = utils::GetPluginRelativePath ();
    xshifter::map_list_state_files_ = utils::list_state_files (plugin_path);

    // loop over the files and initialize the main state container
    for (const auto &[index, file] : xshifter::map_list_state_files_)
    {
      shifter::xshifter::map_states_[index].strct_file_metadata_ = file;
      shifter::xshifter::map_states_[index].parse_config_file (); // Moved to menu handler
    }

    // Force default state as index 0
    if (shifter::xshifter::map_states_.contains (0))
    {
      shifter::xshifter::map_states_[0].strct_file_metadata_.seq = shifter::enums::menuIdRefs::STATE_0;
      shifter::xshifter::map_states_[0].strct_file_metadata_.file_name = "No Shift";
    }
    else
    {
      shifter::state default_state;
      default_state.strct_file_metadata_.seq = shifter::enums::menuIdRefs::STATE_0;
      default_state.strct_file_metadata_.file_name = "No Shift";
      shifter::xshifter::map_states_[0] = default_state;
    }
  }

  // --------------------

  void
  xshifter::ui_check_menu_item (XPLMMenuID&in_menuid, const int &in_item_count, const int &in_index_to_check)
  {
    // reset all menu items
    for (int loop_1 = 0; loop_1 < in_item_count; ++loop_1)
    {
      XPLMMenuCheck outCheck;
      XPLMCheckMenuItemState (in_menuid, loop_1, &outCheck);
      if (outCheck == xplm_Menu_Checked)
        XPLMCheckMenuItem (xshifter::menu_.ui_states_sub_menuid_, loop_1, xplm_Menu_Unchecked);
    }
    XPLMCheckMenuItem (xshifter::menu_.ui_states_sub_menuid_, in_index_to_check, xplm_Menu_Checked);
  }

  // --------------------


  void
  xshifter::init_ui_state_sub_menu (const XPLMMenuID &in_menu_id)
  {
    // Clear all items in the "xshifter::menu_.ui_states_sub_menu_"
    // iterate over all the "states" in the "this->map_states_" and add them to the "xshifter::menu_.ui_states_sub_menu_", parent menu.
    // skip function if the parent menu is invalid
    if (!in_menu_id)
      return;

    // initialize the state container
    init_states_from_config_files ();

    XPLMClearAllMenuItems (in_menu_id);
    XPLMEnableMenuItem (in_menu_id, xplm_Menu_Unchecked, xplm_Menu_Checked );

    for (auto &[index, item_state] : shifter::xshifter::map_states_)
    {
      auto item_id = static_cast<uint8_t> (shifter::enums::menuIdRefs::STATE_0) + static_cast<uint8_t> (index) ;

      if (xshifter::mapCommands_.contains (static_cast<intptr_t>(index)) && item_id < static_cast<int> (shifter::enums::menuIdRefs::COUNT))
      {
        item_state.check_state_             = xplm_Menu_Unchecked;
        item_state.strct_file_metadata_.seq = static_cast<enums::menuIdRefs> (item_id);
        item_state.menu_file_item_          = XPLMAppendMenuItem (in_menu_id, std::format ("{}. {:.20}", index, item_state.strct_file_metadata_.file_name).c_str (), reinterpret_cast<void *> (index), 1);

        if (index == 0)
        {
          item_state.check_state_ = xplm_Menu_Checked;
          XPLMAppendMenuSeparator (in_menu_id);
        }
        XPLMCheckMenuItem (in_menu_id, item_state.menu_file_item_, item_state.check_state_);

      }
    }

    shifter::xshifter::active_file_id = 0;
  }


  // --------------------
  
  bool
  xshifter::plugin_start (const char *outName, char *outSig, char *outDesc)
  {

    /////////////////////////////
    ///// Create Commands //////
    // Command creation must come before Menu creation since some menu items might use commands
    for (auto &code : shifter::g_shift_state_codes)
    {
      if ( code == 0 )
        xshifter::mapCommands_[code] = XPLMCreateCommand (std::format ("{}{}",shifter::g_prefix_shift_number_text, code).c_str (), std::format ("Shift 0 - Ignore The Custom Commands" ).c_str ());
      else 
        xshifter::mapCommands_[code] = XPLMCreateCommand (std::format ("{}{}", shifter::g_prefix_shift_number_text, code).c_str (), std::format ("Shift Set {}", code).c_str ());

      XPLMRegisterCommandHandler (xshifter::mapCommands_[code], xshifter::shift_change_set_handler, true, reinterpret_cast<void *> (code));
    }

    // initialize the custom sub-commands to re-map your joystic button or keyboard
    for (auto &code : shifter::g_shift_commands)
    {
      xshifter::mapCmdCustomCommand_[code] = XPLMCreateCommand (std::format ("{}{}", shifter::g_prefix_shift_custom_cmd_text, code).c_str (), std::format ("Custom Command Line {}", code).c_str ());
      XPLMRegisterCommandHandler (xshifter::mapCmdCustomCommand_[code], xshifter::shift_cmd_custom_button_handler, true, reinterpret_cast<void *> (code));
    }




    ////////////////////////
    ///// Create Menu //////
    xshifter::menu_.main_menu_ = XPLMAppendMenuItem (XPLMFindPluginsMenu (), outName, reinterpret_cast<void *> (shifter::enums::menuIdRefs::MAIN_MENU_ENTRY), 1);
    //xshifter::menu_.plugin_main_menu_entry_ = XPLMCreateMenu (outName, XPLMFindPluginsMenu (), xshifter::menu_.main_menu_, &shifter::xshifter::menu_handler, nullptr);
    xshifter::menu_.plugin_main_entry_menuid_ = XPLMCreateMenu (outName, XPLMFindPluginsMenu (), xshifter::menu_.main_menu_, nullptr, nullptr); // no handling function

    // State Menu
    xshifter::menu_.menu_states_          = XPLMAppendMenuItem (xshifter::menu_.plugin_main_entry_menuid_, "Activate", reinterpret_cast<void *> (shifter::enums::menuIdRefs::MENU_ENTRY_STATE), 1);
    xshifter::menu_.ui_states_sub_menuid_ = XPLMCreateMenu ("Activate >", xshifter::menu_.plugin_main_entry_menuid_, xshifter::menu_.menu_states_, &shifter::xshifter::menu_file_picked_by_user_handler, reinterpret_cast<void *> (shifter::enums::menuIdRefs::UI_SUB_MENU_STATE));

    // Add all state configuration files
    xshifter::init_ui_state_sub_menu (xshifter::menu_.ui_states_sub_menuid_);

    XPLMAppendMenuSeparator (xshifter::menu_.plugin_main_entry_menuid_);

    // Available state options
    xshifter::menu_.menu_available_states_   = XPLMAppendMenuItem (xshifter::menu_.plugin_main_entry_menuid_, "Available Shift Sets", reinterpret_cast<void *> (shifter::enums::menuIdRefs::MENU_ENTRY_AVAILABLE_STATES), 1);
    xshifter::menu_.ui_available_set_sub_menuid_ = XPLMCreateMenu ("Available Shift Sets", xshifter::menu_.plugin_main_entry_menuid_, xshifter::menu_.menu_available_states_, nullptr, nullptr); // no handling function
    XPLMEnableMenuItem (xshifter::menu_.plugin_main_entry_menuid_, xshifter::menu_.menu_available_states_, false); // disable the menu

    XPLMAppendMenuSeparator (xshifter::menu_.plugin_main_entry_menuid_);

    // Config Menu
    xshifter::menu_.menu_config_ = XPLMAppendMenuItem (xshifter::menu_.plugin_main_entry_menuid_, "Config", reinterpret_cast<void *> (shifter::enums::menuIdRefs::MENU_ENTRY_CONFIG), 1);
    xshifter::menu_.ui_config_sub_menuid_ = XPLMCreateMenu ("Config Options", xshifter::menu_.plugin_main_entry_menuid_, xshifter::menu_.menu_config_, xshifter::menu_config_handler, reinterpret_cast<void *> (shifter::enums::menuIdRefs::UI_SUB_MENU_CONFIG)); // no handling function
    // Add "config" menu sub items
    auto add_item_result = XPLMAppendMenuItem (xshifter::menu_.ui_config_sub_menuid_, "Reload \"*.shift\" files", reinterpret_cast<void *> (shifter::enums::menuIdRefs::UI_SUB_MENU_CONFIG_REFRESH_STATE_FILE_LIST), 1);


    shifter::xshifter::active_file_id = 0;

    return true;
  }


  // --------------------
  
  void
  xshifter::plugin_stop ()
  {
  }

  // --------------------

  void
  xshifter::plugin_enable ()
  {
  }

  // --------------------

  void
  xshifter::plugin_disable ()
  {
  }

  // --------------------

  void
  xshifter::menu_config_handler (void *inMenuRef, void *inItemRef)
  {
    auto item_id = reinterpret_cast<intptr_t> (inItemRef);
    if (item_id == static_cast<intptr_t>(enums::menuIdRefs_def::UI_SUB_MENU_CONFIG_REFRESH_STATE_FILE_LIST ) )
    {
      // Add all state configuration files
      xshifter::init_ui_state_sub_menu (xshifter::menu_.ui_states_sub_menuid_);
      XPLMEnableMenuItem (xshifter::menu_.plugin_main_entry_menuid_, xshifter::menu_.menu_available_states_, false); // disable the menu
    }
  }

  // --------------------
  
  void
  xshifter::menu_file_picked_by_user_handler (void *inMenuRef, void *inItemRef)
  {
    const auto item_id = reinterpret_cast<intptr_t> (inItemRef);

    if (map_states_.contains (static_cast<long long> (item_id)))
    {
      xshifter::active_file_id = static_cast<int> (item_id);

      if (xshifter::map_states_[item_id].check_state_ != xplm_Menu_Checked)
      {
        // uncheck all items in menu and then check the picked one
        for (auto &[index, state_item] : xshifter::map_states_)
        {
          if (state_item.check_state_ == xplm_Menu_Checked)
          {
            state_item.check_state_ = xplm_Menu_Unchecked;
            XPLMCheckMenuItem (xshifter::menu_.ui_states_sub_menuid_, state_item.menu_file_item_, xplm_Menu_Unchecked);
          }
        }

        // Check the item
        xshifter::map_states_[item_id].check_state_ = xplm_Menu_Checked;
        XPLMCheckMenuItem (xshifter::menu_.ui_states_sub_menuid_, xshifter::map_states_[item_id].menu_file_item_, xplm_Menu_Checked);

        // initialize the "available states" menu based on the "item_id"
        XPLMClearAllMenuItems( xshifter::menu_.ui_available_set_sub_menuid_ );
        if (!xshifter::map_states_[item_id].map_command_sets_.empty ())
        {
          int counter = 1;
          for (auto& [key_set_number, strct_set] : xshifter::map_states_[item_id].map_command_sets_ )
          {
            strct_set.reset_ui_info (); // we reset the menu index
            if (xshifter::mapCommands_.contains (key_set_number))
            {
              const auto lmbda_get_set_name = [&]()
              {
                if (strct_set.custom_commands.contains (shifter::g_shift_button_key_number) && strct_set.custom_commands[shifter::g_shift_button_key_number].contains ("name"))
                  return std::format ("Set #{} - {:.30}", key_set_number, strct_set.custom_commands[shifter::g_shift_button_key_number]["name"]);

                return std::format ("Set #{}",key_set_number);
              };

              const std::string set_name = lmbda_get_set_name ();
              strct_set.set_menu_index = XPLMAppendMenuItemWithCommand (xshifter::menu_.ui_available_set_sub_menuid_, set_name.c_str (), xshifter::mapCommands_[key_set_number]);
              if (counter == 1)
              {
                strct_set.check_state_ = xplm_Menu_Checked;
                XPLMCheckMenuItem (xshifter::menu_.ui_available_set_sub_menuid_, xshifter::map_states_[item_id].map_command_sets_.begin()->second.set_menu_index, xplm_Menu_Checked);
              }
              counter++;
            }
          }

          // store active set number
          xshifter::active_set_id = xshifter::map_states_[item_id].map_command_sets_.begin()->first; // should be Zero

          XPLMEnableMenuItem (xshifter::menu_.plugin_main_entry_menuid_, xshifter::menu_.menu_available_states_, true); // enable the menu
        }
        else
          XPLMEnableMenuItem (xshifter::menu_.plugin_main_entry_menuid_, xshifter::menu_.menu_available_states_, false); // disable the menu

        // construct the menu that shows the available states
      }
    }
  }

  // --------------------

  int
  xshifter::shift_change_set_handler (XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon)
  {
    const auto new_set_id = reinterpret_cast<intptr_t> (inRefcon);

    if (inPhase == xplm_CommandBegin && new_set_id != xshifter::active_set_id)
    {
      #ifndef RELEASE
      auto b_contain = xshifter::map_states_[active_file_id].map_command_sets_.contains (new_set_id); // debug
      #endif
      if (xshifter::map_states_[active_file_id].map_command_sets_.contains (new_set_id) && xshifter::map_states_[active_file_id].map_command_sets_[new_set_id].check_state_ != xplm_Menu_Checked)
      {
        // uncheck all items in menu and then check the picked one
        for (auto &[set_id, strct_set] : xshifter::map_states_[active_file_id].map_command_sets_)
        {
          if (strct_set.check_state_ == xplm_Menu_Checked)
          {
            strct_set.check_state_ = xplm_Menu_Unchecked;
            XPLMCheckMenuItem (xshifter::menu_.ui_available_set_sub_menuid_, strct_set.set_menu_index, xplm_Menu_Unchecked);
          }
        }

        // Check the item
        xshifter::map_states_[active_file_id].map_command_sets_[new_set_id].check_state_ = xplm_Menu_Checked;
        XPLMCheckMenuItem (xshifter::menu_.ui_available_set_sub_menuid_, xshifter::map_states_[active_file_id].map_command_sets_[new_set_id].set_menu_index, xplm_Menu_Checked);

        active_set_id = new_set_id;
      }
    }
    
    return 1;
  }

  // --------------------

  int
  xshifter::shift_cmd_custom_button_handler (XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon)
  {
    const auto command_number = static_cast<int>( reinterpret_cast<intptr_t> (inRefcon) );
    if (inPhase == xplm_CommandBegin && xshifter::active_file_id > 0)
    {
      xshifter::s_tick_period_every_milli_ = shifter::tick_accelerate_0;
      xshifter::s_hold_timer.reset ();

      if (xshifter::map_states_.contains (xshifter::active_file_id) && xshifter::map_states_[xshifter::active_file_id].map_command_sets_.contains (active_set_id))
      {
        // check vector has commands
        if ( ! xshifter::map_states_[xshifter::active_file_id].map_command_sets_[active_set_id].custom_commands.empty ()
            && xshifter::map_states_[xshifter::active_file_id].map_command_sets_[active_set_id].custom_commands.contains ( command_number ) )
        {
          auto *meta_command_map_ptr = &xshifter::map_states_[xshifter::active_file_id].map_command_sets_[active_set_id].custom_commands[command_number];
          if (meta_command_map_ptr && !meta_command_map_ptr->empty () && meta_command_map_ptr->contains (shifter::attrib_cmd))
          {
            const auto& command_ptr = (*meta_command_map_ptr)[shifter::attrib_cmd];
            xshifter::s_current_command_button_type_ = (meta_command_map_ptr->contains (shifter::attrib_button_type)? (*meta_command_map_ptr)[shifter::attrib_button_type]:shifter::button_type_tick);

            xshifter::s_current_command_ref_ = XPLMFindCommand(command_ptr.c_str() );

            if (xshifter::s_current_command_ref_)
            {
              XPLMCommandOnce(xshifter::s_current_command_ref_); // tick

              if (s_current_command_button_type_ == shifter::button_type_tick )
              {
                xshifter::s_current_command_ref_ = nullptr;
              }
              else if (s_current_command_button_type_ == shifter::button_type_hold)
              {
                shifter::Timer::start (xshifter::s_hold_timer);
                xshifter::s_hold_timer_last_tick = xshifter::s_hold_timer;
              }
            } // end if command_ref is valid
          } // end if we have "cmd" command
        } // end if the command number is in the set
      } // end if "set_id" is in the "set container"
    } // END xplm_CommandBegin

    else if (inPhase == xplm_CommandContinue && xshifter::s_current_command_ref_)
    {
      auto time_passed = Timer::get_os_delta_time_now_vs_start_in_milli (xshifter::s_hold_timer_last_tick.tf_begin);
      if (xshifter::s_current_command_button_type_ == shifter::button_type_hold && time_passed >= xshifter::s_tick_period_every_milli_)
      {
        XPLMCommandOnce (xshifter::s_current_command_ref_); // tick
        // resetart the last tick timer
        Timer::start (xshifter::s_hold_timer_last_tick);
      }
    }

    else if (inPhase == xplm_CommandEnd && xshifter::s_current_command_ref_)
    {
      if (xshifter::s_current_command_button_type_ == shifter::button_type_hold )
      {
        xshifter::s_tick_period_every_milli_ = shifter::tick_accelerate_0;
        xshifter::s_hold_timer.reset ();
        xshifter::s_current_command_ref_ = nullptr;
      }
    }

    return 1;
  }

  // --------------------

  float
  xshifter::m_pluginCallback (float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, int inCounter, void *inRefcon)
  {
    if ( xshifter::s_current_command_ref_ && xshifter::s_current_command_button_type_ == shifter::button_type_hold)
    {
      auto delta_acceleration_passed_sec = Timer::get_os_delta_time_now_vs_start_in_milli (xshifter::s_hold_timer.tf_begin);
      if (delta_acceleration_passed_sec > shifter::timer_delta_for_the_second_acceleration_2)
        xshifter::s_tick_period_every_milli_ = shifter::tick_accelerate_2;
      else if (delta_acceleration_passed_sec > shifter::timer_delta_for_the_first_acceleration_1)
        xshifter::s_tick_period_every_milli_ = shifter::tick_accelerate_1;

      // XPLMDebugString ( std::format ("[{}] s_current_command_tick_counter: {},\ts_current_draw_cb_command_tick_counter_: {}\n", __func__, xshifter::s_current_command_tick_counter, xshifter::s_current_draw_cb_command_tick_counter_ ).c_str () );
    }

    return 1.0f;
  }

  // --------------------
  // --------------------
  // --------------------

  } // shifter