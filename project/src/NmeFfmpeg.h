#ifndef NME_FFMPEG_INCLUDED
#define NME_FFMPEG_INCLUDED

#include "nme/ImageBuffer.h"

#include <string>
#include <vector>

namespace nff
{

void nffThrow(const std::string &err);

class INmeVideo
{
public:
   static INmeVideo *open(const char *inFilename);
   virtual ~INmeVideo() {}

   virtual int getWidth() const = 0;
   virtual int getHeight() const = 0;
   virtual double getFrameRate() const = 0;
   virtual double getDuration() const = 0;
   virtual bool readNextFrame(nme::ImageBuffer *buffer) = 0;

};

} // end namespace nff

#endif
