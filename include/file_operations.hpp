/**
 * @author Peruffo Riccardo, RP96, riccardoperuffo96@gmail.com, github.com/RiccardoPeruffo96
 * @date 2022
 * @file file_operations.hpp
 * @brief This contains declarations to
 * manage the loading of input file's
 * and allows to using memory mapping
 * on windows and linux systems.
 * In others systems it will use
 * standards c++ functions.
 * @version 0.0.1 alpha [exam]
 */

#ifndef FILE_OPERATIONS_HPP_
#define FILE_OPERATIONS_HPP_

#include <fstream> //std::ifstream
#include <iostream> //std::cout
#include <list> //std::list

#if _WIN32 || _WIN64
  #include <windows.h> //HANDLE
  #include <fileapi.h> //CreateFileA
  #include <winbase.h> //CreateFileMappingA
  #include <memoryapi.h> //CreateFileMappingW
  #include <errhandlingapi.h> //GetLastError
#elif __linux__
  #include <errno.h> //errno - number of last error
  #include <fcntl.h> //::open
  #include <string.h> //strerror
  #include <sys/mman.h> //::mmap
  #include <sys/stat.h> //::open
  #include <sys/types.h> //::open
  #include <unistd.h> //::lseek
#endif

/**
 * @brief this contains structs and classes
 * using to implements easly I/O operations
 * and optimize them for each system
 */
namespace _file_operations_rp96_
{
  /**
   * @brief Contains a couple: pointer, size and an
   * extra flag is _struct_will_delete_pointer:
   * user can choose if it wants to delete automatically
   * the memory (delete[] arr) or works manually when
   * the struct goes out-of-scope.
   * This struct is designed to works with
   * class file_operations
   */
  struct file_pointer
  {
  public:
    uint8_t* _file_data; ///< pointer to data
    int64_t _size_file; ///< size data
    bool _struct_will_delete_pointer; ///< flag to understand who is taking care of the pointer

    /**
     * @brief set private flags to false and other vars
     * @param _file_data pointer to data
     * @param _size_file size
     * @param _struct_will_delete_pointer if true the destructor will dealloc the memory: default value is true
     */
    file_pointer(uint8_t* _file_data, int64_t _size_file, bool _struct_will_delete_pointer = true) noexcept(true)
    {
      this->_file_data = _file_data;
      this->_size_file = _size_file;
      this->_struct_will_delete_pointer = _struct_will_delete_pointer;

      if (_file_data == nullptr || _size_file <= 0)
      {
        if (_file_data == nullptr) { std::cout << "something wrong with file_pointer: invalid pointer\n"; }
        if (_size_file <= 0) { std::cout << "something wrong with file_pointer: invalid size\n"; }
        this->_file_data = nullptr;
        this->_size_file = 0;
      }
    }

    /**
     * @brief copy constructor
     * @param ref_fp object file_pointer& to copy
     */
    file_pointer(const file_pointer& ref_fp) noexcept(true):
      _file_data{ ref_fp._file_data },
      _size_file{ ref_fp._size_file },
      _struct_will_delete_pointer{ ref_fp._struct_will_delete_pointer }
    {}

    /**
     * @brief copy assignment operator:
     * will be called ~file_pointer(), so if
     * _struct_will_delete_pointer == false
     * and you don't store _file_data
     * you will have memory leak
     * @param ref_fp object file_pointer& to copy
     */
    file_pointer& operator=(const file_pointer& ref_fp) noexcept(true)
    {
      this->~file_pointer();

      this->_file_data = ref_fp._file_data;
      this->_size_file = ref_fp._size_file;
      this->_struct_will_delete_pointer = ref_fp._struct_will_delete_pointer;

      return *this;
    }

    /**
     * @brief move constructor
     * @param ref_fp object file_pointer&& to swap
     */
    file_pointer(file_pointer&& ref_fp) noexcept(true):
      _file_data{ ref_fp._file_data },
      _size_file{ ref_fp._size_file },
      _struct_will_delete_pointer{ ref_fp._struct_will_delete_pointer }
    {
      ref_fp._file_data = nullptr;
      ref_fp._size_file = 0;
      ref_fp._struct_will_delete_pointer = false;
    }

    /**
     * @brief move assignment operator
     * will be called ~file_pointer(), so if
     * _struct_will_delete_pointer == false
     * and you don't store _file_data
     * you will have memory leak
     * @param ref_fp object file_pointer& to swap
     */
    file_pointer& operator=(file_pointer&& ref_fp) noexcept(true)
    {
      this->~file_pointer();

      this->_file_data = ref_fp._file_data;
      this->_size_file = ref_fp._size_file;
      this->_struct_will_delete_pointer = ref_fp._struct_will_delete_pointer;

      ref_fp._file_data = nullptr;
      ref_fp._size_file = 0;
      ref_fp._struct_will_delete_pointer = false;

      return *this;
    }

    /**
     * @brief clean memory if user don't
     * set this->_struct_will_delete_pointer
     * flag false
     */
    ~file_pointer() noexcept(true)
    {
      if (this->_struct_will_delete_pointer == true)
      {
        delete[] this->_file_data;
      }
      this->_file_data = nullptr;
      this->_size_file = 0;
    }

    /**
     * @brief update the flag _struct_will_delete_pointer
     * @param _struct_will_delete_pointer new value: default value is true
     */
    void set_auto_delete(bool _struct_will_delete_pointer = true)
    {
      this->_struct_will_delete_pointer = _struct_will_delete_pointer;
    }

    /**
     * @brief return this message
     * @return this
     */
    file_pointer* get_this()
    {
      return this;
    }
  };

  /**
   * @brief this class works well with struct file_pointer:
   * this class contains methods to work with structs
   */
  class file_operations
  {
  public:

    /**
     * @brief constructor create the var to understand if I can read the file and how,
     * then I start to read so I have always uint8_t* for data
     * @param _file_name file's name in the program's directory
     * @param _is_read_only default value is false, if true I force to use read-only memory access
     */
    file_operations(std::string _file_name, bool _is_read_only = false) noexcept(true);

    /**
     * @brief copy constructor
     * @param ref_fo object file_operations& to copy
     */
    file_operations(const file_operations& ref_fo) noexcept(true);

    /**
     * @brief copy assignment operator
     * @param ref_fo object file_operations& to copy
     */
    file_operations& operator=(const file_operations& ref_fo) noexcept(true);

    /**
     * @brief move constructor
     * @param ref_fo object file_operations& to move
     */
    file_operations(file_operations&& ref_fo) noexcept(true);

    /**
     * @brief move assignment operator
     * @param ref_fo object file_operations& to move
     */
    file_operations& operator=(file_operations&& ref_fo) noexcept(true);

    /**
     * @brief free memory mapping or delete uint8_t*
     */
    ~file_operations() noexcept(true);

    /**
     * @brief this function create a new file and write it datas from differents pointers.
     * "files" param contains pointers that will be readed using FIFO order.
     * @param output_file_name name of new file to create
     * @param files list to couples of pointers and datas to store, at the end files will be empty
     * @param mode flags in OR like (std::trunc | std::out)
     */
    static int32_t save_third_file(std::string& output_file_name, std::list<file_pointer>& files, int32_t mode = 0);

    /**
     * @brief overwrite this file with input's uint8_t* or append it at the end and recalculate mmap.
     * mmap recalculation can be skipped.
     * @param files list to couples of pointers and datas, at the end files will be empty
     * @param i_want_reload_file if true at the end mmap will be recalculate
     * @param mode flags in OR like (std::trunc | std::out): if (std::trunc | std::app) then the first pointer will use std::trunc, others std::app. there isnt't std::trunc only std::app mode
     * @param _is_read_only default value is false, if true I force to use read-only memory access
     */
    int32_t save_this_file(std::list<file_pointer>& files, bool i_want_reload_file, int32_t mode = 0, bool _is_read_only = false);

    /**
     * @brief to obtain the name of file
     * @return this->_file_name the name of file
     */
    std::string get_file_name() { return this->_file_name; }

    /**
     * @brief to obtain the size of file
     * @return this->_size_file the size of file
     */
    int64_t get_size_file() { return this->_size_file; }

    /**
     * @brief to obtain the file's data
     * @return this->_data_file file's data
     */
    uint8_t* get_data_file()
    {
      if (this->_size_file < 0)
      {
        std::cout << "you try to read a nullptr because the file " << this->_file_name << " it's not found\n";
      }
      return this->_data_file;
    }

  private:

    /// std::string _file_name - file's name
    std::string _file_name;
    /// uint32_t _size_file - data_file's length
    int64_t _size_file;
    /// uint8_t* _data_file - file's bytes
    uint8_t* _data_file;

    ///true if I don't have read issue
    bool _can_we_read_the_file;

    ///true if I want read-only access
    bool _is_read_only;

    ///true if I can run read, false if I can run clean
    bool memory_is_clean;

#if _WIN32 || _WIN64

    ///handle for win32 OS because mmap
    HANDLE handle_file;
    HANDLE file_mapping;
    static std::string get_last_error_as_string();

#elif __linux__
    ///integer for linux OS because mmap
    int32_t file_descriptor;
    static std::string get_last_error_as_string(int64_t errn);

#else
    ///ifstream for non linux or win32 OS
    std::ifstream _file_pointer; //always closed after read
#endif

    /**
     * @brief gives value to data_file
     * @return uint8_t* - file's bytes
     */
    int32_t read_file();

    /**
     * @brief set private values except the name, that is const value for the object
     * @param _is_read_only default value is false, if true I force to use read-only memory access
     */
    int32_t set_constructor_vars(bool _is_read_only = false);

    /**
     * @brief write
     * @param _is_read_only default value is false, if true I force to use read-only memory access
     */
    static int32_t write_file(std::string& file_output, uint8_t* datas, int64_t datas_size, int32_t mode = 0);

    /**
     * @brief to clean memory and release mmap
     */
    int32_t clear();

    template <typename T>
    void clear_this(bool pointer_in_use, T*& pointer);
  };
}

#endif //FILE_OPERATIONS_HPP_
