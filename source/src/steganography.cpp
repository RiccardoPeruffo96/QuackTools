/**
 * @author Peruffo Riccardo, RP96, riccardoperuffo96@gmail.com, github.com/RiccardoPeruffo96
 * @date 30th APR 2022
 * @file steganography.cpp
 * @brief this file contains the definitions of steganography.hpp's declarations
 * @version 0.0.1 alpha [exam]
 */

#include "steganography.hpp"

using namespace _steganography_rp96_;

void steganography::hide_file(uint8_t* bytes_data_file, int64_t bytes_data_length, const std::string& img_name_input, const std::string& file_name_input, std::string img_name_output)
{
	//#######################################
	//HIDE FILE:
	//1. save the entire name of the file and divide name - extension
	//2. check if I can save the name ("only" last 255 letters)
	//   choose last letters 'cause have extension
	//3. store the name length
	//4. open the image
	//   check if i have problems
	//5. check how space I need
	//6. check the img is too tiny
	//#######################################

  //1. save the entire name of the file and divide name - extension:
	//with std::filesystem::path I have yet everything
	std::filesystem::path filepath{file_name_input};
	std::string filename = filepath.filename();

	//2. check if I can save the name ("only" last 255 letters)
	//check if I can save the name ("only" last 255 letters)
	if(filename.length() > 0xFF)
	{
		//filename will contains last 255 name's letters
		filename = filename.substr(filename.length() - 0xFF);
	}

	//3. store the name length
  //after that i use 1 byte to store the filename length
	filename += static_cast<char>(filename.length());

  //4. open the image
	//i use OpenCV to doing this
	cv::Mat img = cv::imread(img_name_input, cv::IMREAD_COLOR);

  //check if i have problems
	if(img.empty())
	{
		std::cout << "something wrong with imread\n";
		return;
	}

  //5. check how space I need inside the img:
	//> size of file to hide in bytes;
	//> there is the filename, where every letter is a char(=byte)
	//  note: the last filename's char is not a letter
	//  but the value filename.length()
	int64_t payload_to_hide = bytes_data_length + filename.length();
	//> miss 64bits, contains size of payload (data + name);
	int64_t size_to_hide = sizeof(int64_t) + payload_to_hide;

	//this little array will be contains only the bits about payload_to_hide
	//bits_of_payload[0] = 7th most relevant size_to_hide byte
	//bits_of_payload[7] = 7th less relevant size_to_hide byte
	std::array<uint8_t, sizeof(int64_t)> bits_of_payload;

	//store the size of bytes_data_file
	//every turns safe 8 bits from payload_to_hide
	//#pragma unroll(sizeof(int64_t))
	for(uint64_t i = 0; i < sizeof(int64_t); ++i)
	{
		//take all 64 bit "static_cast<uint64_t>(payload_to_hide)"
		//shift to right 7 times, 8 bits for each times
		//then with "static_cast<uint8_t>" i take only first byte
		//about this 64 bits
		bits_of_payload[i] = static_cast<uint8_t>(static_cast<uint64_t>(payload_to_hide)
		                     >> ( ( (sizeof(uint64_t)-1) - i ) * BITS_IN_ONE_BYTE));
	}

  //now final check:
	//calc how many bits can use and ow many bits I need
	int64_t img_space_usable = img.rows * img.cols * CHANNELS_BY_COLOR;
	int64_t img_space_needed = size_to_hide * BITS_IN_ONE_BYTE;

	//at the end if needed is higher than usable i cannot do nothing
	if (img_space_needed > img_space_usable)
	{
		std::cout << "the img it's too little for the input\n";
		return;
	}

	//start to hide bits
	//this var help to calc how many bits I need to read
	//about a single var.
	//EXAMPLE: POSITION_START = 0d2 -> 0b01101100
	//----------------------------------------^
	//i need to read the third bit, count start from zero.
	uint32_t position_actual = POSITION_START;
	//this var count the position about the byte to read
	uint32_t index_data = 0;
	//counter about first 63 bits
	uint32_t counter_index = 0;

  //aux var to work with matrix coordinates about image
	uint64_t state_rows = 0, state_cols = 0;
	//img_space_usable > 64 bits always
	//this take every data of length file
	//first double for loop add first 64 bits about length payload

	//Please note: in almost all cases the input img has more than 63 column
	//so this little optimization is necessary
	if(static_cast<uint64_t>(img.cols) < (63/3))
	{
		for(; counter_index < 63; ++state_rows)
		{
			for(; counter_index < 63 && state_cols < static_cast<uint64_t>(img.cols); ++state_cols, counter_index += 3)
			{
				cv::Vec3b& color = img.at<cv::Vec3b>(cv::Point(state_cols, state_rows));
        consume_entire_pixel(color, reinterpret_cast<uint8_t*>(bits_of_payload.data()), index_data, position_actual);
			}
		}
	}
	else
	{
		for(; counter_index < 63; ++state_cols, counter_index += 3)
		{
			cv::Vec3b& color = img.at<cv::Vec3b>(cv::Point(state_cols, 0));
			consume_entire_pixel(color, reinterpret_cast<uint8_t*>(bits_of_payload.data()), index_data, position_actual);
		}
	}

  //the last 64-time i need to consume last bit and about bits_of_payload
	//then consume first 2 bit about payload.
	//payload minimum have 1 byte so i'm sure about this 2 "consume_single_color"
  cv::Vec3b& color_towards_payload = img.at<cv::Vec3b>(cv::Point(state_cols, state_rows));
	//check if I've finish with bits_of_payload that only contains 'size_to_hide bytes'
	//only last bit is from bits_of_payload
	consume_single_color(color_towards_payload[INDEX_BLUE], bits_of_payload.data(), index_data, position_actual);
	index_data = 0;
	//others but if from file to hide
	consume_single_color(color_towards_payload[INDEX_GREEN], bytes_data_file, index_data, position_actual);
	consume_single_color(color_towards_payload[INDEX_RED], bytes_data_file, index_data, position_actual);

	//update actual col and row
	if(state_cols == static_cast<uint64_t>(img.cols) - 1)
	{
		state_cols = 0;
		++state_rows;
	}
	else
	{
		++state_cols;
	}

  //TODO CONTINUARE QUI E TOGLIERE IL MERGE SUGLI ARRAY

	//merge input 'n' name file in a single array
	int64_t data_to_hide_extra_size = bytes_data_length + filename.length();
	uint8_t* data_to_hide_extra_info = new uint8_t[data_to_hide_extra_size];
	for(auto i = 0; i < bytes_data_length; ++i)
	{
		data_to_hide_extra_info[i] = bytes_data_file[i];
	}
	for(uint64_t i = 0; i < filename.length(); ++i)
	{
		data_to_hide_extra_info[i + bytes_data_length] = static_cast<uint8_t>(filename.at(i));
	}

	//restart with other array
	//position_actual == POSITION_START;
	bool cols_state_give = true;
	//this take every data of file
	for (decltype(img.rows) rows = state_rows; rows < img.rows; ++rows)
	{
		for (decltype(img.cols) cols = 0; cols < img.cols; ++cols)
		{
			//just first time
			if(cols_state_give == true)
			{
				cols_state_give = false;
				cols = state_cols;
			}

			//check i'm on last data[i]
			if(index_data == data_to_hide_extra_size - 1)
			{
				for(uint32_t color_choose = 0; color_choose < 8; ++color_choose)
				{
					cv::Vec3b& color = img.at<cv::Vec3b>(cv::Point(cols, rows));

					switch(color_choose % 3)
					{
						case INDEX_BLUE:
							//consume_single_color(color[INDEX_BLUE], bytes_data_file, index_data, position_actual);
							consume_single_color(color[INDEX_BLUE], data_to_hide_extra_info, index_data, position_actual);
							break;
						case INDEX_GREEN:
						  //consume_single_color(color[INDEX_GREEN], bytes_data_file, index_data, position_actual);
							consume_single_color(color[INDEX_GREEN], data_to_hide_extra_info, index_data, position_actual);
							break;
						default:
							//consume_single_color(color[INDEX_RED], bytes_data_file, index_data, position_actual);
							consume_single_color(color[INDEX_RED], data_to_hide_extra_info, index_data, position_actual);

							if(cols < img.cols)
							{
								++cols;
							}
							else
							{
								++rows;
								cols = 0;
							}
							break;
					}
				}
				//after that, I exit
				cols = img.cols;
				rows = img.rows;
				continue;
			}
			else
			{
				cv::Vec3b& color = img.at<cv::Vec3b>(cv::Point(cols, rows));

				//I always turn x3 times 'cause every pixel has 3 color
				//consume_single_color(color[INDEX_BLUE], bytes_data_file, index_data, position_actual);
				//consume_single_color(color[INDEX_GREEN], bytes_data_file, index_data, position_actual);
				//consume_single_color(color[INDEX_RED], bytes_data_file, index_data, position_actual);
				consume_entire_pixel(color, data_to_hide_extra_info, index_data, position_actual);
				//consume_single_color(color[INDEX_BLUE], data_to_hide_extra_info, index_data, position_actual);
				//consume_single_color(color[INDEX_GREEN], data_to_hide_extra_info, index_data, position_actual);
				//consume_single_color(color[INDEX_RED], data_to_hide_extra_info, index_data, position_actual);
			}
		}
	}

  //search about file name is not unique
	if(img_name_output.empty() == true)
	{
		std::filesystem::path file_output = img_name_input;
		img_name_output = file_output.parent_path();
		img_name_output += "/";
		img_name_output += file_output.stem();
		img_name_output += "_hide";
	}

	img_name_output += ".png";
	imwrite(img_name_output, img);

  //clear necessary at now
	delete[] data_to_hide_extra_info;
	data_to_hide_extra_info = nullptr;

}

uint8_t* steganography::seek_file(const std::string& img_name_input, int64_t& bytes_data_length, std::string& file_name_output2)
{
	//01. clean the string
	//with this i'm sure the string will be contains name of file
	std::string file_name_output;

  cv::Mat img = cv::imread(img_name_input, cv::IMREAD_COLOR); //open the img

  //02. check the img + tiny check
	if(img.empty())
	{
		std::cout << "something wrong with imread\n";
		bytes_data_length = -1;
		return nullptr;
	}
	if((img.rows * img.cols) <= 64)
	{
		std::cout << "img too tiny to embed a file\n";
		bytes_data_length = -1;
		return nullptr;
	}

  //03. i'm looking about the size file
	bytes_data_length = 0;
	uint64_t bits_already_read = 0;
	uint64_t total_bits_length = 0;
	decltype(img.cols) rows, cols;

	bool have_i_finish = false;
	uint64_t state_cols_bytes_reading = 0;
	uint64_t state_rows_bytes_reading = 0;

  //with the first 64 bits i have the size of payload
	for (rows = 0; rows < img.rows; ++rows)
	{
		for (cols = 0; cols < img.cols; ++cols)
		{
			cv::Vec3b& color = img.at<cv::Vec3b>(cv::Point(cols, rows));

			//for first 64 times every single less significant bits about color in order [0, 1, 2] (equals to blue, green and red)
			//are a bits to rapresent the dimension of file:
			//ex: I know bytes_data_length = 0xC9 = 0b11101001
			//so at rows = 0, cols = 0 -> then
			//color[INDEX_BLUE] % 2 = 1;
			//color[INDEX_GREEN] % 2 = 1;
			//color[INDEX_RED] % 2 = 1;
			//bytes_data_length = 0 -> bytes_data_file |= (0b111) << 0d(8-3) -> bytes_data_file = 0d224 = 0b11100000
			//so at rows = 0, cols = 1 -> then
			//color[INDEX_BLUE] % 2 = 0;
			//color[INDEX_GREEN] % 2 = 1;
			//color[INDEX_RED] % 2 = 0;
			//bytes_data_length = 0d224 -> bytes_data_file |= (0b010) << 0d(8-6)) -> bytes_data_file = 0d232 = 0b11101000
			//so at rows = 0, cols = 2 -> then
			//color[INDEX_BLUE] % 2 = 0;
			//color[INDEX_GREEN] % 2 = 1;
			//bytes_data_length = 0d232 -> bytes_data_file |= (0b01) << 0d0) -> bytes_data_file = 0d233 = 0b11101001

			if(bits_already_read != 63)
			{
				bits_already_read += 3;
				bytes_data_length |= ((((color[INDEX_BLUE] % 2) << INDEX_RED) | ((color[INDEX_GREEN] % 2) << INDEX_GREEN) | (color[INDEX_RED] % 2)) << (64 - bits_already_read));
			}
			else
			{
				bytes_data_length |= (color[INDEX_BLUE] % 2); //finish
				//then i use bits_already_read like tmp buffer
				//so i need to store it in file_data
				bits_already_read = (((color[INDEX_GREEN] % 2) << INDEX_GREEN) | (color[INDEX_RED] % 2));

				if(cols == img.cols - 1)
				{
					state_cols_bytes_reading = 0;
					state_rows_bytes_reading = rows + 1;
				}
				else
				{
					state_cols_bytes_reading = cols + 1;
					state_rows_bytes_reading = rows;
				}

				cols = img.cols;
				rows = img.rows;
			}
		}
	}

	uint8_t* file_data = nullptr;
	//04. little checks about size found
	if(bytes_data_length <= 0) //check length so I can call this->store_single_color() twice in worst case but never 0
	{
		std::cout << "error with seek: size of file stored is zero\n";
		bytes_data_length = -1;
		return nullptr;
	}

	if(static_cast<uint64_t>(img.rows * img.cols * 3) < ((bytes_data_length + sizeof(int64_t)) * 8)) //last check on correct dimensions
	{
		std::cout << "img too tiny to embed a file of " << bytes_data_length << " bytes \n";
		bytes_data_length = -1;
		return nullptr;
	}

	//at this point, bytes_data_length contains also the dimension of file name
	//so i need to extract this data
	//file_data contains also the name value
	//I will extract the name from file_data after file_data is complete

	//the first bits is the sign of bytes_data_length
	file_data = new uint8_t[bytes_data_length];
	total_bits_length = bytes_data_length * 8; //this is always >= 8

  //using the tmp buffer
	file_data[0] = bits_already_read << (8 - 2);
	bits_already_read = 2;

	for (cols = state_cols_bytes_reading; cols < img.cols; ++cols)
	{
		//I have to read every single bits in img then store it in uint8_t*
		cv::Vec3b& color = img.at<cv::Vec3b>(cv::Point(cols, state_rows_bytes_reading));

		if((bits_already_read + 3) > total_bits_length) //if I risk read too much
		{
			store_last_color(color, file_data, bits_already_read);
			cols = img.cols;
			//rows = img.rows;
			have_i_finish = true;
			continue;
		}

		store_single_color(color, file_data, bits_already_read);
	}

	if(have_i_finish == false)
	{
		for (rows = state_rows_bytes_reading + 1; rows < img.rows; ++rows)
		{
			for (cols = 0; cols < img.cols; ++cols)
			{
				//I have to read every single bits in img then store it in uint8_t*
				cv::Vec3b& color = img.at<cv::Vec3b>(cv::Point(cols, rows));

				if((bits_already_read + 3) > total_bits_length) //if I risk read too much
				{
					store_last_color(color, file_data, bits_already_read);
					cols = img.cols;
					rows = img.rows;
					continue;
				}

				store_single_color(color, file_data, bits_already_read);
			}
		}
  }

	//start to extract the name
	//last byte (bytes_data_lengthss - 1) are the string length
	uint8_t name_size = static_cast<uint8_t>(file_data[--bytes_data_length]);
	//file_data[bytes_data_length] == name_size
	//so file_data[bytes_data_length - 1] is the last file_data letter
	//so I search about every letter
	for (auto i = 1; i <= name_size; ++i)
	{
		file_name_output += static_cast<char>(file_data[bytes_data_length - i]);
	}
	//then reverse the string to obtain the correct name
	std::reverse(file_name_output.begin(), file_name_output.end());
	file_name_output2 = file_name_output;

	//and update the correct bytes_data_length:
	//file_data from 0..(bytes_data_length - file_name_output)
	//it's the real hide content
	bytes_data_length -= name_size;

	return file_data;
}

//I read every bit of data
//then modify the last color bit with
void steganography::consume_single_color(uint8_t& pixel_color, uint8_t* data, uint32_t& index_data, uint32_t& position_actual)
{
	//extract the last bit
	uint8_t single_bit = data[index_data] >> position_actual;

	if ((pixel_color % 2) != (single_bit % 2)) //compare last bit
	{
		((pixel_color % 2 == 0) ? (++pixel_color) : (--pixel_color)); //change last bit
	}
	//this is a countdown,
	//when it's 0 then I read next datas value
	if (position_actual == 0)
	{
		position_actual = POSITION_START;
		++index_data;
	}
	else
	{
		position_actual--;
	}
}

//i read 3 bits
void steganography::store_single_color(cv::Vec3b& color, uint8_t* file_data, uint64_t& bits_already_read)
{
	uint64_t index = bits_already_read / BITS_IN_ONE_BYTE;
	uint64_t offset = bits_already_read % static_cast<uint64_t>(BITS_IN_ONE_BYTE);
	bits_already_read += 3;
	switch(offset)
	{
		case 0:
			file_data[index] = static_cast<uint8_t>((((color[INDEX_BLUE] % 2) << INDEX_RED) | ((color[INDEX_GREEN] % 2) << INDEX_GREEN) | (color[INDEX_RED] % 2)) << ((8 - 3) - offset));
			break;
		case 7:
			file_data[index] |= static_cast<uint8_t>(color[INDEX_BLUE] % 2);
			file_data[index + 1] = static_cast<uint8_t>((((color[INDEX_GREEN] % 2) << INDEX_GREEN) | (color[INDEX_RED] % 2)) << 6);
			break;
		case 6:
			file_data[index] |= static_cast<uint8_t>(((color[INDEX_BLUE] % 2) << 1) | (color[INDEX_GREEN] % 2));
			file_data[index + 1] = static_cast<uint8_t>((color[INDEX_RED] % 2) << 7);
			break;
		default:
			file_data[index] |= static_cast<uint8_t>((((color[INDEX_BLUE] % 2) << INDEX_RED) | ((color[INDEX_GREEN] % 2) << INDEX_GREEN) | (color[INDEX_RED] % 2)) << ((8 - 3) - offset));
			break;
	}
}

void steganography::store_last_color(cv::Vec3b& color, uint8_t* file_data, uint64_t& bits_already_read)
{
	//I'm here because (bits_already_read + 3) > total_bits_length
	//I know total_bits_length % 8 == 0 so
	//   EOF 0 1 2 3 4 5 6 7 0 1 2 3 ...
	//A: - R G B      <-- read only B, G then win
	//B: - G B        <-- read only B then win
	//B: - B          <-- already win

	uint64_t index = bits_already_read / 8;
	switch(bits_already_read % 3)
	{
		case 1:
			bits_already_read += 2;
			file_data[index] |= (color[INDEX_BLUE] % 2);
			break;
		case 2:
		  bits_already_read += 1;
			file_data[index] |= (((color[INDEX_BLUE] % 2) << 1) | (color[INDEX_GREEN] % 2));
			break;
		default: //blue == file_data[(bits_already_read / 8) + 1]
			break;
	}
}

void steganography::consume_entire_pixel(cv::Vec3b& color, uint8_t* data, uint32_t& index_data, uint32_t& position_actual)
{
	consume_single_color(color[INDEX_BLUE], data, index_data, position_actual);
	consume_single_color(color[INDEX_GREEN], data, index_data, position_actual);
	consume_single_color(color[INDEX_RED], data, index_data, position_actual);
}
