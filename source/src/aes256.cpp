/**
 * @author Peruffo Riccardo, RP96, riccardoperuffo96@gmail.com, github.com/RiccardoPeruffo96
 * @date 30th APR 2022
 * @file aes256.cpp
 * @brief this file contains the definitions of aes256.hpp's declarations
 * @version 0.0.1 alpha [exam]
 */

#include "aes256.hpp"

using namespace _aes256_rp96_;

aes256::aes256(uint8_t* data_file, int64_t length_byte_file, std::string key) noexcept(true) :
payload{ data_file },
length_payload{ 0 }
{
  const uint64_t STRING_KEY_LEN = DIM_ARRAY * 4;

  if(this->payload == nullptr ||
    length_byte_file < 1 ||
    key.length() != STRING_KEY_LEN)
  {
    if(key.length() != STRING_KEY_LEN)
    {
      std::cout << "problems with key\n";
    }
    else
    {
      std::cout << "problems with input\n";
    }
    return;
  }

  //I start preparing the x2 array[16] or matrix[4][4] (it's the same 'cause an union struct)
  //counter
  int32_t key_index = 0;
  for (int32_t subk_index = 0; subk_index < NUMBER_SUBKEYS; ++subk_index)
  {
    for (int32_t array_index = 0; array_index < DIM_ARRAY; ++array_index)
    {
      this->_subkeys[subk_index].array_form[array_index] = this->char_to_hex(key[key_index++]) << 4;
      this->_subkeys[subk_index].array_form[array_index] |= this->char_to_hex(key[key_index++]);
    }
  }

  //create an array of start counter block mode
  //i calc it with started from an 256bits hash that
  //i need to convert in 128 bits, so i start to read
  //the string from start and from end and merge each couple
  //with xor operation.
  //remember that every string letter need to be converted in
  //4 bits hex value, example letter 'A' (ascii letter from 0x41)
  //or 'a' (ascii letter from 0x61) become 0x0A;
  //or '9' (ascii letter from 0x39) become 0x09
  //every letter can be only a 4 bits value from 0x00..0x0F
  char* hash_tmp = new char[STRING_KEY_LEN];
  for(uint64_t idx = 0; idx < STRING_KEY_LEN; ++idx)
  {
    hash_tmp[idx] = key.at(idx);
  }
  _sha256_rp96_::sha256 hash_lvl2 = _sha256_rp96_::sha256(hash_tmp, STRING_KEY_LEN);
  hash_lvl2.compute_hash();
  for(uint64_t idx = 0; idx < STRING_KEY_LEN; ++idx)
  {
    hash_tmp[idx] = hash_lvl2.get_SHA256_string().at(idx);
  }
  _sha256_rp96_::sha256 hash_lvl3 = _sha256_rp96_::sha256(hash_tmp, STRING_KEY_LEN);
  hash_lvl3.compute_hash();
  delete[] hash_tmp;
  hash_tmp = nullptr;
  this->start_counter.array_form = { 0 };
  for (uint64_t array_index = 0, string_index = 0; array_index < DIM_ARRAY; ++array_index)
  {
    uint8_t hb_01 = this->char_to_hex(hash_lvl3.get_SHA256_string().at(string_index)); //.at 0 -> 2 -> ...
    uint8_t hb_02 = this->char_to_hex(hash_lvl3.get_SHA256_string().at(++string_index)); //.at 1 -> 3 -> ...
    uint8_t hb_04 = this->char_to_hex(hash_lvl3.get_SHA256_string().at(STRING_KEY_LEN - string_index)); //.at 63 -> 61 -> ...
    uint8_t hb_03 = this->char_to_hex(hash_lvl3.get_SHA256_string().at(STRING_KEY_LEN - (++string_index))); //.at 62 -> 60 -> ...

    //i want that hb_01 ^ hb_04, with x4 less significant bits, become the x4 most significant bits about start_counter
    //then hb_02 ^ hb_03, with x4 less significant bits, become the x4 less significant bits about start_counter
    //so first i calc hb_01 ^ hb_04
    //the "& 0xF" clear others x4 most significant bits
    //then "<< 4" transform less significant bits into most significant bits
    this->start_counter.array_form[array_index] = (((hb_01 ^ hb_04) & 0xF) << 4) | ((hb_02 ^ hb_03) & 0x0F);
  }
  //end counter block mode

  //pad the last 128bits block with CMS (Cryptographic Message Syntax)
  int32_t final_mod_length = (length_byte_file % DIM_ARRAY);

  if(final_mod_length == 0)
  {
    this->value_CMS = 0;
  }
  else
  {
    this->value_CMS = DIM_ARRAY - final_mod_length; //always value between [00..0F]
  }

  this->start_final_block = length_byte_file - final_mod_length;
  if (this->value_CMS != 0)
  {
    this->final_pad.array_form = { 0 };
    for (uint64_t array_index = 0; array_index < DIM_ARRAY; ++array_index)
    {
      if (array_index < static_cast<uint64_t>(final_mod_length))
      {
        final_pad.array_form[array_index] = data_file[start_final_block + array_index];
      }
      else
      {
        //CMS (Cryptographic Message Syntax) pad with the value_CMS
        //value_CMS has range 0..0xF
        final_pad.array_form[array_index] = static_cast<uint8_t>(value_CMS);
      }
    }
  }

  this->length_payload = start_final_block;
  this->i_have_encrypt = false;
  this->i_have_decrypt = false;
}

aes256::aes256(const aes256& a256) noexcept(true) :
  payload{ a256.payload },
  length_payload{ a256.length_payload },
  value_CMS{ a256.value_CMS },
  start_final_block{ a256.start_final_block }
{
  //template<class InputIterator, class Size, class OutputIterator>
  this->final_pad.array_form = a256.final_pad.array_form;
  //std::copy_n(a256.final_pad.array_form.begin(), DIM_ARRAY, this->final_pad.array_form.begin()); //final_pad{ a256.final_pad }
  for(uint64_t i = 0; i < TURN_KEYS+1; ++i)
  {
    this->_subkeys = a256._subkeys;
    //std::copy_n(a256._subkeys[i].array_form.begin(), DIM_ARRAY, this->_subkeys[i].array_form.begin()); //_subkeys{ a256._subkeys }
  }
}

aes256& aes256::operator=(const aes256& a256) noexcept(true)
{
  //copy assignment operator is different between copy constructors
  //because first i need to clear the actual data
  payload = a256.payload;

  length_payload = a256.length_payload;
  value_CMS = a256.value_CMS;
  start_final_block = a256.start_final_block;
  this->final_pad.array_form = a256.final_pad.array_form;
  //std::copy_n(a256.final_pad.array_form.begin(), DIM_ARRAY, this->final_pad.array_form.begin()); //final_pad = a256.final_pad
  for(uint64_t i = 0; i < TURN_KEYS+1; ++i)
  {
    this->_subkeys = a256._subkeys;
    //std::copy_n(a256._subkeys[i].array_form.begin(), DIM_ARRAY, this->_subkeys[i].array_form.begin()); //_subkeys = a256._subkeys
  }
  return *this;
}

aes256::aes256(aes256&& a256) noexcept(true) :
  payload{ a256.payload },
  length_payload{ a256.length_payload },
  value_CMS{ a256.value_CMS },
  start_final_block{ a256.start_final_block }
{
  //phase 1: swap strings + clean
  this->final_pad.array_form.swap(a256.final_pad.array_form);
  for(uint64_t i = 0; i < TURN_KEYS+1; ++i)
  {
    this->_subkeys[i].array_form.swap(a256._subkeys[i].array_form);
  }
}

aes256& aes256::operator=(aes256&& a256) noexcept(true)
{
  if (this == &a256)
  {
    return *this;
  }
  //copy assignment operator is different between copy constructors because first i need to clear the actual data
  //phase 1: clean, but i can't because i work with
  //only pointers managed from external
  //delete[] payload;
  //phase 2: assign
  payload = a256.payload;
  length_payload = a256.length_payload;
  value_CMS = a256.value_CMS;
  start_final_block = a256.start_final_block;
  this->final_pad.array_form.swap(a256.final_pad.array_form);
  for(uint64_t i = 0; i < TURN_KEYS+1; ++i)
  {
    this->_subkeys[i].array_form.swap(a256._subkeys[i].array_form);
  }
  return *this;
}

aes256::~aes256() noexcept(true)
{
  //doesn't call delete because aes doesn't copy the data
  this->payload = nullptr;

  this->value_CMS = 0;
  this->length_payload = 0;
  this->start_final_block = 0;
  this->i_have_decrypt = true;
  this->i_have_encrypt = true;

  //overwrite the actual final pad for avoid security issues
  //#pragma unroll(DIM_ARRAY)
  for(uint64_t i = 0; i < DIM_ARRAY; ++i)
  {
    this->final_pad.array_form[i] = 0xFF;
    this->start_counter.array_form[i] = 0xFF;
  }

  //overwrite the actual key for avoid security issues
  //#pragma unroll(TURN_KEYS+1)
  for(uint64_t i = 0; i < TURN_KEYS+1; ++i)
  {
    //#pragma unroll(DIM_ARRAY)
    for(uint64_t j = 0; j < DIM_ARRAY; ++j)
    {
      this->_subkeys[i].array_form[j] = 0xFF;
    }
  }
}

uint8_t aes256::char_to_hex(char tmp)
{
  //convert a char in the hex value
  //letter 'a' or 'A' become 0x0A == 0d10
  //letter 'f' or 'F' become 0x0F == 0d15
  //letter '6'        become 0x06 == 0d06
  static const char downcase = 'a' - 0xA;
  static const char uppercase = 'A' - 0xA;

  if (tmp >= 'a') //tmp >= 0d97
    {
      return tmp - downcase;
    } //tmp - 'a' == 0 -> 0 + 0x0A == 0x0A
  if (tmp >= 'A') //tmp >= 0d65
    {
      return tmp - uppercase;
    } //tmp - 'A' == 0 -> 0 + 0x0A == 0x0A
  return tmp - '0'; //tmp -= 0d48
}

void aes256::xor_op(uint8_t* block, wrapper_128bits& key)
{
  //it's a xor op between every
  //element about block and key
  for (int32_t i = 0; i < DIM_ARRAY; ++i)
  {
    block[i] ^= key.array_form[i];
  }
}

void aes256::shift_rows(uint8_t* block)
{
  //think about a block like a DIM_MATRIX*DIM_MATRIX matrix
  //there are 4 rows and 4 columns
  //index about rows and cols are between 0..3
  //the second row (index == 1) turn up about 1
  rotate_up<1, 1>(block, DIM_MATRIX, DIM_MATRIX);
  //the third row turn up about 2
  rotate_up<2, 2>(block, DIM_MATRIX, DIM_MATRIX);
  //the fourth row turn up about 3
  rotate_up<3, 3>(block, DIM_MATRIX, DIM_MATRIX);
}

void aes256::sub_bytes(uint8_t* block, const wrapper_2048bits& s_box)
{
  //it's a substitution about 1 bytes for every cycle
  for (int32_t i = 0; i < DIM_ARRAY; ++i)
  {
    block[i] = s_box.array_form[static_cast<uint8_t>(block[i])];
  }
}

void aes256::mix_columns(uint8_t* block)
{
  const wrapper_2048bits mul2{{ 0x00, 0x02, 0x04, 0x06, 0x08, 0x0a, 0x0c, 0x0e, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1a, 0x1c, 0x1e,0x20, 0x22, 0x24, 0x26, 0x28, 0x2a, 0x2c, 0x2e, 0x30, 0x32, 0x34, 0x36, 0x38, 0x3a, 0x3c, 0x3e,0x40, 0x42, 0x44, 0x46, 0x48, 0x4a, 0x4c, 0x4e, 0x50, 0x52, 0x54, 0x56, 0x58, 0x5a, 0x5c, 0x5e,0x60, 0x62, 0x64, 0x66, 0x68, 0x6a, 0x6c, 0x6e, 0x70, 0x72, 0x74, 0x76, 0x78, 0x7a, 0x7c, 0x7e,0x80, 0x82, 0x84, 0x86, 0x88, 0x8a, 0x8c, 0x8e, 0x90, 0x92, 0x94, 0x96, 0x98, 0x9a, 0x9c, 0x9e,0xa0, 0xa2, 0xa4, 0xa6, 0xa8, 0xaa, 0xac, 0xae, 0xb0, 0xb2, 0xb4, 0xb6, 0xb8, 0xba, 0xbc, 0xbe,0xc0, 0xc2, 0xc4, 0xc6, 0xc8, 0xca, 0xcc, 0xce, 0xd0, 0xd2, 0xd4, 0xd6, 0xd8, 0xda, 0xdc, 0xde,0xe0, 0xe2, 0xe4, 0xe6, 0xe8, 0xea, 0xec, 0xee, 0xf0, 0xf2, 0xf4, 0xf6, 0xf8, 0xfa, 0xfc, 0xfe,0x1b, 0x19, 0x1f, 0x1d, 0x13, 0x11, 0x17, 0x15, 0x0b, 0x09, 0x0f, 0x0d, 0x03, 0x01, 0x07, 0x05,0x3b, 0x39, 0x3f, 0x3d, 0x33, 0x31, 0x37, 0x35, 0x2b, 0x29, 0x2f, 0x2d, 0x23, 0x21, 0x27, 0x25,0x5b, 0x59, 0x5f, 0x5d, 0x53, 0x51, 0x57, 0x55, 0x4b, 0x49, 0x4f, 0x4d, 0x43, 0x41, 0x47, 0x45,0x7b, 0x79, 0x7f, 0x7d, 0x73, 0x71, 0x77, 0x75, 0x6b, 0x69, 0x6f, 0x6d, 0x63, 0x61, 0x67, 0x65,0x9b, 0x99, 0x9f, 0x9d, 0x93, 0x91, 0x97, 0x95, 0x8b, 0x89, 0x8f, 0x8d, 0x83, 0x81, 0x87, 0x85,0xbb, 0xb9, 0xbf, 0xbd, 0xb3, 0xb1, 0xb7, 0xb5, 0xab, 0xa9, 0xaf, 0xad, 0xa3, 0xa1, 0xa7, 0xa5,0xdb, 0xd9, 0xdf, 0xdd, 0xd3, 0xd1, 0xd7, 0xd5, 0xcb, 0xc9, 0xcf, 0xcd, 0xc3, 0xc1, 0xc7, 0xc5,0xfb, 0xf9, 0xff, 0xfd, 0xf3, 0xf1, 0xf7, 0xf5, 0xeb, 0xe9, 0xef, 0xed, 0xe3, 0xe1, 0xe7, 0xe5 }};

  const wrapper_2048bits mul3{{ 0x00, 0x03, 0x06, 0x05, 0x0c, 0x0f, 0x0a, 0x09, 0x18, 0x1b, 0x1e, 0x1d, 0x14, 0x17, 0x12, 0x11, 0x30, 0x33, 0x36, 0x35, 0x3c, 0x3f, 0x3a, 0x39, 0x28, 0x2b, 0x2e, 0x2d, 0x24, 0x27, 0x22, 0x21, 0x60, 0x63, 0x66, 0x65, 0x6c, 0x6f, 0x6a, 0x69, 0x78, 0x7b, 0x7e, 0x7d, 0x74, 0x77, 0x72, 0x71, 0x50, 0x53, 0x56, 0x55, 0x5c, 0x5f, 0x5a, 0x59, 0x48, 0x4b, 0x4e, 0x4d, 0x44, 0x47, 0x42, 0x41, 0xc0, 0xc3, 0xc6, 0xc5, 0xcc, 0xcf, 0xca, 0xc9, 0xd8, 0xdb, 0xde, 0xdd, 0xd4, 0xd7, 0xd2, 0xd1, 0xf0, 0xf3, 0xf6, 0xf5, 0xfc, 0xff, 0xfa, 0xf9, 0xe8, 0xeb, 0xee, 0xed, 0xe4, 0xe7, 0xe2, 0xe1, 0xa0, 0xa3, 0xa6, 0xa5, 0xac, 0xaf, 0xaa, 0xa9, 0xb8, 0xbb, 0xbe, 0xbd, 0xb4, 0xb7, 0xb2, 0xb1, 0x90, 0x93, 0x96, 0x95, 0x9c, 0x9f, 0x9a, 0x99, 0x88, 0x8b, 0x8e, 0x8d, 0x84, 0x87, 0x82, 0x81, 0x9b, 0x98, 0x9d, 0x9e, 0x97, 0x94, 0x91, 0x92, 0x83, 0x80, 0x85, 0x86, 0x8f, 0x8c, 0x89, 0x8a, 0xab, 0xa8, 0xad, 0xae, 0xa7, 0xa4, 0xa1, 0xa2, 0xb3, 0xb0, 0xb5, 0xb6, 0xbf, 0xbc, 0xb9, 0xba, 0xfb, 0xf8, 0xfd, 0xfe, 0xf7, 0xf4, 0xf1, 0xf2, 0xe3, 0xe0, 0xe5, 0xe6, 0xef, 0xec, 0xe9, 0xea, 0xcb, 0xc8, 0xcd, 0xce, 0xc7, 0xc4, 0xc1, 0xc2, 0xd3, 0xd0, 0xd5, 0xd6, 0xdf, 0xdc, 0xd9, 0xda, 0x5b, 0x58, 0x5d, 0x5e, 0x57, 0x54, 0x51, 0x52, 0x43, 0x40, 0x45, 0x46, 0x4f, 0x4c, 0x49, 0x4a, 0x6b, 0x68, 0x6d, 0x6e, 0x67, 0x64, 0x61, 0x62, 0x73, 0x70, 0x75, 0x76, 0x7f, 0x7c, 0x79, 0x7a, 0x3b, 0x38, 0x3d, 0x3e, 0x37, 0x34, 0x31, 0x32, 0x23, 0x20, 0x25, 0x26, 0x2f, 0x2c, 0x29, 0x2a, 0x0b, 0x08, 0x0d, 0x0e, 0x07, 0x04, 0x01, 0x02, 0x13, 0x10, 0x15, 0x16, 0x1f, 0x1c, 0x19, 0x1a }};

  for (int32_t i = 0; i < DIM_ARRAY; i+=4)
  {
    uint8_t s0, s1, s2, s3;

    s0 = (uint8_t)mul2.array_form[static_cast<uint8_t>(block[i])] ^ mul3.array_form[static_cast<uint8_t>(block[i + 1])] ^ block[i + 2] ^ block[i + 3];
    s1 = block[i] ^ mul2.array_form[static_cast<uint8_t>(block[i + 1])] ^ mul3.array_form[static_cast<uint8_t>(block[i + 2])] ^ block[i + 3];
    s2 = block[i] ^ block[i + 1] ^ mul2.array_form[static_cast<uint8_t>(block[i + 2])] ^ mul3.array_form[static_cast<uint8_t>(block[i + 3])];
    s3 = mul3.array_form[static_cast<uint8_t>(block[i])] ^ block[i + 1] ^ block[i + 2] ^ mul2.array_form[static_cast<uint8_t>(block[i + 3])];

    block[i] = s0;
    block[i + 1] = s1;
    block[i + 2] = s2;
    block[i + 3] = s3;
  }
}

void aes256::encrypt()
{
  const wrapper_2048bits s_box{{ 0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76, 0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, 0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15, 0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75, 0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, 0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf, 0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8, 0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, 0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73, 0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb, 0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, 0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08, 0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a, 0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e, 0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf, 0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 }};

  //we expand 14 times the keys
  expand_key(s_box);

  //times I need to call AES256
  //I don't count the last block
  uint64_t blocks_to_elaborate = this->length_payload / BYTES_FOR_128BITS; //floor(this->length_payload / BYTES_FOR_128BITS)

  //turn for every 128bit
  std::list<std::thread> _thread_list;

  //const bool ENCRYPT = true;

  for (uint64_t single_block = 0; single_block < blocks_to_elaborate; ++single_block)
  {
    _thread_list.push_back(std::thread([&, single_block]()
    {
      this->thread_aes_create(this->payload + (single_block * static_cast<uint64_t>(BYTES_FOR_128BITS)), s_box, single_block);
    }
    ));
  }
  if (this->value_CMS != 0)
  {
    _thread_list.push_back(std::thread([&, blocks_to_elaborate]() { this->thread_aes_create(this->final_pad.array_form.data(), s_box, blocks_to_elaborate); }));
  }

  for (auto& _single_thread : _thread_list)
  {
    _single_thread.join();
  }
  _thread_list.clear();

  this->i_have_encrypt = true;
  this->i_have_decrypt = false;
}

void aes256::decrypt()
{
  const wrapper_2048bits s_box{{ 0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76, 0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, 0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15, 0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75, 0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, 0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf, 0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8, 0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, 0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73, 0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb, 0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, 0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08, 0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a, 0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e, 0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf, 0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 }};

  //we expand 14 times the keys
  expand_key(s_box);

  //times I need to call AES256
  //I don't count the last block
  int32_t blocks_to_elaborate = this->length_payload / BYTES_FOR_128BITS; //floor(this->length_payload / BYTES_FOR_128BITS)

  //turn for every 128bit
  std::list<std::thread> _thread_list;

  for (uint64_t single_block = 0; single_block < static_cast<uint64_t>(blocks_to_elaborate); ++single_block)
  {
    _thread_list.push_back(std::thread([&, single_block]() { this->thread_aes_create(this->payload + (single_block * static_cast<uint64_t>(BYTES_FOR_128BITS)), s_box, single_block); }));
  }

  /*
  if (this->value_CMS != 0)
  {
    _thread_list.push_back(std::thread([&, blocks_to_elaborate]() { this->thread_aes_create(this->final_pad.array_form.data(), s_box, blocks_to_elaborate); }));
  }
  */

  for (auto& _single_thread : _thread_list)
  {
    _single_thread.join();
  }
  _thread_list.clear();

  //after encrypt every file needs to have this->value_CMS == 0 because padding
  //so after decrypt last block has potentially padding
  //to search the padding we search about a last byte
  //that should contains a value between 1 and F
  //so then I need to find the last byte repeated the last byte value
  //example:
  //last byte 01 -> XX XX XX 01
  //or last byte 02 -> XX XX 02 02
  //or last byte 03 -> XX 03 03 03 ...
  //if this pattern is not strictly follow, padding doesn't exists
  uint64_t counter = this->length_payload - 1; //last byte array index
  uint8_t padding_value = this->payload[counter]; //last byte value

  //first check on last byte value
  bool check_theres_padding = true;
  if(padding_value < 0x1 || padding_value > 0xF)
  {
    check_theres_padding = false;
  }

  //search last, if check_theres_padding == false it's already done
  for(auto idx = 1;
     idx < padding_value && check_theres_padding == true;
     ++idx)
  {
    //--counter;
    //if I enter once then the padding pattern is not strictly follow
    if(padding_value != this->payload[--counter])
    {
      check_theres_padding = false;
    }
  }

  //then then pattern is strictly follow
  if(check_theres_padding == true)
  {
    this->value_CMS = padding_value;
    this->length_payload = this->length_payload - padding_value;
  }

  //------ test for unit test ------//
  if(this->value_CMS > 0)
  {
    uint64_t tmp_len_payload = static_cast<uint64_t>(this->length_payload);
    for(int32_t i = 0; i < DIM_ARRAY - this->value_CMS; ++i, ++tmp_len_payload)
    {
      this->final_pad.array_form[static_cast<uint64_t>(i)] = this->payload[tmp_len_payload];
    }
    for(int32_t i = DIM_ARRAY - this->value_CMS; i < DIM_ARRAY; ++i)
    {
      this->final_pad.array_form[static_cast<uint64_t>(i)] = this->value_CMS;
    }
  }
  //------ end test for unit test ------//

  this->i_have_encrypt = false;
  this->i_have_decrypt = true;
}

void aes256::inv_shift_rows(uint8_t* block)
{
  rotate_down<1, 1>(block, DIM_MATRIX, DIM_MATRIX);
  rotate_down<2, 2>(block, DIM_MATRIX, DIM_MATRIX);
  rotate_down<3, 3>(block, DIM_MATRIX, DIM_MATRIX);
}

void aes256::inv_mix_columns(uint8_t* block)
{
  const wrapper_2048bits mul9 = {{ 0x00, 0x09, 0x12, 0x1b, 0x24, 0x2d, 0x36, 0x3f, 0x48, 0x41, 0x5a, 0x53, 0x6c, 0x65, 0x7e, 0x77,0x90, 0x99, 0x82, 0x8b, 0xb4, 0xbd, 0xa6, 0xaf, 0xd8, 0xd1, 0xca, 0xc3, 0xfc, 0xf5, 0xee, 0xe7,0x3b, 0x32, 0x29, 0x20, 0x1f, 0x16, 0x0d, 0x04, 0x73, 0x7a, 0x61, 0x68, 0x57, 0x5e, 0x45, 0x4c,0xab, 0xa2, 0xb9, 0xb0, 0x8f, 0x86, 0x9d, 0x94, 0xe3, 0xea, 0xf1, 0xf8, 0xc7, 0xce, 0xd5, 0xdc,0x76, 0x7f, 0x64, 0x6d, 0x52, 0x5b, 0x40, 0x49, 0x3e, 0x37, 0x2c, 0x25, 0x1a, 0x13, 0x08, 0x01,0xe6, 0xef, 0xf4, 0xfd, 0xc2, 0xcb, 0xd0, 0xd9, 0xae, 0xa7, 0xbc, 0xb5, 0x8a, 0x83, 0x98, 0x91,0x4d, 0x44, 0x5f, 0x56, 0x69, 0x60, 0x7b, 0x72, 0x05, 0x0c, 0x17, 0x1e, 0x21, 0x28, 0x33, 0x3a,0xdd, 0xd4, 0xcf, 0xc6, 0xf9, 0xf0, 0xeb, 0xe2, 0x95, 0x9c, 0x87, 0x8e, 0xb1, 0xb8, 0xa3, 0xaa,0xec, 0xe5, 0xfe, 0xf7, 0xc8, 0xc1, 0xda, 0xd3, 0xa4, 0xad, 0xb6, 0xbf, 0x80, 0x89, 0x92, 0x9b,0x7c, 0x75, 0x6e, 0x67, 0x58, 0x51, 0x4a, 0x43, 0x34, 0x3d, 0x26, 0x2f, 0x10, 0x19, 0x02, 0x0b,0xd7, 0xde, 0xc5, 0xcc, 0xf3, 0xfa, 0xe1, 0xe8, 0x9f, 0x96, 0x8d, 0x84, 0xbb, 0xb2, 0xa9, 0xa0,0x47, 0x4e, 0x55, 0x5c, 0x63, 0x6a, 0x71, 0x78, 0x0f, 0x06, 0x1d, 0x14, 0x2b, 0x22, 0x39, 0x30,0x9a, 0x93, 0x88, 0x81, 0xbe, 0xb7, 0xac, 0xa5, 0xd2, 0xdb, 0xc0, 0xc9, 0xf6, 0xff, 0xe4, 0xed,0x0a, 0x03, 0x18, 0x11, 0x2e, 0x27, 0x3c, 0x35, 0x42, 0x4b, 0x50, 0x59, 0x66, 0x6f, 0x74, 0x7d,0xa1, 0xa8, 0xb3, 0xba, 0x85, 0x8c, 0x97, 0x9e, 0xe9, 0xe0, 0xfb, 0xf2, 0xcd, 0xc4, 0xdf, 0xd6,0x31, 0x38, 0x23, 0x2a, 0x15, 0x1c, 0x07, 0x0e, 0x79, 0x70, 0x6b, 0x62, 0x5d, 0x54, 0x4f, 0x46 }};

  const wrapper_2048bits mul11 = {{ 0x00, 0x0b, 0x16, 0x1d, 0x2c, 0x27, 0x3a, 0x31, 0x58, 0x53, 0x4e, 0x45, 0x74, 0x7f, 0x62, 0x69,0xb0, 0xbb, 0xa6, 0xad, 0x9c, 0x97, 0x8a, 0x81, 0xe8, 0xe3, 0xfe, 0xf5, 0xc4, 0xcf, 0xd2, 0xd9,0x7b, 0x70, 0x6d, 0x66, 0x57, 0x5c, 0x41, 0x4a, 0x23, 0x28, 0x35, 0x3e, 0x0f, 0x04, 0x19, 0x12,0xcb, 0xc0, 0xdd, 0xd6, 0xe7, 0xec, 0xf1, 0xfa, 0x93, 0x98, 0x85, 0x8e, 0xbf, 0xb4, 0xa9, 0xa2,0xf6, 0xfd, 0xe0, 0xeb, 0xda, 0xd1, 0xcc, 0xc7, 0xae, 0xa5, 0xb8, 0xb3, 0x82, 0x89, 0x94, 0x9f,0x46, 0x4d, 0x50, 0x5b, 0x6a, 0x61, 0x7c, 0x77, 0x1e, 0x15, 0x08, 0x03, 0x32, 0x39, 0x24, 0x2f,0x8d, 0x86, 0x9b, 0x90, 0xa1, 0xaa, 0xb7, 0xbc, 0xd5, 0xde, 0xc3, 0xc8, 0xf9, 0xf2, 0xef, 0xe4,0x3d, 0x36, 0x2b, 0x20, 0x11, 0x1a, 0x07, 0x0c, 0x65, 0x6e, 0x73, 0x78, 0x49, 0x42, 0x5f, 0x54,0xf7, 0xfc, 0xe1, 0xea, 0xdb, 0xd0, 0xcd, 0xc6, 0xaf, 0xa4, 0xb9, 0xb2, 0x83, 0x88, 0x95, 0x9e,0x47, 0x4c, 0x51, 0x5a, 0x6b, 0x60, 0x7d, 0x76, 0x1f, 0x14, 0x09, 0x02, 0x33, 0x38, 0x25, 0x2e,0x8c, 0x87, 0x9a, 0x91, 0xa0, 0xab, 0xb6, 0xbd, 0xd4, 0xdf, 0xc2, 0xc9, 0xf8, 0xf3, 0xee, 0xe5,0x3c, 0x37, 0x2a, 0x21, 0x10, 0x1b, 0x06, 0x0d, 0x64, 0x6f, 0x72, 0x79, 0x48, 0x43, 0x5e, 0x55,0x01, 0x0a, 0x17, 0x1c, 0x2d, 0x26, 0x3b, 0x30, 0x59, 0x52, 0x4f, 0x44, 0x75, 0x7e, 0x63, 0x68,0xb1, 0xba, 0xa7, 0xac, 0x9d, 0x96, 0x8b, 0x80, 0xe9, 0xe2, 0xff, 0xf4, 0xc5, 0xce, 0xd3, 0xd8,0x7a, 0x71, 0x6c, 0x67, 0x56, 0x5d, 0x40, 0x4b, 0x22, 0x29, 0x34, 0x3f, 0x0e, 0x05, 0x18, 0x13,0xca, 0xc1, 0xdc, 0xd7, 0xe6, 0xed, 0xf0, 0xfb, 0x92, 0x99, 0x84, 0x8f, 0xbe, 0xb5, 0xa8, 0xa3 }};

  const wrapper_2048bits mul13 = {{ 0x00, 0x0d, 0x1a, 0x17, 0x34, 0x39, 0x2e, 0x23, 0x68, 0x65, 0x72, 0x7f, 0x5c, 0x51, 0x46, 0x4b,0xd0, 0xdd, 0xca, 0xc7, 0xe4, 0xe9, 0xfe, 0xf3, 0xb8, 0xb5, 0xa2, 0xaf, 0x8c, 0x81, 0x96, 0x9b,0xbb, 0xb6, 0xa1, 0xac, 0x8f, 0x82, 0x95, 0x98, 0xd3, 0xde, 0xc9, 0xc4, 0xe7, 0xea, 0xfd, 0xf0,0x6b, 0x66, 0x71, 0x7c, 0x5f, 0x52, 0x45, 0x48, 0x03, 0x0e, 0x19, 0x14, 0x37, 0x3a, 0x2d, 0x20,0x6d, 0x60, 0x77, 0x7a, 0x59, 0x54, 0x43, 0x4e, 0x05, 0x08, 0x1f, 0x12, 0x31, 0x3c, 0x2b, 0x26,0xbd, 0xb0, 0xa7, 0xaa, 0x89, 0x84, 0x93, 0x9e, 0xd5, 0xd8, 0xcf, 0xc2, 0xe1, 0xec, 0xfb, 0xf6,0xd6, 0xdb, 0xcc, 0xc1, 0xe2, 0xef, 0xf8, 0xf5, 0xbe, 0xb3, 0xa4, 0xa9, 0x8a, 0x87, 0x90, 0x9d,0x06, 0x0b, 0x1c, 0x11, 0x32, 0x3f, 0x28, 0x25, 0x6e, 0x63, 0x74, 0x79, 0x5a, 0x57, 0x40, 0x4d,0xda, 0xd7, 0xc0, 0xcd, 0xee, 0xe3, 0xf4, 0xf9, 0xb2, 0xbf, 0xa8, 0xa5, 0x86, 0x8b, 0x9c, 0x91,0x0a, 0x07, 0x10, 0x1d, 0x3e, 0x33, 0x24, 0x29, 0x62, 0x6f, 0x78, 0x75, 0x56, 0x5b, 0x4c, 0x41,0x61, 0x6c, 0x7b, 0x76, 0x55, 0x58, 0x4f, 0x42, 0x09, 0x04, 0x13, 0x1e, 0x3d, 0x30, 0x27, 0x2a,0xb1, 0xbc, 0xab, 0xa6, 0x85, 0x88, 0x9f, 0x92, 0xd9, 0xd4, 0xc3, 0xce, 0xed, 0xe0, 0xf7, 0xfa,0xb7, 0xba, 0xad, 0xa0, 0x83, 0x8e, 0x99, 0x94, 0xdf, 0xd2, 0xc5, 0xc8, 0xeb, 0xe6, 0xf1, 0xfc,0x67, 0x6a, 0x7d, 0x70, 0x53, 0x5e, 0x49, 0x44, 0x0f, 0x02, 0x15, 0x18, 0x3b, 0x36, 0x21, 0x2c,0x0c, 0x01, 0x16, 0x1b, 0x38, 0x35, 0x22, 0x2f, 0x64, 0x69, 0x7e, 0x73, 0x50, 0x5d, 0x4a, 0x47,0xdc, 0xd1, 0xc6, 0xcb, 0xe8, 0xe5, 0xf2, 0xff, 0xb4, 0xb9, 0xae, 0xa3, 0x80, 0x8d, 0x9a, 0x97 }};

  const wrapper_2048bits mul14 = {{ 0x00, 0x0e, 0x1c, 0x12, 0x38, 0x36, 0x24, 0x2a, 0x70, 0x7e, 0x6c, 0x62, 0x48, 0x46, 0x54, 0x5a,0xe0, 0xee, 0xfc, 0xf2, 0xd8, 0xd6, 0xc4, 0xca, 0x90, 0x9e, 0x8c, 0x82, 0xa8, 0xa6, 0xb4, 0xba,0xdb, 0xd5, 0xc7, 0xc9, 0xe3, 0xed, 0xff, 0xf1, 0xab, 0xa5, 0xb7, 0xb9, 0x93, 0x9d, 0x8f, 0x81,0x3b, 0x35, 0x27, 0x29, 0x03, 0x0d, 0x1f, 0x11, 0x4b, 0x45, 0x57, 0x59, 0x73, 0x7d, 0x6f, 0x61,0xad, 0xa3, 0xb1, 0xbf, 0x95, 0x9b, 0x89, 0x87, 0xdd, 0xd3, 0xc1, 0xcf, 0xe5, 0xeb, 0xf9, 0xf7,0x4d, 0x43, 0x51, 0x5f, 0x75, 0x7b, 0x69, 0x67, 0x3d, 0x33, 0x21, 0x2f, 0x05, 0x0b, 0x19, 0x17,0x76, 0x78, 0x6a, 0x64, 0x4e, 0x40, 0x52, 0x5c, 0x06, 0x08, 0x1a, 0x14, 0x3e, 0x30, 0x22, 0x2c,0x96, 0x98, 0x8a, 0x84, 0xae, 0xa0, 0xb2, 0xbc, 0xe6, 0xe8, 0xfa, 0xf4, 0xde, 0xd0, 0xc2, 0xcc,0x41, 0x4f, 0x5d, 0x53, 0x79, 0x77, 0x65, 0x6b, 0x31, 0x3f, 0x2d, 0x23, 0x09, 0x07, 0x15, 0x1b,0xa1, 0xaf, 0xbd, 0xb3, 0x99, 0x97, 0x85, 0x8b, 0xd1, 0xdf, 0xcd, 0xc3, 0xe9, 0xe7, 0xf5, 0xfb,0x9a, 0x94, 0x86, 0x88, 0xa2, 0xac, 0xbe, 0xb0, 0xea, 0xe4, 0xf6, 0xf8, 0xd2, 0xdc, 0xce, 0xc0,0x7a, 0x74, 0x66, 0x68, 0x42, 0x4c, 0x5e, 0x50, 0x0a, 0x04, 0x16, 0x18, 0x32, 0x3c, 0x2e, 0x20,0xec, 0xe2, 0xf0, 0xfe, 0xd4, 0xda, 0xc8, 0xc6, 0x9c, 0x92, 0x80, 0x8e, 0xa4, 0xaa, 0xb8, 0xb6,0x0c, 0x02, 0x10, 0x1e, 0x34, 0x3a, 0x28, 0x26, 0x7c, 0x72, 0x60, 0x6e, 0x44, 0x4a, 0x58, 0x56,0x37, 0x39, 0x2b, 0x25, 0x0f, 0x01, 0x13, 0x1d, 0x47, 0x49, 0x5b, 0x55, 0x7f, 0x71, 0x63, 0x6d,0xd7, 0xd9, 0xcb, 0xc5, 0xef, 0xe1, 0xf3, 0xfd, 0xa7, 0xa9, 0xbb, 0xb5, 0x9f, 0x91, 0x83, 0x8d }};

  for (int32_t i = 0; i < DIM_ARRAY; i += 4)
  {
    uint8_t s0, s1, s2, s3;

    s0 = mul14.array_form[static_cast<uint8_t>(block[i])] ^ mul11.array_form[static_cast<uint8_t>(block[i + 1])] ^ mul13.array_form[static_cast<uint8_t>(block[i + 2])] ^ mul9.array_form[static_cast<uint8_t>(block[i + 3])];
    s1 = mul9.array_form[static_cast<uint8_t>(block[i])] ^ mul14.array_form[static_cast<uint8_t>(block[i + 1])] ^ mul11.array_form[static_cast<uint8_t>(block[i + 2])] ^ mul13.array_form[static_cast<uint8_t>(block[i + 3])];
    s2 = mul13.array_form[static_cast<uint8_t>(block[i])] ^ mul9.array_form[static_cast<uint8_t>(block[i + 1])] ^ mul14.array_form[static_cast<uint8_t>(block[i + 2])] ^ mul11.array_form[static_cast<uint8_t>(block[i + 3])];
    s3 = mul11.array_form[static_cast<uint8_t>(block[i])] ^ mul13.array_form[static_cast<uint8_t>(block[i + 1])] ^ mul9.array_form[static_cast<uint8_t>(block[i + 2])] ^ mul14.array_form[static_cast<uint8_t>(block[i + 3])];

    block[i] = s0;
    block[i + 1] = s1;
    block[i + 2] = s2;
    block[i + 3] = s3;
  }
}

void aes256::aes256_algorithm(uint8_t* block, const wrapper_2048bits& s_box)
{
  //std::cout << "round[ 0"; //std::cout << "].input\t"; this->print_arr(block); //std::cout << std::endl;
  //std::cout << "round[ 0"; //std::cout << "].k_sch\t"; this->print_arr(this->_subkeys[0].array_form.data()); //std::cout << std::endl;

  this->xor_op(block, this->_subkeys[0]);

  for (int32_t round = 0; round < TURN_KEYS; )
  {
    ++round;
    //std::cout << "round["; (round < 10 ? (//std::cout <<" " << round) : (//std::cout << round)); //std::cout << "].start\t"; this->print_arr(block); //std::cout << std::endl;
    this->sub_bytes(block, s_box);
    //std::cout << "round["; (round < 10 ? (//std::cout << " " << round) : (//std::cout << round)); //std::cout << "].s_box\t"; this->print_arr(block); //std::cout << std::endl;
    this->shift_rows(block);

    //std::cout << "round["; (round < 10 ? (//std::cout <<" " << round) : (//std::cout << round)); //std::cout << "].s_row\t"; this->print_arr(block); //std::cout << std::endl;
    if (round < TURN_KEYS)
    {
      this->mix_columns(block);
      //std::cout << "round["; (round < 10 ? (//std::cout <<" " << round) : (//std::cout << round)); //std::cout << "].m_col\t"; this->print_arr(block); //std::cout << std::endl;
    }
    this->xor_op(block, this->_subkeys[round]);
    //std::cout << "round["; (round < 10 ? (//std::cout <<" " << round) : (//std::cout << round)); //std::cout << "].k_sch\t"; this->print_arr(this->_subkeys[round].array_form.data()); //std::cout << std::endl;
  }
  //std::cout << "round[14].output\t"; this->print_arr(block); //std::cout << std::endl;
}

void aes256::inv_aes256_algorithm(uint8_t* block, const wrapper_2048bits& inv_s_box)
{
  //output
  //std::cout << "round[ 0"; //std::cout << "].iinput\t"; this->print_arr(block); //std::cout << std::endl;
  //std::cout << "round[ 0"; //std::cout << "].ik_sch\t"; this->print_arr(this->_subkeys[TURN_KEYS].array_form.data()); //std::cout << std::endl;

  this->xor_op(block, this->_subkeys[TURN_KEYS]);
  //std::cout << "round[ 0].istart\t"; this->print_arr(block); //std::cout << std::endl;

  for (int64_t round = 0; round < TURN_KEYS; )
  {
    ++round;

    this->inv_shift_rows(block);
    //std::cout << "round["; (round < 10 ? (//std::cout << " " << round) : (//std::cout << round)); //std::cout << "].is_row\t"; this->print_arr(block); //std::cout << std::endl;

    this->sub_bytes(block, inv_s_box);
    //std::cout << "round["; (round < 10 ? (//std::cout << " " << round) : (//std::cout << round)); //std::cout << "].is_box\t"; this->print_arr(block); //std::cout << std::endl;

    this->xor_op(block, this->_subkeys[TURN_KEYS - round]);
    //std::cout << "round["; (round < 10 ? (//std::cout << " " << round) : (//std::cout << round)); //std::cout << "].ik_sch\t"; this->print_arr(this->_subkeys[TURN_KEYS - round].array_form.data()); //std::cout << std::endl;
    //std::cout << "round["; (round < 10 ? (//std::cout << " " << round) : (//std::cout << round)); //std::cout << "].ik_add\t"; this->print_arr(block); //std::cout << std::endl;

    if (round < TURN_KEYS)
    {
      this->inv_mix_columns(block);
      //std::cout << "round["; (round < 10 ? (//std::cout << " " << round) : (//std::cout << round)); //std::cout << "].istart\t"; this->print_arr(block); //std::cout << std::endl;
    }
  }
  //std::cout << "round[14].output\t"; this->print_arr(block); //std::cout << std::endl;
}

void aes256::expand_key(const wrapper_2048bits& s_box)
{
  const uint8_t rcon[7] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40 };

  for (int64_t idx_key = 2; idx_key < TURN_KEYS + 1; ++idx_key)
  {
    if (idx_key % 2 == 0)
    {
      //first col transfer                                                                         //subbytes                                   //rotate last column
      //key#0                                   =  ----key_original#0----                         ^  s_box[----key_original#1----]                                    ^  rcon[round];
      this->_subkeys[idx_key].matrix_form[0][0] = (this->_subkeys[idx_key - 2].matrix_form[0][0]) ^ (s_box.array_form[this->_subkeys[idx_key - 1].matrix_form[3][1]]) ^ (rcon[(idx_key - 1)/2]);
      this->_subkeys[idx_key].matrix_form[0][1] = (this->_subkeys[idx_key - 2].matrix_form[0][1]) ^ (s_box.array_form[this->_subkeys[idx_key - 1].matrix_form[3][2]]);
      this->_subkeys[idx_key].matrix_form[0][2] = (this->_subkeys[idx_key - 2].matrix_form[0][2]) ^ (s_box.array_form[this->_subkeys[idx_key - 1].matrix_form[3][3]]);
      this->_subkeys[idx_key].matrix_form[0][3] = (this->_subkeys[idx_key - 2].matrix_form[0][3]) ^ (s_box.array_form[this->_subkeys[idx_key - 1].matrix_form[3][0]]);
    }
    else
    {
      //first col transfer                                                                         //subbytes                                   //I take last column
      //key#1                                   =  ----key_original#1----                         ^  s_box[----key_original#0----];
      this->_subkeys[idx_key].matrix_form[0][0] = (this->_subkeys[idx_key - 2].matrix_form[0][0]) ^ (s_box.array_form[this->_subkeys[idx_key - 1].matrix_form[3][0]]);
      this->_subkeys[idx_key].matrix_form[0][1] = (this->_subkeys[idx_key - 2].matrix_form[0][1]) ^ (s_box.array_form[this->_subkeys[idx_key - 1].matrix_form[3][1]]);
      this->_subkeys[idx_key].matrix_form[0][2] = (this->_subkeys[idx_key - 2].matrix_form[0][2]) ^ (s_box.array_form[this->_subkeys[idx_key - 1].matrix_form[3][2]]);
      this->_subkeys[idx_key].matrix_form[0][3] = (this->_subkeys[idx_key - 2].matrix_form[0][3]) ^ (s_box.array_form[this->_subkeys[idx_key - 1].matrix_form[3][3]]);
    }

    for (int64_t col = 1; col < 4; ++col)
    {
      //key#0                                        =  ----key_original#0----                        ^  previous_col_key#0;
      this->_subkeys[idx_key].matrix_form[col][0] = (this->_subkeys[idx_key - 2].matrix_form[col][0]) ^ (this->_subkeys[idx_key].matrix_form[col - 1][0]);
      this->_subkeys[idx_key].matrix_form[col][1] = (this->_subkeys[idx_key - 2].matrix_form[col][1]) ^ (this->_subkeys[idx_key].matrix_form[col - 1][1]);
      this->_subkeys[idx_key].matrix_form[col][2] = (this->_subkeys[idx_key - 2].matrix_form[col][2]) ^ (this->_subkeys[idx_key].matrix_form[col - 1][2]);
      this->_subkeys[idx_key].matrix_form[col][3] = (this->_subkeys[idx_key - 2].matrix_form[col][3]) ^ (this->_subkeys[idx_key].matrix_form[col - 1][3]);
    }
  }
}

void aes256::thread_aes_create(uint8_t* block, const wrapper_2048bits& s_box, uint64_t single_block)
{
  wrapper_128bits counter;
  sum_w128(this->start_counter, single_block, counter);

  /*
  uint64_t numbers_hex_need = msb(single_block) / 8;
  uint8_t value_CMS_sb = 0x10 - (numbers_hex_need + 1);

  for (int64_t array_index = numbers_hex_need; array_index >= 0; --array_index)
  {
    counter.array_form[array_index] = static_cast<uint8_t>((single_block >> (8 * (numbers_hex_need - array_index))) % 8);
  }
  for (int64_t array_index = numbers_hex_need + 1; array_index < DIM_ARRAY; ++array_index)
  {
    //CMS (Cryptographic Message Syntax) pad with the value_CMS
    //value_CMS has range 0..0xF
    counter.array_form[array_index] = static_cast<uint8_t>(value_CMS_sb);
  }
  */

  this->aes256_algorithm(counter.array_form.data(), s_box);
  //this->xor_op(this->payload + (single_block * static_cast<uint64_t>(BYTES_FOR_128BITS)), counter);
  this->xor_op(block, counter);
}

void aes256::sum_w128(const wrapper_128bits& input_w, uint64_t input_n, wrapper_128bits& output)
{
  output.array_form[DIM_ARRAY - 1] = 0; //last value start from 0
  //#pragma unroll(DIM_ARRAY - 1)
  for(uint64_t idx = DIM_ARRAY - 1; idx > 0; --idx)
  {
    //start value
    uint16_t value = 0;

    //sum the 8bits value and store it in 16 bits to check overflow
    value = static_cast<uint8_t>(input_n) + static_cast<uint8_t>(input_w.array_form[idx]);

    //if i have overflow
    if(value > 0xFF)
    {
      output.array_form[idx - 1] = 1;
    }
    else
    {
      output.array_form[idx - 1] = 0;
    }

    //afterall i make the sum
    output.array_form[idx] += value;

    //and clear input_n
    input_n >>= sizeof(uint8_t);
  }

  //for last case overflow doesn't matter
  output.array_form[0] += static_cast<uint8_t>(input_n) + static_cast<uint8_t>(input_w.array_form[0]);
}

template<typename T>
T _aes256_rp96_::gcd(T a, T b)
{
  if (b == 0)
  {
    return a;
  }
  else
  {
    return gcd(b, a % b);
  }
}

template<int32_t row, int32_t round, typename T>
void _aes256_rp96_::rotate_left(T* pointer_arr, uint32_t num_rows, uint32_t num_cols)
{
  int32_t row_to_work;
  if(row > num_rows)
  {
    row_to_work = row % num_rows;
  }
  else
  {
    row_to_work = row;
  }

  int32_t lateral_movement = round % num_cols;
  if (lateral_movement <= 0)
  {
    if (lateral_movement < 0)
    {
      lateral_movement = num_cols - lateral_movement;
    }
    else
    {
      return;
    }
  }

  T* ptr = &pointer_arr[row_to_work * num_cols];
  int32_t g_c_d = gcd<int32_t>(lateral_movement, num_cols);
  for (int32_t i = 0; i < g_c_d; i++)
  {
    // move i-th values of blocks
    auto temp = *(ptr + i);
    int32_t j = i;

    bool finish = false;
    while (!finish) {
      int32_t k = j + lateral_movement;
      if (k >= static_cast<int32_t>(num_cols))
      {
        k = k - num_cols;
      }
      if (k == i)
      {
        finish = true;
      }
      else
      {
        *(ptr + j) = *(ptr + k);
        j = k;
      }
    }
    *(ptr + j) = temp;
  }
  return;
}

template<int32_t row, int32_t round, typename T>
void _aes256_rp96_::rotate_right(T* pointer_arr, uint32_t num_rows, uint32_t num_cols)
{
  int32_t row_to_work;
  if(row > num_rows)
  {
    row_to_work = row % num_rows;
  }
  else
  {
    row_to_work = row;
  }

  int32_t lateral_movement = round % num_cols;
  if (lateral_movement == 0)
  {
    return;
  }
  else if (lateral_movement > 0)
  {
    lateral_movement = num_cols - lateral_movement;
  }
  else if (lateral_movement < 0)
  {
    lateral_movement = num_cols + lateral_movement;
  }
  //*(*(*(arr + i ) + j ) + k) is equivalent to the subscript expression arr[i][j][k].
  //*(*(arr + i ) + j ) is equivalent to the subscript expression arr[i][j].

  T* ptr = &pointer_arr[row_to_work * num_cols];
  int32_t g_c_d = gcd<int32_t>(lateral_movement, num_cols);
  for (int32_t i = 0; i < g_c_d; i++)
  {
    // move i-th values of blocks
    auto temp = *(ptr + i);
    int32_t j = i;

    bool finish = false;
    while (!finish) {
      int32_t k = j + lateral_movement;
      if (k >= static_cast<int32_t>(num_cols))
      {
        k = k - num_cols;
      }
      if (k == i)
      {
        finish = true;
      }
      else
      {
        //*(*(pointer_arr + row_to_work) + j) = *(*(pointer_arr + row_to_work) + k);
        *(ptr + j) = *(ptr + k);
        j = k;
      }
    }
    *(ptr + j) = temp;
  }
  return;
}

template<int32_t col, int32_t round, typename T>
void _aes256_rp96_::rotate_up(T* pointer_arr, uint32_t num_rows, uint32_t num_cols)
{
  int32_t lateral_movement = round % num_rows;
  if (lateral_movement <= 0)
  {
    if (lateral_movement < 0)
    {
      lateral_movement = num_rows - lateral_movement;
    }
    else
    {
      return;
    }
  }

  T* ptr = &pointer_arr[col]; //first element of the column
  int32_t g_c_d = gcd<int32_t>(lateral_movement, num_rows);
  for (int32_t i = 0; i < g_c_d; i++)
  {
    // move i-th values of blocks
    auto temp = *(ptr + (static_cast<int64_t>(i) * static_cast<int64_t>(num_cols)));
    int32_t j = i;

    bool finish = false;
    while (!finish) {
      int32_t k = j + lateral_movement;
      if (static_cast<int64_t>(k) >= static_cast<int64_t>(num_rows))
      {
        k = k - num_rows;
      }
      if (k == i)
      {
        finish = true;
      }
      else
      {
        *(ptr + (static_cast<int64_t>(j) * static_cast<int64_t>(num_cols))) = *(ptr + (static_cast<int64_t>(k) * static_cast<int64_t>(num_cols)));
        j = k;
      }
    }
    *(ptr + (static_cast<int64_t>(j) * static_cast<int64_t>(num_cols))) = temp;
  }
  return;
}

template<int32_t col, int32_t round, typename T>
void _aes256_rp96_::rotate_down(T* pointer_arr, uint32_t num_rows, uint32_t num_cols)
{
  int32_t lateral_movement = round % num_cols;
  if (lateral_movement == 0)
  {
    return;
  }
  else if (lateral_movement > 0)
  {
    lateral_movement = num_rows - lateral_movement;
  }
  else if (lateral_movement < 0)
  {
    lateral_movement = num_rows + lateral_movement;
  }

  T* ptr = &pointer_arr[col]; //first element of the column
  int32_t g_c_d = gcd<int32_t>(lateral_movement, num_rows);
  for (int32_t i = 0; i < g_c_d; i++)
  {
    // move i-th values of blocks
    auto temp = *(ptr + (static_cast<int64_t>(i) * static_cast<int64_t>(num_cols)));
    int32_t j = i;

    bool finish = false;
    while (!finish)
    {
      int32_t k = j + lateral_movement;
      if (static_cast<int64_t>(k) >= static_cast<int64_t>(num_rows))
      {
        k = k - num_rows;
      }
      if (k == i)
      {
        finish = true;
      }
      else
      {
        *(ptr + (static_cast<int64_t>(j) * static_cast<int64_t>(num_cols))) = *(ptr + (static_cast<int64_t>(k) * static_cast<int64_t>(num_cols)));
        j = k;
      }
    }
    *(ptr + (static_cast<int64_t>(j) * static_cast<int64_t>(num_cols))) = temp;
  }
  return;
}

template<typename T>
uint64_t _aes256_rp96_::msb(T value)
{
  uint64_t integer_value = static_cast<uint64_t>(value);
  if (integer_value == 0)
  {
    return 0;
  }
  return msb_recursion(integer_value, sizeof(value), 1);
}

uint64_t _aes256_rp96_::msb_recursion(uint64_t value, uint64_t size_lim_max, uint64_t size_lim_min)
{
  if (size_lim_max == size_lim_min)
  {
    uint64_t n_bits = size_lim_max * 8;
    uint8_t final_value = static_cast<uint8_t>(value >> (n_bits - 8));
    if (final_value & 0b1000000) return n_bits - 1;
    if (final_value & 0b100000) return n_bits - 2;
    if (final_value & 0b10000) return n_bits - 3;
    if (final_value & 0b1000) return n_bits - 4;
    if (final_value & 0b100) return n_bits - 5;
    if (final_value & 0b10) return n_bits - 6;
    if (final_value & 0b1) return n_bits - 7;
    return -1;
  }

  uint64_t half = ((size_lim_max + size_lim_min) / 2);
  uint64_t test = 1;
  test <<= (half * 8);

  if (value >= test)
  {
    if (half == size_lim_min)
      ++half;
    return msb_recursion(value, size_lim_max, half);
  }

  if (half == size_lim_max)
    --half;
  return msb_recursion(value, half, size_lim_min);
}

int64_t aes256::get_final_block_size()
{
  if(this->have_i_encrypt() == true) //I have encrypt last op
  {
    if (this->value_CMS == 0)
    {
      return 0;
    }
    return DIM_ARRAY;
  }
  return DIM_ARRAY - this->value_CMS;
}

uint8_t* aes256::get_payload()
{
  return this->payload;
}

int64_t aes256::get_length_payload()
{
  return this->length_payload;
}

uint8_t* aes256::get_final_block()
{
  if (this->value_CMS == 0)// || this->i_have_decrypt == true)
  {
    return nullptr;
  }
  return this->final_pad.array_form.data();
}

bool aes256::have_i_encrypt()
{
  return this->i_have_encrypt;
}

bool aes256::have_i_decrypt()
{
  return this->i_have_decrypt;
}

int32_t aes256::get_value_CMS()
{
  return this->value_CMS;
}
