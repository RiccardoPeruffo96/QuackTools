/**
 * @author Peruffo Riccardo, RP96, riccardoperuffo96@gmail.com, github.com/RiccardoPeruffo96
 * @date 2022
 * @file steganography.hpp
 * @brief It allows to hide datas in a multimedial file, after that you can seek this data:
 * now it works well only with images
 * @version 0.0.1 alpha [exam]
 */

#ifndef STEGANOGRAPHY_RP96_HPP_
#define STEGANOGRAPHY_RP96_HPP_

#include <iostream> //std::string
#include <opencv2/opencv.hpp> //cv::imread
#include <filesystem> //std::filesystem::path

/**
 * @brief this namespace define
 * statics methods to hide and seek
 * char* into multimedial files:
 * now it supports only images
 */
namespace _steganography_rp96_
{
  //auxiliary constants vars
  const uint32_t BITS_IN_ONE_BYTE = 8; ///< bits in one byte
  const uint32_t CHANNELS_BY_COLOR = 3; ///< channels by color for each pixel
  const uint32_t POSITION_START = 7; ///< index to single bit

  /**
   * @brief mapping of channels for
   * each pixel
   */
  enum index_channels
  {
    INDEX_BLUE = 0, ///< blue channel
    INDEX_GREEN, ///< green channel
    INDEX_RED ///< red channel
  };
  //const uint32_t INDEX_BLUE = 0;
  //const uint32_t INDEX_GREEN = 1;
  //const uint32_t INDEX_RED = 2;

  /**
   * @brief this class is used only with statics methods
   * the class is used to hide privates functions
   */
  class steganography
  {
  public:

    ///delete constructor
    steganography() = delete;
    ///delete copy constructor
    steganography(const steganography& steg) = delete;
    ///delete copy assignment operator
    steganography& operator=(const steganography& steg) = delete;
    ///delete move constructor
    steganography(steganography&& steg) = delete;
    ///delete move assignment operator
    steganography& operator=(steganography&& steg) = delete;
    ///delete destructor
    ~steganography() = delete;

   /**
     * @brief hide the data in the img at the string path
     * @param bytes_data_file data
     * @param bytes_data_length data's size
     * @param img_name_input file's image name
     * @param img_name_output file's image new name
     */
    static void hide_file(uint8_t* bytes_data_file, int64_t bytes_data_length, std::string img_name_input, std::string file_name_input, std::string img_name_output = "");

   /**
     * @brief seek the data in the img at the string path, then return a char*. AAA: THE POINTER WILL NOT BE DELETE AUTOMATICALLY WHEN DECOSTRUCTOR OR OTHERS, WHO CALLS THE FUNCTION NEED TO KNOW HOW TO DO WITH THIS
     * @param img_name_input file's image name
     * @param bytes_data_length this param will be cleared and will contains the size of the return pointer
     * @param img_name_output file's image new name
     * @return pointer to datas
     */
    static uint8_t* seek_file(std::string img_name_input, int64_t& bytes_data_length, std::string& file_name_output);

  private:

   /**
     * @brief read the single color about one pixel and store it a bit of data
     * @param pixel_color color to change: blue, green, red are 0,1,2
     * @param data data
     * @param index_data index about data
     * @param position_actual the correct bit to read about data[index_data]
     */
    static void consume_single_color(uint8_t& pixel_color, uint8_t* data, uint32_t& index_data, uint32_t& position_actual);

    /**
      * @brief read the lsb for each color and store it in file_data and set bits counter
      * @param color matrix for blue, green and red values
      * @param file_data when to store the lsb
      * @param bits_already_read bits counter
      */
    static void store_single_color(cv::Vec3b& color, uint8_t* file_data, uint64_t& bits_already_read);

    /**
     * @brief like store_single_color but ((bits_already_read + 3) > total_bits_length), so it's the "final lap"
     * @param color matrix for blue, green and red values
     * @param file_data when to store the lsb
     * @param bits_already_read bits counter
     * @param total_bits_length max bits readable, always total_bits_length % 8 == 0
     */
    static void store_last_color(cv::Vec3b& color, uint8_t* file_data, uint64_t& bits_already_read, uint64_t& total_bits_length);
  };
}

#endif //STEGANOGRAPHY_RP96_HPP_
