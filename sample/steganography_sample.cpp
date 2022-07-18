#include "steganography.hpp"

int main()
{
  std::string scrt = "SECRET\t:)\n"; //data to hide and seek
  
  uint8_t* data_to_hide = nullptr;
  uint8_t* data_to_seek = nullptr;
  int64_t length_data_to_seek = 0;
  
  data_to_hide = new uint8_t[scrt.length()];
  //copy scrt to data_to_hide
  uint64_t i = -1;
  for(char letter : scrt)
  {
    data_to_hide[++i] = static_cast<uint8_t>(letter); 
  }
  
  //image's name
  std::string img_name = "trust_me.jpg";
  
  //static void hide_file(
    //uint8_t* bytes_data_file, 
    //int64_t bytes_data_length, 
    //std::string img_name_input
  //);
  _steganography_rp96_::steganography.hide_file(
    data_to_hide, 
    scrt.length(), 
    img_name
  ); //update the img with the input_file
  
  //static uint8_t* seek_file(
    //std::string img_name_input, 
    //int64_t& bytes_data_length
  //);
  data_to_seek = _steganography_rp96_::seek_file(
    img_name, 
    length_data_to_seek
  );
  
  //final check if data_to_seek == data_to_hide
  if(length_data_to_seek != scrt.length())
  {
    std::cout << "Something wrong: length\n";
    
    delete[] data_to_hide;
    data_to_hide = nullptr;
    delete[] data_to_seek;
    data_to_seek = nullptr;
    
    return 0;
  }
  for(auto i = 0; i < length_data_to_seek; ++i)
  {
    if(data_to_seek[i] != data_to_hide[i])
    {
      std::cout << "Something wrong: data\n";
      
      delete[] data_to_hide;
      data_to_hide = nullptr;
      delete[] data_to_seek;
      data_to_seek = nullptr;
      
      return 0;
    }
  }
  
  std::cout << "All went well :)\n";
  delete[] data_to_hide;
  data_to_hide = nullptr;
  delete[] data_to_seek;
  data_to_seek = nullptr;
  
  return 0;
}