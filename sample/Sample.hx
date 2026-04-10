import nme.display.*;
import nme.events.*;   
import nff.Video;

class Sample extends Sprite
{
   var video:Video;
   var videoOk = false;
   var bitmapData:BitmapData;
   var bitmap:Bitmap;
   var seekBar:Sprite;
   var dragging = false;
   var dragPosition = 0.0;


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

      seekBar = new Sprite();
      addChild(seekBar);
      seekBar.addEventListener(MouseEvent.MOUSE_DOWN, beginSeekDrag);
      seekBar.addEventListener(MouseEvent.MOUSE_UP, endSeekDrag);

      if (videoOk)
      {
         addEventListener(Event.ENTER_FRAME, (_) -> update());
         stage.addEventListener(Event.RESIZE, (_) -> onResize());

         onResize();
      }

   }

   function beginSeekDrag(e:MouseEvent)
   {
      dragging = true;
      stage.addEventListener(MouseEvent.MOUSE_MOVE, seekDrag);
   }

   function endSeekDrag(e:MouseEvent)
   {
      dragging = false;
      stage.removeEventListener(MouseEvent.MOUSE_MOVE, seekDrag);
      video.position = dragPosition;
   }

   function seekDrag(e:MouseEvent)
   {
      var mouseX = e.stageX;
      var newPosition = (mouseX - 6) / (stage.stageWidth - 12) * video.duration;
      if (newPosition < 0) newPosition = 0;
      if (newPosition > video.duration) newPosition = video.duration;
      dragPosition = newPosition;
   }

   public function update()
   {
      if (videoOk)
      {
         videoOk = video.getNextFrame(bitmapData);
         var gfx = seekBar.graphics;
         gfx.clear();
         gfx.lineStyle(0.5, 0x000080);
         gfx.beginFill(0x000000, 0.1);
         gfx.drawRect(5.5, stage.stageHeight - 24.5, stage.stageWidth - 10, 20);
         gfx.lineStyle();

         gfx.beginFill(0x800000, 0.5);
         var barWidth = (stage.stageWidth-13)* (video.position / video.duration);
         gfx.drawRect(7, stage.stageHeight - 23, barWidth, 17);

         if (dragging)
         {
            gfx.endFill();
            gfx.lineStyle(1, 0xffffff);
            var dragBarPos = (stage.stageWidth-12)* (dragPosition / video.duration);
            gfx.moveTo(dragBarPos + 6, stage.stageHeight - 24);
            gfx.lineTo(dragBarPos + 6, stage.stageHeight - 6);
         }
      }
   }

   public function onResize()
   {
      if (bitmapData!=null)
      {
         var aspect = video.pixelAspectRatio;
         var scaleX = stage.stageWidth / bitmapData.width / aspect;
         var scaleY = stage.stageHeight / bitmapData.height;
         var scale = Math.min(scaleX, scaleY);
         bitmap.scaleX = scale * aspect;
         bitmap.scaleY = scale;
         bitmap.x = (stage.stageWidth - bitmapData.width * scale * aspect) / 2;
         bitmap.y = (stage.stageHeight - bitmapData.height * scale) / 2;
      }
   }

}
