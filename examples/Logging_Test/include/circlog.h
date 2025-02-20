//-----------------------------------------------------------------------------
// Circular Logging System
// 19.02.2025 Halid Y.
//-----------------------------------------------------------------------------
#ifndef CIRCLOG_H
#define CIRCLOG_H

#include <filesystem>

enum log_tags_e{
  MESSAGE,
  WARNING,
  ERROR
};

//-----------------------------------------------------------------------------
class CircLog{
  private:
    void load_config();
    void save_config();
    void create_log_now();
    time_t get_newest_log_file_date();
    void delete_oldest_log_file();
    unsigned int get_log_file_count();
    std::filesystem::path get_newest_log_file();
  public:
    CircLog(const char* log_type = "log");
    void log(const char* Text);
    void log(const char* Text, log_tags_e tag);
    void log(const char* Text, const char* custom_tag);
};

//-----------------------------------------------------------------------------
#endif