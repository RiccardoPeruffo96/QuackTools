/**
 * @author Peruffo Riccardo, RP96, riccardoperuffo96@gmail.com, github.com/RiccardoPeruffo96
 * @date 30th APR 2022
 * @file base64.cpp
 * @brief this file contains the definitions of base64.hpp's declarations
 * @version 0.0.1 alpha [exam]
 */

#include "base64.hpp"

using namespace _base64_rp96_;

uint8_t* base64::base64_encode(uint8_t* input_string, int64_t length_string, int64_t& output_lenght, int64_t mode)
{
	//check if length and pointer is valid
	if (input_string == nullptr || length_string < 1)
	{
		return nullptr;
	}

  //choose the standard to work with
	const char* const& translater__ = ((mode == RFC4648_STD) ?
	  (TRANSLATER_RFC4648_STD) :
		((mode == RFC4648_URL) ?
		  (TRANSLATER_RFC4648_URL) :
			(TRANSLATER_RFC3501_STD))
	);

	//obtain the size of output
	output_lenght = _base64_rp96_::base64::base64_enc_size(length_string);
	uint8_t* output = new uint8_t[output_lenght];

  //from a string, i need to take every 3 letters (24 bits) and
	//convert them into x4 letters: every new letter has only 6 bits
	//then i have x4 letter with value 0..63 and an char* (TRANSLATER_RFC4648_STD)
	//about exactly 64 letters.
	//every x4 letter is an index and the final value is, for each letter,
	//output[counter] = translater__[value];
	int64_t counter = -1;
	for (int64_t i = 0; i < length_string; ++i)
	{
		//i work always in the same value with every triplet of numbers:
		//the logic start from default
		switch (i % 3)
		{
		case 1:
      //take the 2 bits that i know are stored and append the x4 most significant bits
			//x4 less significant bits become x4 most significant bits for next value
			output[counter] = static_cast<uint8_t>(translater__[((input_string[i] & 0b11110000) >> 4) | output[counter]]);
			output[++counter] = ((input_string[i] & 0b00001111) << 2);
			break;
		case 2:
		  //take x2 missing bits and append to x4 most significant bits about case 1
      //then i take last 6 bits and i finish
			output[counter] = static_cast<uint8_t>(translater__[((input_string[i] & 0b11000000) >> 6) | output[counter]]);
			output[++counter] = static_cast<uint8_t>(translater__[input_string[i] & 0b00111111]);
			break;
		default:
		  //I start so first i must take x6 most significant bits and become a single value
			//other is stored in another var like x2 most significant bits
			output[++counter] = static_cast<uint8_t>(translater__[input_string[i] >> 2]);
			output[++counter] = ((input_string[i] & 0b00000011) << 4);
			break;
		}
	}

  //now i need to understand if padding ('=' chars) is necessary
	if (length_string % 3 != 0)
	{
		output[counter] = static_cast<uint8_t>(translater__[static_cast<uint64_t>(output[counter])]);
		if(length_string % 3 == 1)
		{
			output[++counter] = static_cast<uint8_t>('=');
		}
		output[++counter] = static_cast<uint8_t>('=');
	}

	return output;
}

uint8_t* base64::base64_decode(uint8_t* input_string, int64_t length_string, int64_t& output_lenght)
{
	//i assume that the input follow RFC4648
	//but it works also with RFC3501
	//first of all check if input is valid
	if (input_string == nullptr || length_string < 1)
	{
		return nullptr;
	}

  //02. base64_dec_size return output_lenght correct value but
	//if input don't follow the correct base64 rule
	//(every value is an element of TRANSLATER_RFC4648_STD)
	//return an invalid numeric value
	//int64_t output_lenght = base64_dec_size(input_string, length_string);
	output_lenght = base64_dec_size(input_string, length_string);
	if (output_lenght < 1)
	{
		return nullptr;
	}

  //03. now i can create the output array
	uint8_t* output = new uint8_t[output_lenght];

  //04. i read every time x4 input letter then convert into x3 original message
	int64_t counter = -1; //counter about input_string's letters
	int64_t i = -1; //index about output[i]
	while(i < output_lenght - 3)
	{
		//store every single input letter
		//conversion__ convert the letter into numerical value
		//from TRANSLATER_RFC4648_STD to ascii table
		//for each input_string value
		//i after convert the value with conversion__
		//then i have the ascii value and save it
		int8_t var[4];
		var[0] = conversion__(input_string[++counter]);
		var[1] = conversion__(input_string[++counter]);
		var[2] = conversion__(input_string[++counter]);
		var[3] = conversion__(input_string[++counter]);

    //if something is -1 some letter are not convertible
		//this should never happen because base64_dec_size makes the check
		if (var[0] < 0 || var[1] < 0 || var[2] < 0 || var[3] < 0)
		{
			delete[] output;
			output = nullptr;
			return nullptr;
		}

    //i have 4 number and only number use only 6 bits
		//I need to transform they in 3 values:
    //image to concat the 6x4 bits like
		//first || second || third || fourth
		//then divide the 24 bits into 3 byte
		//output[0] || output[1] || output[2]
		output[++i] = concat01(var[0], var[1]);
		output[++i] = concat02(var[1], var[2]);
		output[++i] = concat03(var[2], var[3]);
	}

  //check if now i'm already done
  if(i == output_lenght - 1)
	{
		return output;
	}

  //now i make the same thing but i add more controls because I know that
	//it's not guaranteed I have a 6x4 bits: maybe I have padding or something different
	int8_t var[4];
	var[0] = conversion__(input_string[++counter]);
	var[1] = conversion__(input_string[++counter]);
	if (var[0] < 0 || var[1] < 0) //this should never happen
	{
		delete[] output;
		output = nullptr;
		return nullptr;
	}
	var[2] = conversion__(input_string[++counter]);
	var[3] = conversion__(input_string[++counter]);
	output[++i] = concat01(var[0], var[1]);
	//now everytime need to check 'i' var
	if (i == output_lenght - 1 || var[2] < 0)
	{
		return output;
	}
	//check 'i' var
	output[++i] = concat02(var[1], var[2]);
	if (i == output_lenght - 1 || var[3] < 0)
	{
		return output;
	}
  //now i are forced to be equals to output_lenght - 1
	output[++i] = concat03(var[2], var[3]);
	return output;
}

int64_t base64::base64_dec_size(uint8_t* input_string, int64_t length_string)
{
	//if input zero, return zero
	if (length_string == 0)
	{
		return 0;
	}
	//if first value it's a padding, the string has empty value
	if (input_string[0] == '=')
	{
		return 0;
	}
	//now i run from end to start:
	//i search everything different from '='
	//when i found it i can only accept 1 or 2 '=' for padding AND the length % 4 == 0
	for (int64_t ls = length_string - 1; ls >= 0; --ls)
	{
		if (input_string[ls] != '=')
		{
			int64_t mod_4 = ls % 4;

			switch (mod_4)
			{
			case 3:
				return (((ls + 1) / 4) * 3);
			case 2:
				return ((((ls + 2) / 4) * 3) - 1);
			case 1:
				return ((((ls + 3) / 4) * 3) - 2);
			default: //in this case i have #3 '=' for padding, it's an invalid configuration
				return -1;
			}
			//length_string += 4 - mod_4;
			//return (((length_string / 4) * 3) - mod_4);
		}
	}
	//else i have some error
	return -1;
}

constexpr char _base64_rp96_::conversion__(char input)
{
	if (input >= 'a')
	{
		if (input > 'z')
		{
			return -1;
		}
		return input - 71; //'a' - 71 == 26 -> translater__[26] == 'a';
	}
	if (input >= 'A')
	{
		if (input > 'Z')
		{
			return -1;
		}
		return input - 65; //'A' - 65 == 0 -> translater__[0] == 'A';
	}
	if (input >= '0')
	{
		if (input > '9')
		{
			return -1;
		}
		return input + 4; //'0' + 4 == 52 -> translater__[52] == '0';
	}
	if (input == '+' || input == '-')
	{
		return 62;
	}
	if (input == '/' || input == '_' || input == ',')
	{
		return 63;
	}
	if (input == '=')
	{
		return -2;
	}
	return -1;
}

constexpr uint8_t _base64_rp96_::concat01(int8_t v01, int8_t v02)
{
	return static_cast<uint8_t>(((v01 & 0b00111111) << 2) | ((v02 & 0b00110000) >> 4));
}

constexpr uint8_t _base64_rp96_::concat02(int8_t v01, int8_t v02)
{
	return static_cast<uint8_t>(((v01 & 0b00001111) << 4) | ((v02 & 0b00111100) >> 2));
}

constexpr uint8_t _base64_rp96_::concat03(int8_t v01, int8_t v02)
{
	return static_cast<uint8_t>(((v01 & 0b00000011) << 6) | (v02 & 0b00111111));
}
