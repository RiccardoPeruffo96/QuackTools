/**
 * @author Peruffo Riccardo, RP96, riccardoperuffo96@gmail.com, github.com/RiccardoPeruffo96
 * @date 2022
 * @file aes256.hpp
 * @brief It allows to calculate aes256 ctr standard function, how declared at https://csrc.nist.gov/publications/detail/fips/197/final<br>
 * #####################################################################
 * <p>remember: to create a faster algorithm it modify the input and don't use new command, but only if input.lenght % 128 bits (16 chars) == 0<br>
 * in other cases we need to add padding to last block and to minimize the consume of memory and do useless operations,<br>
 * the function alloc and copy only last bytes,<br>
 * i report an example:</p>
 * <ul>
 * <li>you work with a file with size 1.234.567.890 bytes (~1.2GB); <em>//get_final_block</em>
 * </li><li>we easily calculate 1.234.567.890 % 16 = 2
 * <em>('%' means "modulo operation" for who don't know c-like language)</em>;
 * </li><li>aes256 only work with 128bits' blocks, so when you call "encode()",
 * the new size will forced to be (1.234.567.890 + 16 - (1.234.567.890 % 16)),
 * because it's first number greater then 1.234.567.890 and divisible by 16;
 * </li><li>the first (1.234.567.890 - (1.234.567.890 % 16)) = 1.234.567.888
 * bytes will overwrite the original input data;
 * </li><li>the for other 2 bytes: let's assume that the first byte is AB and
 * the second CD, we will have such a populated array (using PKCS#7 padding)<br>
 *    AB CD 0E 0E 0E 0E 0E 0E 0E 0E 0E 0E 0E 0E 0E 0E<br>
 *    that will be encrypted
 * </li></ul>
 * <strong>THIS MEANS IN YOUR CODE YOU CAN DO THIS:</strong><br>
 * uint8_t* input_file_data = my_personal_function_to_read_file("my_secret.zip");<br>
 * int64_t input_file_len = my_personal_function_to_get_size("my_secret.zip");<br>
 * std::string key = sha256.get_string("duffy duck"); <em>//"duffy duck" it's not a good password but a good example</em><br>
 * <em>//key == fca2b42874aa397d1ef6fa65a4a525592210ffabcfe69a18ba1f4299b23d69ae</em><br>
 * auto my_aes256 = aes256(input_file_data, input_file_len, key);<br>
 * my_aes256.encode();<br>
 * for(auto i = 0; i < my_aes256.get_length_payload(); ++i)<br>
 * {<br>
 * &nbsp;&nbsp;std::cout << my_aes256.get_payload()[i]; <em>//std::cout << input_file_data[i]; works well</em><br>
 * }<br>
 * if(my_aes256.get_final_block != nullptr)<br>
 * {<br>
 * &nbsp;&nbsp;for(auto i = 0; i < my_aes256.get_final_block_size(); ++i)<br>
 * &nbsp;&nbsp;{<br>
 * &nbsp;&nbsp;&nbsp;&nbsp;std::cout << my_aes256.get_final_block[i];<br>
 * &nbsp;&nbsp;}<br>
 * }<br>
 * std::cout << "\n";<br>
 * delete[] input_file_data;<br>
 * input_file_data = nullptr;<br>
 * <em>//last 16 bytes are saved on stack ('cause are very small amount of data) so you don't need to use delete[]<br>
 * //"delete[] input_file_data;" it's enough to avoid memory leak</em><br>
 * @version 0.0.1 alpha [exam]
 */

#ifndef AES256_RP96_HPP_
#define AES256_RP96_HPP_

#include <array> //std::array
#include <functional> //std::copy_n
#include <list> //std::list
#include <thread> //std::thread

#include "sha256.hpp" //_sha256_rp96_::sha256(), _sha256_rp96_::compute_hash()

/**
 * @brief This namespace only contains a modular interface
 * who define:
 * <ul>
 * <li>a separate class;</li>
 * <li>properly create data structs: in this case union;</li>
 * <li>templates;</li>
 * </ul>
 * that together allow to work with aes256 protocol.
 */
namespace _aes256_rp96_
{
  //auxiliar const vars
  const int32_t BITS_FOR_BYTE = 8; ///< bits in a byte
  const int32_t BYTES_FOR_128BITS = 16; ///< bytes in a 128 bits
  const int32_t DIM_ARRAY = 16; ///< array dimension that contains 128 bits sizeof(char)'s elements
  const int32_t DIM_MATRIX = 4; ///< matrix square dimension that contains 128 bits sizeof(char)'s elements
  const int32_t NUMBER_SUBKEYS = 2; ///< logic variable using to follow AES standard with key expansion
  const int32_t TURN_KEYS = 14; ///< logic variable using to follow AES standard with key expansion
  const int32_t SUBKEY_N0 = 0; ///< logic variable using to follow AES standard with key expansion
  const int32_t SUBKEY_N1 = 1; ///< logic variable using to follow AES standard with key expansion

  /**
   * @brief template to calculate gcd about a number
   * @param a 1st number
   * @param b 2nd number
   */
  template<typename T>
  T gcd(T a, T b);

  /**
   * @brief template to make matrix-operations in an array, in this case rotate left a single matrix row
   * @param row what row i want to rotate left (0..num_rows)
   * @param round how many positions i want to rotate, it allows to use negative number
   * @param pointer_arr array to "convert" in num_rows*num_cols matrix
   * @param num_rows number of rows
   * @param num_cols number of cols
   */
  template<int32_t row, int32_t round, typename T>
  void rotate_left(T* pointer_arr, uint32_t num_rows, uint32_t num_cols);

  /**
   * @brief template to make matrix-operations in an array, in this case rotate right a single matrix row
   * @param row what row i want to rotate right (0..num_rows)
   * @param round how many positions i want to rotate, it allows to use negative number
   * @param pointer_arr array to "convert" in num_rows*num_cols matrix
   * @param num_rows number of rows
   * @param num_cols number of cols
   */
  template<int32_t row, int32_t round, typename T>
  void rotate_right(T* pointer_arr, uint32_t num_rows, uint32_t num_cols);

  /**
   * @brief template to make matrix-operations in an array, in this case rotate up a single matrix column
   * @param col what column i want to rotate up (0..num_cols)
   * @param round how many positions i want to rotate, it allows to use negative number
   * @param pointer_arr array to "convert" in num_rows*num_cols matrix
   * @param num_rows number of rows
   * @param num_cols number of cols
   */
  template<int32_t col, int32_t round, typename T>
  void rotate_up(T* pointer_arr, uint32_t num_rows, uint32_t num_cols);

  /**
   * @brief template to make matrix-operations in an array, in this case rotate down a single matrix column
   * @param col what column i want to rotate down (0..num_cols)
   * @param round how many positions i want to rotate, it allows to use negative number
   * @param pointer_arr array to "convert" in num_rows*num_cols matrix
   * @param num_rows number of rows
   * @param num_cols number of cols
   */
  template<int32_t col, int32_t round, typename T>
  void rotate_down(T* pointer_arr, uint32_t num_rows, uint32_t num_cols);

  /**
   * @brief template to calculate most significant bit
   * @param value input number of any type
   */
  template<typename T>
  uint64_t msb(T value);

  /**
   * @brief auxiliary recursive function used by msb
   * @param value
   * @param size_lim_max
   * @param size_lim_min
   */
  uint64_t msb_recursion(uint64_t value, uint64_t size_lim_max, uint64_t size_lim_min);

  /**
   * @union wrapper_128bits
   * union that contains 128 bits and allow to work with like an uint8_t array
   * (dimension = DIM_ARRAY) or matrix (dimension = DIM_MATRIX*DIM_MATRIX)
   */
  union wrapper_128bits
  {
    std::array<uint8_t, DIM_ARRAY> array_form;
    std::array<std::array<uint8_t, DIM_MATRIX>, DIM_MATRIX> matrix_form;
  };

  /**
   * @union wrapper_2048bits
   * union that contains 2048 bits and allow to work with like an uint8_t array
   * (dimension = DIM_ARRAY^2) or matrix (dimension = DIM_ARRAY*DIM_ARRAY)
   */
  union wrapper_2048bits
  {
    std::array<uint8_t, DIM_ARRAY * DIM_ARRAY> array_form;
    std::array<std::array<uint8_t, DIM_ARRAY>, DIM_ARRAY> matrix_form;
  };

  /**
   * @class aes256
   *
   * @brief This class is used to manage encrypt and decrypt with
   * a simple interface.
   * This class has the target to work with a little function's subset,
   * it works directly with pointer and don't make any useless copies,
   * except to calculate final padding.
   *
   * Also it use only std functions to enlarge the compatibily with
   * others systems, but in a future should'll be implement
   * an specific optimization for differents processors like
   * INTEL and AMD that support specific asm commands to calculate
   * aes256.
   */
  class aes256
  {
  public:

    // Each 256bit key is split in two 128 bits sub-keys
    /**
     * @brief prepare input byte's sequence, if the input % 128 bits != 0 then padding is added using PKCS#7 method
     * @param data_file pointer at the file or byte's sequence
     * @param length_byte_file length's data_file
     * @param key a string that contains the key
     */
    aes256(uint8_t* data_file, int64_t length_byte_file, std::string key);

    //rule of five
    /**
     * @brief copy constructors
     * @param a256 input to copy
     */
    aes256(const aes256& a256);

    /**
     * @brief copy assignment operator
     * @param a256 input to copy
     * @return a input's copy
     */
    aes256& operator=(const aes256& a256);

    /**
     * @brief move constructor
     * @param a256 input to swap
     */
    aes256(aes256&& a256);

    /**
     *
     * @brief move assignment operator
     * @param a256 input to swap
     * @return a input's copy
     */
    aes256& operator=(aes256&& a256);

    /**
     * @brief destructor
     */
    ~aes256();

    /**
     * @brief start to encrypt the stored input
     */
    void encrypt();

    /**
     * @brief start to decrypt the stored input
     */
    void decrypt();

    /**
     * @brief to obtain the uint8_t* stored and equals to initial input if you not call encrypt or decrypt
     * @return pointer to stored input
     */
    uint8_t* get_payload();

    /**
     * @brief the input size that don't need padding, so if (sizeof(input) % 16 == 0) then {return sizeof(input)} else {return sizeof(input) - (return sizeof(input) % 16)}
     * @return get_payload()'s length'
     */
    int64_t get_length_payload();

    /**
     * @brief to obtain the uint8_t* about last 16 bytes of input data + padding, if sizeof(input) % 16 == 0 this pointer it's useless bacause input don't need padding, so it return nullptr
     * @return pointer to static array (not new command used) OR nullptr
     */
    uint8_t* get_final_block();

    /**
     * @brief return the static size of final block, you can obtain it with get_final_block, if get_final_block return nullptr then get_final_block_size it's useless
     * @return DIM_ARRAY = 16
     */
     int64_t get_final_block_size();

    /**
     * @brief check what is the last called method between encrypt and decrypt
     * @return true if it is encrypt
     */
    bool have_i_encrypt();

    /**
     * @brief check what is the last called method between encrypt and decrypt
     * @return true if it is decrypt
     */
    bool have_i_decrypt();

    /**
     * @brief CMS (Cryptographic Message Syntax) using by PKCS#7, I return the value equals to: (16 - (sizeof(input_data) % 16))
     * @return the value equals to: (16 - (length_byte_file % 16))
     *
     * @see aes256
     */
    int32_t get_value_CMS();

  private:
    /*
     * first is input key
     * _subkeys[idx_key = 0..15]
     * _subkeys[0] = first half key
     * _subkeys[1] = second half key
     */
    ///this contains the key and every expansion's step
    std::array<wrapper_128bits, TURN_KEYS+1> _subkeys;

    ///pointer to original data
    uint8_t* payload; //received

    ///payload's length
    int64_t length_payload; //received

    ///last 16 chars where use padding
    wrapper_128bits final_pad;

    ///16 chars store the start value for counter block mode
    wrapper_128bits start_counter;

    ///using for padding: allows to understand how many byte missing (example if value_CMS == 0xA
    ///then final_pad it will ends with 0xA times the byte 0A: .. 0A 0A 0A 0A 0A 0A 0A 0A 0A 0A)
    int64_t value_CMS;

    ///start_final_block
    int64_t start_final_block;

    ///if true i have call decrypt() how last method betweeb encrypt() and decrypt()
    bool i_have_decrypt;

    ///if true i have call encrypt() how last method betweeb encrypt() and decrypt()
    bool i_have_encrypt;

    /**
     * @brief encrpyt using aes256 a single 128bits block
     */
    void aes256_algorithm(uint8_t* block, const wrapper_2048bits& s_box);

    /**
     * @brief decrpyt using aes256 a single 128bits block
     */
    void inv_aes256_algorithm(uint8_t* block, const wrapper_2048bits& s_box);

    /**
     * @brief makes xor ops between each element from block and key in a simple for loop, example:
     * for(auto i = 0; i < get_final_block_size(); block[i] ^= key[i], ++i);
     */
    void xor_op(uint8_t* block, wrapper_128bits& key);
    void shift_rows(uint8_t* block);
    void sub_bytes(uint8_t* block, const wrapper_2048bits& s_box);
    void mix_columns(uint8_t* block);
    void expand_key(const wrapper_2048bits& s_box);
    void inv_shift_rows(uint8_t* block);
    void inv_mix_columns(uint8_t* block);

    uint8_t char_to_hex(char tmp);

    void thread_aes_create(uint8_t* block, const wrapper_2048bits& s_box, uint64_t single_block);

    void sum_w128(const wrapper_128bits& input_w, uint64_t input_n, wrapper_128bits& output);
  };

} //namespace _aes256_rp96_

#endif //AES256_RP96_HPP_
