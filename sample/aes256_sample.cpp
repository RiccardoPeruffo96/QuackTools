/**
 * @author Peruffo Riccardo, RP96, riccardoperuffo96@gmail.com, github.com/RiccardoPeruffo96
 * @date 2022
 * @file aes256_sample.cpp
 */

#include "aes256.hpp"

int main()
{
  std::string input = "text to encrypt and decrypt with CRT-AES256";
  std::string key('a', 64);

  //convert string into uint8_t*
  int64_t input_length = input.length();
  uint8_t* input_data = new uint8_t[input_length];
  for(int64_t i = 0; i < input_length; ++i)
  {
    input_data[i] = static_cast<uint8_t>(input.at(i));
  }

  //start to encrypt
  /**
   * @brief prepare input byte's sequence, if the input % 128 bits != 0 then padding is added using PKCS#7 method
   * @param data_file pointer at the file or byte's sequence
   * @param length_byte_file length's data_file
   * @param key a string that contains the key
   */
  _aes256_rp96_::aes256 data_to_encrypt_01 = _aes256_rp96_::aes256(input_data, input_length, key);
  data_to_encrypt_01.encrypt();

  //print the encrypt value
  int64_t size = data_to_encrypt_01.get_length_payload() + data_to_encrypt_01.get_final_block();
  uint8_t* data = new uint8_t[size];
  int64_t lim = data_to_encrypt_01.get_length_payload();
  for(int64_t i = 0; i < lim; ++i)
  {
    data[i] = data_to_encrypt_01.payload()[i];
    std::cout << data[i] << '\n';
  }
  if(data_to_encrypt_01.get_final_block() != nullptr)
  {
    for(int64_t i = 0; i < data_to_encrypt_01.get_final_block_size(); ++i)
    {
      data[i + lim] = data_to_encrypt_01.get_final_block()[i];
      std::cout << data[i + lim] << '\n';
    }
  }

  _aes256_rp96_::aes256 data_to_decrypt_02 = _aes256_rp96_::aes256(data, size, key);
  data_to_decrypt_02.decrypt();

  //now the values: (were || means concat)
    //"data_to_decrypt_02.payload() || data_to_decrypt_02.get_final_block()"
    //"data_to_encrypt_01.decrypt();" then "data_to_encrypt_01.payload() || data_to_encrypt_01.get_final_block()"
    //input
    //input_data
  //contains the same string "text to encrypt and decrypt with CRT-AES256"

  //delete and avoid memory segmentation
  delete input_data;
  input_data = nullptr;
  delete data;
  data = nullptr;

  return 0;
}
