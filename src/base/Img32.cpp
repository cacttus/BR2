#include "../base/Img32.h"
#include "../base/Allocator.h"
#include "../base/Gu.h"

#include "../base/Logger.h"
#include "../base/BinaryFile.h"

namespace BR2 {
Img32::Img32(string_t path, bool bLoad) {
  _strNameOrFilePath = path;
  if (bLoad) {
    //TODO: this
    BRThrowNotImplementedException();
  }
}
Img32::Img32(int w, int h, uint8_t* data) {
  init(w, h, data);
}
Img32::~Img32() {
  releaseData();
}
void Img32::init(int w, int h, uint8_t* data) {
  create(w, h);
  if (data != nullptr) {
    int pixelSizeBytes = 4;
    _pData->copyFrom(data, w * h * pixelSizeBytes);
  }
}
void Img32::fillWithColor(Color4ub c) {
  for (int i = 0; i < getWidth(); ++i) {
    for (int j = 0; j < getHeight(); ++j) {
      setPixel32(i, j, c);
    }
  }
}
Pixel4ub Img32::invert(Pixel4ub p) {
  p.r() = ~p.r();
  p.g() = ~p.g();
  p.b() = ~p.b();
  return p;
}
// boo
void Img32::invert() {
  for (uint32_t i = 0; i < _iWidth; ++i) {
    for (uint32_t j = 0; j < _iHeight; ++j) {
      setPixel32(i, j, invert(getPixel32(i, j)));
    }
  }

}
/**
*   The alloc/release data functions are legacy and have been
*   superceded by various buffers that make use of the memory system indirectly,
*   such as allocBuffer.
*/
void Img32::allocData(size_t size) {
  releaseData();
  _pData = new Allocator<uint8_t>(size);
}
void Img32::releaseData() {
  DEL_MEM(_pData);
}
void Img32::freeData() {
  releaseData();
}
size_t Img32::getSizeBytes() const { return _pData->byteSize(); }              // - Image size  in bytes.

/**
*   @fn create()
*   @brief Discard the old image data and realloc this image.
*/
void Img32::create(int w, int h) {
  releaseData();

  _iWidth = w;        // - Width of the image in pixels.
  _iHeight = h;        // - Height of the image in pixels.
  //bpp=count_format_bytes_per_pixel(format)*8;        // - Bits per pixel.
  int pixSize = 0;

  pixSize = sizeof(Pixel4ub);

  AssertOrThrow2(pixSize == 4);

  int32_t imgsize = w * h * pixSize;    // - Image size in bytes.

  allocData(imgsize);
}
/**
*   @fn toGray()
*   @brief Desaturate this image.
*/

int8_t Img32::toGray(Pixel4ub& in) {
  return (11 * in.r() + 16 * in.g() + 5 * in.b()) / 32;
}
Pixel4ub* Img32::pixelOff(int32_t x, int32_t y) {
  AssertOrThrow2(_pData != NULL);
  AssertOrThrow2(x < (int32_t)_iWidth && x >= 0);
  AssertOrThrow2(y < (int32_t)_iHeight && y >= 0);

  size_t off = vofftos((size_t)x, (size_t)y, (size_t)getWidth());
  off *= getBytesPerPixel();    //StaticBufffer is a char array so we must scale the size

  return  (Pixel4ub*)(_pData->ptrOff(off));//unsafe cast
}
Pixel4ub& Img32::pixelAt32(int32_t x, int32_t y) {
  return  *(pixelOff(x, y));
}
void Img32::setPixel32(int32_t x, int32_t y, Pixel4ub& pix) {
  *(pixelOff(x, y)) = pix;
}
int32_t Img32::hwrap(int32_t off) {
  int32_t ret = off % _iWidth;
  if (ret < 0)
    ret += _iWidth;
  return ret;
}
int32_t Img32::vwrap(int32_t off) {
  int32_t ret = off % _iHeight;
  if (ret < 0)
    ret += _iHeight;
  return ret;
}
/**
*    Applies a 3x3 filter to the image.
*    nPasses - The number of passes to apply
*    wrap - If we are to wrap the filter.
*
*    TODO: this wraps no matter WHAT!!!!!!!!!
*
*/
std::shared_ptr<Img32> Img32::applyFilter(Matrix3x3& kernel, size_t nPasses, bool wrap) {
  if (!_pData)
    return 0;

  //PixelGrid3x3_32 grid;
  ////Pixel_rgb32f pix;
  //Pixel4ub pix8;

  //for (size_t i = 0; i < nPasses; ++i)
  //    for (int y = 0; y < getHeight(); ++y)
  //        for (int x = 0; x < getWidth(); ++x)
  //        {
  //            grid = getGridAt(x, y, wrap);
  //            pix8.r() = (int)(255.0*(
  //                (t_float)grid.p11.r()*kernel._m11 / 255.0 +
  //                (t_float)grid.p12.r()*kernel._m12 / 255.0 +
  //                (t_float)grid.p13.r()*kernel._m13 / 255.0 +
  //                (t_float)grid.p21.r()*kernel._m21 / 255.0 +
  //                (t_float)grid.p22.r()*kernel._m22 / 255.0 +
  //                (t_float)grid.p23.r()*kernel._m23 / 255.0 +
  //                (t_float)grid.p31.r()*kernel._m31 / 255.0 +
  //                (t_float)grid.p32.r()*kernel._m32 / 255.0 +
  //                (t_float)grid.p33.r()*kernel._m33 / 255.0));
  //            pix8.g() = (int)(255.0*(
  //                (t_float)grid.p11.g()*kernel._m11 / 255.0 +
  //                (t_float)grid.p12.g()*kernel._m12 / 255.0 +
  //                (t_float)grid.p13.g()*kernel._m13 / 255.0 +
  //                (t_float)grid.p21.g()*kernel._m21 / 255.0 +
  //                (t_float)grid.p22.g()*kernel._m22 / 255.0 +
  //                (t_float)grid.p23.g()*kernel._m23 / 255.0 +
  //                (t_float)grid.p31.g()*kernel._m31 / 255.0 +
  //                (t_float)grid.p32.g()*kernel._m32 / 255.0 +
  //                (t_float)grid.p33.g()*kernel._m33 / 255.0));
  //            pix8.b() = (int)(255.0*(
  //                (t_float)grid.p11.b()*kernel._m11 / 255.0 +
  //                (t_float)grid.p12.b()*kernel._m12 / 255.0 +
  //                (t_float)grid.p13.b()*kernel._m13 / 255.0 +
  //                (t_float)grid.p21.b()*kernel._m21 / 255.0 +
  //                (t_float)grid.p22.b()*kernel._m22 / 255.0 +
  //                (t_float)grid.p23.b()*kernel._m23 / 255.0 +
  //                (t_float)grid.p31.b()*kernel._m31 / 255.0 +
  //                (t_float)grid.p32.b()*kernel._m32 / 255.0 +
  //                (t_float)grid.p33.b()*kernel._m33 / 255.0));

  //            setPixel32(x, y, pix8);
  //        }


  return 0;
}
/**
*    @fn getGridAt()
*    @brief Returns a grid of pixel pointers from the given location.
*
*
*    TODO: this wraps no matter WHAT!!!!!!!!!
*    use pixel pointers, make a pixelAt32PTR function()
*/
//PixelGrid3x3_32 BaseImage32Spec::getGridAt(int32_t x, int32_t y, bool wrap)
//{
//    PixelGrid3x3_32 grid;
//
//    //11 21 31
//    //12 22 32
//    //13 23 33
//
//    grid.p11 = pixelAt32(x - 1, y - 1);
//    grid.p12 = pixelAt32(x - 1, y);
//    grid.p13 = pixelAt32(x - 1, y + 1);
//    grid.p21 = pixelAt32(x, y - 1);
//    grid.p22 = pixelAt32(x, y);
//    grid.p23 = pixelAt32(x, y + 1);
//    grid.p31 = pixelAt32(x + 1, y - 1);
//    grid.p32 = pixelAt32(x + 1, y);
//    grid.p33 = pixelAt32(x + 1, y + 1);
//
//    //if(x-1<0)
//    //    
//    //else if(wrap)
//    //    grid.p12 = pixelAt(x+getWidth(),y);
//    //else grid.w12=false;
//
//    //if(x+1>=getWidth())
//    //    grid.p32 = pixelAt(x+1,y);
//    //else if(wrap)
//    //    grid.p32 = pixelAt(x-getWidth(),y);
//    //else grid.w32=false;
//
//    //if(y-1<0)
//    //    grid.p21 = pixelAt(x,y-1);
//    //else if(wrap)
//    //    grid.p21 = pixelAt(x,y+getHeight());
//    //else grid.w21=false;
//
//    //if(y+1>=getHeight())
//    //    grid.p23 = pixelAt(x,y+1);
//    //else if(wrap)
//    //    grid.p23 = pixelAt(x,y-getHeight());
//    //else grid.w23=false;
//
//    return grid;
//}

/**
*   clear to a color
*
*/
void Img32::clearTo(Pixel4ub p) {
  int bytes = 4;

  for (uint32_t i = 0; i < _iWidth * _iHeight * (uint32_t)bytes; i += bytes) {
    _pData[i + 2] = p.b();
    _pData[i + 0] = p.r();
    _pData[i + 1] = p.g();
  }
}
/**
*    @fn normalize()
*    @brief Normalizes an input image as in it creates a bump map texture for it.
*    @remarks The returned image data must be handled by the application.  It is
*    not automatically freed.
*    @return GR_OK if successful. GR_FAIL if the factory image is empty.
**/
RetCode Img32::normalize(std::shared_ptr<Img32>& pRet, float depth) {
  if (!_pData) {
    return GR_FAIL;
  }

  AssertOrThrow2(_pData != NULL);

  pRet->copyParams(shared_from_this());
  pRet->allocData(_pData->byteSize());

  for (int i = 0; i < this->getWidth(); ++i) {
    for (int j = 0; j < this->getHeight(); ++j) {
      pRet->setPixel32(j, i, normalizePixel32(i, j, depth));
    }
  }

  return GR_OK;
}
//Note there could be a bug here: we're using a convolution filter to normalize
// but we're referencing the same image we changed.
RetCode Img32::normalize(float depth) {
  if (!_pData) {
    return GR_FAIL;
  }

  std::shared_ptr<Img32> pRet = std::make_shared<Img32>();//Heap

  pRet->copyParams(shared_from_this());
  pRet->allocData(_pData->byteSize());

  for (int i = 0; i < getWidth(); ++i) {
    for (int j = 0; j < getHeight(); ++j) {
      pRet->setPixel32(j, i, normalizePixel32(i, j, depth));
    }
  }

  // Data copy
  delete _pData;
  _pData = pRet->_pData;
  pRet->_pData = NULL;

  pRet = nullptr;//delete pRet;

  return GR_OK;
}
std::shared_ptr<Img32> Img32::normalized(float depth) {
  if (!_pData) {
    BRThrowException(" Image data was null for normalization ");
  }

  std::shared_ptr<Img32> pRet = std::make_shared<Img32>();

  pRet->copyParams(shared_from_this());
  pRet->allocData(_pData->byteSize());

  for (int i = 0; i < this->getWidth(); ++i) {
    for (int j = 0; j < this->getHeight(); ++j) {
      pRet->setPixel32(j, i, normalizePixel32(i, j, depth));
    }
  }

  return pRet;
}
void Img32::copyParams(const std::shared_ptr<Img32> rhs) {
  this->_iWidth = rhs->getWidth();
  this->_iHeight = rhs->getHeight();
}
/**
*
*
*/
Pixel4ub Img32::normalizePixel(int32_t x, int32_t y, float depth) {
  AssertOrThrow2(_pData != NULL);

  Pixel4ub pix;
  int Gh = 0, Gv = 0, i;
  float len;
  int mat[9] = {
      toGray(pixelAt32(hwrap(x - 1),vwrap(y - 1))),toGray(pixelAt32(hwrap(x - 0),vwrap(y - 1))),toGray(pixelAt32(hwrap(x + 1),vwrap(y - 1))),
      toGray(pixelAt32(hwrap(x - 1),vwrap(y + 0))),toGray(pixelAt32(hwrap(x - 0),vwrap(y + 0))),toGray(pixelAt32(hwrap(x + 1),vwrap(y + 0))),
      toGray(pixelAt32(hwrap(x - 1),vwrap(y + 1))),toGray(pixelAt32(hwrap(x - 0),vwrap(y + 1))),toGray(pixelAt32(hwrap(x + 1),vwrap(y + 1)))
  };
  static int sobel_v[9] = {
      -1,    -2,    -1,

      0,    0,    0,

      1,    2,    1
  };
  static int sobel_h[9] = {
      1,    0,    -1,

      2,    0,    -2,

      1,    0,    -1
  };

  for (i = 0; i < 9; ++i)
    Gh += mat[i] * sobel_h[i];
  for (i = 0; i < 9; ++i)
    Gv += mat[i] * sobel_v[i];

  float Fh = (float)(Gh + 1024) / 2040.0f;
  float Fv = (float)(Gv + 1024) / 2040.0f;

  len = sqrtf((float)(Fh * Fh) + (float)(Fv * Fv) + depth * depth);
  Fh /= len;
  Fv /= len;
  depth /= len;

  pix.r() = (t_byte)(BR2::MathUtils::round(Fh * 255.0f));
  pix.g() = (t_byte)(BR2::MathUtils::round(Fv * 255.0f));
  pix.b() = (t_byte)(BR2::MathUtils::round(depth * 255.0f));
  pix.a() = 255;

  return pix;
}
Pixel4ub Img32::normalizePixel32(int32_t x, int32_t y, float depth) {
  AssertOrThrow2(_pData != nullptr);

#define pav(x,y) contrast(pixelAverage24(pixelAt32(hwrap(x),vwrap(y))))

  float mat[9] = {
      pav(x - 1,y - 1),pav(x  ,y - 1),pav(x + 1,y - 1),
      pav(x - 1,y),pav(x  ,y),pav(x + 1,y),
      pav(x - 1,y + 1),pav(x  ,y + 1),pav(x + 1,y + 1)
  };

  //scharr operator
  static float sobel_v1[9] = {
       3, 10, 3,
       0,     0, 0,
      -3,-10,-3
  };
  static float sobel_h1[9] = {
       3, 0,-3,
      10, 0,-10,
       3, 0,-3
  };
  //static int sobel_v2[9] = {
  //    1, 2, 1,
  //    0, 0, 0,
  //   -1,-2,-1
  //};
  //static int sobel_h2[9] = {
  //    -1,    0, 1,
  //    -2,    0, 2,
  //    -1,    0, 1
  //};

  float sumH1 = 0, sumV1 = 0, sumH2 = 0, sumV2 = 0;
  for (int i = 0; i < 9; ++i) {
    sumH1 += mat[i] * sobel_h1[i];
    sumV1 += mat[i] * sobel_v1[i];
    //sumH2 += mat[i]*sobel_h2[i];
    //sumV2 += mat[i]*sobel_v2[i];
  }

  static const float div_1 = 1.0f / 32.0f;

  float fH1 = (sumH1 + 16.0f) * div_1;//max range of this kernel will be [-4,4] - scale into [0,1]
  float fV1 = (sumV1 + 16.0f) * div_1;
  //float fH2 = ((float)sumH2 + 1024.0f)*div_1;//max range of this kernel will be [-1024,1024] - scale into [0,1]
  //float fV2 = ((float)sumV2 + 1024.0f)*div_1;

  // average sobel in both directions.
  // This doesn't work they smooth out to zero in both directions
  //float fH = (fH1 + fH2) *.5;
  //float fV = (fV1 + fV2) *.5;
  //static Vec3f sum(0,0,0);

  //**Due to the problems with normal mapping the y really can't be any different from 1 or else the
  //filter will skew the bump map in +z or +x
  Vec3f v(fH1, .67f, fV1);//problem with bump mapping normals being negative forced us to do this. they are negative when you have a high depth magnitude +-1.0 
              //no values below or at .5
              // 1 = little or no bump
              //.58 - .8 
  v = (v * 2.0f) - 1.0f;//untranslate in order to normalize

  //salt
  v += vec3::normalize(Vec3f(Alg::meft11(x | rand()), 0, Alg::meft11(y | rand()))) * 0.002262058f;

  v.normalize();

  v = (v + 1.0f) / 2.0f;//translate back

  //v = Vec3f(0,1,0);//debug
  //v = (v+1.0f)*0.5f;//store translated so we can convert it in shader. (v+1) / 2 
  v *= 255.0f;

  // - swapping g with b doesn't work. Z points into the screen.
  Pixel4ub pix;
  pix.r() = (t_byte)(v.x);
  pix.g() = (t_byte)(v.y);
  pix.b() = (t_byte)(v.z);
  pix.a() = pixelAt32(hwrap(x), vwrap(y)).a();//allow for transparency don't change!

  return pix;
}

/**
*    flipH
*    Flip image horizontally
*/
void Img32::flipH() {
  Allocator<uint8_t>* st = new Allocator<uint8_t>(_pData->byteSize());
  size_t pSize = getBytesPerPixel();
  size_t rowsize = pSize * getWidth();

  // Swap the scanlines
  for (int v = 0; v < getHeight(); ++v) {
    for (int h = 0; h < getWidth(); ++h) {
      memcpy(st->ptr() + v * rowsize + pSize * h, _pData->ptrOff(v * rowsize + pSize * (getWidth() - h - 1)), pSize);
    }
  }

  _pData->copyFrom(st);
  delete st;
}
/**
*    flipV
*    Flip image vertically
*/
void Img32::flipV() {
  Allocator<uint8_t>* st = new Allocator<uint8_t>(_pData->byteSize());

  size_t rowsize = getBytesPerPixel() * getWidth();
  size_t h = getHeight();

  // Swap the scanlines
  for (int v = 0; v < this->getHeight(); ++v) {
    memcpy(st->ptr() + v * rowsize, _pData->ptrOff((h - v - 1) * rowsize), rowsize);
  }

  _pData->copyFrom(st);
  delete st;
}

void Img32::shiftV(int numPixels, Img32& __out_ bi) {
  //*In order to shift LEFT you can just add negative width pixels
  //*image width - 1, -2, etc..
  //Create the new image and copy it from this image.
//   bi.create(getWidth(), getHeight());
//   bi.getData()->copyFrom(getData());

  if (numPixels >= getWidth()) {
    numPixels = numPixels % getWidth();
  }

  size_t pixSize = getBytesPerPixel();
  size_t rowsize = pixSize * getWidth();
  size_t shiftSize = rowsize * numPixels;

  size_t begA;
  size_t begSiz;
  size_t begB;
  uint8_t* ptA;
  uint8_t* ptB;
  size_t totSize = (getHeight()) * rowsize;

  begA = 0;
  begSiz = shiftSize;
  begB = totSize - shiftSize;

  ptA = _pData->ptr() + begA;
  ptB = bi.getData()->ptr() + begB;
  memcpy(ptB, ptA, begSiz);

  begA = shiftSize;
  begSiz = totSize - shiftSize;
  begB = 0;

  ptA = _pData->ptr() + begA;
  ptB = bi.getData()->ptr() + begB;
  memcpy(ptB, ptA, begSiz);
  Gu::checkMemory();
}

void Img32::shiftH(int numPixels, Img32& __out_ bi) {
  //*In order to shift LEFT you can just add negative width pixels
  //*image width - 1, -2, etc..
  //Create the new image and copy it from this image.
 // bi.create( getWidth(), getHeight() );
//  bi.getData()->copyFrom( getData() );

  if (numPixels >= getWidth()) {
    numPixels = numPixels % getWidth();
  }

  size_t pixSize = getBytesPerPixel();
  size_t rowsize = pixSize * getWidth();
  size_t shiftSize = numPixels * pixSize;

  size_t begA;
  size_t begSiz;
  size_t begB;
  uint8_t* ptA;
  uint8_t* ptB;
  // Swap the scanlines
  for (int h = 0; h < getHeight(); ++h) {
    //Wipe the left part right, then copy the second right part to the left.
    begA = (h * rowsize);
    begSiz = rowsize - shiftSize;
    begB = (h * rowsize) + shiftSize;

    ptA = _pData->ptr() + begA;
    ptB = bi.getData()->ptr() + begB;
    memcpy(ptB, ptA, begSiz);

    //Copy second part.
    begA = (h * rowsize) + rowsize - shiftSize;
    begSiz = shiftSize;
    begB = (h * rowsize);

    ptA = _pData->ptr() + begA;
    ptB = bi.getData()->ptr() + begB;
    memcpy(ptB, ptA, begSiz);
  }
}
void Img32::wipeH(int numPixels, Img32& __out_ bi) {
  //Create the new image and copy it from this image.
  //Analog of ShiftH
//  bi.create(getWidth(), getHeight());
//  bi.getData()->copyFrom(getData());

  if (numPixels >= getWidth()) {
    numPixels = numPixels % getWidth();
  }

  size_t pixSize = getBytesPerPixel();
  size_t rowsize = pixSize * getWidth();
  size_t shiftSize = numPixels * pixSize;

  // Swap the scanlines
  for (int h = 0; h < getHeight(); ++h) {
    size_t begA = (h * rowsize);
    size_t begSiz = rowsize - shiftSize;
    size_t begB = (h * rowsize) + shiftSize;

    // size_t shiftA = offB - offA;

    uint8_t* ptA = _pData->ptr() + begA;
    uint8_t* ptB = bi.getData()->ptr() + begB;
    memcpy(ptB, ptA, begSiz);
  }
}
void Img32::wipeV(int numPixels, Img32& __out_ bi) {
  //Create the new image and copy it from this image.
  //Analog of ShiftV
//  bi.create(getWidth(), getHeight());
 // bi.getData()->copyFrom(getData());

  if (numPixels >= getWidth()) {
    numPixels = numPixels % getWidth();
  }

  size_t pixSize = getBytesPerPixel();
  size_t rowsize = pixSize * getWidth();
  size_t shiftSize = rowsize * numPixels;

  size_t begA;
  size_t begSiz;
  size_t begB;
  uint8_t* ptA;
  uint8_t* ptB;
  begA = 0;
  begSiz = shiftSize;
  begB = (getHeight()) * rowsize - shiftSize; //data size - shiftSize

  ptA = _pData->ptr() + begA;
  ptB = bi.getData()->ptr() + begB;
  memcpy(ptB, ptA, begSiz);

}
void Img32::flipRB() {
  flipRB32();
}

//Flip red/blue for TGA
void Img32::flipRB32() {
  int32_t h = getHeight();
  int32_t w = getWidth();
  Pixel4ub pix;

  // Swap the colors
  for (int32_t iH = 0; iH < h; ++iH) {
    for (int32_t iW = 0; iW < w; ++iW) {
      pix = getPixel32(iW, iH);
      int8_t tmp = pix.r();
      pix.r() = pix.b();
      pix.b() = tmp;
      setPixel32(iW, iH, pix);
    }
  }
}
std::shared_ptr<Img32> Img32::copySubImageTo(Vec2i& off, Vec2i& size) {
  std::shared_ptr<Img32> ret = std::make_shared<Img32>();
  ret->create(size.x, size.y);

  ret->copySubImageFrom(ivec2(0, 0), off, size, shared_from_this());

  return ret;
}
//Image formats must be identical
void Img32::copySubImageFrom(Vec2i& myOff, Vec2i& otherOff, Vec2i& size, std::shared_ptr<Img32> pOtherImage) {
  if (getData() == NULL) {
    BRThrowException("Copy SubImage 2 - From image was not allocated");
  }
  if (pOtherImage == NULL) {
    BRThrowException("Copy SubImage 1 - Input Image was null.");
  }
  if (pOtherImage->getData() == NULL) {
    BRThrowException("Copy SubImage 3 - Input Image TO was not allocated");
  }
  //size constraint validation
  if (myOff.x < -1 || myOff.y < -1) {
    BRThrowException("Copy SubImage 4");
  }
  if (myOff.x >= (int)_iWidth || myOff.y >= (int)_iHeight) {
    BRThrowException("Copy SubImage 5.  This hits if you put too many textures in the db_atlas.dat file.  \
        There can only be XxX textres (usually 16x16)");
  }
  if (otherOff.x < 0 || otherOff.y < 0) {
    BRThrowException("Copy SubImage 6");
  }
  if (otherOff.x >= pOtherImage->getWidth() || otherOff.y >= pOtherImage->getHeight()) {
    BRThrowException("Copy SubImage 7");
  }

  Vec2i scanPos = myOff;
  int32_t scanLineByteSize = size.x * getBytesPerPixel();
  int32_t nLines = size.y;

  AssertOrThrow2(scanLineByteSize >= 0);

  for (int iScanLine = 0; iScanLine < nLines; ++iScanLine) {
    void* vdst = (void*)pixelOff(scanPos.x, scanPos.y + iScanLine);
    void* vsrc = (void*)pOtherImage->pixelOff(otherOff.x, otherOff.y + (nLines - iScanLine - 1));//Note: we do this here because the tga images are flipped upside down for some reason in the texture composer.
    memcpy(vdst, vsrc, scanLineByteSize);
  }
  _CrtCheckMemory();
}

void Img32::convertDataFrom24BitTo32Bit(t_byte defaultAlpha) {
  AssertOrThrow2(_pData != NULL);

  // - apply alpha channel.
  Allocator<uint8_t>* newBuffer = new Allocator<uint8_t>(getWidth() * getHeight() * sizeof(Pixel4ub));

  int c3size = sizeof(Pixel3ub);
  int c4size = sizeof(Pixel4ub);
  Pixel3ub* c3;
  Pixel4ub* c4;

  int32_t iw = getWidth();

  for (int32_t iy = 0; iy < getHeight(); ++iy)
    for (int32_t ix = 0; ix < getWidth(); ++ix) {
      //Do not use any offset methods here.
      c3 = (Pixel3ub*)((_pData->ptr() + c3size * (iw * iy + ix)));
      c4 = (Pixel4ub*)((newBuffer->ptr() + c4size * (iw * iy + ix)));
      c4->r() = c3->r();
      c4->g() = c3->g();
      c4->b() = c3->b();
      c4->a() = defaultAlpha;
    }

  DEL_MEM(_pData);
  _pData = newBuffer;

}
void Img32::flipImage20161206(uint8_t* image, int width, int height) {
  int rowSiz = width * 4;

  uint8_t* rowTmp1 = new uint8_t[rowSiz];
  int h2 = height / 2;

  for (int j = 0; j < h2; ++j) {
    uint8_t* ptRowDst = image + rowSiz * j;
    uint8_t* ptRowSrc = image + rowSiz * (height - j - 1);

    memcpy(rowTmp1, ptRowDst, rowSiz);
    memcpy(ptRowDst, ptRowSrc, rowSiz);
    memcpy(ptRowSrc, rowTmp1, rowSiz);
  }

  delete[] rowTmp1;
}


//Needed for some parts of SDL
int32_t Img32::getPitch() const {
  int32_t pitch = getBytesPerPixel() * getWidth();
  return pitch;
}
int32_t Img32::getRMask() const {
  if (Gu::isBigEndian()) {
    return 0xff000000;
  }
  else {
    return 0xff000000;//ARGB
  }
}
int32_t Img32::getGMask() const {
  if (Gu::isBigEndian()) {
    return 0x00ff0000;
  }
  else {
    return 0x00ff0000;
  }
}
int32_t Img32::getBMask() const {
  if (Gu::isBigEndian()) {
    return 0x0000ff00;
  }
  else {
    return 0x0000ff00;
  }
}
int32_t Img32::getAMask() const {
  if (Gu::isBigEndian()) {
    return 0x000000ff;
  }
  else {
    return 0x000000ff;
  }
}
bool Img32::parseImagePatch(std::shared_ptr<Img32> master, std::vector<std::shared_ptr<Img32>>& ret, bool vFlip) {
  //Returns false if there was an error.
  //NOTE: you'll have to flip the image Vertically because we automatically flip images when we load them (for OpenGL)
  bool bRet = true;

  //We gonna split img into  patches.
  std::vector<int32_t> vecXMarkers;
  std::vector<int32_t> vecYMarkers;
  vecXMarkers.push_back(1);
  vecYMarkers.push_back(1);

  bool lastw = false;
  int nTileMarkersW = 0;
  int nTileMarkersH = 0;

  if (vFlip) {
    master->flipV();
  }


  //Get Tile Markers
  for (int x = 0; x < master->getWidth(); ++x) {
    Pixel4ub* pix = master->pixelOff(x, 0);
    if ((pix->w > 127) != lastw) {
      nTileMarkersW++;
      vecXMarkers.push_back(x);
      lastw = (pix->w > 127);
    }
  }
  for (int y = 0; y < master->getHeight(); ++y) {
    Pixel4ub* pix = master->pixelOff(0, y);

    if ((pix->w > 127) != lastw) {
      nTileMarkersH++;
      vecYMarkers.push_back(y);

      lastw = (pix->w > 127);
    }
  }
  //Add image height (if it isn't)
  if (vecYMarkers[vecYMarkers.size() - 1] != master->getHeight() - 1) {
    vecYMarkers.push_back(master->getHeight() - 1);
  }
  if (vecXMarkers[vecXMarkers.size() - 1] != master->getWidth() - 1) {
    vecXMarkers.push_back(master->getWidth() - 1);
  }

  //Confirm X + Y markers are ordered
  for (size_t ix = 1; ix < vecXMarkers.size(); ix++) {
    if (vecXMarkers[ix] < vecXMarkers[ix - 1]) {
      //Error
      BRLogError("Image Patch makers were out of order ");
      bRet = false;
      //Gui2d::error(TStr("Image Patch makers were out of order for ", file));
      vecXMarkers.erase(vecXMarkers.begin() + ix);
      ix--;
    }
  }
  for (size_t iy = 1; iy < vecYMarkers.size(); iy++) {
    if (vecYMarkers[iy] < vecYMarkers[iy - 1]) {
      //Error
      BRLogError("Image Patch makers were out of order ");
      bRet = false;
      // Gui2d::error(TStr("Image Patch makers were out of order for ", file));
      vecYMarkers.erase(vecYMarkers.begin() + iy);
      iy--;
    }
  }

  // Cut the imge
  std::shared_ptr<Img32> img;
  ivec2 voff, vsiz;
  for (size_t iy = 1; iy < vecYMarkers.size(); iy++) {
    for (size_t ix = 1; ix < vecXMarkers.size(); ix++) {
      int32_t x0 = vecXMarkers[ix - 1];
      int32_t x1 = vecXMarkers[ix];
      int32_t y0 = vecYMarkers[iy - 1];
      int32_t y1 = vecYMarkers[iy];

      if (x1 > x0&& y1 > y0) {
        voff.construct(x0, y0);
        vsiz.construct(x1 - x0, y1 - y0);
        img = master->copySubImageTo(voff, vsiz);
        ret.push_back(img);//std::make_shared<Texture2DSpec>(img, Gu::getGraphicsContext(), eFilter));
      }
      else {
        //Invalid Patch segment
        Gu::debugBreak();
      }
    }
  }

  return bRet;

}

void Img32::serialize(std::shared_ptr<BinaryFile> bf) {
  size_t nbytes = getData()->byteSize();
  bf->writeString(std::move(_strNameOrFilePath));
  bf->writeUint32(std::move((uint32_t)_iWidth));
  bf->writeUint32(std::move((uint32_t)_iHeight));
  bf->writeUint32(std::move((uint32_t)nbytes));
  bf->write((const char*)getData()->ptr(), nbytes);
}
void Img32::deserialize(std::shared_ptr<BinaryFile> bf) {
  bf->readString(_strNameOrFilePath);
  bf->readUint32(_iWidth);
  bf->readUint32(_iHeight);
  uint32_t nBytes = 0;
  bf->readUint32(nBytes);
  create(_iWidth, _iHeight);

  //So data->size should be nBytse.
  if (getData()->byteSize() != nBytes) {
    BRLogError("Deserializing image '" + _strNameOrFilePath + "'.. image too big.");
  }
  else if (getData()->ptr() == nullptr) {
    BRLogError("Deserializing image, data not allocated.");
  }
  else {
    bf->read((const char*)getData()->ptr(), nBytes);
  }

}














}//ns game