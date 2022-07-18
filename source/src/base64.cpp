/**
 * @author Peruffo Riccardo, RP96, riccardoperuffo96@gmail.com, github.com/RiccardoPeruffo96
 * @date 30th APR 2022
 * @file base64.cpp
 * @brief this file contains the definitions of base64.hpp's declarations
 * @version 0.0.1 alpha [exam]
 */

#include "base64.hpp"

using namespace _base64_rp96_;

uint8_t* base64::base64_encode(uint8_t* input_string, int64_t length_string, int64_t mode)
{
	assert(TRANSLATER_RFC4648_STD[62] == '+' && TRANSLATER_RFC4648_STD[63] == '/');
  assert(TRANSLATER_RFC4648_URL[62] == '-' && TRANSLATER_RFC4648_URL[63] == '/');
  assert(TRANSLATER_RFC3501_STD[62] == '+' && TRANSLATER_RFC3501_STD[63] == ',');

	if (input_string == nullptr || length_string < 1)
	{
		return nullptr;
	}

	const char* const& translater__ = ((mode == RFC4648_STD) ?
	  (TRANSLATER_RFC4648_STD) :
		((mode == RFC4648_URL) ?
		  (TRANSLATER_RFC4648_URL) :
			(TRANSLATER_RFC3501_STD))
	);

	int64_t b64_length = _base64_rp96_::base64::base64_enc_size(length_string);
	uint8_t* b64 = new uint8_t[b64_length];
	int64_t counter = 0;

	b64[0] = static_cast<uint8_t>(translater__[input_string[0] >> 2]);
	b64[++counter] = ((input_string[0] & 0b00000011) << 4);

	for (int64_t i = 1; i < length_string; ++i)
	{
		switch (i % 3)
		{
		case 1:
			b64[counter] = static_cast<uint8_t>(translater__[((input_string[i] & 0b11110000) >> 4) | b64[counter]]);
			b64[++counter] = ((input_string[i] & 0b00001111) << 2);
			break;
		case 2:
			b64[counter] = static_cast<uint8_t>(translater__[((input_string[i] & 0b11000000) >> 6) | b64[counter]]);
			b64[++counter] = static_cast<uint8_t>(translater__[input_string[i] & 0b00111111]);
			break;
		default:
			b64[++counter] = static_cast<uint8_t>(translater__[input_string[i] >> 2]);
			b64[++counter] = ((input_string[i] & 0b00000011) << 4);
			break;
		}
	}

	if (length_string % 3 != 0)
	{
		b64[counter] = static_cast<uint8_t>(translater__[static_cast<uint64_t>(b64[counter])]);
		if(length_string % 3 == 1)
		{
			b64[++counter] = static_cast<uint8_t>('=');
		}
		b64[++counter] = static_cast<uint8_t>('=');
	}

	return b64;
}

uint8_t* base64::base64_decode(uint8_t* input_string, int64_t length_string)
{
	if (input_string == nullptr || length_string < 1)
	{
		return nullptr;
	}

	int64_t output_lenght = base64_dec_size(input_string, length_string);
	if (output_lenght < 1)
	{
		return nullptr;
	}

	uint8_t* output = new uint8_t[output_lenght];

	int64_t counter = -1;
	int64_t i = -1;
	while(i < output_lenght - 3)
	{
		int8_t var[4];
		var[0] = conversion__(input_string[++counter]);
		var[1] = conversion__(input_string[++counter]);
		var[2] = conversion__(input_string[++counter]);
		var[3] = conversion__(input_string[++counter]);

		if (var[0] < 0 || var[1] < 0 || var[2] < 0 || var[3] < 0)
		{
			/*std::cout << "0: " << (int) input_string[counter - 3] << " - " << (int) var[0] << "\n";
			std::cout << "1: " << (int) input_string[counter - 2] << " - " << (int) var[1] << "\n";
			std::cout << "2: " << (int) input_string[counter - 1] << " - " << (int) var[2] << "\n";
			std::cout << "3: " << (int) input_string[counter - 0] << " - " << (int) var[3] << "\n";*/
			delete[] output;
			output = nullptr;
			return nullptr;
		}

		output[++i] = static_cast<uint8_t>(((var[0] & 0b00111111) << 2) | ((var[1] & 0b00110000) >> 4));
		output[++i] = static_cast<uint8_t>(((var[1] & 0b00001111) << 4) | ((var[2] & 0b00111100) >> 2));
		output[++i] = static_cast<uint8_t>(((var[2] & 0b00000011) << 6) | (var[3] & 0b00111111));
	}

	int8_t var[4];
	var[0] = conversion__(input_string[++counter]);
	var[1] = conversion__(input_string[++counter]);
	if (var[0] < 0 || var[1] < 0)
	{
		delete[] output;
		output = nullptr;
		return nullptr;
	}
	var[2] = conversion__(input_string[++counter]);
	var[3] = conversion__(input_string[++counter]);
	output[++i] = static_cast<uint8_t>(((var[0] & 0b00111111) << 2) | ((var[1] & 0b00110000) >> 4));
	if (var[2] < 0)
	{
		return output;
	}
	output[++i] = static_cast<uint8_t>(((var[1] & 0b00001111) << 4) | ((var[2] & 0b00111100) >> 2));
	if (var[3] < 0)
	{
		return output;
	}
	output[++i] = static_cast<uint8_t>(((var[2] & 0b00000011) << 6) | (var[3] & 0b00111111));
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
