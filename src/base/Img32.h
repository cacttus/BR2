/**
*    @file BaseImage.h
*    //@file imgImage.h
*
*    @author Derek Page
*
*    © 2011
*    @date 7 / 9 / 2008
*    modified: 2/21/2010
*
*/
#pragma once
#ifndef __BASE_IMAGE_H__
#define __BASE_IMAGE_H__
#include "../base/BaseHeader.h"
#include "../math/MathAll.h"
#include "../base/RetCode.h"
namespace Game {
;
/**
*    @class BaseImage
*    @brief The base class for images.  The BaseImage class also includes
*       functions for convolution filtering.
*   Conga Engine: All images are 32 bit
*   All images are loaded from "PNG" And are RGBA format.
*/
class Img32 : public VirtualMemoryShared<Img32> {
protected:
    typedef uint8_t Ele;
    // std::shared_ptr<GLContext> _pContext;

    uint32_t _iWidth = 0;        // - Width of the image in pixels.
    uint32_t _iHeight = 0;        // - Height of the image in pixels.
    Allocator<uint8_t>* _pData = nullptr;
    t_string _strNameOrFilePath;

    virtual void freeData();    // - Override from ResourceData
    void releaseData();
    void allocData(size_t size);

public:
    Img32() {}
    Img32(t_string path, bool bLoad = false);    // - Creates this class with no image data.
    Img32(int w, int h, uint8_t* data = nullptr);    // - Creates this class with no image data.
    virtual ~Img32() override;

    void init(int w, int h, uint8_t* data = nullptr);
    size_t getSizeBytes() const;
    int32_t getWidth() const { return _iWidth; }                             // - Get the width of the image.
    int32_t getHeight() const { return _iHeight; }                           // - Get the height of the image.
    int32_t getBitsPerPixel() const { return getBytesPerPixel() * 8; }        // - Get the bits per pixel of the image.
    int32_t getBytesPerPixel() const { return 4; }                           // - Get the bits per pixel of the image.
    Allocator<uint8_t>* getData() const { return _pData; }


    int32_t getPitch() const;//Needed for some parts of SDL
    int32_t getRMask() const;
    int32_t getGMask() const;
    int32_t getBMask() const;
    int32_t getAMask() const;
    t_string getName() { return _strNameOrFilePath; }

    // Copy
    void clearTo(Pixel4ub);  // Clear the image to a single color
    void copyParams(const std::shared_ptr<Img32> rhs);    // - Copy parameters from another image, allocates data, does not copy the data
    void create(int w, int h);    // - Discards the old data for this image and creates a new image.
    void copySubImageFrom(Vec2i& myOff, Vec2i& otherOff, Vec2i& otherSize, std::shared_ptr<Img32> pOtherImage);
    std::shared_ptr<Img32> copySubImageTo(Vec2i& off, Vec2i& size);

    Pixel4ub* pixelOff(int32_t x, int32_t y);
    Pixel4ub& pixelAt32(int32_t x, int32_t y);    // - Returns 32-bit pixel (warning: must be a 32-bit image)
    inline Pixel4ub& getPixel32(int32_t x, int32_t y) { return pixelAt32(x, y); }
    void setPixel32(int32_t x, int32_t y, Pixel4ub& pix);
    void fillWithColor(Color4ub c); // - Fills the image with the given color.

    // Filters / Ops
    std::shared_ptr<Img32> applyFilter(Matrix3x3& kernel, size_t nPasses = 1, bool wrap = true);    // - Returns a new image which is the filtered image ** note that the new image will be the same size as the original image.
    RetCode normalize(std::shared_ptr<Img32>& _in, float depth = 1.0); // - Returns an image that is normalized.  Useful for normal maps.
    std::shared_ptr<Img32> normalized(float depth = 1.0); // - Returns an image that is normalized.  Useful for normal maps.
    RetCode normalize(float depth = 1.0); // - Returns an image that is normalized.  Useful for normal maps.
    Pixel4ub normalizePixel(int32_t x, int32_t y, float depth = 1.0);  // - Returns a normalized pixel.
    Pixel4ub normalizePixel32(int32_t x, int32_t y, float depth = 1.0); // 32 bit versrion (alpha channel is preserved)
    t_byte toGray(Pixel4ub& in);  // returns the gray value of this pixel.
    static Pixel4ub    invert(Pixel4ub); // - Inverts a pixel
    void invert();    // - Inverts the entire image
    void convertDataFrom24BitTo32Bit(t_byte defaultAlpha = 255);//try to change to given format.  if same do nothing.
    static float pixelAverage24(Pixel4ub& in);
    static float contrast(float x, float C = 0.79378f);

    //Wrap/flip
    int32_t hwrap(int32_t off);    // - Wrap an offset into the image
    int32_t vwrap(int32_t off);
    void flipH();    // - Flip image horizontally.
    void flipV();    // - Flip image Vertically.
    void flipRB();
    void flipRB32();    // - Flip Red/Blue color values. = 32 biot
    void wipeH(int numPixels, Img32& __out_ bi); // produces a "wipe" animation
    void wipeV(int numPixels, Img32& __out_ bi); // produces a "wipe" animation

    void shiftH(int numPixels, Img32& __out_ bi);
    void shiftV(int numPixels, Img32& __out_ bi);

    static void flipImage20161206(uint8_t* image, int width, int height);
    static bool parseImagePatch(std::shared_ptr<Img32> master, std::vector<std::shared_ptr<Img32>>& ret, bool vFlip = true);

    void serialize(std::shared_ptr<BufferedFile> bf);
    void deserialize(std::shared_ptr<BufferedFile> fb);
};



FORCE_INLINE float Img32::pixelAverage24(Pixel4ub& inpx)
{
    static const float f_13 = (1.0f / 3.0f);
    static const float c_255 = (1.0f / 255.0f);
    return (
        (float)(inpx.r())*c_255 +
        (float)(inpx.g())*c_255 +
        (float)(inpx.b())*c_255)*f_13; // (r+g+b) /3
}

FORCE_INLINE float Img32::contrast(float pix, float C)
{
    return powf(pix, C);
}

}
#endif