#ifndef LOG_OPERATIONS_RP96_HPP_
#define LOG_OPERATIONS_RP96_HPP_

#include <fstream>

namespace _log_operations_rp96_
{
  #define __LOG_HEADER(log_stream) log_stream.pointer_log << __DATE__ << " " << __TIME__ << " - " << __FILE__ << ":" << __LINE__ << " "
  #define __LOG_GET _log_operations_rp96_::log_operations::get_instance()

  class log_operations
  {
  public:

    static log_operations& get_instance()
    {
      static log_operations instance;
      return instance;
    }

    /*std::ofstream log_write()
    {
      return pointer_log;
    } */

    //singleton's context
    std::ofstream pointer_log;

    //no copy
    log_operations(const log_operations& log_op) = delete;
    log_operations& operator=(const log_operations& log_op) = delete;
    log_operations(log_operations&& log_op) = delete;
    log_operations& operator=(log_operations&& log_op) = delete;

  private:

    //private construct
    log_operations()
    {
      pointer_log = std::ofstream("log", std::ofstream::app);
    }

    ~log_operations()
    {
      pointer_log.close();
    };

  };
} //namespace _log_operations_rp96_

#endif //LOG_OPERATIONS_RP96_HPP_
