/**
 * @author Peruffo Riccardo, RP96, riccardoperuffo96@gmail.com, github.com/RiccardoPeruffo96
 * @date 30th APR 2022
 * @file steganography.cpp
 * @brief this file contains the definitions of steganography.hpp's declarations
 * @version 0.0.1 alpha [exam]
 */

#include "steganography.hpp"

using namespace _steganography_rp96_;

void steganography::hide_file(uint8_t* bytes_data_file, int64_t bytes_data_length, std::string img_name_input, std::string file_name_input, std::string img_name_output)
{
  std::filesystem::path filepath{file_name_input};
  std::string filename = filepath.filename();
  if(filename.length() > 0xFF)
  {
    //filename will contains last 255 name's letters
    filename = filename.substr(filename.length() - 0xFF);
  }
  filename += static_cast<char>(filename.length());

  cv::Mat img = cv::imread(img_name_input, cv::IMREAD_COLOR); //open the img

  if(img.empty())
  {
    std::cout << "something wrong with imread\n";
    return;
  }

  if((img.rows * img.cols) <= 64)
  {
    std::cout << "file too tiny\n";
    return;
  }

  //first 64 bits are the size of bytes_data_file
  //then the *uint8_t
  int64_t total_data_size = bytes_data_length + filename.length();
  int64_t size_to_hide = sizeof(int64_t) + total_data_size;

  //uint8_t* data_to_hide = new uint8_t[size_to_hide];
  std::array<uint8_t, sizeof(int64_t)> data_to_hide;

  //store the size of bytes_data_file
  //data_to_hide[0] = 7th most relevant size_to_hide bits
  //data_to_hide[7] = 7th less relevant size_to_hide bits
  //#pragma unroll(sizeof(int64_t))
  for(uint64_t i = 0; i < sizeof(int64_t); ++i)
  {
    data_to_hide[i] = static_cast<uint8_t>(static_cast<uint64_t>(total_data_size) >> ( ( (sizeof(uint64_t)-1) - i ) * BITS_IN_ONE_BYTE));
  }
  //i have already a *uint8_t for datas (bytes_data_file)

  int64_t img_space_usable = img.rows * img.cols * CHANNELS_BY_COLOR;
  int64_t img_space_needed = size_to_hide * BITS_IN_ONE_BYTE;

  if (img_space_needed > img_space_usable)
  {
    std::cout << "the img it's too little for the input\n";
    return;
  }

  //start to hide bits
  uint32_t position_actual = POSITION_START;
  uint32_t index_data = 0;
  uint32_t counter_index = 0;

  uint64_t state_rows = 0, state_cols = 0;
  //img_space_usable > 64 bits always
  //this take every data of length file
  for (decltype(img.rows) rows = 0; rows < img.rows; ++rows)
  {
    for (decltype(img.cols) cols = 0; cols < img.cols; ++cols)
    {
      cv::Vec3b& color = img.at<cv::Vec3b>(cv::Point(cols, rows));

      //check if I've finish with data_to_hide that only contains 'size_to_hide bytes'
      if(counter_index == 63)
      {
        consume_single_color(color[INDEX_BLUE], data_to_hide.data(), index_data, position_actual);
        index_data = 0;
        consume_single_color(color[INDEX_GREEN], bytes_data_file, index_data, position_actual);
        consume_single_color(color[INDEX_RED], bytes_data_file, index_data, position_actual);

        if(cols == img.cols - 1)
        {
          state_cols = 0;
          state_rows = rows + 1;
        }
        else
        {
          state_cols = cols + 1;
          state_rows = rows;
        }
        cols = img.cols;
        rows = img.rows;
      }
      else
      {
        //I always turn x3 times 'cause every pixel has 3 color
        consume_single_color(color[INDEX_BLUE], data_to_hide.data(), index_data, position_actual);
        ++counter_index;
        consume_single_color(color[INDEX_GREEN], data_to_hide.data(), index_data, position_actual);
        ++counter_index;
        consume_single_color(color[INDEX_RED], data_to_hide.data(), index_data, position_actual);
        ++counter_index;
      }
    }
  }

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
        consume_single_color(color[INDEX_BLUE], data_to_hide_extra_info, index_data, position_actual);
        consume_single_color(color[INDEX_GREEN], data_to_hide_extra_info, index_data, position_actual);
        consume_single_color(color[INDEX_RED], data_to_hide_extra_info, index_data, position_actual);
      }
    }
  }

  delete[] data_to_hide_extra_info;
  data_to_hide_extra_info = nullptr;

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
}

uint8_t* steganography::seek_file(std::string img_name_input, int64_t& bytes_data_length, std::string& file_name_output)
{
  file_name_output.clear();

  cv::Mat img = cv::imread(img_name_input, cv::IMREAD_COLOR); //open the img

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

  bytes_data_length = 0;
  uint64_t bits_already_read = 0;
  uint64_t total_bits_length = 0;
  decltype(img.cols) rows, cols;

  bool have_i_finish = false;
  uint64_t state_cols_bytes_reading = 0;
  uint64_t state_rows_bytes_reading = 0;

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
  if(bytes_data_length <= 0) //check length so I can call this->store_single_color() twice in worst case but never 0
  {
    std::cout << "error with seek: size of file stored is zero\n";
    bytes_data_length = -1;
    return  nullptr;
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

  file_data[0] = bits_already_read << (8 - 2);
  bits_already_read = 2;

  for (cols = state_cols_bytes_reading; cols < img.cols; ++cols)
  {
    //I have to read every single bits in img then store it in uint8_t*
    cv::Vec3b& color = img.at<cv::Vec3b>(cv::Point(cols, state_rows_bytes_reading));

    if((bits_already_read + 3) > total_bits_length) //if I risk read too much
    {
      store_last_color(color, file_data, bits_already_read, total_bits_length);
      cols = img.cols;
      rows = img.rows;
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
        cv::Vec3b& color = img.at<cv::Vec3b>(cv::Point(cols, state_rows_bytes_reading));

        if((bits_already_read + 3) > total_bits_length) //if I risk read too much
        {
          store_last_color(color, file_data, bits_already_read, total_bits_length);
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
    (pixel_color % 2 == 0) ? ++pixel_color : --pixel_color; //change last bit
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

void steganography::store_last_color(cv::Vec3b& color, uint8_t* file_data, uint64_t& bits_already_read, uint64_t& total_bits_length)
{
  std::cout << "total_bits_length: " << total_bits_length << "\n";
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
