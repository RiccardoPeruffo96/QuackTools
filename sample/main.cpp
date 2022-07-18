#include "main_header.hpp"

int main()
{
	int32_t control = 0;

	//_log_operations_rp96_::log_operations logs = _log_operations_rp96_::log_operations::get_instance(); //singleton
	__LOG_HEADER(__LOG_GET) << "start program\n";

	std::cout << "Welcome!\n";

	std::ifstream config_file("config.json");

	if(config_file.is_open() == false)
	{
		//std::cout << "config.json doesnt found in exe path\nforced exit\n";
		__LOG_HEADER(__LOG_GET) << "config.json doesnt found in exe path\nforced exit\n";
		return 0;
	}

	Json::Value root_json;
	config_file >> root_json;
	config_file.close();

	std::filesystem::current_path("..");

	while (control == 0)
	{
		switch (menu())
		{
		case 1: //I want to hide datas in an image
		{
			//we read the json param
			Json::Value file_to_hide = root_json["hide"]["file_input"]; //read the value at hide->file_input
			Json::Value img_that_hide = root_json["hide"]["img_input"]; //read the value at hide->img_input

			_file_operations_rp96_::file_operations input_file = _file_operations_rp96_::file_operations(file_to_hide["name"].asString()); //get value as std::string

			if(input_file.get_data_file() == nullptr) //file not found
			{
				std::cout << "error with hide: read input file\n";
				break;
			}
			if (img_that_hide["name"].asString().compare("") == 0) //img not found
			{
				std::cout << "error with hide: read img file\n";
				break;
			}

			_steganography_rp96_::steganography steganography = _steganography_rp96_::steganography(); //constructor
			steganography.hide_file(input_file.get_data_file(), input_file.get_size_file(), img_that_hide["name"].asString()); //update the img with the file

			break;
		}
		case 2: //I want to seek datas from an image
		{
			//we read the json param
			Json::Value file_to_seek = root_json["seek"]["file_output"]; //read the value at seek->file_output
			Json::Value img_where_seek = root_json["seek"]["img_input"]; //read the value at seek->img_input

			//_file_operations_rp96_::file_operations output_file = _file_operations_rp96_::file_operations(file_to_seek["name"].asString()); //get value as std::string
			//
			//if(output_file.get_data_file() != nullptr) //file found
			//{
			//	std::cout << "error with seek: the output file already exists\n";
			//	break;
			//}
			if (img_where_seek["name"].asString().compare("") == 0) //img not found
			{
				std::cout << "error with seek: read img file\n";
				break;
			}

			_steganography_rp96_::steganography steganography = _steganography_rp96_::steganography(); //constructor
			//std::cout << "name: " << img_where_seek["name"].asString() << "\n";
			int64_t file_output_length = 0;
			uint8_t* pointer_data = steganography.seek_file(img_where_seek["name"].asString(), file_output_length); //obtain the hide data

			//std::ofstream myfile;
		  //myfile.open("payload2.txt");
			//myfile << "data:\n";
			//for(int64_t i = 0; i < file_output_length; ++i)
			//{
			//	myfile << "D" << i << ": " << std::bitset<8>{pointer_data[i]} << "\n";
			//}
			//myfile.close();

			//I create a list of a number between 1 and N, the file will contains data
			std::list<_file_operations_rp96_::file_pointer> list_file;
		  list_file.push_back(_file_operations_rp96_::file_pointer(pointer_data, file_output_length, false));

			if(_file_operations_rp96_::file_operations::save_third_file(file_to_seek["name"].asString(), list_file, std::ofstream::trunc) == -1) //update the empty file
			{
				std::cout << "error with save_third_file\n";
			}
			else
			{
				std::cout << "file " << file_to_seek["name"].asString() << " are update or create\n";
			}

			delete[] pointer_data;
			pointer_data = nullptr;

			break;
		}
		case 3: //I want to encrypt a file with AES-256
		{
		  //we read json param
		  Json::Value file_to_encrypt = root_json["encrypt"]["file_input"]; //read the value at encrypt->file_input

		  //then we read the file
		  _file_operations_rp96_::file_operations input_file = _file_operations_rp96_::file_operations(file_to_encrypt["name"].asString()); //get value as std::string

		  if(input_file.get_data_file() == nullptr) //file not found
			{
				std::cout << "error with hide: read input file\n";
				break;
			}

		  //we ask for the key
		  std::string key;
		  do
		  {
		    std::cout << "insert the 256-bits key:\n";
		  } while (!string_is_hex_key(key, BITS256));

		  //start to encrypt
		  _aes256_rp96_::aes256 aes256 = _aes256_rp96_::aes256(input_file.get_data_file(), input_file.get_size_file(), key);
		  aes256.encrypt();

		  //obtain results
		  std::list<_file_operations_rp96_::file_pointer> list_file;
		  list_file.push_back(_file_operations_rp96_::file_pointer(aes256.get_payload(), aes256.get_length_payload()));
		  list_file.push_back(_file_operations_rp96_::file_pointer(aes256.get_final_block(), aes256.get_final_block_size()));

		  //save results in the same file
			if(input_file.save_this_file(list_file, false, std::ofstream::trunc) == -1) //update the empty file
			{
				std::cout << "error with save_this_file\n";
			}
			else
			{
				std::cout << "file " << input_file.get_file_name() << " encrypted\n";
			}

		  break;
		}
		case 4: //I want to decrypt a file with AES-256
		{
		  //we read json param
		  Json::Value file_to_decrypt = root_json["decrypt"]["file_input"]; //read the value at decrypt->file_input

		  //then we read the file
		  _file_operations_rp96_::file_operations output_file = _file_operations_rp96_::file_operations(file_to_decrypt["name"].asString()); //get value as std::string

		  if(output_file.get_data_file() != nullptr) //file found
			{
				std::cout << "error with seek: the output file already exists\n";
				break;
			}

			//we ask for the key
		  std::string key;
		  do
		  {
		    std::cout << "insert the 256-bits key:\n";
		  } while (!string_is_hex_key(key, BITS256));

		  //start to encrypt
		  _aes256_rp96_::aes256 aes256 = _aes256_rp96_::aes256(output_file.get_data_file(), output_file.get_size_file(), key);
		  //aes256.decrypt();

		  //obtain results
		  std::list<_file_operations_rp96_::file_pointer> list_file;
			list_file.push_back(_file_operations_rp96_::file_pointer(aes256.get_payload(), aes256.get_length_payload()));
		  list_file.push_back(_file_operations_rp96_::file_pointer(aes256.get_final_block(), aes256.get_final_block_size()));

		  //save results in the same file
			if(output_file.save_this_file(list_file, false, std::ofstream::trunc) == -1) //update the empty file
			{
				std::cout << "error with save_this_file\n";
			}
			else
			{
				std::cout << "file " << output_file.get_file_name() << " encrypted\n";
			}

		  break;
		}
		case 5: //encrypt a file with AES-256 then hide it in an image
		{
		  //we read json param
		  Json::Value file_to_encrypt = root_json["hide"]["file_input"]; //read the value at encrypt->file_input
			Json::Value img_that_hide = root_json["hide"]["img_input"]; //read the value at seek->img_input

		  //then we read the file
		  _file_operations_rp96_::file_operations input_file = _file_operations_rp96_::file_operations(file_to_encrypt["name"].asString()); //get value as std::string

		  if(input_file.get_data_file() == nullptr) //file not found
			{
				std::cout << "error with hide: read input file\n";
				break;
			}

			//we ask for the key
		  std::string key;
		  do
		  {
		    std::cout << "insert the 256-bits key:\n";
		  } while (string_is_hex_key(key, BITS256) == false);

		  //start to encrypt
		  _aes256_rp96_::aes256 aes256 = _aes256_rp96_::aes256(input_file.get_data_file(), input_file.get_size_file(), key);
		  aes256.encrypt();

		  //obtain results
		  std::list<_file_operations_rp96_::file_pointer> list_file;
			list_file.push_back(_file_operations_rp96_::file_pointer(aes256.get_payload(), aes256.get_length_payload()));
		  list_file.push_back(_file_operations_rp96_::file_pointer(aes256.get_final_block(), aes256.get_final_block_size()));

		  //merge pointer then hide
			//save results in the same file
			if(input_file.save_this_file(list_file, true, std::ofstream::trunc) == -1) //update the empty file
			{
				std::cout << "error with save_this_file\n";
			}

			_steganography_rp96_::steganography steganography = _steganography_rp96_::steganography(); //constructor
			steganography.hide_file(input_file.get_data_file(), input_file.get_size_file(), img_that_hide["name"].asString()); //update the img with the file

		  break;
		}
		case 6: //seek datas from an image then decrypt it with AES-256
		{
		  //we read the json param
			Json::Value file_to_seek = root_json["seek"]["file_output"]; //read the value at seek->file_output
			Json::Value img_where_seek = root_json["seek"]["img_input"]; //read the value at seek->img_input

			//_file_operations_rp96_::file_operations output_file = _file_operations_rp96_::file_operations(file_to_seek["name"].asString()); //get value as std::string

      //if(output_file.get_data_file() != nullptr) //file found
			//{
			//	std::cout << "error with seek: the output file already exists\n";
			//	break;
			//}
			if (img_where_seek["name"].asString().compare("") == 0) //img not found
			{
				std::cout << "error with seek: read img file\n";
				break;
			}

			//we ask for the key
		  std::string key;
		  do
		  {
		    std::cout << "insert the 256-bits key:\n";
		  } while (!string_is_hex_key(key, BITS256));

			_steganography_rp96_::steganography steganography = _steganography_rp96_::steganography(); //constructor

			int64_t file_output_length = 0;
			uint8_t* pointer_data = steganography.seek_file(img_where_seek.asString(), file_output_length); //obtein the hide data

		  //start to decrypt
		  _aes256_rp96_::aes256 aes256 = _aes256_rp96_::aes256(pointer_data, file_output_length, key);
		  //aes256.decrypt();

			//obtain results
		  std::list<_file_operations_rp96_::file_pointer> list_file;
			list_file.push_back(_file_operations_rp96_::file_pointer(aes256.get_payload(), aes256.get_length_payload()));
		  list_file.push_back(_file_operations_rp96_::file_pointer(aes256.get_final_block(), aes256.get_final_block_size()));

		  //save the file without load it into memory
			if(_file_operations_rp96_::file_operations::save_third_file(file_to_seek["name"].asString(), list_file, std::ofstream::trunc) == -1) //update the empty file
			{
				std::cout << "error with save_third_file\n";
			}
			else
			{
				std::cout << "file " << file_to_seek["name"].asString() << " exists\n";
			}

		  break;
		}
		case 7: //create a new 256bits half-key with Diffie-Hellman
		{
		  _diffie_hellman_rp96_::diffie_hellman diffie_hellman = _diffie_hellman_rp96_::diffie_hellman();

		  diffie_hellman.halfkey_generator_256bits();

		  std::cout << "the 256-bits secret half-key generated is:\n" << diffie_hellman.get_private_string() << "\n";
		  std::cout << "the 256-bits public half-key generated is:\n" << diffie_hellman.get_public_string() << "\n";
		  break;
		}
		case 8: //create a new 256bits full-key from half-key
		{
		  std::string secret_key;
		  do
		  {
		    std::cout << "insert the secret key:\n";
		  } while (!string_is_hex_key(secret_key, BITS256));
		  std::string public_key;
		  do
		  {
		    std::cout << "insert the public key:\n";
		  } while (!string_is_hex_key(public_key, BITS256));

		  _diffie_hellman_rp96_::diffie_hellman diffie_hellman = _diffie_hellman_rp96_::diffie_hellman(secret_key, public_key);

		  diffie_hellman.key_generator_from_halfkey_256bits();

		  std::cout << "the 256-bits key generated is:\n" << diffie_hellman.get_shared_string() << "\n";
		  break;
		}
		case 9: //calculate file's sha256
		{
			Json::Value file_to_calc_hash = root_json["hash"]["file_input"];
			_file_operations_rp96_::file_operations input_file = _file_operations_rp96_::file_operations(file_to_calc_hash["name"].asString());

			_sha256_rp96_::sha256 hash = _sha256_rp96_::sha256(reinterpret_cast<char*>(input_file.get_data_file()), input_file.get_size_file());
			hash.compute_hash();
			std::cout << file_to_calc_hash["name"].asString() << "'s SHA256:\n" << hash.get_SHA256_string() << "\n";
			break;
		}
    default:
    {
      std::cout << "\nexit...\n";
      ++control;
      break;
    }
    }
  }

  std::cout << "\nGoodbye!\n";
	__LOG_HEADER(__LOG_GET) << "exit correctly.\n";
	
	return 0;
}

int32_t menu()
{
	std::string input_text;

	int32_t input_menu = 0;

	do
	{
		std::cout <<
			"What do you want to do now:\n\t"
			"0) exit the program\n\t"
			"1) hide datas in an image\n\t"
			"2) seek datas from an image\n\t"
			"3) encrypt a file with AES-256\n\t"
			"4) decrypt a file with AES-256\n\t"
			"5) encrypt a file with AES-256 then hide it in an image\n\t"
			"6) seek datas from an image then decrypt it with AES-256\n\t"
			"7) create a new 256bits half-key with Diffie-Hellman\n\t"
			"8) create a new 256bits full-key from half-key\n\t"
			"9) calculate file's sha256\n\t"
			"Choose a value between 0-9 and press start:\n> ";

		std::cin >> input_text;

		input_menu = input_text.at(0) - '0';

		if (input_menu < 0 || input_menu > 9)
		{
			std::cout << "Input doesn't accept: retry\n";
		}

	} while (input_menu < 0 || input_menu > 9);  //while (inputMenu < 1 || inputMenu > 15);

	return input_menu;
}

bool string_is_hex_key(std::string& input_key, uint32_t size_key)
{
  input_key.clear();
  std::cin >> input_key;
  if(input_key.length() != size_key)
  {
    std::cout << "input length is not equals to " << size_key << "\n";
    return false;
  }
  if (!std::all_of(input_key.begin(), input_key.end(), ::isxdigit))
  {
    std::cout << input_key << " doesn't contains only hexadecimal digits\n";
    return false;
  }
  return true;
}
