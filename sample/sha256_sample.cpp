/**
 * @author Peruffo Riccardo, RP96, riccardoperuffo96@gmail.com, github.com/RiccardoPeruffo96
 * @date 2022
 * @file sha256_sample.cpp
 */

#include "sha256.hpp"

int main()
{
  //I create an array with something that i want to calc
  std::string input("Sample 4 using sha256 with QuackTools!");
  //with this reference i have a pointer to work with
  char*& inp_char = input.c_str();

  //call the object
  _sha256_rp96_::sha256 hash = _sha256_rp96_::sha256(inp_char, input.length());
  //calc hash
  hash.compute_hash();

  //show output
  std::cout << "The sha256 of the follow string:\n" << input << "\nis:\n" << hash.get_SHA256_string() << "\n";

  return 0;
}
