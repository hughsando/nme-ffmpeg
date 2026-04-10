package nff;
import nme.display.BitmapData;


class Video
{
   var nffHandle:Dynamic;

   public var width(get,null) : Int;
   public var height(get,null) : Int;
   public var frameRate(get,null) : Float;
   public var duration(get,null) : Float;
   public var position(get,set) : Float;
   public var pixelAspectRatio(get,null) : Float;

   public function new(filename:String)
   {
      nffHandle = nffOpen(filename);
   }

   public function toString() return 'Video($width x $height, $frameRate fps, $duration seconds)';

   public function createBitmapData() : BitmapData
   {
      return new BitmapData(width, height, false, 0x000000);
   }

   public function getNextFrame(bitmapData:BitmapData) : Bool
   {
      return nffGetNextFrame(nffHandle, bitmapData.nmeHandle);
   }

   public function close() nffRelease(nffHandle);


   function get_width() : Int return nffGetWidth(nffHandle);
   function get_height() : Int return nffGetHeight(nffHandle);
   function get_frameRate() : Float return nffGetFrameRate(nffHandle);
   function get_duration() : Float return nffGetDuration(nffHandle);
   function get_position() : Float return nffGetPosition(nffHandle);
   function set_position(value:Float) : Float { nffSetPosition(nffHandle, value); return value; }
   function get_pixelAspectRatio() : Float return nffGetPixelAspectRatio(nffHandle);



   static var nffOpen = Loader.load("nffOpen", "oo");
   static var nffRelease = Loader.load("nffRelease", "ov");
   static var nffGetWidth = Loader.load("nffGetWidth", "oi");
   static var nffGetHeight = Loader.load("nffGetHeight", "oi");
   static var nffGetFrameRate = Loader.load("nffGetFrameRate", "od");
   static var nffGetDuration = Loader.load("nffGetDuration", "od");
   static var nffGetPosition = Loader.load("nffGetPosition", "od");
   static var nffSetPosition = Loader.load("nffSetPosition", "odv");
   static var nffGetNextFrame = Loader.load("nffGetNextFrame", "oob");
   static var nffGetPixelAspectRatio = Loader.load("nffGetPixelAspectRatio", "od");

}
