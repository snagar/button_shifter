//
// Created by xplane on 08/11/2025.
//

#ifndef BUTTON_SHIFTER_UTILS_H
#define BUTTON_SHIFTER_UTILS_H

#include "XPLMMenus.h"
#include "XPLMUtilities.h"
#include "xx_share_config.h"

#include <filesystem>
#include <fstream>
#include <map>
#include <string>

namespace shifter
{
  static std::ofstream g_logFile;

  class utils
  {
  private:

  public:
    static std::string GetPluginRelativePath();
    // --------------------------
    // Function 1: Create or open the log file
    static bool create_log_file(const std::string& filename);
    // --------------------------
    // Function 2: Write formatted text into the log
    // static void write_log(const std::string& text, Args&&... args);
    static void write_log(const std::string& text);

    // --------------------------
    // numbers
    // --------------------------
    static bool is_number(std::string_view s, double &out_value);

    // --------------------------
    // Helper: check if a file is ASCII (simple heuristic)
    static bool                       is_ascii_file(const std::filesystem::path& filePath);
    // --------------------------

    // get the list of all ".state" files in the plugin folder
    static std::map<size_t, shifter::strct::strct_state_file> list_state_files(const std::string& plugin_folder_path);

    // --------------------------
    static std::string trim(const std::string &str, const std::string& delimiters = " \t");
    // --------------------------
    static std::vector<std::string> split_and_trim(const std::string &line, char delimiter);

    // --------------------------

    template <class T>
    static std::vector<T> split_string_to_numbers (const std::string &text, char delimiter)
    {
      std::vector<T> result_vector;
      for (const auto vec_split_s = split_and_trim(text, ',')
         ; const auto& element : vec_split_s)
      {
        if (double value
            ; utils::is_number(element, value))
          result_vector.push_back(value);
        else
        {
          result_vector.clear();
          break; // exit loop
        }
      }

      return result_vector;
    }

    // --------------------------
    // each value in the vector should have a "key=value" which we need to split and put in a map for later use
    static std::map <std::string, std::string> parse_vector_of_strings_to_key_value_pairs (const std::vector<std::string> &in_values, char delimiter);
    // --------------------------
    static bool parse_dataref (shifter::strct::st_command_ref & inout_command_strct);
    static bool parse_command (shifter::strct::st_command_ref & inout_command_strct);
  };
} // shifter

#endif //BUTTON_SHIFTER_UTILS_H