
#include "../base/DiskFile.h"

#include "../bottle/GameFile.h"
#include "../bottle/WorldMaker.h"
#include "../bottle/World25.h"
#include "../bottle/Lair.h"

namespace Game {
///////////////////////////////////////////////////////////////////
GameFile::GameFile()
{

}
GameFile::~GameFile()
{

}
///////////////////////////////////////////////////////////////////

void GameFile::pkp(std::vector<t_string>& tokens)
{
    int iind = 1;
    if (lcmp(tokens[0], "g_beg", 1)) {
        //if (_pCurModDataLoad == nullptr) {
        //    _pCurModDataLoad = new ModDataLoad();
        //    _pCurModDataLoad->_strModName = getCleanToken(tokens, iind);
        //}
        //else {
        //    parseErr("Cur Mod Data was not null.");
        //}
    }
    else if (lcmp(tokens[0], "g_world", 2)) {
        _strWorldName = getCleanToken(tokens, iind);
    }
}
void GameFile::preLoad() {


}
void GameFile::postLoad() {

}
void GameFile::save(t_string loc, std::shared_ptr<World25> pw) {
    AssertOrThrow2(pw != nullptr);
    _fileLoc = loc;

    DiskFile df;
    df.openForWrite(loc);
    df.writeTextLine("#Game File ");

    AssertOrThrow2(pw->getWorldMaker() != nullptr);
    //AssertOrThrow2(pw->getDungeonMaker()->getCurrentLair() != nullptr);
    LairSpec* ls = pw->getWorldMaker()->getCurrentLair();

  //  if (ls == nullptr) {
  //      parseErr(TStr("Error saving, lair was null"), true, true);
  //  }
    df.writeTextLine("g_beg");
   // df.writeTextLine(TStr("g_lair \"", ls->getName(), "\""));
    if (StringUtil::isEmpty(pw->getWorldName())) {
        parseErr("Error saving, cur world was null", true, false);
    }
    df.writeTextLine(Stz "g_world \""+ pw->getWorldName()+ "\"");
    df.writeTextLine("g_end");
    //df.writeTextLine(TStr("#Derek Page ", dateTimeToStr(getDateTime())));
    ////sprite image size
    //df.writeTextLine(TStr("isw ", _pSpriteSpec->getSize().x));
    //df.writeTextLine(TStr("ish ", _pSpriteSpec->getSize().y));
    ////atlas reference
    //if (_pSpriteSpec->getAtlas() != NULL)
    //    mf.writeTextLine(TStr("ats ", _pSpriteSpec->getAtlas()->getAtlasFileName()));
    //else
    //    mf.writeTextLine(TStr("ats none"));

    //AssertOrThrow2(_pSpriteSpec->getAnimations() != NULL);
    //SpriteSpec::AnimationMap::iterator aite = _pSpriteSpec->getAnimations()->begin();
    //for (; aite != _pSpriteSpec->getAnimations()->end(); aite++)
    //{
    //    TRef(SpriteAnimationSpec) pAnim = aite->second;
    //    AssertOrThrow2(pAnim != NULL);
    //    //begin animation
    //    mf.writeTextLine(TStr("anb ", pAnim->getName()));

    //    SpriteAnimationSpec::FrameMap::iterator fite = pAnim->getFrames()->begin();
    //    for (; fite != pAnim->getFrames()->end(); fite++)
    //    {
    //        AtlasCellFrameSpec* pFrame = fite->second;
    //        AssertOrThrow2(pFrame != NULL);
    //        //aniamtion frame
    //        mf.writeTextLine(TStr("fra ", pFrame->getCell().x, " ", pFrame->getCell().y));
    //    }
    //    //end of animation
    //    mf.writeTextLine(TStr("ane"));
    //}

    df.close();


}








}//ns Game
