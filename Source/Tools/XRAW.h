#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <sstream>

namespace XRAW {

  enum class ColorChannelDataType {
    UnsignedInteger = 0,
    SignedInteger = 1,
    Float = 2
  };

  enum class NumberOfColorChannels {
    RGBA = 4,
    RGB  = 3,
    RG   = 2,
    R    = 1,   
  };
  enum class BitsPerChannel {
    Char  = 8,
    Short = 16,
    UInt  = 32
  };
  enum class BitsPerIndex {
    Char       = 8,//256 colors
    Short      = 16,//32768 colors
    NoPallete  = 0 //no pallete
  };

  enum class NumberOfPalleteColors {
    Amount256   = 256,
    Amount32768 = 32768
  };

  struct header {
    size_t getDataSize();

    //0: unsigned integer
    //1: signed integer
    //2: float
    ColorChannelDataType colorChannelDataType;

    //4 RGBA
    //3 RGB
    //2 RG
    //1 R
    NumberOfColorChannels numOfColorChannels;

    //Data size for each color
    //8
    //16
    //32
    BitsPerChannel bitsPerChannel;

    // 8  : 256 colors
    // 16 : 32768 colors
    // 0  : no pallete
    BitsPerIndex bitsPerIndex;

    unsigned int xResolution  = 8;
    unsigned int yResolution = 8;
    unsigned int zResolution  = 8;

    NumberOfPalleteColors numberOfPalleteColors; //256 or 32768
  };

  //https://twitter.com/ephtracy/status/740421363048054784
  //importable by magicaVox (and simple to implement)
  class XRAW {
  public:
    static void write(const std::string& filename, const bool*);
    static std::unique_ptr<bool[]> read(const std::string& filename);

    static header readHeader(const std::string& filename);
    //static std::vector<std::array<float, 4>> readRGBAFloat(const std::string& filename);
    static std::unique_ptr<bool[]> readBool(const std::string& filename);
    //static std::vector<short> readIndexed(const std::string& filename, std::vector<std::array<float, 4>>& pallete);

  private:
    static void readBuffer(const std::string& filename, std::vector<unsigned char>& data);
    static header readHeader(const std::vector<unsigned char>& data, size_t& position);


    static unsigned int readUInt(const std::vector<unsigned char>& data, size_t& position);
  };
}