#include "NmeFfmpeg.h"


namespace nff
{

class NmeVideo : public INmeVideo
{
public:
   NmeVideo(const char *inFilename)
   {
   }


   int getWidth() const { return 640; }
   int getHeight() const { return 480; }

};

INmeVideo *INmeVideo::open(const char *inFilename)
{
   return new NmeVideo(inFilename);
}


} // end namespace nff
