/**
 * @author Peruffo Riccardo, RP96, riccardoperuffo96@gmail.com, github.com/RiccardoPeruffo96
 * @date 30th APR 2022
 * @file file_operations.cpp
 * @brief this file contains the definitions of file_operations.hpp's declarations
 * @version 0.0.1 alpha [exam]
 */

#include "file_operations.hpp"

using namespace _file_operations_rp96_;

file_operations::file_operations(std::string _file_name, bool _is_read_only) noexcept(true)
{
  //set global vars
  this->_file_name = std::move(_file_name);
  //then using a third-function to set everything
  //and store the return code
  auto result = this->set_constructor_vars(_is_read_only);
  //check if everything goes well
  if (result == -1)
  {
    std::cout << "costructor file_operations() goes wrong\n";
  }
}

file_operations::file_operations(const file_operations& ref_fo) noexcept(true)
{
  //set global vars
  this->_file_name = ref_fo._file_name;
  //then using a third-function to set everything
  //and store the return code
  auto result = this->set_constructor_vars(ref_fo._is_read_only);
  //check if everything goes well
  if (result == -1)
  {
    std::cout << "costructor file_operations() goes wrong\n";
  }
}

file_operations& file_operations::operator=(const file_operations& ref_fo) noexcept(true)
{
  this->~file_operations();

  this->_file_name = ref_fo._file_name;

  //then using a third-function to set everything
  //and store the return code
  auto result = this->set_constructor_vars(ref_fo._is_read_only);
  //check if everything goes well
  if (result == -1)
  {
    std::cout << "costructor file_operations() goes wrong\n";
  }

  return *this;
}

file_operations::file_operations(file_operations&& ref_fo) noexcept(true):
  _size_file{ ref_fo._size_file },
  _data_file{ ref_fo._data_file },
  _can_we_read_the_file{ ref_fo._can_we_read_the_file },
  _is_read_only{ ref_fo._is_read_only },
  memory_is_clean{ ref_fo.memory_is_clean }
{
  this->_file_name = ref_fo._file_name;

  #if _WIN32 || _WIN64
  this->handle_file = ref_fo.handle_file;
  this->file_mapping = ref_fo.file_mapping;
  ref_fo.handle_file = INVALID_HANDLE_VALUE;
  ref_fo.file_mapping = NULL;
  #elif __linux__
  this->file_descriptor = ref_fo.file_descriptor;
  ref_fo.file_descriptor = -1;
  #else
  //only move assignment 'cause copy is deprecated
  this->_file_pointer = ref_fo._file_pointer;
  #endif

  ref_fo._data_file = nullptr;
  ref_fo._file_name.clear();
  ref_fo._size_file = -1;
  ref_fo.memory_is_clean = true;
}

file_operations& file_operations::operator=(file_operations&& ref_fo) noexcept(true)
{
  this->~file_operations();

  this->_file_name = ref_fo._file_name;
  this->_size_file = ref_fo._size_file;
  this->_data_file = ref_fo._data_file;
  this->_can_we_read_the_file = ref_fo._can_we_read_the_file;
  this->_is_read_only = ref_fo._is_read_only;
  this->memory_is_clean = ref_fo.memory_is_clean;

  #if _WIN32 || _WIN64
  this->handle_file = ref_fo.handle_file;
  this->file_mapping = ref_fo.file_mapping;
  ref_fo.handle_file = INVALID_HANDLE_VALUE;
  ref_fo.file_mapping = NULL;
  #elif __linux__
  this->file_descriptor = ref_fo.file_descriptor;
  ref_fo.file_descriptor = -1;
  #else
  //only move assignment 'cause copy is deprecated
  this->_file_pointer = ref_fo._file_pointer;
  #endif

  ref_fo._data_file = nullptr;
  ref_fo._file_name.clear();
  ref_fo._size_file = -1;
  ref_fo.memory_is_clean = true;

  return *this;
}

file_operations::~file_operations() noexcept(true)
{
  //delete the actual mmap if setted
  auto result = this->clear();
  //check if everything goes well
  if (result == -1)
  {
    std::cout << "destructor ~file_operations() goes wrong\n";
  }
}

int32_t file_operations::read_file()
{
  if (this->memory_is_clean == false)
  {
    return 0;
  }
#if _WIN32 || _WIN64

  auto FLAG_TMP_HANDLEF_PERM = ((this->_is_read_only == true) ? (GENERIC_READ) : (GENERIC_READ | GENERIC_WRITE));
  auto FLAG_TMP_HANDLEF_ATTRB = ((this->_is_read_only == true) ? (FILE_ATTRIBUTE_READONLY) : (FILE_ATTRIBUTE_NORMAL));
  auto FLAG_TMP_FMAP = ((this->_is_read_only == true) ? (PAGE_READONLY) : (PAGE_READWRITE));
  auto FLAG_TMP_MAPVIEW = ((this->_is_read_only == true) ? (FILE_MAP_READ) : (FILE_MAP_ALL_ACCESS));

  this->handle_file = CreateFileA(this->_file_name.c_str(), FLAG_TMP_HANDLEF_PERM, 0, NULL, OPEN_EXISTING, FLAG_TMP_HANDLEF_ATTRB, NULL);

  if(this->handle_file == INVALID_HANDLE_VALUE)
  {
    std::cout << "wrong something with CreateFileA: " << this->get_last_error_as_string() << "\n";
    return -1;
  }

  this->file_mapping = CreateFileMappingA(this->handle_file, NULL, FLAG_TMP_FMAP, 0, 0, NULL);

  if (this->file_mapping == NULL)
  {
    std::cout << "wrong something with CreateFileMappingA: " << this->get_last_error_as_string() << "\n";
    return -1;
  }

  this->_data_file = reinterpret_cast<uint8_t*>(MapViewOfFile(this->file_mapping, FLAG_TMP_MAPVIEW, 0, 0, 0));

  if (this->_data_file == NULL)
  {
    this->_data_file = nullptr;
    std::cout << "wrong something with MapViewOfFile: " << this->get_last_error_as_string() << "\n";
    return -1;
  }

#elif __linux__

  int64_t err_value = -1;

  auto FLAG_OPEN = ((this->_is_read_only == true) ? (O_RDONLY) : (O_RDWR));
  auto FLAG_MMAP = ((this->_is_read_only == true) ? (PROT_READ) : (PROT_EXEC | PROT_READ | PROT_WRITE));

  this->file_descriptor = ::open(this->_file_name.c_str(), FLAG_OPEN);

  if (this->file_descriptor == -1)
  {
    //'cause the following indications: hxxps://www.man7[.]org/linux/man-pages/man3/errno.3.html
    //save always the error like first operation
    err_value = errno;
    std::cout << "wrong something with ::open: " << get_last_error_as_string(err_value) << "\n";
    return -1;
  }

  if (::lseek(file_descriptor, static_cast<off_t>(this->_size_file - 1), SEEK_SET) == -1) // try to get the last byte
  {
    //'cause the following indications: hxxps://www.man7[.]org/linux/man-pages/man3/errno.3.html
    //save always the error like first operation
    err_value = errno;
    std::cout << "wrong something with ::lseek: " << get_last_error_as_string(err_value) << "\n";
    return -1;
  }

  //set the read flags
  //this if a different ways to declare
  this->_data_file = reinterpret_cast<uint8_t*>(::mmap(nullptr, this->_size_file, FLAG_MMAP, MAP_PRIVATE, this->file_descriptor, 0));

  //check if last operation is completed
  if (this->_data_file == MAP_FAILED)
  {
    //'cause the following indications: hxxps://www.man7[.]org/linux/man-pages/man3/errno.3.html
    //save always the error like first operation
    err_value = errno;
    std::cout << "wrong something with ::mmap: " << get_last_error_as_string(err_value) << "\n";
    return -1;
  }

  // Advise sequential access
  if (::madvise(this->_data_file, this->_size_file, MADV_SEQUENTIAL) == -1)
  {
    //'cause the following indications: hxxps://www.man7[.]org/linux/man-pages/man3/errno.3.html
    //save always the error like first operation
    err_value = errno;
    std::cout << "wrong something with ::madvise: " << get_last_error_as_string(err_value) << "\n";
    return -1;
  }

  //MemoryMapped Operations
  //read from/write to "mmap_ptr" as a normal array: mmap_ptr[i]

#else

  this->_data_file = new uint8_t[this->_size_file];

  for (int64_t i = 0; i < this->_size_file; ++i)
  {
    this->_data_file[i] = _file_pointer.get();
  }

  this->_file_pointer.close();

#endif
  //this flag means the reading goes well
  //and the file is stored
  this->memory_is_clean = false;

  //I return 0 that it means "ok"
  return 0;
}

int32_t file_operations::clear()
{
  if (this->memory_is_clean == true)
  {
    return 0;
  }
  if (this->_can_we_read_the_file == false)
  {
    return 0;
  }

  // Close Memory Mapped file
#if _WIN32 || _WIN64
  if (CloseHandle(this->file_mapping) == NULL)
  {
    std::cout << "wrong something with CloseHandle(file_mapping): " << this->get_last_error_as_string() << "\n";
    return -1;
  }
  if (UnmapViewOfFile(this->_data_file) == NULL)
  {
    std::cout << "wrong something with UnmapViewOfFile(mmap_ptr): " << this->get_last_error_as_string() << "\n";
    return -1;
  }
  if (CloseHandle(this->handle_file) == NULL)
  {
    std::cout << "wrong something with CloseHandle(handle_file): " << this->get_last_error_as_string() << "\n";
    return -1;
  }
#elif __linux__

  int64_t err_value = -1;

  if (::munmap(_data_file, this->_size_file) == -1)
  {
    //'cause the following indications: hxxps://www.man7[.]org/linux/man-pages/man3/errno.3.html
    //save always the error like first operation
    err_value = errno;
    std::cout << "wrong something with ::munmap: " << get_last_error_as_string(err_value) << "\n";
    return -1;
  }
  if (::close(file_descriptor) == -1)
  {
    //'cause the following indications: hxxps://www.man7[.]org/linux/man-pages/man3/errno.3.html
    //save always the error like first operation
    err_value = errno;
    std::cout << "wrong something with ::close: " << get_last_error_as_string(err_value) << "\n";
    return -1;
  }
#else
  delete[] this->_data_file;
  this->_data_file = nullptr;
#endif

  this->memory_is_clean = true;
  this->_data_file = nullptr;

  return 0;
}

int32_t file_operations::set_constructor_vars(bool _is_read_only)
{
  this->_data_file = nullptr;
  this->memory_is_clean = true;

  //if we are in linux or win, we use mmap
  //else we continue to use ifstream

#if (_WIN32  || _WIN64 || __linux__)
  std::ifstream _file_pointer(this->_file_name, std::ios::binary | std::ios::in); //we open the file
  this->_is_read_only = _is_read_only; //param only for mmap creation, useless with ifstream
#else
  this->_file_pointer = std::ifstream(this->_file_name, std::ios::binary | std::ios::in);
#endif

  this->_can_we_read_the_file = _file_pointer.is_open();

  if (this->_can_we_read_the_file == false)
  {
    std::cout << "file " << this->_file_name << " not found\n";
    this->_size_file = -1;
    return -1;
  }

  //I save the file's size
  _file_pointer.seekg(0, std::ios::end);
  this->_size_file = _file_pointer.tellg();
  _file_pointer.seekg(0, std::ios::beg);

  //if we are not in linux or win32, I need to use the ifstream obj
#if (_WIN32  || _WIN64 || __linux__)
  _file_pointer.close();
#endif

  if (this->_size_file > 0)
  {
    if (this->read_file() == -1)
    {
      std::cout << "error with read file";
      return -1;
    }
  }
  return 0;
}

int32_t file_operations::write_file(std::string& file_output, uint8_t* datas, int64_t datas_size, int32_t mode)
{
  //obj to save new file
  std::ofstream write_stream;

  //select the flags to use, it's the way to save the file
  decltype(std::ofstream::out) mode_wanted = static_cast<std::_Ios_Openmode>(mode);
  write_stream = std::ofstream(file_output,
    (std::ofstream::out |
      std::ofstream::binary |
      mode_wanted));

  //i have to open the file
  if (write_stream.is_open() == false)
  {
    std::cout << "wrong something with write_file: std::ofstream\n";
    return -1;
  }

  //if data is nullptr I save the datas that already have
  if (datas_size < 0)
  {
    std::cout << "wrong something with write_file: file's size is negative\n";
    return -1;
  }
  if (datas == nullptr)
  {
    std::cout << "wrong something with write_file: data's pointer is nullptr\n";
    return -1;
  }

  //then start to write the file
  for (int64_t i = 0; i < datas_size; ++i)
  {
    write_stream << datas[i];
  }

  //then close the stream
  write_stream.close();
  return 0;
}

int32_t file_operations::save_third_file(std::string& output_file_name, std::list<file_pointer>& files, int32_t mode)
{
  if(files.empty() == true)
  {
    return 0;
  }

  bool is_first_trunc_mode = ((mode | std::ofstream::trunc) == mode); //there is trunc mode?

  uint8_t* pointer_d = files.front()._file_data;
  int64_t pointer_l = files.front()._size_file;

  //if true, then the first pointer will be trunc the file, else will be append
  if (file_operations::write_file(output_file_name, pointer_d, pointer_l, ((is_first_trunc_mode == true) ? (std::ofstream::trunc) : (std::ofstream::app))) == -1)
  {
    std::cout << "wrong something with save_third_files\n";
    return -1;
  }
  files.pop_front(); //clear the first pointer

  if(files.empty() == true)
  {
    return 0;
  }

  //others can only use append mode
  for (file_pointer& file : files)
  {
    if (file_operations::write_file(output_file_name, file._file_data, file._size_file, std::ofstream::app) == -1)
    {
      std::cout << "wrong something with save_third_files\n";
      return -1;
    }
  }

  files.clear(); //clear others

  //I have create a new file with new name witout problems
  //I have used 1 or more pointer
  //I can use another "this" from another object
  return 0;
}

int32_t file_operations::save_this_file(std::list<file_pointer>& files, bool i_want_reload_file, int32_t mode, bool _is_read_only)
{
  if ((mode | std::ofstream::in | std::ofstream::app) == mode) //c++98 std::ofstream fails with this
  {
    std::cout << "you can't read and append the file toghether\n";
    return -1;
  }

  bool _this_already_found = false;
  uint8_t* this_data_file = nullptr;

  for (file_pointer& file : files)
  {
    if(file._file_data == this->get_data_file())
    {
      //it's a copy of actual get file
      if (file._size_file > this->get_size_file())
      {
        std::cout << "you can't read more bytes than " << this->get_size_file() << " for this->get_data_file()\n";
        this->clear_this(_this_already_found, this_data_file);
        return -1;
      }
      if (_this_already_found == false)
      {
        _this_already_found = true;
        this_data_file = new uint8_t[file._size_file];
        //files.front()._size_file it's always = min(files.front()._size_file, this->get_size_file())
        uint8_t* t_f = this->get_data_file();
        for (int64_t i = 0; i < file._size_file; this_data_file[i] = t_f[i], ++i);
      }
      file._file_data = this_data_file;
    }
  }

  if (this->clear() == -1) //delete the actual mmap if setted
  {
    this->clear_this(_this_already_found, this_data_file);
    return -1;
  }

  if(files.empty() == true)
  {
    return 0;
  }

  bool is_first_trunc_mode = ((mode | std::ofstream::trunc) == mode);

  //if clear runs properly I should can overwrite the file this->_file_name
  //first write can be std::ofstream::trunc OR std::ofstream::app
  if (file_operations::write_file(this->_file_name, files.front()._file_data, files.front()._size_file, ((is_first_trunc_mode == true) ? (std::ofstream::trunc) : (std::ofstream::app))) == -1)
  {
    std::cout << "wrong something with save_this_files\n";
    this->clear_this(_this_already_found, this_data_file);
    return -1;
  }
  files.pop_front();

  if(files.empty() == true)
  {
    return 0;
  }

  //others can only use append mode
  for (file_pointer& file : files)
  {
    if (file_operations::write_file(this->_file_name, file._file_data, file._size_file, std::ofstream::app) == -1)
    {
      std::cout << "wrong something with save_this_files\n";
      this->clear_this(_this_already_found, this_data_file);
      return -1;
    }
  }

  files.clear();

  //set private values then read file
  if (i_want_reload_file == true)
  {
    if (this->set_constructor_vars(_is_read_only) == -1)
    {
      this->clear_this(_this_already_found, this_data_file);
      return -1;
    }
  }

  this->clear_this(_this_already_found, this_data_file);
  return 0;
}

template <typename T>
void file_operations::clear_this(bool pointer_in_use, T*& pointer)
{
  if (pointer_in_use == true)
  {
    delete[] pointer;
    pointer = nullptr;
  }
}

#if _WIN32 || _WIN64
//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
std::string file_operations::get_last_error_as_string()
{
  //Get the error message ID, if any.
  DWORD error_message_id = ::GetLastError();
  if (error_message_id == 0) {
    return std::string("no error found"); //No error message has been recorded
  }

  LPSTR message_buffer = nullptr;

  //Ask Win32 to give us the string version of that message ID.
  //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
  size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
    NULL, error_message_id, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&message_buffer, 0, NULL);

  //Copy the error message into a std::string.
  std::string message(message_buffer, size);

  //Free the Win32's string's buffer.
  LocalFree(message_buffer);

  std::string res("error #" + std::to_string(static_cast<uint64_t>(error_message_id)));
  res.append(": ");
  res += message;

  return res;
}
#elif __linux__
std::string file_operations::get_last_error_as_string(int64_t err_value)
{
  int64_t error_message_id = err_value;
  //Get the error message ID, if any
  std::string res("error #" + std::to_string(static_cast<uint64_t>(error_message_id)));
  res.append(": ");
  res += strerror(error_message_id);

  return res;
}
#endif
