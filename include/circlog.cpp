//-----------------------------------------------------------------------------
#include <filesystem>
#include <iostream>
#include <fstream>
#include "nlohmann/json.hpp"
#include "circlog.h"
#include <ctime>
#include <string>
#include <chrono>

namespace fs = std::filesystem;
using namespace std;
using json = nlohmann::json;

//-----------------------------------------------------------------------------
// Until Functions
static time_t logdate_to_time(string date);
static time_t loginterval_to_time(string date);

//-----------------------------------------------------------------------------
json config_json;

//-----------------------------------------------------------------------------
// Initialization fuction for circular logging object
CircLog::CircLog(const char* log_type){
  // Search in the current directory for config file
  uint8_t find_cfg = false;
  uint8_t find_log_dir = false;
  for (const auto& file : fs::directory_iterator(fs::current_path())) {
    fs::path f_path = file.path();
    if(f_path.filename() == "circlog.config"){
      find_cfg = true;
    }
  }
  // If config file not found, create with default values
  if(!find_cfg){
    fstream config_file("circlog.config", ios::out);
    config_json = {
                    // File type extention ".log"
                    {"Logging_Type", "log"},
                    // Max number of Log files will saved 
                    {"Max_Log", 15},
                    // Day can be any lenght except none                 
                    //                    dThh-mm-ss
                    {"Logging_Interval", "1T00-00-00"},
                    // Where log files will stored
                    {"Log_Path", "./log"}
                  };
    config_file << config_json;
    config_file.close();
  }
  CircLog::load_config();
  // Check Log Directory
  if(!fs::is_directory(config_json.at("Log_Path"))){
    fs::create_directory(config_json.at("Log_Path"));
  }
}
//-----------------------------------------------------------------------------
void CircLog::log(const char* Text){
  CircLog::log(Text, MESSAGE);
}
//-----------------------------------------------------------------------------
void CircLog::log(const char* Text, log_tags_e tag){
  if(tag == MESSAGE) CircLog::log(Text, "MESSAGE");
  else if(tag == WARNING) CircLog::log(Text, "WARNING");
  else if(tag == ERROR) CircLog::log(Text, "ERROR");
}
//-----------------------------------------------------------------------------
void CircLog::log(const char* Text, const char* custom_tag){
  auto now = chrono::system_clock::now();
  time_t now_tt = chrono::system_clock::to_time_t(now);
  time_t newest_file_t = CircLog::get_newest_log_file_date();
  time_t log_interval = loginterval_to_time(config_json.at("Logging_Interval"));
  // If log interval time passed until the last log, create new log
  if(now_tt - newest_file_t >= log_interval){
    CircLog::create_log_now();
  }
  // Deletes older files if max number of files exceeted 
  while(CircLog::get_log_file_count() > config_json.at("Max_Log")){
    CircLog::delete_oldest_log_file();
  }
  // Open latest log file
  fs::path f_path = CircLog::get_newest_log_file();
  fstream file(f_path, ios::out);
  // Writes the log message "[date] [tag] message"
  stringstream ss;
  file << put_time(localtime(&now_tt), "[%Y-%m-%d %H:%M:%S] ");
  file << "[" << custom_tag << "] ";
  file << Text << endl;
  file.close();
}
//-----------------------------------------------------------------------------
void CircLog::load_config(){
  fstream config_file("circlog.config", ios::in);
  config_json = json::parse(config_file);
  config_file.close();
}
//-----------------------------------------------------------------------------
void CircLog::save_config(){
  fstream config_file("circlog.config", ios::out);
  config_file << config_json;
  config_file.close();
}
//-----------------------------------------------------------------------------
void CircLog::create_log_now(){
  auto now = chrono::system_clock::now();
  time_t now_tt = chrono::system_clock::to_time_t(now);
  //...
  stringstream ss;
  ss << put_time(localtime(&now_tt), "%Y-%m-%dT%H-%M-%S");
  string new_filename = ss.str();
  new_filename.append(".");
  new_filename.append(config_json.at("Logging_Type"));
  string path_name = config_json.at("Log_Path");
  path_name.append("/");
  path_name.append(new_filename);
  //...
  fstream file(path_name, ios::out);
  file.close();
}
//-----------------------------------------------------------------------------
time_t CircLog::get_newest_log_file_date(){
  fs::path f_path = CircLog::get_newest_log_file();
  string file_name = f_path.filename().generic_string();
  // Convert file name to time_t
  time_t file_time = logdate_to_time(file_name.substr(0, 19));
  return(file_time);
}
//-----------------------------------------------------------------------------
void CircLog::delete_oldest_log_file(){
  time_t oldest_file_time = -1;
  fs::path oldest_file_path;
  uint8_t any = false;
  for (const auto& file : fs::directory_iterator(config_json.at("Log_Path"))) {
    // Get file name
    fs::path f_path = file.path();
    string file_name = f_path.filename().generic_string();
    // Convert file name to time_t
    time_t file_time = logdate_to_time(file_name.substr(0, 19));
    // Select first file as oldest
    if(oldest_file_time == -1){
      any = true;
      oldest_file_time = file_time;
      oldest_file_path = f_path;
    }
    // Compare other file with current kown oldest
    else if(file_time < oldest_file_time){
      oldest_file_time = file_time;
      oldest_file_path = f_path;
    }
  }
  // Delete log file if there is any 
  if(any){
    fs::remove(oldest_file_path);
  }
}
//-----------------------------------------------------------------------------
unsigned int CircLog::get_log_file_count(){
  unsigned int count = 0;
  for (const auto& file : fs::directory_iterator(config_json.at("Log_Path"))) {
    count++;
  }
  return(count);
}
//-----------------------------------------------------------------------------
std::filesystem::path CircLog::get_newest_log_file(){
  time_t newest_file_time = 0;
  fs::path newest_file_path;
  for (const auto& file : fs::directory_iterator(config_json.at("Log_Path"))) {
    // Get file name
    fs::path f_path = file.path();
    string file_name = f_path.filename().generic_string();
    // Convert file name to time_t
    time_t file_time = logdate_to_time(file_name.substr(0, 19));
    if(file_time > newest_file_time){
      newest_file_time = file_time;
      newest_file_path = f_path;
    }
  }
  return(newest_file_path);
}
//-----------------------------------------------------------------------------
// Converts yyyy-mm-ddThh-mm-ss format string to time_t
time_t logdate_to_time(string date){
  tm date_tm = {};
  istringstream ss(date);
  ss >> get_time(&date_tm, "%Y-%m-%dT%H-%M-%S");
  return(mktime(&date_tm));
}
//-----------------------------------------------------------------------------
// Converts yyyy-mm-ddThh-mm-ss format string to time_t
time_t loginterval_to_time(string date){
  //...
  string day;
  uint8_t i = 0;
  while(true){
    // Append each char to day until 'T'
    if(date[i] == 'T'){
      break;
    }
    else{
      day.append(date, i, 1);
    }
    i++;
  }
  //...
  time_t time = stoi(day)*86400;
  time += stoi(date.substr(i+1, 2)) * 3600; // Hour
  time += stoi(date.substr(i+4, 2)) * 60; // Minute
  time += stoi(date.substr(i+7, 2)); // Second
  return(time);
}