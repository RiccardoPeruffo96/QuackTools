#ifndef TESTS_RP96_HPP_
#define TESTS_RP96_HPP_

#include <vector>

//#include <catch2/catch_test_macros.hpp>

#ifndef CATCH_CONFIG_MAIN
  #define CATCH_CONFIG_MAIN
#endif
#include "catch.hpp"

#include "aes256.hpp"
#include "base64.hpp"
#include "diffie_hellman.hpp"
#include "sha256.hpp"
#include "steganography.hpp"

namespace _tests_header_rp96_
{
  bool tests_sha256();
  bool tests_aes256();
  bool tests_base64();
  bool tests_diffie_hellman();
  bool tests_steganography();
} //_tests_header_rp96_

#endif //TESTS_RP96_HPP_
