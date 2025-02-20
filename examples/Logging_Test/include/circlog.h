//-----------------------------------------------------------------------------
// Circular Logging System
// 19.02.2025 Halid Y.
//-----------------------------------------------------------------------------
#ifndef CIRCLOG_H
#define CIRCLOG_H

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
    time_t get_oldest_log_file_date();
  public:
    CircLog(const char* log_type = "log");
    //void log(const char* Text, log_tags_e tag = MESSAGE);
    void log(const char* Text, const char* custom_tag);
};

//-----------------------------------------------------------------------------
#endif