FFMPEG SDKS
-----------

The build files uses NME_FFMPEG_SDK variable to locate the include, lib and bin directories for the build process.

This was tested by downloading the windows build from: https://github.com/BtbN/FFmpeg-Builds/releases and unzipping here.

This created files like:

nme-ffmpeg/sdk/sdk/ffmpeg-n8.1-latest-win64-lgpl-shared-8.1/lib/...

nme-ffmpeg/sdk/sdk/ffmpeg-n8.1-latest-win64-lgpl-shared-8.1/bin/...

nme-ffmpeg/sdk/sdk/ffmpeg-n8.1-latest-win64-lgpl-shared-8.1/include/...

And, in this case, the NME_FFMPEG_SDK was set to "${haxelib:nme-ffmpeg}/sdl/ffmpeg-n8.1-latest-win64-lgpl-shared-8.1"

The NME_FFMPEG_SDK should be set when building the ndll.
It should also be set when building an NME project, unless NME_SYSTEM_FFMPEG is set, in which case it is assumed the dlls will be found in the system path.

For macos, you can use homebrew to install ffmpeg, and point the SDK at, eg,  /usr/local/Cellar/ffmpeg/8.1



