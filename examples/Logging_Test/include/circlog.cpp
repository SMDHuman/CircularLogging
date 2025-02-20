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
time_t loginterval_to_time(string date);

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
                    {"Logging_Interval", "1T00-00-00"},
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
  time_t newest_file_t = CircLog::get_newest_log_file_date();
  time_t log_interval = loginterval_to_time(config_json.at("Logging_Interval"));

  if(now_tt - newest_file_t > log_interval){
    CircLog::create_log_now();
  }
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
time_t CircLog::get_newest_log_file_date(){
  time_t newest_file_time = 0;
  for (const auto& file : fs::directory_iterator(config_json.at("Log_Path"))) {
    // Get file name
    fs::path f_path = file.path();
    string file_name = f_path.filename().generic_string();
    // Convert file name to time_t
    time_t file_time = logdate_to_time(file_name.substr(0, 19));
    if(file_time > newest_file_time){
      newest_file_time = file_time;
    }
  }
  return(newest_file_time);
}//-----------------------------------------------------------------------------
time_t CircLog::get_oldest_log_file_date(){
  time_t newest_file_time = 0;
  for (const auto& file : fs::directory_iterator(config_json.at("Log_Path"))) {
    // Get file name
    fs::path f_path = file.path();
    string file_name = f_path.filename().generic_string();
    // Convert file name to time_t
    time_t file_time = logdate_to_time(file_name.substr(0, 19));
    if(file_time > newest_file_time){
      newest_file_time = file_time;
    }
  }
  return(newest_file_time);
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