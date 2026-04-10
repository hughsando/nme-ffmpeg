import nme.display.*;
import nff.Video;

class Sample extends Sprite
{
   var video:Video;
   public function new()
   {
      super();
      video = new Video("filename.mp4");
      trace('Loaded video: $video');
   }


}
