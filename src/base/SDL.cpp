#include "../base/Base.h"
#include "../base/SDL.h"
#include "../base/FileSystem.h"
#include "../base/OglErr.h"
#include "../base/SDL.h"


namespace Game {

void SDL::trySetWindowIcon(SDL_Window* w, t_string iconPath) {
    if (iconPath.length()) {
        std::shared_ptr<Img32> img = nullptr;
        SDL_Surface* ss = nullptr;
        SDL::createSurfaceFromImage(iconPath, img, ss);
        if (ss != nullptr) {
            SDL_SetWindowIcon(w, ss);
            SDL::checkSDLErr();
            SDL_FreeSurface(ss);
            SDL::checkSDLErr();
        }
        if (img != nullptr) {
            //SDL uses the image pointer, so you have to free the image AFTER you free the surface.
            Gu::freeImage(img);
        }
    }
}
void SDL::createSurfaceFromImage(const t_string strImage,
    std::shared_ptr<Img32>& __out_ pImage, SDL_Surface*& __out_ pSurface) {
    pImage = nullptr;
    pSurface = nullptr;
    if (FileSystem::fileExists(strImage)) {
        pImage = Gu::loadImage(strImage);
        pSurface = Gu::SDLCreateSurfaceFromImage(pImage);
    }
    else {
        BroLogError("Could not icon image '" + strImage + "'");
    }
}
SDL_Surface* SDL::SDLCreateSurfaceFromImage(const std::shared_ptr<Img32> pImage) {
    SDL_Surface* pSurface = SDL_CreateRGBSurfaceFrom(
        pImage->getData(), pImage->getWidth(), pImage->getHeight(),
        pImage->getBitsPerPixel(), pImage->getPitch(),
        pImage->getRMask(), pImage->getGMask(), pImage->getBMask(), pImage->getAMask());

    checkSDLErr();

    return pSurface;
}
void SDL::checkSDLErr(bool doNotLog) {
    //Do SDL errors here as well
    const char* c;
    while ((c = SDL_GetError()) != nullptr && *c != 0) {
        if (doNotLog == false) {
            BroLogError("SDL: " + c);
        }

        if (Gu::getEngineConfig()->getBreakOnSDLError() == true) {
            Gu::debugBreak();
        }
        SDL_ClearError();
    }

}







}//ns Game
