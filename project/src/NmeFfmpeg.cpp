#include "NmeFfmpeg.h"
#include <iostream>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
//#include <libavutil/imgutils.h>
}

namespace nff
{

class NmeVideo : public INmeVideo
{
   AVFormatContext* format_ctx = nullptr;
   AVCodecContext* codec_ctx = nullptr;
   AVPacket* packet = nullptr;
   AVFrame* frame = nullptr;
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

   bool extractNextFrame()
   {
      bool frame_extracted = false;

      while (av_read_frame(format_ctx, packet) >= 0 && !frame_extracted) {
         if (packet->stream_index == video_stream_index) {
               // Send packet to decoder
               if (avcodec_send_packet(codec_ctx, packet) == 0) {
                  // Receive decoded frame
                  if (avcodec_receive_frame(codec_ctx, frame) == 0) {
                     std::cout << "Successfully extracted frame: " << frame->width << "x" << frame->height << std::endl;
                     frame_extracted = true;
                  }
               }
         }
         av_packet_unref(packet);
      }
      return frame_extracted;
   }

   ~NmeVideo()
   {
    // Cleanup
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
