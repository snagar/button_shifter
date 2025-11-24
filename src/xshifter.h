//
// Created by xplane on 08/11/2025.
//

#ifndef BUTTON_SHIFTER_XSHIFTER_H
#define BUTTON_SHIFTER_XSHIFTER_H
#include "utils.h"
#include <XPLMMenus.h>
#include "state.h"
#include "Timer.h"

namespace shifter
{
  class xshifter
  {
    private:
    // for each config file we should create a submenu
    static std::map<size_t, shifter::strct::st_state_file> map_list_state_files_;

    // commands
    static std::map<intptr_t, XPLMCommandRef> mapCommands_;
    static std::map<intptr_t, XPLMCommandRef> mapCmdCustomCommand_;
    static std::map<XPLMCommandRef, int>      mapSelCmdRef_;

    static XPLMCommandRef s_current_command_ref_;
    static std::string    s_current_command_button_type_;

    static Timer s_hold_timer; // we never reset during the button hold
    static Timer s_hold_timer_last_tick; // hold the last tick that was taken
    static float s_tick_period_every_milli_;

    // menu
    struct strct_menu_ref
    {
      XPLMMenuID id;
      int        menu_state;
    };

    typedef struct menu_def
    {
      int main_menu_;
      XPLMMenuID plugin_main_entry_menuid_{0};
      int menu_states_{0};
      XPLMMenuID ui_states_sub_menuid_{0};
      int menu_available_states_{0};
      XPLMMenuID ui_available_set_sub_menuid_{0};
      int menu_config_{0};
      XPLMMenuID ui_config_sub_menuid_{0};
    } strct_menu;
    static strct_menu menu_;

    // --------------------
    static void init_states_from_config_files ();
    static void ui_check_menu_item (XPLMMenuID &in_menuid, const int & in_item_count, const int & in_index_to_check );
    static void pick_the_first_config_file();
    // --------------------
    // --------------------
  public:
    xshifter();
    virtual ~xshifter();
    // --------------------
    static int active_file_id;
    static intptr_t active_set_id;

    static std::map<size_t, shifter::state> map_states_;

    // --------------------
    static void init_ui_state_sub_menu (const XPLMMenuID &in_menu_id);
    bool plugin_start (const char * outName, char* outSig, char* outDesc);
    void plugin_stop();
    void plugin_enable();
    void plugin_disable();

    // --------------------
    static void menu_config_handler(void *inMenuRef, void *inItemRef);
    static void menu_file_picked_by_user_handler(void *inMenuRef, void *inItemRef);
    static int shift_change_set_handler (XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon);
    static int shift_cmd_custom_button_handler (XPLMCommandRef inCommand, XPLMCommandPhase inPhase, void *inRefcon);
    static float m_pluginCallback (float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, int inCounter, void *inRefcon); // fcb

  };
} // shifter

#endif //BUTTON_SHIFTER_XSHIFTER_H