#ifndef NME_FFMPEG_INCLUDED
#define NME_FFMPEG_INCLUDED

#include "nme/NmeApi.h"

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

};

} // end namespace nff

#endif
