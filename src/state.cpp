//
// Created by Saar.Nagar on 09/11/2025.
//

#include "state.h"

namespace shifter
{
state::state () {}

state::~state () {}


bool
state::parse_config_file ()
{
  std::ifstream file(strct_file_metadata_.full_file_path);
  if (!file.is_open()) {
    utils::write_log( std::format("Failed to open file: {}\n", strct_file_metadata_.full_file_path) );
    return false;
  }

  std::string line;
  int shift_button_key_number = -1;
  int custom_command_line_index = 0;
  int current_set_id_value = -1;
  // shifter::strct::st_custom_commands_set current_cmd_set;
  // std::map<int, std::vector<std::string> >  command_sets;
  std::map<int, std::map<std::string, std::string> >  command_sets;


  auto commit_current_state = [&]() {
    if (current_set_id_value >= 0)
    {
      map_command_sets_[current_set_id_value].set_id = current_set_id_value;
      map_command_sets_[current_set_id_value].custom_commands = std::move(command_sets);
    }
  };

  while (std::getline(file, line)) {
    line = utils::trim(line);
    if (line.empty() || line.starts_with(';'))
      continue;

    auto parts = utils::split_and_trim(line, '|');
    // remove commented entries
    std::erase_if(parts, [](const std::string &s) {
      return utils::trim(s).empty() || utils::trim (s).starts_with(';');
    });

    if (parts.empty())
      continue;

    // Detect a new key_state section
    if (parts[0].starts_with('#')) {
      // Commit the previous one
      commit_current_state();

      // Reset new structure
      command_sets = {};
      custom_command_line_index = 0;

      try {
        shift_button_key_number = std::stoi(parts[0].substr(1));
      } catch (...) {
        shift_button_key_number = -1;
      }

      if (shift_button_key_number < 0 || shift_button_key_number > 6)
        continue;

      current_set_id_value = shift_button_key_number;
      if (parts.size() > 1)
      {
        command_sets[shifter::g_shift_button_key_number] = utils::parse_vector_of_strings_to_key_value_pairs ( std::vector<std::string>(parts.begin() + 1, parts.end()), '=' );
      }
      continue;
    }

    // Only process if we have a valid shift_button_key_number
    if (shift_button_key_number >= 0 && shift_button_key_number <= 6) {
      if (custom_command_line_index >= 10)
        continue;

      if (!parts.empty()) {
        command_sets [custom_command_line_index] = std::move(utils::parse_vector_of_strings_to_key_value_pairs (parts, '=') );
        custom_command_line_index++;
      }
    }
  }

  // Commit the last one
  commit_current_state();

  return true;
}
} // shifter