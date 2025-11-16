//
// Created by xplane on 08/11/2025.
//

#ifndef BUTTON_SHIFTER_XX_SHARE_CONFIG_H
#define BUTTON_SHIFTER_XX_SHARE_CONFIG_H
#include "XPLMDataAccess.h"
#include "XPLMDisplay.h"
#include "XPLMMenus.h"

#include <chrono>
#include <map>
#include <set>
#include <string>

namespace shifter
{
extern "C" {}

// --- Global Statics
static const std::set<intptr_t> g_shift_state_codes            = { 0, 1, 2, 3, 4, 5, 6 };
static const std::set<intptr_t> g_shift_commands               = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
static constexpr auto           g_prefix_shift_number_text     = "button_shifter/step_1_map_shift_buttons/shift_";
static constexpr auto           g_prefix_shift_custom_cmd_text = "button_shifter/step_2_map_a_button_to_a_command/command_";

static XPLMDataRef gFrameRateDR = nullptr;

static constexpr auto config_file_extension    = ".shift";
static constexpr int g_shift_button_key_number = 100;

static XPLMHostApplicationID host_id;
static int                   xplm_version;
static int                   xplane_ver_i;
static int                   xplane_using_modern_driver_b;

constexpr static int XP12_VERSION_NO = 120000;
constexpr static int XP11_COMPATIBILITY = 11;
constexpr static int XP12_COMPATIBILITY = 12;

static XPLMDrawingPhase  s_drawPhase = xplm_Phase_Window;

static constexpr auto attrib_cmd         = "cmd";
static constexpr auto attrib_button_type = "button_type";
static constexpr auto button_type_hold   = "hold";
static constexpr auto button_type_tick   = "tick";


static constexpr auto tick_accelerate_0   = 250.0f; // milli
static constexpr auto tick_accelerate_1   = 50.0f; // milli
static constexpr auto tick_accelerate_2   = 10.0f; // milli

static constexpr auto timer_delta_for_the_first_acceleration_1  = 1000.0f; // milli
static constexpr auto timer_delta_for_the_second_acceleration_2 = 3000.0f; // milli

// static std::deque<std::string> s_deque_commands;

namespace enums
{

// between evaluation types
enum class bs_between_types_def : uint8_t
{
  gt_min_eqles_max = 0, // should be the norm
  gt_min_less_max = 1, // no equality
  both_can_be_equal = 2, // both can be equal
  eqgt_min_less_max = 3 // greater or equal min but less than max
};

// Menu Commands/sub menus
typedef enum class menuIdRefs_def
  : uint8_t
{
  MAIN_MENU_ENTRY, // Menu seed under plug-in menu
  MENU_ENTRY_CONFIG, // Create the menu hidden object
  UI_SUB_MENU_CONFIG,
  UI_SUB_MENU_CONFIG_REFRESH_STATE_FILE_LIST, // add an entry to the hidden menu object
  MENU_ENTRY_AVAILABLE_STATES, //
  UI_MENU_ENTRY_AVAILABLE_STATES, //
  MENU_ENTRY_STATE, // Create the menu hidden object
  UI_SUB_MENU_STATE, // add an entry to the hidden menu object
  LAST_ENTRY_STATIC_MENU, // make sure this is always the last entry in this enumeration
  STATE_0,
  STATE_1,
  STATE_2,
  STATE_3,
  STATE_4,
  STATE_5,
  STATE_6,
  STATE_7,
  STATE_8,
  STATE_9,
  STATE_10,
  COUNT
}menuIdRefs;

}

// ---- END enums namespace

namespace strct
{
  typedef struct st_state_file
  {
    shifter::enums::menuIdRefs  seq;
    std::string file_name;
    std::string path_only;
    std::string full_file_path;
    st_state_file () = default;
    st_state_file (const int i, const std::string & in_file_name, const std::string & in_path, const std::string & in_full_file_path)
    {
      seq = shifter::enums::menuIdRefs::STATE_0;
      file_name = in_file_name;
      path_only = in_path;
      full_file_path = in_full_file_path;
    };
  } strct_state_file;


  struct st_custom_commands_set
  {
    // Stores the "#" number
    int           set_id{ -1 };
    int           set_menu_index{ -1 };
    XPLMMenuCheck check_state_{ xplm_Menu_Unchecked };

    // Commands will be stored as a map of vectores for future expandability.
    // The topic command will be stored as key: "100". We only allow 10 custom commands.
    // std::map<int, std::vector<std::string> > custom_commands;
    std::map<int, std::map<std::string, std::string> > custom_commands;
    st_custom_commands_set () = default;

    void reset_ui_info()
    {
      set_menu_index = -1;
      check_state_ = xplm_Menu_Unchecked;
    }
  };






  struct time_fragment_def
  {
    using nano_s  = std::chrono::nanoseconds;
    using micro_s = std::chrono::microseconds;
    using milli_s = std::chrono::milliseconds;
    using seconds = std::chrono::seconds;
    using minutes = std::chrono::minutes;
    using hours   = std::chrono::hours;

    // Chrono
    std::chrono::time_point<std::chrono::steady_clock> os_clock;
    long long                                          deltaOsClock_milli = 0;
    std::chrono::duration<double>                      deltaOsClock_seconds {0.0};

    time_fragment_def (): os_clock(std::chrono::steady_clock::now()) {}

    time_fragment_def& operator=(time_fragment_def const& in_time_fragment) = default;

    long long operator-(const time_fragment_def & inTime)
    {
      #ifndef RELEASE
      const auto delta_time = inTime.os_clock - os_clock;
      #endif

      this->deltaOsClock_milli   = std::chrono::duration_cast<std::chrono::milliseconds> (inTime.os_clock - this->os_clock).count ();
      this->deltaOsClock_seconds = inTime.os_clock - this->os_clock;

      return this->deltaOsClock_milli;
    }

    void reset()
    {
      deltaOsClock_milli = 0;
      os_clock = std::chrono::steady_clock::now();
    }

  };

} // end strct namespace

}

#endif //BUTTON_SHIFTER_XX_SHARE_CONFIG_H