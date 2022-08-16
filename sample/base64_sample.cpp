/**
 * @author Peruffo Riccardo, RP96, riccardoperuffo96@gmail.com, github.com/RiccardoPeruffo96
 * @date 2022
 * @file base64_sample.cpp
 */

#include <iostream>

#include "base64.hpp"

int main()
{
  std::string input = "text to convert with base64";

  //convert string into uint8_t*
  int64_t input_length = input.length();
  uint8_t* input_data = new uint8_t[input_length];
  for(int64_t i = 0; i < input_length; ++i)
  {
    input_data[i] = static_cast<uint8_t>(input.at(i));
  }

  //now we have our input so finally we can use base64_encode and base64_decode
  int64_t encoded_size = 0;
  /*----------------------------------------------
   * @brief encode with base64 protocol an input
   * @param input_string input to convert
   * @param length_string size(input_string)
   * @param output_lenght length about new pointer
   * @param mode = 0b1 define the convert array used followed base64_standards enum
   * @return pointer to new text encoded
   * static uint8_t* base64_encode(uint8_t* input_string, int64_t length_string, int64_t& output_lenght, int64_t mode = 0);
   *----------------------------------------------*/
  uint8_t* encoded_data = _base64_rp96_::base64::base64_encode(input_data, input_length, encoded_size);

  //now decode
  int64_t decoded_size = 0;
  /*----------------------------------------------
   * @brief decode with base64 protocol an input
   * @param input_string input to convert
   * @param length_string size(input_string)
   * @param output_lenght length about new pointer
   * @return pointer to new text decoded
   * static uint8_t* base64_decode(uint8_t* input_string, int64_t length_string, int64_t& output_lenght);
   *----------------------------------------------*/
  uint8_t* decoded_data = _base64_rp96_::base64::base64_decode(encoded_data, encoded_size, decoded_size);

  //then print all
  for(int64_t i = 0; i < input_length; ++i)
  {
    std::cout << input_data[i] << "\n"; //"text to convert with base64"
  }
  for(int64_t i = 0; i < encoded_size; ++i)
  {
    std::cout << encoded_data[i] << "\n"; //"dGV4dCB0byBjb252ZXJ0IHdpdGggYmFzZTY0"
  }
  for(int64_t i = 0; i < decoded_size; ++i)
  {
    std::cout << decoded_data[i] << "\n"; //"text to convert with base64"
  }

  //delete and avoid memory segmentation
  delete input_data;
  input_data = nullptr;
  delete encoded_data;
  encoded_data = nullptr;
  delete decoded_data;
  decoded_data = nullptr;

  return 0;
}
