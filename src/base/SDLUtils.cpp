#include "../base/Logger.h"
#include "../base/FileSystem.h"
#include "../base/EngineConfig.h"
#include "../base/OglErr.h"
#include "../base/SDLUtils.h"
#include "../base/Img32.h"


namespace Game {

void SDLUtils::trySetWindowIcon(SDL_Window* w, t_string iconPath) {
  if (iconPath.length()) {
    std::shared_ptr<Img32> img = nullptr;
    SDL_Surface* ss = nullptr;
    SDLUtils::createSurfaceFromImage(iconPath, img, ss);
    if (ss != nullptr) {
      SDL_SetWindowIcon(w, ss);
      SDLUtils::checkSDLErr();
      SDL_FreeSurface(ss);
      SDLUtils::checkSDLErr();
    }
    if (img != nullptr) {
      //SDL uses the image pointer, so you have to free the image AFTER you free the surface.
      Gu::freeImage(img);
    }
  }
}
void SDLUtils::createSurfaceFromImage(const t_string strImage,
  std::shared_ptr<Img32>& __out_ pImage, SDL_Surface*& __out_ pSurface) {
  pImage = nullptr;
  pSurface = nullptr;
  if (FileSystem::fileExists(strImage)) {
    pImage = Gu::loadImage(strImage);
    pSurface = createSurfaceFromImage(pImage);
  }
  else {
    BRLogError("Could not icon image '" + strImage + "'");
  }
}
SDL_Surface* SDLUtils::createSurfaceFromImage(const std::shared_ptr<Img32> pImage) {
  SDL_Surface* pSurface = SDL_CreateRGBSurfaceFrom(
    pImage->getData(), pImage->getWidth(), pImage->getHeight(),
    pImage->getBitsPerPixel(), pImage->getPitch(),
    pImage->getRMask(), pImage->getGMask(), pImage->getBMask(), pImage->getAMask());

  checkSDLErr();

  return pSurface;
}
void SDLUtils::checkSDLErr(bool doNotLog) {
  //Do SDL errors here as well
  const char* c;
  while ((c = SDL_GetError()) != nullptr && *c != 0) {
    if (doNotLog == false) {
      BRLogError("SDL: " + c);
    }

    if (Gu::getEngineConfig()->getBreakOnSDLError() == true) {
      Gu::debugBreak();
    }
    SDL_ClearError();
  }

}







}//ns Game
