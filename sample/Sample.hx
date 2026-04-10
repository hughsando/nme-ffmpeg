import nme.display.*;
import nme.events.*;   
import nff.Video;

class Sample extends Sprite
{
   var video:Video;
   var videoOk = false;
   var bitmapData:BitmapData;
   var bitmap:Bitmap;


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
         bitmapData = video.createBitmapData();
         bitmap = new Bitmap(bitmapData, true);
         addChild(bitmap);
         videoOk = true;
      }

      if (videoOk)
      {
         addEventListener(Event.ENTER_FRAME, (_) -> update());
         stage.addEventListener(Event.RESIZE, (_) -> onResize());

         onResize();
      }
   }

   public function update()
   {
      if (videoOk)
         videoOk = video.getNextFrame(bitmapData);
   }

   public function onResize()
   {
      if (bitmapData!=null)
      {
         var scaleX = stage.stageWidth / bitmapData.width;
         var scaleY = stage.stageHeight / bitmapData.height;
         var scale = Math.min(scaleX, scaleY);
         bitmap.scaleX = bitmap.scaleY = scale;
         bitmap.x = (stage.stageWidth - bitmapData.width * scale) / 2;
         bitmap.y = (stage.stageHeight - bitmapData.height * scale) / 2;
      }
   }

}
