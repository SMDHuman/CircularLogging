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
time_t logdate_to_time(string date);

//-----------------------------------------------------------------------------
json config_json;

//-----------------------------------------------------------------------------
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
                    {"Logging_Type", "log"},
                    {"Max_Log", "20"},
                    {"Logging_Interval", "0000-00-01T00-00-00"},
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
void CircLog::log(const char* Text, const char* custom_tag){
  auto now = chrono::system_clock::now();
  time_t now_tt = chrono::system_clock::to_time_t(now);
  printf("now_tt: %d\n", now_tt);
  time_t last_file_t = CircLog::get_last_log_file_date();
  printf("last file: %d\n", last_file_t);
  time_t log_interval = logdate_to_time(config_json.at("0000-00-01T00-00-00"));
  printf("log_interval: %d\n", log_interval);
  if(now_tt - last_file_t > log_interval){

  }
}
//-----------------------------------------------------------------------------
void CircLog::load_config(){
  ifstream config_file("circlog.config");
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
time_t CircLog::get_last_log_file_date(){
  time_t last_file_time = 0;
  for (const auto& file : fs::directory_iterator(config_json.at("Log_Path"))) {
    // Get file name
    fs::path f_path = file.path();
    string file_name = f_path.filename().generic_string();
    // Convert file name to time_t
    time_t file_time = logdate_to_time(file_name.substr(0, 17));
    if(file_time > last_file_time){
      last_file_time = file_time;
    }
  }
  return(last_file_time);
}
//-----------------------------------------------------------------------------
// Converts yyyy-mm-ddThh-mm-ss format string to time_t
time_t logdate_to_time(string date){
  tm date_tm = {};
  istringstream ss(date);
  ss >> get_time(&date_tm, "%Y-%m-%dT%H-%M-%S");
  return(mktime(&date_tm));
}