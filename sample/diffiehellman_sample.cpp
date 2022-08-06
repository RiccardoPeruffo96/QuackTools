/**
 * @author Peruffo Riccardo, RP96, riccardoperuffo96@gmail.com, github.com/RiccardoPeruffo96
 * @date 2022
 * @file diffiehellman_sample.cpp
 */

#include <iostream>

#include "diffie_hellman.hpp"

int main()
{
  //I want to create a 256bits key that two users can
  //generate in local
  //the two users need to exchange an "half-key"
  //that a third-user can read without generate
  //any security leak

  //PART 1:
  //TEST USER A-------------------
  //create df object
  _diffie_hellman_rp96_::diffie_hellman userA_run1 = _diffie_hellman_rp96_::diffie_hellman();
  //calculate private and public key
  userA_run1.halfkey_generator_256bits();

  //now i'm waiting until someone
  //else gives his "public key"
  //so i start with USER B

  //TEST USER B-------------------
  //create df object
  _diffie_hellman_rp96_::diffie_hellman userB_run1 = _diffie_hellman_rp96_::diffie_hellman();
  //calculate private and public key
  userB_run1.halfkey_generator_256bits();

  //now i have x2 public keys to exchange each others

  //to show the keys:
  //std::cout << "USER A PRIVATE KEY TO HIDE: " << userA_run1.get_private_string() << "\n";
  //std::cout << "USER A PUBLIC KEY TO SHARE WITH USER B: " << userA_run1.get_public_string() << "\n\n";
  //std::cout << "USER B PRIVATE KEY TO HIDE: " << userB_run1.get_private_string() << "\n";
  //std::cout << "USER B PUBLIC KEY TO SHARE WITH USER A: " << userB_run1.get_public_string() << "\n\n";

  //PART 2:
  //TEST USER A-------------------
  //create another df object
  _diffie_hellman_rp96_::diffie_hellman userA_run2 = _diffie_hellman_rp96_::diffie_hellman(
    userA_run1.get_private_string(), //private key calculate previously
    userB_run1.get_public_string() //public key obtained from USER B
  );
  //then calculate the shared key
  userA_run2.key_generator_from_halfkey_256bits();

  //TEST USER B-------------------
  //create another df object
  _diffie_hellman_rp96_::diffie_hellman userB_run2 = _diffie_hellman_rp96_::diffie_hellman(
    userB_run1.get_private_string(), //private key calculate previously
    userA_run1.get_public_string() //public key obtained from USER B
  );
  //then calculate the shared key
  userB_run2.key_generator_from_halfkey_256bits();

  //final check
  std::cout << ((userA_run2.get_shared_string().compare(userB_run2.get_shared_string()) != 0) ?
  ("The shared key are equals.\n") : ("The shared key are not equals.\n"));

  //to show the final calculated key, ready for encrypting file
  //std::cout << "USER A FINAL KEY: " << userA_run2.get_shared_string() << "\n";
  //std::cout << "USER B FINAL KEY: " << userB_run2.get_shared_string() << "\n";

  return 0;
}
