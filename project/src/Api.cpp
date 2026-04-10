#ifdef NME_FFMPEG_STATIC_LINK
  #define STATIC_LINK
#else
  #define IMPLEMENT_API
#endif

#define NOMINMAX

#include <hx/CFFIPrime.h>
#include "nme/ImageBuffer.h"
#include "NmeFfmpeg.h"

#ifdef _MSC_VER
#pragma warning(disable:4624)
#endif

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <math.h>

namespace nff
{

vkind gObjectKind;
vkind dataKind;
vkind videoKind;



static int _id_name;

extern "C" void InitIDs()
{
   kind_share(&dataKind,"data");
   kind_share(&videoKind,"nffvideo");
   kind_share(&gObjectKind,"nme::Object");

   _id_name = val_id("name");
}

DEFINE_ENTRY_POINT(InitIDs)

void nffThrow(const std::string &err)
{
   gc_try_unblocking();
   val_throw( alloc_string(err.c_str()) );
}

nme::ImageBuffer *toImageBuffer(value inValue)
{
   if (!val_is_kind(inValue,gObjectKind))
   {
      nffThrow("Not an image buffer object");
   }
   nme::Object *obj = (nme::Object *)val_data(inValue);
   nme::ImageBuffer *buffer = obj->asImageBuffer();
   if (!buffer)
      nffThrow("Not an image buffer");
   return buffer;
}

#define TO_VIDEO \
   if (val_kind(inVideo)!=videoKind) val_throw(alloc_string("object not a nme video object")); \
   nff::INmeVideo *video = (nff::INmeVideo *)val_data(inVideo);

#define TO_DATA \
   if (val_kind(in_##name)!=dataKind) val_throw(alloc_string(#name " is not a data")); \
   type name = (type)val_data(in_##name);


void destroyVideo(value inVideo)
{
   nff::INmeVideo *video = (nff::INmeVideo *)val_data(inVideo);
   delete video;
}

value toValue(INmeVideo *inVideo)
{
   value v = alloc_abstract(videoKind, inVideo);
   val_gc(v, destroyVideo);
   return v;
}

void nffRelease(value inVideo)
{
   TO_VIDEO
   if (video)
   {
      delete video;
      val_gc(inVideo,0);
   }
}
DEFINE_PRIME1v(nffRelease)


value nffOpen(value inName)
{
   const char *name = val_string(inName);
   INmeVideo *video = INmeVideo::open(name);
   return toValue(video);
}
DEFINE_PRIME1(nffOpen)


int nffGetWidth(value inVideo)
{
   TO_VIDEO
   return video->getWidth();
}
DEFINE_PRIME1(nffGetWidth)


int nffGetHeight(value inVideo)
{
   TO_VIDEO
   return video->getHeight();
}
DEFINE_PRIME1(nffGetHeight)

double nffGetFrameRate(value inVideo)
{
   TO_VIDEO
   return video->getFrameRate();
}
DEFINE_PRIME1(nffGetFrameRate)


double nffGetDuration(value inVideo)
{
   TO_VIDEO
   return video->getDuration();
}
DEFINE_PRIME1(nffGetDuration)



} // namespace nff



