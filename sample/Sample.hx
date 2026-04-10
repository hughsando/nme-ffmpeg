import nme.display.*;
import nff.Video;

class Sample extends Sprite
{
   var video:Video;
   public function new()
   {
      super();

      var args = nme.system.System.getArgs();
      if (args.length == 0)
      {
         trace("Usage: Sample <video file>");
      }
      else
      {
         video = new Video(args[0]);
         trace('Loaded video: $video');
      }
   }


}
