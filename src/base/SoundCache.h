/**
*
*    @file SoundCache.h
*    @date January 19, 2017
*    @author Derek Page
*
*    © 2017 
*
*
*/
#pragma once
#ifndef __SOUNDCACHE_1484887033955700376_H__
#define __SOUNDCACHE_1484887033955700376_H__

#include "../base/BaseHeader.h"

namespace Game {
class SoundPlayInfo : public VirtualMemory {
public:
    bool _bLoop = false;
    float _fLoopPos = 0.0f;// [0,1]
    SoundPlayInfo() {}
    SoundPlayInfo(bool loop, float fLoopPos) {
        _bLoop = loop;
        _fLoopPos = fLoopPos;
    }
};
class SoundSpec : public VirtualMemory {
    std::vector<std::shared_ptr<SoundInst>> _vecInstances;
    t_string _sFilePath;
    //    Mix_Music* _pMusic = nullptr;
    //Mix_Chunk* _pChunk = nullptr;
    int32_t _iChannels = 0;
    int32_t _nSamples = 0;
    int32_t _iSampleRate = 0;
    int16_t* _pSoundData = nullptr;
    int32_t _iSoundDataLenBytes = 0;
    // SDL_AudioSpec _audioSpec ;
    LoadState::e _eLoadState = LoadState::e::NotLoaded;
public:

    std::vector<std::shared_ptr<SoundInst>>& getInstances() { return _vecInstances; }

    SoundSpec(t_string sFile);
    virtual ~SoundSpec() override;

    void update();
    void load(t_string file);
    std::shared_ptr<SoundInst> play(SoundPlayInfo inf);
};
class SoundInst : public VirtualMemory {
    int8_t* start_pos = nullptr;
    int32_t start_len = 0;

    void checkPlayback();
public:
    SoundPlayInfo _playInfo;
    PlayState::e _ePlayState = PlayState::e::Stopped;
    bool _bError = false;
    //SDL_AudioSpec _desired;
    // SDL_AudioSpec _have ;
    std::shared_ptr<SoundSpec> _pSoundSpec = nullptr;


    int8_t* audio_pos = nullptr; // **THIS MUST BE A BYTE BECAUSE WE OFFSET IT BY BYTES!!
    int32_t audio_len = 0; // remaining length of the sample we have to play
                           // SDL_AudioDeviceID _iSDLAudioDevice = 0;

    SoundInst(int16_t* pos, int32_t len);
    virtual ~SoundInst() override;

    void play(SoundPlayInfo inf);
    void mixIntoBuffer(uint8_t *buf, int buflen);

};
class SoundCache : public VirtualMemory {
    typedef std::map<Hash32, std::shared_ptr<SoundSpec>> SoundMap;
    SoundMap _cache;
    bool _bError = false;
    SDL_AudioSpec _desired, _have;
    std::mutex _mutex;

    void printSoundInfo();
    static void my_audio_callback(void *userdata, uint8_t *stream, int len);
public:

    // TEST
//no
//  static bool bSoundTestSpace = false;
//  if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
//      if (bSoundTestSpace == false) {
//          bSoundTestSpace = true;
//          std::shared_ptr<SoundSpec> ss = _pEngine->getContext()->getSoundCache()->getOrLoad("./data-dc/snd/ogg-test.ogg");
//          ss->play();
//      }
//  }
//  else {
//      bSoundTestSpace = false;
//  }

    void mixSamplesAsync(uint8_t *stream, int len);

    SoundCache();
    virtual ~SoundCache() override;

    void init();
    std::shared_ptr<SoundSpec> getOrLoad(std::string file);
    void update();
    std::shared_ptr<SoundInst> tryPlay(std::string file);
    std::shared_ptr<SoundInst> tryPlay(std::string file, SoundPlayInfo inf);
};

}//ns Game



#endif
