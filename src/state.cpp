//
// Created by Saar.Nagar on 09/11/2025.
//

#include "state.h"

namespace shifter
{
state::state () {}

// ---------------------------------

state::~state () {}

// ---------------------------------


// ---------------------------------
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
  int current_set_id = -1;
  // shifter::strct::st_custom_commands_set current_cmd_set;
  // std::map<int, std::vector<std::string> >  command_sets;
  std::map<int, std::map<std::string, std::string> >  map_all_commands_in_the_current_set;
  std::map<int, strct::st_command_ref >  map_strct_commands;


  auto lmbda_commit_current_state = [&]()
  {
    if (current_set_id >= 0)
    {
      map_command_sets_[current_set_id].set_id = current_set_id;
      // map_command_sets_[current_set_id].map_all_lines_and_their_split_commands = std::move(map_all_commands_in_the_current_set);
      map_command_sets_[current_set_id].map_lines_metadata = std::move(map_strct_commands);
    }
  };

  while (std::getline(file, line))
  {
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
      lmbda_commit_current_state();

      // Reset new structure
      map_all_commands_in_the_current_set = {};
      map_strct_commands = {}; // v0.3
      custom_command_line_index = 0;

      // validate we have a new "shifter id" number Must be between 1..6
      try {
        shift_button_key_number = std::stoi(parts[0].substr(1));
      } catch (...) {
        shift_button_key_number = -1;
      }

      if (shift_button_key_number < 0 || shift_button_key_number > 6)
        continue;

      current_set_id = shift_button_key_number;
      if (parts.size() > 1)
      {
        map_all_commands_in_the_current_set[shifter::g_shift_button_key_number] = utils::parse_vector_of_strings_to_key_value_pairs ( std::vector<std::string>(parts.begin() + 1, parts.end()), '=' );
        map_strct_commands[shifter::g_shift_button_key_number].map_command_metadata = utils::parse_vector_of_strings_to_key_value_pairs ( std::vector<std::string>(parts.begin() + 1, parts.end()), '=' );
      }
      continue;
    }

    // Only process if we have a valid shift_button_key_number
    if (shift_button_key_number >= 0 && shift_button_key_number <= 6) {
      if (custom_command_line_index >= 10)
        continue;

      if (!parts.empty()) {
        map_all_commands_in_the_current_set [custom_command_line_index] = std::move(utils::parse_vector_of_strings_to_key_value_pairs (parts, '=') );
        // todo: replace this line with the "std::move" once plugin is stable
        map_strct_commands[custom_command_line_index].map_command_metadata = map_all_commands_in_the_current_set [custom_command_line_index];

        // check command or dataref
        if (map_strct_commands[custom_command_line_index].map_command_metadata.contains(attrib_cmd) )
        {
          map_strct_commands[custom_command_line_index].type = enums::command_type::command;
          utils::parse_command(map_strct_commands[custom_command_line_index]);
        }
        else if (map_strct_commands[custom_command_line_index].map_command_metadata.contains(attrib_drf) )
        { // check dref
          map_strct_commands[custom_command_line_index].type = enums::command_type::dataref;
          utils::parse_dataref(map_strct_commands[custom_command_line_index]);
        }

        // increase index
        custom_command_line_index++;
      }
    }
  }

  // Commit the last one
  lmbda_commit_current_state();

  return true;
}

// ---------------------------------
// ---------------------------------
// ---------------------------------

} // namespace shifter