#include "../base/GLContext.h"
#include "../base/InputManager.h"
#include "../base/Gu.h"

#include "../math/Box2x.h"
#include "../math/Vec4x.h"

#include "../gfx/TexCache.h"
#include "../gfx/CameraNode.h"
#include "../gfx/RenderViewport.h"
#include "../gfx/FrustumBase.h"
#include "../gfx/ShaderBase.h"
#include "../gfx/Picker.h"
#include "../gfx/ShaderBase.h"
#include "../gfx/ShaderMaker.h"
#include "../gfx/RenderUtils.h" 
#include "../base/Img32.h"
#include "../gfx/UiBase.h"
#include "../gfx/UiControls.h"

#include "../model/MeshNode.h"
#include "../model/MeshSpec.h"
#include "../model/UtilMeshInline.h"
#include "../model/TextBufferMesh.h"


namespace BR2 {
std::shared_ptr<UiBorderDim> UiBorderDim::create(uDim d0, uDim d1, uDim d2, uDim d3) {
  std::shared_ptr<UiBorderDim> ret = std::make_shared<UiBorderDim>();
  ret->_border[0] = d0;
  ret->_border[1] = d1;
  ret->_border[2] = d2;
  ret->_border[3] = d3;
  return ret;
}
Orientation::e Orientation::parse(std::string str) {
  if (StringUtil::equals(str, "Horizontal")) { return Orientation::e::Horizontal; }
  else if (StringUtil::equals(str, "Vertical")) { return Orientation::e::Vertical; }
  return e::Horizontal;
}
std::string Orientation::toString(Orientation::e eOr) {
  if (eOr == Orientation::e::Horizontal) { return "Horizontal"; }
  else if (eOr == Orientation::e::Vertical) { return "Vertical"; }
  return "Invalid Enum!";
}
//////////////////////////////////////////////////////////////////////////
void UiDragInfo::update(std::shared_ptr<InputManager> pFingers) {
  float mw = 1.0f / UiScreen::getDesignMultiplierW();
  float mh = 1.0f / UiScreen::getDesignMultiplierH();

  if (_bDragStart) {
    if (pFingers->getLmbState() == ButtonState::e::Up) {
      //Avoid sticking
      _bDrag = false;
      _bDragStart = false;
    }
    else {
      //Check for mouse delta to prevent unnecessary updates.
      vec2 dp = pFingers->getMousePos() - _vDragStart;
      if (MathUtils::fuzzyEquals(dp.x, 0.0f) == false || MathUtils::fuzzyEquals(dp.y, 0.0f) == false) {
        _bDrag = true;
      }
      else {
        _bDrag = false;
      }
      if (_bDrag) {
        //Multiply the distance by the design size.
        dp.x *= mw;
        dp.y *= mh;

        _func(dp);

        //Reset drag start
        _vDragStart = pFingers->getMousePos();
      }
    }
  }
}

#pragma region uDim
float uDim::px(float p) {
  _eDimUnit = UiDimUnit::e::Pixel;
  _units = p;
  return _units;
}
bool UiParser::parseSize(std::string cssSize, float& __out_ fsize, UiDimUnit::e& __out_ eunit) {
  std::string num;
  std::string unit;
  bool bunit = true;
  for (size_t i = 0; i < cssSize.length(); ++i) {
    char c = cssSize[i];
    if (c == ';') {
      break;
    }
    else if (bunit && (isdigit(c) || c == '.' || c == '-' || c == '+' || c == 'e')) {
      num += c;
    }
    else {
      bunit = true;
      unit += c;
    }
  }
  bool bSize = true, bUnit = true;
  eunit = parseLayoutUnit(unit, bUnit);

  if (eunit == UiDimUnit::e::Auto) {
    //Auto: avergae of remaining parent width for all elements
    bSize = true;
    fsize = 8.0085f; // Not Used, but prevents weird values while debugging.
  }
  else {
    fsize = TypeConv::strToFloat(num, bSize);
  }
  return bSize && bUnit;
}
UiDimUnit::e UiParser::parseLayoutUnit(std::string str, bool& bSuccess) {
  if (StringUtil::equalsi(str, "px")) {
    bSuccess = true;
    return UiDimUnit::e::Pixel;
  }
  else if (StringUtil::equalsi(str, "%")) {
    bSuccess = true;
    return UiDimUnit::e::Percent;
  }
  else if (StringUtil::equalsi(str, "auto")) {
    bSuccess = true;
    return UiDimUnit::e::Auto;
  }
  //else if (StringUtil::equalsi(str, "initial")) {
  //    bSuccess = true;
  //    return DimUnit::e::Initial;
  //}
  else {
    UiScreen::error(Stz "Invalid layout unit " + str);
    bSuccess = false;
    return UiDimUnit::e::Pixel;
  }
}
//////////////////////////////////////////////////////////////////////////
uDim::uDim(char* str) : uDim(std::string(str)) {
  if (str == 0) {
    UiScreen::error("Passed 0 in for a char* string to uDim");
  }
}
uDim::uDim(std::string str) {
  setSize(std::string(str));
}
uDim::uDim(float val, UiDimUnit::e eUnit) {
  _units = val;
  _eDimUnit = eUnit;
  debugThunk();
}
void uDim::debugThunk() {
#ifdef _DEBUG
  return;
  //ContentContainer is huge..this will catch huge values
  float f = fabsf(_units);
  int i = (int)f;
  if ((i > 1000) && (i != 999999)) {
    int nnn = 0;
    nnn++;
  }
#endif
}
uDim& uDim::operator=(float f) {
  _units = f;
  debugThunk();
  return *this;
}
uDim& uDim::operator=(int f) {
  _units = (float)f;
  debugThunk();
  return *this;
}
uDim& uDim::operator=(const uDim& rhs) {
  _units = rhs._units;
  _eDimUnit = rhs._eDimUnit;
  debugThunk();
  return *this;
}
uDim& uDim::operator=(std::string str) {
  setSize(std::string(str));
  return *this;
}
uDim& uDim::operator=(char* str) {
  //So the error here is int sometimes gets casted as an char*
  //we should probably fix this riht?
  AssertOrThrow2(str != nullptr);
  setSize(std::string(str)); return *this;
}
void uDim::setSize(std::string str) {
  UiDimUnit::e eunit;
  float fs;
  if (UiParser::parseSize(str, fs, eunit)) {
    if (eunit == UiDimUnit::e::Percent && ((fs < 0) || (fs > 100))) {
      UiScreen::error(Stz "Dim %units were not within 0/100" + fs);
    }
    _units = fs;
    _eDimUnit = eunit;
  }
  else {
    UiScreen::error("Failed to parse seize.");
  }

  debugThunk();
}
float uDim::pctOf(float f) {
  return (_units * 0.01f) * f;
}//Supply the width/height of the parent element.
uDim uDim::autoHeight(std::shared_ptr<UiTex> tex) {
  if (_eDimUnit == UiDimUnit::e::Pixel) {
    if (tex->getWidth() != 0) {
      float ratio = (float)tex->getHeight() / (float)tex->getWidth();
      return uDim(px() * ratio, UiDimUnit::e::Pixel);
    }
    else {
      return uDim(0.0, UiDimUnit::e::Pixel);
    }
  }
  else {
    UiScreen::error("Tried to auto-height a non-pixel udim");
  }
  return uDim(1.0f, UiDimUnit::e::Pixel);
}
uDim uDim::autoWidth(std::shared_ptr<UiTex> tex) {
  if (_eDimUnit == UiDimUnit::e::Pixel) {
    if (tex->getHeight() != 0) {
      float ratio = (float)tex->getWidth() / (float)tex->getHeight();
      return uDim(px() * ratio, UiDimUnit::e::Pixel);
    }
    else {
      return uDim(0.0, UiDimUnit::e::Pixel);
    }
  }
  else {
    UiScreen::error("Tried to auto-height a non-pixel udim");
  }
  return uDim(1.0f, UiDimUnit::e::Pixel);
}
#pragma endregion





}//ns Game
