#include "../base/SoundCache.h"
#include "../base/Hash.h"
#include "../base/TStr.h"
#include "../base/Logger.h"
#include "../base/FileSystem.h"


namespace Game {
#pragma region SoundSpec
SoundSpec::SoundSpec(t_string sFIle) : _sFilePath(sFIle)
{
    load(sFIle);
}
SoundSpec::~SoundSpec()
{
    free(_pSoundData);
}
void SoundSpec::load(t_string file) {
    if (_eLoadState == LoadState::e::NotLoaded) {

        if (!FileSystem::fileExists(file)) {
            BroLogError("File ", file, " does not exist.");
            _eLoadState = LoadState::e::LoadFailed;
        }
        else {
            int ret = Gu::loadSound(file, _iChannels, _iSampleRate, _pSoundData, _nSamples, _iSoundDataLenBytes);
            if (ret == 1) {
                _eLoadState = LoadState::e::Loaded;
            }
            else {
                BroLogError("STB load audio failed with code: ", ret);
            }
        }
    }
}
std::shared_ptr<SoundInst> SoundSpec::play(SoundPlayInfo inf) {
    std::shared_ptr<SoundInst> r = nullptr;

    if (_eLoadState == LoadState::e::NotLoaded) {
        load(_sFilePath);
    }

    if (_eLoadState == LoadState::e::Loaded) {
        //Create and play a new "sound inst."        
        r = std::make_shared<SoundInst>(_pSoundData, _iSoundDataLenBytes);
        r->play(inf);
        SDL_LockAudio();
        {
            _vecInstances.push_back(r);
        }
        SDL_UnlockAudio();
    }

    return r;
}
void SoundSpec::update() {
    //Delete ended instances.
    SDL_LockAudio();
    {
        for (int i = (int)_vecInstances.size() - 1; i >= 0; --i) {
            std::shared_ptr<SoundInst> si = _vecInstances[i];
            if (si->_ePlayState == PlayState::e::Ended) {
                //si->closeDevice();
                si = nullptr;
                _vecInstances.erase(_vecInstances.begin() + i);
            }
        }
    }
    SDL_UnlockAudio();
}
#pragma endregion

#pragma region SoundInst
SoundInst::SoundInst(int16_t* pos, int32_t len) {
    audio_pos = (int8_t*)pos; //**THIS MUST BE A BYTE BECAUSE WE SUB BYTES
    audio_len = len;

    start_pos = audio_pos;
    start_len = audio_len;
}
SoundInst::~SoundInst() {
}
void SoundInst::play(SoundPlayInfo inf) {
    _ePlayState = PlayState::e::Playing;
    _playInfo = inf;
}
void SoundInst::mixIntoBuffer(uint8_t *stream, int streamlen) {
    checkPlayback();

    if (_ePlayState != PlayState::e::Playing) {
        return;
    }

    int mixlen = (streamlen > audio_len ? audio_len : streamlen);

    //Note: SDL_MixAudio mixes only the format of stream 1 and it is invalid with SDL_OpenAudioDevice
    SDL_MixAudio(stream, (Uint8*)audio_pos, (Uint32)mixlen, SDL_MIX_MAXVOLUME);

    audio_pos += mixlen;
    audio_len -= mixlen;
}
void SoundInst::checkPlayback() {
    if (audio_len <= 0) {
        static int xx = 0;
        if (_playInfo._bLoop) {
            if (_playInfo._fLoopPos < 0.0f) {
                _playInfo._fLoopPos = 0.0f;
                if (xx == 0) { Gu::debugBreak(); }
            }
            if (_playInfo._fLoopPos > 1.0f) {
                _playInfo._fLoopPos = 1.0f;
                if (xx == 0) { Gu::debugBreak(); }
            }

            int32_t iOff = (int32_t)((float)start_len * _playInfo._fLoopPos);
            audio_pos = start_pos;
            audio_len = iOff;
        }
        else {
            //Sound Spec will remove the ended sound.
            _ePlayState = PlayState::Ended;
        }
    }
}
#pragma endregion

#pragma region SoundCache
void SoundCache::my_audio_callback(void *userdata, uint8_t *stream, int len) {
    SoundCache* sc = (SoundCache*)userdata;
    if (sc == nullptr) {
        return;
    }

    sc->mixSamplesAsync(stream, len);
}
SoundCache::SoundCache() {
    init();
}
SoundCache::~SoundCache(){
    //for (SoundMap::iterator ite = _cache.begin(); ite != _cache.end(); ite++) {
    //    std::shared_ptr<SoundSpec> ss = ite->second;
    //    ss = null
    //    DEL_MEM(ss);
    //}
    _cache.clear();
    SDL_LockAudio();
    SDL_CloseAudio();
    SDL_UnlockAudio();
}
void SoundCache::init() {
    int buffer = 1024;

    _desired.callback = my_audio_callback;
    _desired.channels = 2;
    _desired.format = AUDIO_S16; //*this is teh stb_vorbis input format
    _desired.freq = 44100;
    _desired.samples = 1024;
    _desired.userdata = this;

    if (SDL_OpenAudio(&_desired, &_have) < 0) {
        //SDL_OpenAudioDevice(NULL, 0, &_desired, &_have, SDL_AUDIO_ALLOW_ANY_CHANGE);
        // if (_iSDLAudioDevice <= 0) {
        BroLogError("SDL Couldn't open audio: ", SDL_GetError());

        _bError = true;
    }
    else {
        SDL_PauseAudio(0);
    }

    printSoundInfo();
}
void SoundCache::printSoundInfo() {
    /* print out some info on the formats this run of SDL_mixer supports */
    //int i, n = Mix_GetNumChunkDecoders();
    //BroLogInfo("There are ", n, " available chunk(sample) decoders");
    //for (i = 0; i < n; ++i) {
    //    BroLogInfo("   ", Mix_GetChunkDecoder(i));
    //}
    //n = Mix_GetNumMusicDecoders();
    //BroLogInfo("There are ", n, " available music decoders");
    //for (i = 0; i < n; ++i) {
    //    printf("	", Mix_GetMusicDecoder(i));
    //}
    //int bits = 0;
    //int sample_size = 0;
    //int rate = 0;
    //int audio_rate, audio_channels;
    //Uint16 audio_format;
    //Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);
    //bits = audio_format & 0xFF;
    //sample_size = bits / 8 + audio_channels;
    //rate = audio_rate;
    //BroLogInfo("Opened audio at ", audio_rate," Hz ", bits," bit ", audio_channels > 1 ? "stereo" : "mono");

}
void SoundCache::mixSamplesAsync(uint8_t *stream, int len) {
    SDL_LockAudio();
    {
        //Silence the buffer.
        SDL_memset(stream, _have.silence, len);

        int debug_NumMixed = 0;
        for(std::pair<Hash32, std::shared_ptr<SoundSpec>> p : _cache){
            std::shared_ptr<SoundSpec> ss = p.second;

            //Mix the dumb spec into the dumb buffer.
            for(std::shared_ptr<SoundInst> si : ss->getInstances()) {
                si->mixIntoBuffer(stream, len);
                debug_NumMixed++;
            }
        }
    }
    SDL_UnlockAudio();
}
std::shared_ptr<SoundSpec> SoundCache::getOrLoad(std::string file) {
    if(_bError == true) {
        return nullptr;
    }

    std::shared_ptr<SoundSpec> ret = nullptr;

    Hash32 ih = Hash::computeStringHash32bit(file, 0);
    SoundMap::iterator ite = _cache.find(ih);

    if (ite != _cache.end()) {
        ret = ite->second;
    }
    else {
        ret = std::make_shared<SoundSpec>(file);
        _cache.insert(std::make_pair(ih, ret));
    }

    return ret;
}
void SoundCache::update() {
    for (SoundMap::iterator ite = _cache.begin(); ite != _cache.end(); ite++) {
        std::shared_ptr<SoundSpec> ss = ite->second;
        ss->update();
    }
}
std::shared_ptr<SoundInst> SoundCache::tryPlay(std::string file){
    return tryPlay(file, SoundPlayInfo(false, 0.0f));

}
std::shared_ptr<SoundInst> SoundCache::tryPlay(std::string file, SoundPlayInfo inf) {
    std::shared_ptr<SoundSpec> ss = getOrLoad(file);
    if (ss != nullptr) {
        return ss->play(inf);
    }
    return nullptr;
}
#pragma endregion



}//ns Game
