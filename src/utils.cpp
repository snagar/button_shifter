//
// Created by xplane on 08/11/2025.
//

#include "utils.h"

#include "XPLMPlugin.h"

#include <algorithm>
#include <format>
#include <fstream>
#include <iostream>
#include <set>



namespace shifter
{

std::string
utils::GetPluginRelativePath ()
{
  char pluginPath[2048];
  auto plugin_id = XPLMGetMyID ();
  XPLMGetPluginInfo (XPLMGetMyID (), nullptr, pluginPath, nullptr, nullptr);

  std::string_view            plugin_path_vu     = pluginPath;
  const std::filesystem::path plugin_binary_path = plugin_path_vu;
  if (plugin_path_vu.find ("win_x64") != std::string_view::npos
      || plugin_path_vu.find ("lin_x64") != std::string_view::npos
      || plugin_path_vu.find ("mac_x64") != std::string_view::npos)
    return plugin_binary_path.parent_path ().parent_path ().string ();

  // Fallback to xp11 binary location
  return plugin_binary_path.parent_path ().string ();
}

bool
utils::create_log_file (const std::string &filename)
{
  g_logFile.open (filename, std::ios::out | std::ios::app);
  return g_logFile.is_open ();
}

// template<typename ... Args>
// void
// utils::write_log (const std::string &text, Args &&...args)
void
utils::write_log (const std::string &text)
{
  if (!g_logFile.is_open ())
    return;

  // Get current time
  auto now     = std::chrono::system_clock::now ();
  auto timeStr = std::format ("{:%Y-%m-%d %H:%M:%S}", now);

  // Format the message
  // std::string formatted = std::format(text, std::forward<Args>(args)...);

  // Write to log file with timestamp
  g_logFile << std::format ("[{}] {}\n", timeStr, text);
  g_logFile.flush (); // ensure it is written immediately
}

bool
utils::is_ascii_file (const std::filesystem::path &filePath)
{
  std::ifstream file (filePath, std::ios::binary);
  if (!file.is_open ())
    return false;

  char c;
  while (file.get (c))
  {
    auto uc = static_cast<unsigned char> (c);
    // If byte is not printable ASCII or common whitespace, consider binary
    if (uc < 9 || (uc > 13 && uc < 32) || uc > 126)
      return false;
  }
  return true;
}

std::map<size_t, shifter::strct::st_state_file>
utils::list_state_files (const std::string &plugin_folder_path)
{
  std::set<std::string>                                ordered_file_list_set;
  std::map<std::string, shifter::strct::st_state_file> map_state_files; // [file name], [file struct]

  try
  {
    for (const auto &entry : std::filesystem::directory_iterator (plugin_folder_path))
    {
      if (entry.is_regular_file () && entry.path ().extension () == shifter::config_file_extension) // "Should be: ".shift"
      {
        // Verify the file is ASCII by opening and checking for non-printable characters
        std::ifstream file (entry.path (), std::ios::binary);
        if (file)
        {

          #ifndef RELEASE
          bool is_ascii = is_ascii_file (entry.path ());
          #endif           
          //if (is_ascii) // ignore
          {
            strct::st_state_file st_file;
            st_file.file_name      = entry.path ().filename ().string ();
            st_file.full_file_path = entry.path ().string ();
            st_file.path_only      = entry.path ().parent_path ().string ();

            ordered_file_list_set.insert (st_file.file_name);

            auto index                         = map_state_files.size () - 1;
            map_state_files[st_file.file_name] = st_file;
          }
        } // end if "file" is valid

      } // end if the file is "regular" and ends with ".state"

    } // end iterator
  }
  catch (const std::filesystem::filesystem_error &e)
  {
    std::cerr << "Filesystem error: " << e.what () << '\n';
  }

  // loop over "set" and order the map correctly
  std::map<size_t, shifter::strct::st_state_file> map_final_state_files;
  int                                             index = 1;
  for (const auto &key : ordered_file_list_set)
  {
    map_state_files[key].seq                                 = shifter::enums::menuIdRefs::STATE_0;
    map_final_state_files[map_final_state_files.size () + 1] = map_state_files[key];
    ++index;
  }

  return map_final_state_files;
}


std::string
utils::trim (const std::string &str, const std::string& delimiters)
{
  const auto start = str.find_first_not_of(delimiters);
  const auto end = str.find_last_not_of(delimiters);
  if (start == std::string::npos)
    return "";

  return str.substr(start, end - start + 1);
}


std::vector<std::string>
utils::split_and_trim (const std::string &line, const char delimiter)
{
  std::vector<std::string> parts;
  std::stringstream ss(line);
  std::string item;

  while (std::getline(ss, item, delimiter)) {
    auto trimmed = trim(item);

    if (!trimmed.empty())
      parts.push_back(std::move(trimmed));
  }
  return parts;
}

std::map<std::string, std::string>
utils::parse_vector_of_strings_to_key_value_pairs (const std::vector<std::string> &in_values, const char delimiter)
{
  std::map<std::string, std::string> map_result;
  for (const auto &entry : in_values)
  {
    auto vec = utils::split_and_trim(entry, delimiter);
    if (vec.size() > 1)
      map_result[vec[0]] = vec[1];
  }

  return map_result;
}

// float
// utils::GetFPS ()
// {
//   if (!gFrameRateDR)
//   {
//     gFrameRateDR = XPLMFindDataRef("sim/operation/misc/frame_rate_period");
//     if (!gFrameRateDR) return 0.0f;
//   }
//
//   float frameTime = XPLMGetDataf(gFrameRateDR);
//   if (frameTime <= 0.0f) return 0.0f;
//
//   return 1.0f / frameTime;
// }


} // shifter