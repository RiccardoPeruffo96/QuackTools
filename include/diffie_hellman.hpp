/**
 * @author Peruffo Riccardo, RP96, riccardoperuffo96@gmail.com, github.com/RiccardoPeruffo96
 * @date 2022
 * @file diffie_hellman.hpp
 * @brief This file contains a namespace who define a class
 * and method to generate a random 128 or 256 bits keys;<br>
 * With this generation you'll obtain a private and public keys
 * If you exchange the public key with others people you can
 * generate a common equal third-key to encrypt or decrypt files
 * @version 0.0.1 alpha [exam]
 */

#ifndef DIFFIEHELLMAN_RP96_HPP_
#define DIFFIEHELLMAN_RP96_HPP_

#include <algorithm> //std::reverse
#include <sstream> //std::string, std::dec, std::hex, std::stringstream
#include <random> //std::random_device, std::ranlux48, std::uniform_int_distribution

/**
 * @brief this namespace contains an interface to
 * using diffie_hellman protocol to create a secret
 * key only with an half key exchange
 */
namespace _diffie_hellman_rp96_
{
  /**
   * @brief this class contains a declaration
   * of the method used to calculate
   * diffie_hellman protocol to create a secret
   * key only with an half key exchange
   */
  class diffie_hellman
  {
  public:
    /**
     * @brief gives empty values to the private vars:
     * std::string privkey_str, std::string pubkey_str and std::string sharedkey_str
     */
    diffie_hellman() noexcept(true);

    /**
     * @brief to create second half key store the string privkey_str
     * and pubkey_str plus gives empty value to the private vars std::string sharedkey_str
     * @param privkey_stored_str value that will be padded in privkey_str
     * @param pubkey_received_str value that will be padded in pubkey_str
     */
    diffie_hellman(std::string privkey_stored_str, std::string pubkey_received_str) noexcept(true);

    //rule of five
    ///copy constructors
    diffie_hellman(const diffie_hellman& b64) noexcept(true);

    ///copy assignment operator
    diffie_hellman& operator=(const diffie_hellman& b64) noexcept(true);

    ///move constructor
    diffie_hellman(diffie_hellman&& b64) noexcept(true);

    ///move assignment operator
    diffie_hellman& operator=(diffie_hellman&& b64) noexcept(true);

    /**
     * @brief default destructor
     */
    ~diffie_hellman() noexcept(true) = default;

    /**
     * @brief generate a string with size == 32 hex char, that is a 128 bits half-key with DiffieHellman algorithm: the secret half key will be stored in privkey_str, the public half key will be stored in pubkey_str
     */
    void halfkey_generator_128bits();

    /**
     * @brief generate a string with size == 32 hex char, that is a 128bits key, with DiffieHellman algorithm: the secret need to be stored in privkey_str, the friend's public half key need to be stored in pubkey_str
     */
    void key_generator_from_halfkey_128bits();

    /**
     * @brief generate a string with size == 64 hex char, that is a 256 bits half-key with DiffieHellman algorithm: the secret half key will be stored in privkey_str, the public half key will be stored in pubkey_str
     */
    void halfkey_generator_256bits();

    /**
     * @brief generate a string with size == 64 hex char, that is a 256bits key, with DiffieHellman algorithm: the secret need to be stored in privkey_str, the friend's public half key need to be stored in pubkey_str
     */
    void key_generator_from_halfkey_256bits();

    /**
     * @brief to obtain the string sharedkey_str
     * @return this->sharedkey_str
     */
    std::string get_shared_string() { return this->sharedkey_str; }

    /**
     * @brief to obtain the string privkey_str
     * @return this->privkey_str
     */
    std::string get_private_string() { return this->privkey_str; }

    /**
     * @brief to obtain the string pubkey_str
     * @return this->pubkey_str
     */
    std::string get_public_string() { return this->pubkey_str; }

  private:
    const uint32_t Q = 0xFFFFFFFB; ///< largest prime number with 32 bit
    const uint32_t ALPHA = 0x2; ///< coprime integers with Q, so gcd(q, alpha) = 1
    //const int32_t ZERO = 0; //< auxiliary constants vars
    const int32_t CYCLES_128BITS = 4; ///< loop necessary to generate a 128bit random number
    const int32_t CYCLES_256BITS = 8; ///< loop necessary to generate a 256bit random number

    //auxiliary strings (std::string)
    std::string privkey_str; ///< private key generated and to store safety
    std::string pubkey_str; ///< public key generated and to exchange with others pubkey_str
    std::string sharedkey_str; ///< secret key calculated and shared only with other pubkey_str's owner

    /**
     * @brief compute (Y ^ X) % q with recursion, complexity: O(log(X)) with tail recursion
     * @param Y - base
     * @param X - exponent
     * @param std::string auxiliary - aux string used by tail recursion
     * @return (Y ^ X) % q
     */
    uint32_t calc_exp_mod(uint32_t Y, uint32_t X, std::string auxiliary);

    /**
     * @brief extract a uint32_t from string: every string's char it's a hex number and input.length() mod 8 == 0.
     * @param input - input.length() mod 8 == 0. every char it's a hex number
     * @param index - offset starting to extract the number
     * @return number between [0..2^32-1] extracted starting from input[index * 8] and input[(index * 8) + 8]
     */
    uint32_t uint32bit_from_string(std::string input, uint32_t index);

    /**
     * @brief create a random number smallest then Q
     * @return a random number between [1..(this->q - 1)]
     */
    uint32_t create_u32_rand_value();

    /**
     * @brief create strings privkey_str and pubkey_str with random value
     */
    void hex32bit_append_to_string(std::string& str, uint32_t param);

   /**
     * @brief read strings privkey_str and pubkey_str and create sharedkey_str
     */
    void create_uint32_n_store();

   /**
     * @brief read strings privkey_str and pubkey_str and create sharedkey_str
     * @param index offset to create
     */
    void create_uint32_n_store(int32_t index);


  };
}

#endif //DIFFIEHELLMAN_RP96_HPP_
