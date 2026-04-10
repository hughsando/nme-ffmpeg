package nff;


class Video
{
   var nffHandle:Dynamic;

   public var width(get,null) : Int;
   public var height(get,null) : Int;
   public var frameRate(get,null) : Float;
   public var duration(get,null) : Float;

   public function new(filename:String)
   {
      nffHandle = nffOpen(filename);
   }

   public function toString() return 'Video($width x $height, $frameRate fps, $duration seconds)';

   public function close() nffRelease(nffHandle);


   function get_width() : Int return nffGetWidth(nffHandle);
   function get_height() : Int return nffGetHeight(nffHandle);
   function get_frameRate() : Float return nffGetFrameRate(nffHandle);
   function get_duration() : Float return nffGetDuration(nffHandle);


   static var nffOpen = Loader.load("nffOpen", "oo");
   static var nffRelease = Loader.load("nffRelease", "ov");
   static var nffGetWidth = Loader.load("nffGetWidth", "oi");
   static var nffGetHeight = Loader.load("nffGetHeight", "oi");
   static var nffGetFrameRate = Loader.load("nffGetFrameRate", "od");
   static var nffGetDuration = Loader.load("nffGetDuration", "od");

}
