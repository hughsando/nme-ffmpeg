#include "NmeFfmpeg.h"
#include <iostream>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libswscale/swscale.h>
}

namespace nff
{

class NmeVideo : public INmeVideo
{
   AVFormatContext* format_ctx = nullptr;
   AVCodecContext* codec_ctx = nullptr;
   AVPacket* packet = nullptr;
   AVFrame* frame = nullptr;
   SwsContext* sws_ctx = nullptr;
   int sws_src_width = 0;
   int sws_src_height = 0;
   AVPixelFormat sws_src_format = AV_PIX_FMT_NONE;
   int sws_dst_width = 0;
   int sws_dst_height = 0;
   int video_stream_index = -1;

   int frameWidth = 0;
   int frameHeight = 0;
   double frameRate = 0.0;
   double duration = 0.0;

public:
   NmeVideo(const char *inFilename)
   {
      // 1. Open input file
      if (avformat_open_input(&format_ctx, inFilename, nullptr, nullptr) < 0)
         nffThrow("Could not open input file");
      if (avformat_find_stream_info(format_ctx, nullptr) < 0)
      {
         avformat_close_input(&format_ctx);
         nffThrow("Could not find stream information");
      }

      // 2. Find video stream and codec
      AVCodecParameters* codec_params = nullptr;
      const AVCodec* codec = nullptr;

      for (unsigned int i = 0; i < format_ctx->nb_streams; i++) {
         if (format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
               video_stream_index = i;
               codec_params = format_ctx->streams[i]->codecpar;
               codec = avcodec_find_decoder(codec_params->codec_id);
               break;
         }
      }

      if (video_stream_index == -1 || !codec) {
         avformat_close_input(&format_ctx);
         nffThrow("Could not find video stream or codec");
      }

      AVStream* stream = format_ctx->streams[video_stream_index];
      AVCodecParameters* params = stream->codecpar;

      // --- GET FRAME SIZE ---
      frameWidth = params->width;
      frameHeight = params->height;

      // --- GET FPS ---
      // av_guess_frame_rate is preferred over r_frame_rate
      AVRational fps_rational = av_guess_frame_rate(format_ctx, stream, nullptr);
      frameRate = av_q2d(fps_rational);

      // --- GET DURATION ---
      // Duration is in AV_TIME_BASE (usually microseconds)
      duration = (double)format_ctx->duration / AV_TIME_BASE;

      // 3. Initialize codec context
      codec_ctx = avcodec_alloc_context3(codec);
      avcodec_parameters_to_context(codec_ctx, codec_params);
      avcodec_open2(codec_ctx, codec, nullptr);

      // Prepare to read frames
      packet = av_packet_alloc();
      frame = av_frame_alloc();
   }

   bool frameToBuffer(AVFrame* frame, nme::ImageBuffer* buffer)
   {
      if (buffer->Format() != nme::pfRGB) {
         printf("Only RGB format is supported for now");
         return false;
      }

      unsigned char *dest = buffer->Edit();
      int stride = buffer->GetStride();
      int destW = buffer->Width();
      int destH = buffer->Height();

      AVPixelFormat srcFormat = (AVPixelFormat)frame->format;
      int srcW = frame->width;
      int srcH = frame->height;

      // Calculate crop region to center the source in the destination
      int cropX = 0;
      int cropY = 0;
      int cropW = srcW;
      int cropH = srcH;

      if (srcW > destW) {
         cropX = (srcW - destW) / 2;
         cropW = destW;
      }
      if (srcH > destH) {
         cropY = (srcH - destH) / 2;
         cropH = destH;
      }

      // Recreate SwsContext if parameters changed
      if (sws_ctx == nullptr ||
          sws_src_width != cropW || sws_src_height != cropH ||
          sws_src_format != srcFormat ||
          sws_dst_width != destW || sws_dst_height != destH) {

         if (sws_ctx) {
            sws_freeContext(sws_ctx);
         }

         sws_ctx = sws_getContext(
            cropW, cropH, srcFormat,
            destW, destH, AV_PIX_FMT_RGB24,
            SWS_BILINEAR, nullptr, nullptr, nullptr);

         if (!sws_ctx) {
            printf("Failed to create SwsContext");
            return false;
         }

         sws_src_width = cropW;
         sws_src_height = cropH;
         sws_src_format = srcFormat;
         sws_dst_width = destW;
         sws_dst_height = destH;
      }

      // Adjust source data pointers to account for cropping
      const uint8_t* srcSlice[4];
      int srcStride[4];

      for (int i = 0; i < 4; i++) {
         srcSlice[i] = frame->data[i];
         srcStride[i] = frame->linesize[i];
      }

      // Apply crop offset to the source plane(s)
      // For planar formats, we need to handle Y, U, V planes differently
      if (srcFormat == AV_PIX_FMT_YUV420P || srcFormat == AV_PIX_FMT_YUVJ420P) {
         // Y plane: full resolution crop
         srcSlice[0] = frame->data[0] + cropY * frame->linesize[0] + cropX;
         // U and V planes: half resolution crop
         srcSlice[1] = frame->data[1] + (cropY / 2) * frame->linesize[1] + (cropX / 2);
         srcSlice[2] = frame->data[2] + (cropY / 2) * frame->linesize[2] + (cropX / 2);
      } else if (srcFormat == AV_PIX_FMT_YUV422P || srcFormat == AV_PIX_FMT_YUVJ422P) {
         // Y plane: full resolution
         srcSlice[0] = frame->data[0] + cropY * frame->linesize[0] + cropX;
         // U and V planes: half width, full height
         srcSlice[1] = frame->data[1] + cropY * frame->linesize[1] + (cropX / 2);
         srcSlice[2] = frame->data[2] + cropY * frame->linesize[2] + (cropX / 2);
      } else if (srcFormat == AV_PIX_FMT_YUV444P || srcFormat == AV_PIX_FMT_YUVJ444P) {
         // All planes have same resolution
         srcSlice[0] = frame->data[0] + cropY * frame->linesize[0] + cropX;
         srcSlice[1] = frame->data[1] + cropY * frame->linesize[1] + cropX;
         srcSlice[2] = frame->data[2] + cropY * frame->linesize[2] + cropX;
      } else if (srcFormat == AV_PIX_FMT_NV12 || srcFormat == AV_PIX_FMT_NV21) {
         // Y plane: full resolution
         srcSlice[0] = frame->data[0] + cropY * frame->linesize[0] + cropX;
         // UV interleaved plane: half height, cropX must be even
         srcSlice[1] = frame->data[1] + (cropY / 2) * frame->linesize[1] + (cropX & ~1);
      } else {
         // For packed formats (RGB, BGR, etc.), apply offset directly
         int bytesPerPixel = av_get_bits_per_pixel(av_pix_fmt_desc_get(srcFormat)) / 8;
         if (bytesPerPixel > 0) {
            srcSlice[0] = frame->data[0] + cropY * frame->linesize[0] + cropX * bytesPerPixel;
         }
      }

      // Set up destination
      uint8_t* dstSlice[4] = { dest, nullptr, nullptr, nullptr };
      int dstStride[4] = { stride, 0, 0, 0 };

      // Perform the conversion
      sws_scale(sws_ctx, srcSlice, srcStride, 0, cropH, dstSlice, dstStride);

      buffer->Commit();
      return true;
   }

   bool readNextFrame(nme::ImageBuffer *buffer )
   {
      bool frame_extracted = false;

      while (av_read_frame(format_ctx, packet) >= 0 && !frame_extracted) {
         if (packet->stream_index == video_stream_index) {
               // Send packet to decoder
               if (avcodec_send_packet(codec_ctx, packet) == 0) {
                  // Receive decoded frame
                  if (avcodec_receive_frame(codec_ctx, frame) == 0) {
                     frame_extracted = true;
                  }
               }
         }
         av_packet_unref(packet);
      }
      if (frame_extracted)
         frame_extracted = frameToBuffer(frame, buffer);
      return frame_extracted;
   }

   ~NmeVideo()
   {
    // Cleanup
    if (sws_ctx)
       sws_freeContext(sws_ctx);
    if (frame)
       av_frame_free(&frame);
    if (packet)
       av_packet_free(&packet);
    if (codec_ctx)
       avcodec_free_context(&codec_ctx);
    if (format_ctx)
       avformat_close_input(&format_ctx);
   }



   int getWidth() const
   {
      return frameWidth;
   }
   int getHeight() const
   {
      return frameHeight;
   }
   double getFrameRate() const
   {
      return frameRate;
   }
   double getDuration() const
   {
      return duration;
   }

};

INmeVideo *INmeVideo::open(const char *inFilename)
{
   return new NmeVideo(inFilename);
}


} // end namespace nff
