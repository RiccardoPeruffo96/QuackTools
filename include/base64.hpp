/**
 * @author Peruffo Riccardo, RP96, riccardoperuffo96@gmail.com, github.com/RiccardoPeruffo96
 * @date 2022
 * @file base64.hpp
 * @brief This file allows to encoding and decoding *chars
 * it allow to use 3 types of base64 standard: RFC4648 standard,
 * RFC4648 url and RFC3501 standard
 * @version 0.0.1 alpha [exam]
 */

#ifndef BASE64_RP96_HPP_
#define BASE64_RP96_HPP_

#include <cassert> //assert
#include <cstdint> //uint8_t, uint32_t, ecc...

/**
 * @brief a simple namespace who contains
 * interfaces to use base64 encoding
 */
namespace _base64_rp96_
{
  /**
   * @brief this enumeration rapresent a different base64 standard
   */
  enum base64_standards
  {
    RFC4648_STD = 0, ///< most used standard RFC4648
    RFC4648_URL, ///< used by url RFC4648
    RFC3501_STD ///< standard RFC3501
  };

  const char* const TRANSLATER_RFC4648_STD{"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"}; ///< RFC4648 STD convert array
  const char* const TRANSLATER_RFC4648_URL{"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_"}; ///< RFC4648 URL convert array
  const char* const TRANSLATER_RFC3501_STD{"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+,"}; ///< RFC3501 STD convert array

  /**
   * @brief convert a char into his index inside the base64 convert array:
   * conversion__('B') return 1 because TRANSLATER_RFC4648_STD[1] = 'B';
   * conversion__('+') return 62 because TRANSLATER_RFC4648_STD[62] = '+';
   * ecc...
   * @param input char to convert
   * @return input index inside the convert array
   */
  constexpr char conversion__(char input);

  /**
   * @brief take less significant x6 bits about first value and concat with
   * most significant x2 bits about second value then return the byte
   * @param v01 first value about less significant x6 bits
   * @param v02 second value about most significant x2 bits
   * @return concat about x6bits||x2bits
   */
  constexpr uint8_t concat01(int8_t v01, int8_t v02);

  /**
   * @brief take less significant x4 bits about first value and concat with
   * most significant x4 bits about second value then return the byte
   * @param v01 first value about less significant x4 bits
   * @param v02 second value about most significant x4 bits
   * @return concat about x4bits||x4bits
   */
  constexpr uint8_t concat02(int8_t v01, int8_t v02);

  /**
   * @brief take less significant x2 bits about first value and concat with
   * most significant x6 bits about second value then return the byte
   * @param v01 first value about less significant x2 bits
   * @param v02 second value about most significant x6 bits
   * @return concat about x2bits||x6bits
   */
  constexpr uint8_t concat03(int8_t v01, int8_t v02);

  /**
   * @brief this class contains the minimum number of methods
   * to use the base64 standard and integrate with custom code
   */
  class base64
  {
  public:

    /**
     * @brief encode with base64 protocol an input
     * @param input_string input to convert
     * @param length_string size(input_string)
     * @param output_lenght length about new pointer
     * @param mode = 0b1 define the convert array used followed base64_standards enum
     * @return pointer to new text encoded
     */
    static uint8_t* base64_encode(uint8_t* input_string, int64_t length_string, int64_t& output_lenght, int64_t mode = 0);

    /**
     * @brief decode with base64 protocol an input
     * @param input_string input to convert
     * @param length_string size(input_string)
     * @param output_lenght length about new pointer
     * @return pointer to new text decoded
     */
    static uint8_t* base64_decode(uint8_t* input_string, int64_t length_string, int64_t& output_lenght);

    /**
     * @brief from an hypothetical input length, return the exact size needed to stored the same input but after the encode
     * @param length_string length to convert
     * @return size needed to stored the same encoded input
     */
    static int64_t base64_enc_size(int64_t length_string)
    {
      if (length_string == 0)
      {
        return 0;
      }
      return (((length_string + 2) / 3) * 4);
    }

    /**
     * @brief from an hypothetical input length, return the exact size needed to stored the same input but after the decode
     * @param input_string the string to convert
     * @param length_string length to convert
     * @return size needed to stored the same decoded input
     */
    static int64_t base64_dec_size(uint8_t* input_string, int64_t length_string);

  private:

    //rule of five but only for delete
    ///default constructors
    base64() = delete;
    ///copy constructors
    base64(const base64& b64) = delete;
    ///copy assignment operator
    base64& operator=(const base64& b64) = delete;
    ///move constructor
    base64(const base64&& b64) = delete;
    ///move assignment operator
    base64& operator=(const base64&& b64) = delete;
    ///destructor
    ~base64() = default;
  };
}

#endif //BASE64_RP96_HPP_
