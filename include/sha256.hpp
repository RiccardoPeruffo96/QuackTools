/**
 * @author Peruffo Riccardo, RP96, riccardoperuffo96@gmail.com, github.com/RiccardoPeruffo96
 * @date 2022
 * @file sha256.hpp
 * @brief It allows to calculate sha256 standard function, how declared at https://csrc.nist.gov/publications/detail/fips/180/4/final <br>
 * #####################################################################
 * <strong>sample to use this interface:</strong><br>
 * int64_t input_len = ...;<br>
 * char* input_data = ...;<br>
 * auto crypto_hash = sha256(input_data, input_len);<br>
 * crypto_hash.compute_hash(); //this method will consume<br>
 * std::cout << crypto_hash.get_SHA256_string() << "\n";<br>
 * @version 0.0.1 alpha [exam]
 */

#ifndef SHA256_RP96_HPP_
#define SHA256_RP96_HPP_

#include <array> //std::array
#include <iostream> //std::hex
#include <list> //std::list
#include <mutex> //std::mutex
#include <string> //std::string
#include <sstream> //std::stringstream
#include <thread> //std::thread
#include <vector> //std::vector

/**
 * @brief This namespace only contains a modular interface
 * who define:
 * <ul>
 * <li>a separate class;</li>
 * <li>properly create data structs: in this case a struct;</li>
 * <li>templates;</li>
 * </ul>
 * that together allow to work with sha256 protocol.
 */
namespace _sha256_rp96_
{
  const int32_t _SIZE_CONSTANTS_TABLE = 64; ///< array length used by constant table
  const int32_t _SIZE_WORKING_ARRAYS = 64; ///< array length used to store input 128 bits and preprocessing values, for 512 total bits
  const int32_t _SIZE_HASH_TABLE = 8; ///< integer (32 bits) used to store the hash value
  const int32_t _BITS_HASH = 256; ///< total bits about sha256
  const int32_t _SIZE_WORKSPACE_FILLED_WITH_MESSAGE = 16; ///< integers (32 bits) used by input, only 16 out of 64
  const uint64_t SIZE_1GB_MESSAGE_LIST = ((1024*1024*1024) / 288 * 512); ///< limitator of RAM usage, need to be not equals to 0
  //legth message to have 1GB, 288 == sizeof(struct)

  /**
   * @struct message
   * it's a wrap who rapresent a single SHA256 block
   * we know the input it's always padded, so (lenght data + padding) % 512 bits (or 16 bytes) == 0
   * we receive a char*, so the firsts 16 bytes are stored with a pointer
   * the seconds 48 bytes are stored with an uint32s' array
   */
  struct message final
  {
  public:

    bool _has_00_15_read; ///< flag if the pointer is ready
    bool _has_00_15_to_heap; ///< flag if the pointer is deleatable with dealloc, so the pointer contains padding
    uint8_t* _message_00_15; ///< pointer to data
    std::array <uint32_t, _SIZE_WORKING_ARRAYS - _SIZE_WORKSPACE_FILLED_WITH_MESSAGE> _message_16_63; ///< preprocessing data
    std::mutex _flag_mtx; ///< avoid multiple access to _has_00_15_read
    //protecs _has_00_15_read

    /**
     * @brief set private flags to false and other vars
     */
    message() noexcept(true)
    {
      this->_has_00_15_read = { false };
      this->_has_00_15_to_heap = { false };
      this->_message_16_63 = { 0 };
      this->_message_00_15 = nullptr;
    }

    /**
     * @brief copy constructor
     * @param msg object message& to copy
     */
    message(const message& msg) noexcept(true):
      _has_00_15_read{ msg._has_00_15_read },
      _has_00_15_to_heap{ msg._has_00_15_to_heap },
      _message_00_15{ msg._message_00_15 }
    {
      for(uint64_t i = 0; i < _SIZE_WORKSPACE_FILLED_WITH_MESSAGE; ++i)
      {
        _message_00_15[i] = msg._message_00_15[i];
      }
      this->_message_16_63 = msg._message_16_63;
      //std::copy_n(this->_message_16_63, _SIZE_WORKING_ARRAYS - _SIZE_WORKSPACE_FILLED_WITH_MESSAGE, msg._message_16_63);
    }

    /**
     * @brief copy assignment operator
     * @param msg object message& to copy
     */
    message& operator=(const message& msg) noexcept(true)
    {
      this->_has_00_15_read = msg._has_00_15_read;
      this->_has_00_15_to_heap = msg._has_00_15_to_heap;
      for(uint64_t i = 0; i < _SIZE_WORKSPACE_FILLED_WITH_MESSAGE; ++i)
      {
        _message_00_15[i] = msg._message_00_15[i];
      }
      this->_message_16_63 = msg._message_16_63;
      //std::copy_n(this->_message_16_63, _SIZE_WORKING_ARRAYS - _SIZE_WORKSPACE_FILLED_WITH_MESSAGE, msg._message_16_63);
      return *this;
    }

    /**
     * @brief move constructor
     * @param msg object message& to copy
     */
    message(message&& msg) noexcept(true):
      _has_00_15_read{ msg._has_00_15_read },
      _has_00_15_to_heap{ msg._has_00_15_to_heap },
      _message_00_15{ msg._message_00_15 }
    {
      this->_message_16_63.swap(msg._message_16_63);
    }

    /**
     * @brief move assignment operator
     * @param msg object message& to swap
     */
    message& operator=(message&& msg) noexcept(true)
    {
      this->_has_00_15_read = msg._has_00_15_read;
      this->_has_00_15_to_heap = msg._has_00_15_to_heap;
      this->_message_00_15 = msg._message_00_15;
      this->_message_16_63.swap(msg._message_16_63);
      return *this;
    }

    /**
     * @brief return this message
     * @return this
     */
    message* get_this()
    {
      return this;
    }

    /**
     * @brief destructor: clean memory if I can
     */
    ~message() noexcept(true)
    {
      if (this->_has_00_15_to_heap == false)
        return;

      delete[] this->_message_00_15;
      this->_message_00_15 = nullptr;
    }
  };

  /**
   * @class sha256
   *
   * @brief This class is used to calculate and store sha256 with
   * a simple interface.
   * This class has the target to work with a little function's subset,
   * doesn't make any useless copy but will can use a lot of RAM because
   * SHA256 preprocessing use 512 bits EVERY 128 input's bit!
   *
   * the namespace const SIZE_1GB_MESSAGE_LIST contains a superior limit
   * to don't use a large amount of RAM and it can be easly modified.
   * SIZE_1GB_MESSAGE_LIST can't be less than 1 (infinite loop).
   *
   * Also it use only std functions to enlarge the compatibily with
   * others systems, but in a future should'll be implement
   * an specific optimization for differents processors like
   * INTEL and AMD that support specific asm commands to calculate
   * sha256.
   */
  class sha256 final //sizeof(sha256) == 1160 -> 1149 Bytes + 11(padding) 'cause 40-Bytes alignment
  {
  public:

 /**
   * @brief constructor: set private vars from char* input and lenght input
   * @param file_data pointer to data
   * @param size_data lenght data
   */
    sha256(char* file_data, int64_t size_data) noexcept(true);

  /**
   * @brief copy constructor
   * @param s256 object sha256& to copy
   */
    sha256(const sha256& s256) noexcept(true);

    /**
     * @brief copy assignment operator
     * @param s256 object sha256& to copy
     * @return a copy of input
     */
    sha256& operator=(const sha256& s256) noexcept(true);

    /**
     * @brief move constructor
     * @param s256 object sha256& to swap
     */
    sha256(sha256&& s256) noexcept(true);

    /**
     * @brief move assignment operator
     * @param s256 object sha256& to swap
     * @return a copy of input
     */
    sha256& operator=(sha256&& s256) noexcept(true);

 /**
   * @brief default destructor
   */
    ~sha256() noexcept(true) = default;

   /**
     * @brief start the threads to calculate the hash
     */
    void compute_hash();

   /**
     * @brief convert digest in string from x8 uint32
     * @return digest in string format
     */
    std::string get_SHA256_string();

   /**
     * @brief obtain the uint32 that rapresent the digest
     * @return this->_hash_result
     */
    std::array <uint32_t, _SIZE_HASH_TABLE> get_SHA256_uint32_array();

   /**
     * @brief get _BITS_HASH const value
     * @return 256
     */
    int32_t hash_length_bits() { return _BITS_HASH; }

   /**
     * @brief get _SIZE_HASH_TABLE const value
     * @return 8
     */
    int32_t hash_length_bytes() { return _SIZE_HASH_TABLE; }

  private:

    ///flag if the file is readable
    bool _can_we_read_the_file;
    ///flag if hash is already calculate
    bool _hash_is_already_calc;
    //sizeof = 4
    ///number of zeros for padding
    uint32_t _number_of_zeros_for_padding; //sizeof = 4
    //sizeof = 8
    ///cycles for calc hash
    uint64_t _number_of_cycles_for_compute_sha256; //sizeof = 8
    ///size's message in bits
    uint64_t _sizes_bits_input; //sizeof = 8
    ///size's message in bytes
    uint64_t _sizes_bytes_input; //sizeof = 8
    ///bytes already read
    uint64_t _bytes_already_reads; //sizeof = 8
    ///pointer to message
    char* _file_data;

    ///struct with message and padding and preprocessing
    std::list<message> _workspace;
    //sizeof = 32
    ///where I store the digest
    std::array <uint32_t, _SIZE_HASH_TABLE> _hash_result; //sizeof = 32 //These words were obtained by taking the first thirty - two bits of the fractional parts of the square roots of the first eight prime numbers.

    //sizeof = 256
    ///const value to calculate digest
    const std::array <uint32_t, _SIZE_CONSTANTS_TABLE> _constants_table; //sizeof = 256 //These words represent the first thirty-two bits of the fractional parts of the cube roots of the first sixty - four prime numbers.In hex, these constant words are(from left to right)

    ///protect message from multiple threads access
    //std::mutex _flag_mtx_list; //protecs _list_in_use
    //std::mutex _flag_mtx_list; //protecs _list_in_use

    ///mutex to protec _list_in_use
    //std::mutex _flag_mtx_list_p; //protecs _list_in_use
    std::mutex _flag_mtx_list_p; //protecs _list_in_use

    ///struct
    //std::list<message> _workspace_p;

   /**
     * @brief start the thread to prepare struct message
     */
    void prepare_message();

   /**
     * @brief create the padding for last, or two lasts, struct message
     */
    void prepare_padding();

   /**
     * @brief start the thread to read struct message
     */
    void start_read();

   /**
     * @brief read and clean each struct message
     */
    void read_data();

    constexpr uint32_t uint32_value_from_char_array(const uint8_t* array_data, const uint32_t& index);

   /**
     * @brief check if _workspace_p.front() it's readable, with safe-thread mode
     * @return _workspace_p.front()->_has_00_15_read
     */
    bool ctrl_front();

   /**
     * @brief gives _workspace_p.back()->_has_00_15_read = true, with safe-thread mode
     */
    void unlock_back();

   /**
     * @brief call _workspace_p.push_back() with safe-thread mode
     */
    void safe_t_pushback();

   /**
     * @brief call _workspace_p.pop_front() with safe-thread mode
     */
    void safe_t_popfront();

   /**
     * @brief return _workspace_p.size(), with safe-thread mode
     * @return _workspace_p.size()
     */
    uint32_t safe_t_get_size();
    //sizeof = 272

   /**
     * @brief print the hex number always with 8 chars, it's like the inverse of atoi()
     * @param _input_value the integer to transform in char string
     * @return string with size == 8
     */
    std::string uint32_to_hex_string_with_8char(uint32_t _input_value);

   /**
     * @brief standard operation defined in SHA256 that modify the input
     * @param x input param
     * @return ROTR(x, 7) ^ ROTR(x, 18) ^ SHR(x, 3)
     */
    constexpr uint32_t sigma0(uint32_t x);

  /**
     * @brief standard operation defined in SHA256 that modify the input
     * @param x input param
    * @return ROTR(x, 17) ^ ROTR(x, 19) ^ SHR(x, 10)
    */
    constexpr uint32_t sigma1(uint32_t x);

   /**
     * @brief rotate right the input param times, the less significative bits become more significative bits
     * @param x the integer to modify
     * @param param times to turn the x
     * @return (x >> param) | (x << ((sizeof(uint32_t) * 8) - param))
     */
    constexpr uint32_t Ch(uint32_t x, uint32_t y, uint32_t z);

   /***
     * @brief rotate right the input param times, the less significative bits become more significative bits
     * @param x the integer to modify
     * @param param times to turn the x
     * @return (x >> param) | (x << ((sizeof(uint32_t) * 8) - param))
     */
    constexpr uint32_t Maj(uint32_t x, uint32_t y, uint32_t z);

   /**
     * @brief rotate right the input param times, the less significative bits become more significative bits
     * @param x the integer to modify
     * @param param times to turn the x
     * @return (x >> param) | (x << ((sizeof(uint32_t) * 8) - param))
     */
    constexpr uint32_t ROTR(uint32_t x, uint32_t param);

   /**
     * @brief rotate left the input param times, the less significative bits become more significative bits
     * @param x the integer to modify
     * @param param times to turn the x
     * @return (x << param) | (x >> ((sizeof(uint32_t) * 8) - param))
     */
    constexpr uint32_t ROTL(uint32_t x, uint32_t param);

   /**
     * @brief shift right input param times
     * @param x the integer to modify
     * @param param times to turn the x
     * @return x >> param
     */
    constexpr uint32_t SHR(uint32_t x, uint32_t param);

  /**
   * @brief standard operation defined in SHA256 that modify the input
   * @param x input param
   * @return ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22)
   */
    constexpr uint32_t SIGMA0(uint32_t x);

  /**
   * @brief standard operation defined in SHA256 that modify the input
   * @param x input param
   * @return ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25)
   */
    constexpr uint32_t SIGMA1(uint32_t x);
  };

} //namespace _sha256_rp96_

#endif //SHA256_RP96_HPP_
