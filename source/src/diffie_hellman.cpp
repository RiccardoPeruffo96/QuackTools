/**
 * @author Peruffo Riccardo, RP96, riccardoperuffo96@gmail.com, github.com/RiccardoPeruffo96
 * @date 30th APR 2022
 * @file diffie_hellman.cpp
 * @brief this file contains the definitions of diffie_hellman.hpp's declarations
 * @version 0.0.1 alpha [exam]
 */

#include "diffie_hellman.hpp"

using namespace _diffie_hellman_rp96_;

/**
 * @brief gives empty values to the private vars:
 * std::string privkey_str, std::string pubkey_str and std::string sharedkey_str
 */
diffie_hellman::diffie_hellman() noexcept(true):
  privkey_str{ std::string("") },
  pubkey_str{ std::string("") },
  sharedkey_str{ std::string("") }
{}

diffie_hellman::diffie_hellman(std::string privkey_stored_str, std::string pubkey_received_str) noexcept(true):
  sharedkey_str{ std::string("") }
{
  this->privkey_str = std::move(privkey_stored_str);
  this->pubkey_str = std::move(pubkey_received_str);
}

diffie_hellman::diffie_hellman(const diffie_hellman& b64) noexcept(true):
  privkey_str{ b64.privkey_str },
  pubkey_str{ b64.pubkey_str },
  sharedkey_str{ b64.sharedkey_str }
{}

diffie_hellman& diffie_hellman::operator=(const diffie_hellman& b64) noexcept(true)
{
  //copy assignment operator is different between copy constructors because first i need to clear the actual data
  //phase 1: clean + assign
  this->privkey_str = b64.privkey_str;
  this->pubkey_str = b64.privkey_str;
  this->sharedkey_str = b64.privkey_str;
  return *this;
}

diffie_hellman::diffie_hellman(diffie_hellman&& b64) noexcept(true)
{
  //phase 1: swap strings
  std::swap(this->privkey_str, b64.privkey_str);
  std::swap(this->pubkey_str, b64.pubkey_str);
  std::swap(this->sharedkey_str, b64.sharedkey_str);
  //phase 2: clean source
  b64.privkey_str.clear();
  b64.pubkey_str.clear();
  b64.sharedkey_str.clear();
}

diffie_hellman& diffie_hellman::operator=(diffie_hellman&& b64) noexcept(true)
{
  if (this != &b64)
  {
    return *this;
  }
  //phase 1: swap strings
  std::swap(this->privkey_str, b64.privkey_str);
  std::swap(this->pubkey_str, b64.pubkey_str);
  std::swap(this->sharedkey_str, b64.sharedkey_str);
  //phase 2: clean source
  b64.privkey_str.clear();
  b64.pubkey_str.clear();
  b64.sharedkey_str.clear();
  return *this;
}

uint32_t diffie_hellman::calc_exp_mod(uint32_t Y, uint32_t X, std::string auxiliary)
{
  if (X == 0)
  {
    uint64_t tmp = 1;

    //reverse string
    std::reverse(std::begin(auxiliary), std::end(auxiliary));

    //and makes operation
    for (uint64_t i = 0; i < static_cast<uint64_t>(auxiliary.length()); ++i)
    {
      tmp *= tmp;
      tmp %= this->Q;
      if (auxiliary[i] == '1')
      {
        tmp *= Y;
        tmp %= this->Q;
      }
    }
    return tmp;
  }
  if (X % 2 == 0)
  {
    auxiliary.append("0");
  }
  else
  {
    auxiliary.append("1");
  }
  return calc_exp_mod(Y, X / 2, auxiliary);
}

uint32_t diffie_hellman::uint32bit_from_string(std::string output_omega, uint32_t index)
{
  //const values
  static const char downcase = 'a' - 0xA;
  static const char uppercase = 'A' - 0xA;
  const int32_t NUMBER_CHAR_TO_READ = 8;

  //return value
  uint32_t tmp = 0;

  //read 8 hex char
  for (uint32_t i = index * NUMBER_CHAR_TO_READ; i < ((index * NUMBER_CHAR_TO_READ) + NUMBER_CHAR_TO_READ); ++i)
  {
    tmp <<= 4;

    //transform output_omega[i] char in numeric hex value
    //then append the hex value to tmp
    if (output_omega[i] >= 'a') //tmp >= 97
    {
      tmp |= static_cast<uint8_t>(output_omega[i] - downcase); //tmp - 'a' == 0 -> 0 + 0xA == 0xA
    }
    else if (output_omega[i] >= 'A') //tmp >= 65
    {
      tmp |= static_cast<uint8_t>(output_omega[i] - uppercase); //tmp - 'A' == 0 -> 0 + 0xA == 0xA
    }
    else
    {
      tmp |= static_cast<uint8_t>(output_omega[i] - '0'); //tmp -= 48
    }
  }
  return tmp;
}

uint32_t diffie_hellman::create_u32_rand_value()
{
  //(1) Select the seed
  std::random_device rnd_seed;
  //(2) Define the random engine <type of random engine> generator(seed)
  //Pseudo-random generation based on Lagged Fibonacci algorithm
  std::ranlux48 random_value(rnd_seed()); //very slow for multiple numbers, but unpredictable with state
  //(3) Define the distribution <type of distribution> distribution(range start, range end)
  std::uniform_int_distribution<uint32_t> distribution(1, this->Q-1);
  //(4) Produce the random number distribution(generator)
  return distribution(random_value);
}

void diffie_hellman::hex32bit_append_to_string(std::string& str, uint32_t param)
{
  const int64_t SIZE_OF_32BIT = 32;
  const int64_t BIT_FOR_HEX = 4;
  int64_t log2_plus1 = log2(param) + 1;
  str.append((SIZE_OF_32BIT - log2_plus1)/BIT_FOR_HEX, '0');

  std::stringstream string_str;
  string_str << std::hex << param << std::dec;
  str.append(string_str.str());
}

void diffie_hellman::halfkey_generator_128bits()
{
  //for every cycle I create a 32bit string
  for (int32_t i = 0; i < this->CYCLES_128BITS; ++i)
  {
    this->create_uint32_n_store();
  }
}

void diffie_hellman::key_generator_from_halfkey_128bits()
{
  for (int32_t i = 0; i < this->CYCLES_128BITS; ++i)
  {
    this->create_uint32_n_store(i);
  }
}

void diffie_hellman::halfkey_generator_256bits()
{
  //for every cycle I create a 32bit string
  for (int32_t i = 0; i < this->CYCLES_256BITS; ++i)
  {
    this->create_uint32_n_store();
  }
}

void diffie_hellman::key_generator_from_halfkey_256bits()
{
  for (int32_t i = 0; i < this->CYCLES_256BITS; ++i)
  {
    this->create_uint32_n_store(i);
  }
}

void diffie_hellman::create_uint32_n_store()
{
  //create a random Xa < q
  uint32_t Xa = this->create_u32_rand_value();
  std::string aux("");

  //I calc Ya
  uint32_t Ya = this->calc_exp_mod(this->ALPHA, Xa, aux);

  //I need to append Xa and Ya to this->privkey_str / this->pubkey_str
  //but if aren't length == 8 then I need to append 0's first
  this->hex32bit_append_to_string(this->privkey_str, Xa);
  this->hex32bit_append_to_string(this->pubkey_str, Ya);
}

void diffie_hellman::create_uint32_n_store(int32_t index)
{
  uint32_t Xa = 0;
  uint32_t Yb = 0;
  uint32_t Ka = 0;

  Yb = this->uint32bit_from_string(this->pubkey_str, index);
  Xa = this->uint32bit_from_string(this->privkey_str, index);

  std::string aux("");
  Ka = this->calc_exp_mod(Yb, Xa, aux);

  this->hex32bit_append_to_string(this->sharedkey_str, Ka);
}
