#include "../display/Viewport.h"
#include "../display/DisplayHeader.h"

namespace Game {;
Viewport::Viewport(
    int32_t w, 
    int32_t h, 
    VIEWPORT_LOCATION location, 
    ViewportConstraint constraint
)
    : _viewportLocation(location),
    _aspectRatio(0),//aspect_ratio_16_9),
    _constraint(constraint)
{
    _windowWidth = w;
    _windowHeight = h;
    glEnable(GL_SCISSOR_TEST);

    //TODO: load this.
    //_resolution = res_1024x768;//res_1600x900; //Unused
    _currentViewportRect._max.x = w;
    _currentViewportRect._max.y = h;
    _currentViewportRect._min.x = 0;
    _currentViewportRect._min.y = 0;

    setWidth(w); // - Constrains the window to the aspect ratio.
    setHeight(h);

    _lastViewportRect = 0;

    //**Force an initial viewport update
    updateChanged(true);
}
float Viewport::pctW(float f) {
    return (float)((float)getWidth() * (f*0.01));
}
float Viewport::pctH(float f){
    return (float)((float)getHeight() * (f*0.01));
}
Box2f Viewport::getClientQuad(){
    return Box2f(Vec2f(0,0),Vec2f((float)getWidth(),(float)getHeight()));
}
void Viewport::centerViewport()
{
    
    //if( _viewportLocation==VIEWPORT_TOPLEFTCORNER )
    //{
    //    _currentViewportRect._min.x = 0;
    //    _currentViewportRect._min.y = _windowHeight-getHeight();
    //}
    //else if( _viewportLocation==VIEWPORT_CENTER_H )
    //{
    //    if( _windowWidth > getWidth() )
    //        _currentViewportRect._min.x = ( _windowWidth - getWidth() ) / 2;
    //    else 
    //        _currentViewportRect._min.x = 0;
    //}
    //else if( _viewportLocation==VIEWPORT_CENTER_V )
    //{
    //    if( _windowHeight > getHeight() )
    //        _currentViewportRect._min.y = ( _windowHeight - getHeight() ) / 2;
    //    else 
    //        _currentViewportRect._min.y = 0;
    //}
    //else if( _viewportLocation==VIEWPORT_CENTER_HV )
    //{
    //    if( _windowWidth > getWidth() )
    //        _currentViewportRect._min.x = ( _windowWidth - getWidth() ) / 2;
    //    else 
    //        _currentViewportRect._min.x = 0;
    //
    //    if( _windowHeight > getHeight() )
    //        _currentViewportRect._min.y = ( _windowHeight - getHeight() ) / 2;
    //    else 
    //        _currentViewportRect._min.y = 0;
    //}
   // else 
   //     if( _viewportLocation==VIEWPORT_CENTER_NONE )
   // {}

}
// - Set the width / height of the viewport given the window's width/height parameters. (not the viewport's)
void Viewport::setHeight(int32_t h){
    _windowHeight=h;
  //  if( _constraint==VP_MAINTAIN_ASPECT_RATIO ){
  //      // - sets the width relative to the height given the aspect ratio value.
  //      float oldH = (float)_currentViewportRect._max.y;
  //      _currentViewportRect._max.y=h;
  //
  //      _currentViewportRect._max.x = static_cast<int32_t>((float)getHeight() * getAspectRatio());
  //  }
  //  else if( _constraint==VP_FILL_WINDOW ){
  //      // - Setting height here only will fill the window
  //      _currentViewportRect._max.y =h;
  //  }
  //  else if( _constraint==VP_DONOTCHANGE ){
  //  }
    _currentViewportRect._max.y = _windowHeight;

    //if(_height<=1) _height = 1;

    //-*WE CANNOT HAVE THE WIDTH < THE HEIGHT
    if(getWidth()<getHeight())
    {
        setHeight(getWidth()-1);//_width=_height;
    }
    else
        centerViewport();
    

    updateAspectRatio();
  //  updateChanged();
}
void Viewport::setWidth(int32_t w){
    _windowWidth=w;
 //   if( _constraint==VP_MAINTAIN_ASPECT_RATIO ){
 //       float oldW = (float)getWidth();
 //       _currentViewportRect._max.x = w;
 //
 //       _currentViewportRect._max.y = static_cast<int32_t>((float)getWidth() / getAspectRatio());
 //   }
 //   else if( _constraint==VP_FILL_WINDOW ){
 //       _currentViewportRect._max.x=w;
 //   }
 //   else if( _constraint==VP_DONOTCHANGE ){
 //   }
    _currentViewportRect._max.x = _windowWidth;
    //if(_width<=2) _width = 2;

    //-*WE CANNOT HAVE THE WIDTH < THE HEIGHT
    if(getWidth()<getHeight())
    {
        setHeight(getWidth()-1);
        //_width=_height;
    }
    else
        centerViewport();

    updateAspectRatio();
   // updateChanged();
}

int32_t Viewport::getWidth()
{
   // std::lock_guard<std::mutex> guard(_mtClassMtx);
    return _currentViewportRect._max.x;
}
int32_t Viewport::getHeight()
{
    //std::lock_guard<std::mutex> guard(_mtClassMtx);
    return _currentViewportRect._max.y;
}
int32_t Viewport::getX()
{
    return _currentViewportRect._min.x;
}
int32_t Viewport::getY()
{
    return _currentViewportRect._min.y;
}
void Viewport::setX(int32_t x) 
{ 
    _currentViewportRect._min.x = x; 
    updateAspectRatio(); 
}
void Viewport::setY(int32_t y) 
{ 
    _currentViewportRect._min.y = y; 
    updateAspectRatio(); 
}
float Viewport::getAspectRatio()
{ 
    return _aspectRatio; 
}//getAspectRatiof(getRatioForResolution(_resolution)); }
float Viewport::getAspectRatio_1()
{ 
    return _aspectRatio_1; 
}//getAspectRatiof(getRatioForResolution(_resolution)); }

void Viewport::updateAspectRatio()
{
    if(getHeight()==0)
        _aspectRatio=1.0f;
    else
        _aspectRatio = (float)getWidth() / (float)getHeight();
    
    if(_aspectRatio!=0)
        _aspectRatio_1 = 1.0f/_aspectRatio;

    updateReciprocalValues();
}
void Viewport::updateReciprocalValues()
{
    _width_1 = 1.0f / (float)getWidth();
    _height_1 = 1.0f / (float)getHeight();
}
bool Viewport::containsPointRelativeToWindow(vec2& mp)
{
    if(mp.x<0.0f)
        return false;
    if(mp.x>_windowWidth-1)
        return false;
    if(mp.y<0.0f)
        return false;
    if(mp.y>_windowHeight-1)
        return false;

    return true;
}
void Viewport::updateChanged(bool blnForce)
{
    if(_lastViewportRect != _currentViewportRect || blnForce)
    {
        int32_t ix = getX();
        int32_t iy = getY();
        int32_t iw = getWidth();
        int32_t ih = getHeight();


        glViewport(ix, iy, iw, ih);
    
        //Scissor testing
        glScissor(ix, iy, iw, ih);

        _lastViewportRect = _currentViewportRect;
    }
}



}//ns game
