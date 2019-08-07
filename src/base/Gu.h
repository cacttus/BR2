    /**
*
*    @file Gu.h
*    @date November 6, 2016
*    @author Derek Page
*
*    � 2016 
*
*
*/
#pragma once
#ifndef __GU_14784108333124451735_H__
#define __GU_14784108333124451735_H__

#include "../base/BaseHeader.h"
#include "../display/DisplayHeader.h"
#include "../math/MathHeader.h"
#include "../base/Stopwatch.h"


namespace Game {
/**
*    @class Gu
*    @brief Global Utils For everything.
*
*/
class Gu : public GameMemory {
    //static std::map<Hash32, Stopwatch> _stopw;
    static t_string _strCachedProf ;
    static std::stack<Stopwatch> _stopw;
    static std::stack<GLenum> _eLastCullFaceStack;
    static std::stack<GLenum> _eLastBlendStack;
    static std::stack<GLenum> _eLastDepthTestStack;
    static const int MaxStackSize = 32;
    static std::shared_ptr<Logger> _pLogger;
    static std::shared_ptr<EngineConfig> _pEngineConfig;
    static std::shared_ptr<GLContext> _pContext;
	static int _iSupportedDepthSize;
    
	static void SDLCreateSurfaceFromImage(const t_string strImage,
        std::shared_ptr<Img32>& __out_ pImage, SDL_Surface*& __out_ pSurface);
    static SDL_Surface* SDLCreateSurfaceFromImage(const std::shared_ptr<Img32> bi);
    static void pulsePerf();
public:
    static std::shared_ptr<GLContext> getContext() { return _pContext; }
	static std::shared_ptr<Package> getPackage();
    static void checkErrorsDbg();
    static void checkErrorsRt();
    static void initGlobals(std::shared_ptr<AppBase> rb, std::vector<std::string>& args);
    static void deleteGlobals();
    static void createGLContext(std::shared_ptr<AppBase> rb);
    static std::shared_ptr<Logger> getLogger() { return _pLogger;  }
    static std::shared_ptr<EngineConfig> getEngineConfig() { return _pEngineConfig; }
    static void debugBreak();
    static void beginPerf();
    static void endPerf();
    static void pushPerf();
    static void popPerf();
    //static void SDLFileFree(char*& pOutData);
    //static int SDLFileRead(std::string fname, char*& pOutData, uint32_t& _iOutSizeBytes, bool addNull = true);
    //static int SDLFileWrite(std::string fname, char* pData, uint32_t _iDataSizeBytes);
    //static bool SDLFileExists(std::string fname);
    //static std::string getFilePath(std::string rhs);
    static bool isDebug();
    //#Font resolution -- FontBitmapSize / CharSize = size of a base char in pixels.
    //static float getBakedFontCharSize()  ;
    //static int getFontBitmapSizePixels() ;
    static t_timeval  getMicroSeconds();
    static t_timeval  getMilliSeconds();
    static bool is64Bit();
    static bool isBigEndian();
    static std::shared_ptr<Img32> loadImage(std::string path);
    static bool saveImage(std::string path, std::shared_ptr<Img32> spec);
    static void freeImage(std::shared_ptr<Img32> b);
    
    static int loadSound(std::string path, int& iChannels, int& iSampleRate,
        int16_t*& pData, int& nSamples, int& iDataLenBytes);
    static std::vector<t_string> argsToVectorOfString(int argc, char** argv, char delimiter = '=');
    //Use Random.h
    //static float frand(); //returns 0,1
    //static float frand1(); //returns -1,1
    //static vec3 rv3();
    static void inlineDrawBoxCont(const Box3f* b);
    static float fade(float t);
    static void beginWin32InlineDebug(std::shared_ptr<GLContext>);
    static void checkMemory();
    static t_string getOperatingSystemName();
    static uint32_t getCurrentThreadId();
    static int32_t getSupportedDepthSize();
    static void setSupportedDepthSize(int32_t ee) { _iSupportedDepthSize =ee; }

    static void SDLTrySetWindowIcon(SDL_Window* w, t_string img);

    static void pushCullFace();
    static void popCullFace();
    static void pushBlend();
    static void popBlend();
    static void pushDepthTest();
    static void popDepthTest();
    static void print(char msg);
    static void print(const char* msg);
    static void print(const t_string& msg);
    static uint64_t getFrameNumber();

    static void guiQuad2d(Box2f& pq, std::shared_ptr<Viewport> vp);//Transforms a quad for the matrix-less 

    template < typename Tx >
    static bool addIfDoesNotContain(std::vector<Tx>& vec, Tx& rhs) { 
        for(size_t i=0; i<vec.size(); ++i){
            if(vec[i] == rhs) { 
                return false;
            }
        }
        vec.push_back(rhs);
        return true;
    }
    template <typename Tx> 
    static void incrementEnum(Tx& __in_ eValue, Tx iMaxValue){
        eValue = (Tx)((int)eValue + 1);
        if ((int)eValue >= iMaxValue) { 
            eValue = (Tx)0;
        }
    }
};

class FpsMeter {
    t_timeval _last = 0;
    t_timeval _tmr = 0;
    float _fpsLast = 60;
    uint64_t _iFrame = 0; //Current frame number
public:
    bool deltaMs(t_timeval& __inout_ last, t_timeval ms){
        t_timeval cur = Gu::getMicroSeconds();
        if(cur - last >= ms){
            last = cur;
            return true;
        }
        return false;
    }
    float getFps() { return _fpsLast; }
    void update() {
        //Only call this once per frame.
        t_timeval cur = Gu::getMicroSeconds();
        if (cur - _tmr > 500000) {//Update every seec
            t_timeval delta = cur - _last;
            double divisor = 1000000;
            if (delta == 0) {
                delta = (t_timeval)divisor;
            }
            _fpsLast = (float)(divisor / (double)delta);
            _tmr = cur;
        }
        _last = cur;

        _iFrame++;
    }
    uint64_t getFrameNumber() { return _iFrame; }
    bool frameMod(uint64_t i){
        return (_iFrame % i) == 0;
    }

};


}//ns Game



#endif
