package nff;


class Video
{
   var nffHandle:Dynamic;

   public var width(get,null) : Int;
   public var height(get,null) : Int;

   public function new(filename:String)
   {
      nffHandle = nffOpen(filename);
   }

   public function toString() return 'Video($nffHandle,$width x $height)';

   public function close() nffRelease(nffHandle);


   function get_width() : Int return nffGetWidth(nffHandle);
   function get_height() : Int return nffGetHeight(nffHandle);


   static var nffOpen = Loader.load("nffOpen", "oo");
   static var nffRelease = Loader.load("nffRelease", "ov");
   static var nffGetWidth = Loader.load("nffGetWidth", "oi");
   static var nffGetHeight = Loader.load("nffGetHeight", "oi");

}
