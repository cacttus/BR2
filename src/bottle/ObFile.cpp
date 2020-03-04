#include "../base/Logger.h"
#include "../base/Exception.h"
#include "../base/GLContext.h"
#include "../base/Gu.h"
#include "../base/Hash.h"
#include "../base/Img32.h"
#include "../base/Allocator.h"
#include "../base/FileSystem.h"
#include "../base/ApplicationPackage.h"
#include "../model/MobFile.h"
#include "../model/ModelCache.h"
#include "../model/Model.h"
#include "../bottle/ObFile.h"
#include "../bottle/SpriteBucket.h"
#include "../bottle/WorldObj.h"
#include "../bottle/W25Config.h"
#include "../bottle/Lair.h"
#include "../bottle/Tile25Spec.h"
#include "../bottle/WalkerSpec.h"
#include "../bottle/WorldObj.h"

namespace BR2 {
ObFile::ObFile() {
  _pBucket = std::make_shared<SpriteBucket>();
}
ObFile::~ObFile() {
}
void ObFile::preLoad() {
  _pW25Config = std::make_shared<W25Config>();//Deleted in ~World25
}
void ObFile::postLoad() {
  BRLogInfo("ObFile: Debug mode is " + (_pW25Config->getIsDebugMode() ? "Enabled" : "Disabled"));
}
void ObFile::pkp(std::vector<string_t>& tokens) {

  // - Parse the given token buffer
  if (tokens.size() > 0) {
    parseVer(tokens);
    parseConfig(tokens);
    parseSprites(tokens);
    parseMobs(tokens);
    parseTiles(tokens);
    parseMorphTiles(tokens);
    parseObjects(tokens);
    parseWalkers(tokens);
    parseLairs(tokens);
  }
}
void ObFile::parseConfig(std::vector<string_t>& tokens) {
  int iind = 1;
  bool _bDebugNewGrid = false;
  bool _bDebugFlatGrid = false;
  int _iDebugSeed = 9;

  if (lcmp(tokens[0], "CF_ENABLE_DBG", 2)) {
    _pW25Config->_bDebugMode = TypeConv::strToBool(getCleanToken(tokens, iind));
    //#ifndef _DEBUG
    //        //Only enable debug mode in the debug build
    //        BRLogWarn("Can't enable debug features in release build.");
    //        _pW25Config->_bDebugMode = false;
    //#endif
  }
  else if (lcmp(tokens[0], "CF_SKT", 2)) {
    _pW25Config->_bDebugSkipTitle = TypeConv::strToBool(getCleanToken(tokens, iind));
  }
  else if (lcmp(tokens[0], "CF_NEW", 2)) {
    _pW25Config->_bDebugNewGrid = TypeConv::strToBool(getCleanToken(tokens, iind));
  }
  else if (lcmp(tokens[0], "CF_00", 2)) {
    _pW25Config->_bDebugGenerate00Only = TypeConv::strToBool(getCleanToken(tokens, iind));
  }
  else if (lcmp(tokens[0], "CF_GM", 2)) {
    _pW25Config->_eDebugGridMode = (DebugGridMode::e)TypeConv::strToInt(getCleanToken(tokens, iind));
    if ((int)_pW25Config->_eDebugGridMode >= (int)DebugGridMode::e::MaxDebugGridModes) {
      BRLogError("Grid mode " + _pW25Config->_eDebugGridMode + " is invalid. Max modes = " + ((int)DebugGridMode::e::MaxDebugGridModes - 1));
      Gu::debugBreak();
      _pW25Config->_eDebugGridMode = (DebugGridMode::e)0;
    }
  }
  else if (lcmp(tokens[0], "CF_SEED", 2)) {
    _pW25Config->_iDebugSeed = TypeConv::strToInt(getCleanToken(tokens, iind));
  }
  //else if (lcmp(tokens[0], "CF_VERBOSE_ERR", 2)) {
  //    _pW25Config->_bRtErrorCheck = TypeConv::strToBool(getCleanToken(tokens, iind));
  //}
  //else if (lcmp(tokens[0], "CF_GL_BUF_MAP", 2)) {
  //    _pW25Config->_bGlUseMapBuffer = TypeConv::strToBool(getCleanToken(tokens, iind));
  //}
  else if (lcmp(tokens[0], "CF_BLOCK", 2)) {
    _pW25Config->_bRenderBlocks = TypeConv::strToBool(getCleanToken(tokens, iind));
  }
  //else if (lcmp(tokens[0], "CF_FS", 2)) {
  //    _pW25Config->_bStartFullscreen = TypeConv::strToBool(getCleanToken(tokens, iind));
  //}
  //else if (lcmp(tokens[0], "CF_SW", 2)) {
  //    _pW25Config->_uiStartWidth = TypeConv::strToUint(getCleanToken(tokens, iind));
  //}
  //else if (lcmp(tokens[0], "CF_SH", 2)) {
  //    _pW25Config->_uiStartHeight = TypeConv::strToUint(getCleanToken(tokens, iind));
  //}
  //else if (lcmp(tokens[0], "CF_FORCE_A", 2)) {
  //    _pW25Config->_bForceAspectRatio = TypeConv::strToBool(getCleanToken(tokens, iind));
  //}
  else if (lcmp(tokens[0], "CF_MAX_GRIDS_GEN_ASYNC", 2)) {
    uint32_t ui = TypeConv::strToUint(getCleanToken(tokens, iind));
    if (ui > 128) {
      BRLogWarn(tokens[0] + ": Too many grids, " + ui + " limiting to 128. ");
    }
    _pW25Config->_iMaxGridsGenAsync = ui;
  }
  else if (lcmp(tokens[0], "CF_GRID_LIMIT_32BIT", 2)) {
    if (!Gu::is64Bit()) {
      _pW25Config->_iGridCountLimit = TypeConv::strToUint(getCleanToken(tokens, iind));
    }

  }
  else if (lcmp(tokens[0], "CF_GRID_LIMIT_64BIT", 2)) {
    if (Gu::is64Bit()) {
      _pW25Config->_iGridCountLimit = TypeConv::strToUint(getCleanToken(tokens, iind));
    }
  }

}
void ObFile::parseVer(std::vector<string_t>& tokens) {
  //version
  if (lcmp(tokens[0], "VER", 2)) {
    string_t ver = StringUtil::stripDoubleQuotes(tokens[1]);
    if (ver != CONGA_SPRITE_FILE_VERSION) {
      BRThrowException(Stz "Sprite file version mismatch, File is " + ver + ", Engine wanted " + CONGA_SPRITE_FILE_VERSION);
    }
  }
}
void ObFile::parseMobs(std::vector<string_t>& tokens) {
  int iind = 1;
  if (lcmp(tokens[0], "MOB", 6)) {
    string_t mobFolder = getCleanToken(tokens, iind);
    string_t strBox = getCleanToken(tokens, iind);
    uint32_t typeID = TypeConv::strToUint(getCleanToken(tokens, iind));
    string_t strPlace = getCleanToken(tokens, iind);
    string_t friendlyName = getCleanToken(tokens, iind);

    //Find Duplicate
    for (std::shared_ptr<WorldObj> ws : _vecWorldObjs) {
      if (StringUtil::equalsi(ws->getMobName(), mobFolder)) {
        parseErr(Stz "Duplicate mob file found in game config " + mobFolder, true, true);
      }
    }

    std::shared_ptr<WorldObj> pObj = WorldObj::create(mobFolder, typeID, friendlyName, strBox, strPlace);
    _vecWorldObjs.push_back(pObj);

  }
}

void ObFile::parseSprites(std::vector<string_t>& tokens) {
  if (lcmp(tokens[0], "SPR")) {
    int iMinArgCount = 6; // all args including SPR, and one of the images at the end

    if ((int)tokens.size() >= iMinArgCount) {
      int iind = 1;

      //Parse the args line.
      string_t name = getCleanToken(tokens, iind);
      float fDuration = TypeConv::strToFloat(getCleanToken(tokens, iind));
      bool bLoop = TypeConv::strToBool(getCleanToken(tokens, iind));
      bool bRandomSel = TypeConv::strToBool(getCleanToken(tokens, iind));
      //int iSolidTileIndex = TypeConv::strToInt(getCleanToken(tokens, iind));
      //int iLiquidTileIndex = TypeConv::strToInt(getCleanToken(tokens, iind));
     // int iMaskIndex = TypeConv::strToInt(getCleanToken(tokens, iind));

      //Parse sprite file names.
      std::vector<string_t> files;
      for (size_t i = (iMinArgCount - 1); i < tokens.size(); ++i) {
        string_t path = StringUtil::stripDoubleQuotes(tokens[i]);



        //Attempt to find absolute path.  If we can't find, then attempt to append path.
        if (!FileSystem::fileExists(path)) {
          path = Gu::getPackage()->makeAssetPath("spr", path);
        }
        if (FileSystem::fileExists(path)) {

          //Prevent Duplicates when we hash the name.
          path = StringUtil::lowercase(path);

          files.push_back(path);
        }
        else {
          files.push_back(Gu::getPackage()->makeAssetPath("spr", "default.png"));
          BRLogError(name + ": Failed to find Sprite path: '" + path + "'. A default sprite will show.");
          Gu::debugBreak();
        }
      }

      //Verify no duplicates.
      bool bVerified = true;
      for (size_t iMot = 0; iMot < _vecMotionSpecs.size(); ++iMot) {
        std::shared_ptr<SpriteSpec> ps = _vecMotionSpecs[iMot];

        if (StringUtil::equalsi(ps->getName(), name)) {
          BRLogWarn(name + ", " + ps->getName() + ": Duplicate sprite names found. Ignoring duplicate sprite.");
          bVerified = false;
          Gu::debugBreak();
        }
      }

      if (files.size() > 1 && fDuration == 0) {
        BRLogWarn(name + ", Animation had frames but didn't have a duration set. The animation will not play.");
        Gu::debugBreak();
      }

      //Add sprite
      if (bVerified == true) {
        processMotion(name, files, fDuration, bLoop, bRandomSel);

      }


    }
    else {
      BRLogWarn("Invalid SPR - incorrect number of arguments.  Wanted " + iMinArgCount + " got " + tokens.size());
      Gu::debugBreak();
    }
  }
}

void ObFile::parseTiles(std::vector<string_t>& tokens) {
  if (lcmp(tokens[0], "TIL")) {
    int iMinArgCount = 13; // all args including TIL, and all images at the end

    if ((int)tokens.size() >= iMinArgCount) {
      _bCurSpecValid = true;
      int iind = 1;


      //Parse the args line.
      string_t name = getCleanToken(tokens, iind);
      string_t matter = StringUtil::uppercase(getCleanToken(tokens, iind));
      int32_t index = TypeConv::strToInt(getCleanToken(tokens, iind));

      ClimateSpec cp = parseClimate(tokens, name, iind);

      float rarity = TypeConv::strToFloat(getCleanToken(tokens, iind));

      string_t strTop = (getCleanToken(tokens, iind));
      string_t strSide = strTop, strBot = strTop;

      if (iind < (int)tokens.size()) {
        strSide = (getCleanToken(tokens, iind));
      }
      if (iind < (int)tokens.size()) {
        strBot = (getCleanToken(tokens, iind));
      }

      bool bVerified = true;

      GridMeshLayer::e eMatterMode = GridMeshLayer::e::Invalid;
      if (StringUtil::equals(matter, "S")) { eMatterMode = GridMeshLayer::e::Opaque; }
      else if (StringUtil::equals(matter, "L")) { eMatterMode = GridMeshLayer::e::Transparent; }
      else if (StringUtil::equals(matter, "G")) {
        BRLogWarn("Gas not supported, converting to liquid");
        eMatterMode = GridMeshLayer::e::Transparent;
      }
      if (eMatterMode == GridMeshLayer::e::Invalid) {
        BRLogWarn(name + ",  Invalid TileMatter specification '" + matter + "'");
        bVerified = false;
        Gu::debugBreak();
      }


      for (size_t iTile = 0; iTile < _vecTileSpecs.size(); ++iTile) {
        Tile25Spec* ps = _vecTileSpecs[iTile];
        if (StringUtil::equalsi(ps->getName(), name)) {
          BRLogWarn(name + ", " + ps->getName() + ": Duplicate tile names found. Ignoring duplicate tile.");
          bVerified = false;
          Gu::debugBreak();
        }
        if (ps->getTileIndex() == index) {
          BRLogWarn(name + ", " + ps->getName() + ": Duplicate tile indexes '" + index + "' were found in the sprite file. These must be unique. Ignoring duplicate.");
          bVerified = false;
          Gu::debugBreak();
        }
      }

      std::shared_ptr<SpriteSpec> pTop, pSide, pBot;
      pTop = getMot(strTop);
      pSide = getMot(strSide);
      pBot = getMot(strBot);

      if (pTop == nullptr) {
        BRLogWarn(name + ": Top sprite '" + strTop + "'not found.  (Check the if sprite name is misspelled).");
        bVerified = false;
        Gu::debugBreak();
      }
      if (pSide == nullptr) {
        BRLogWarn(name + ": Side sprite '" + strSide + "'not found.  (Check the if sprite name is misspelled).");
        bVerified = false;
        Gu::debugBreak();
      }
      if (pBot == nullptr) {
        BRLogWarn(name + ": Bot sprite '" + strBot + "'not found.  (Check the if sprite name is misspelled).");
        bVerified = false;
        Gu::debugBreak();
      }

      if (index < 0 || index>255) {
        BRLogWarn(name + ": Index cannot be larger than 255!");
        bVerified = false;
        Gu::debugBreak();
      }
      if (bVerified == true && _bCurSpecValid == true) {
        processTile(name, index, eMatterMode, pTop, pSide, pBot, cp, rarity);

      }

    }
    else {
      BRLogWarn("Invalid Tile - incorrect number of arguments, wanted " + iMinArgCount + " got " + tokens.size());
      Gu::debugBreak();
    }
  }
}
void ObFile::parseObjects(std::vector<string_t>& tokens) {
  int iind = 1;

  //if (lcmp(tokens[0], "OBB", 2)) {
  //    if (_pCurObjSpec != nullptr) {
  //        BRThrowException("'OBB' Failed to read Objects: Format Error - already begun an object desc.");
  //    }
  //    _bCurSpecValid = true;
  //    _pCurObjSpec = new WorldObjectSpec();
  //    string_t name = getCleanToken(tokens, iind);
  //    _pCurObjSpec->_iType = STRHASH(name);
  //    _pCurObjSpec->_strName = name;
  //    
  //}
  //else if (lcmp(tokens[0], "OB_MOB", 2)) {
  //    string_t strMob = getCleanToken(tokens, iind);
  //    _pCurObjSpec->_strMobName = strMob; 
  //}
  //else if (lcmp(tokens[0], "OB_IDOV", 2)) {
  //    int iType = TypeConv::strToInt(getCleanToken(tokens, iind));
  //    _pCurObjSpec->_iType = iType;
  //}
  //else if (lcmp(tokens[0], "OB_TRSP", 2)) {
  //    float trsp = TypeConv::strToFloat(getCleanToken(tokens, iind));
  //    _pCurObjSpec->_fTransparency = trsp;
  //}
  //else if (lcmp(tokens[0], "OB_HOLL", 2)) {
  //    bool bHollow = TypeConv::strToBool(getCleanToken(tokens, iind));
  //    _pCurObjSpec->_bHollow = bHollow;
  //}
  //else if (lcmp(tokens[0], "OB_BBXY", 2)) {
  //    float bb = TypeConv::strToFloat(getCleanToken(tokens, iind));
  //    _pCurObjSpec->_fBoundBoxRadiusXY = bb;
  //}
  //else if (lcmp(tokens[0], "OB_LSRC", 2)) {
  //    bool bs = TypeConv::strToBool(getCleanToken(tokens, iind));
  //    _pCurObjSpec->_bIsEmitter = bs;
  //}
  //else if (lcmp(tokens[0], "OB_LSRR", 2)) {
  //    float flr = TypeConv::strToFloat(getCleanToken(tokens, iind));
  //    _pCurObjSpec->_fEmitterRadius = flr;
  //}
  //else if (lcmp(tokens[0], "OB_LSCL", 5)) {
  //    float c1 = TypeConv::strToFloat(getCleanToken(tokens, iind));
  //    float c2 = TypeConv::strToFloat(getCleanToken(tokens, iind));
  //    float c3 = TypeConv::strToFloat(getCleanToken(tokens, iind));
  //    float c4 = TypeConv::strToFloat(getCleanToken(tokens, iind));

  //    _pCurObjSpec->_vEmitterColor = vec4(c1,c2,c3,c4);
  //}
  //else if (lcmp(tokens[0], "OB_ISPC", 2)) {
  //    bool bs = TypeConv::strToBool(getCleanToken(tokens, iind));
  //    _pCurObjSpec->_bPlayerChar = bs;
  //}
  //else if (lcmp(tokens[0], "OB_ALIV", 2)) {
  //    bool bs = TypeConv::strToBool(getCleanToken(tokens, iind));
  //    _pCurObjSpec->_bAlive = bs;
  //}
  //else if (lcmp(tokens[0], "OB_MVSC", 2)) {
  //    float msc = TypeConv::strToFloat(getCleanToken(tokens, iind));
  //    _pCurObjSpec->_fMoveSpeedCreep = msc;
  //}
  //else if (lcmp(tokens[0], "OB_MVSR", 2)) {
  //    float msc = TypeConv::strToFloat(getCleanToken(tokens, iind));
  //    _pCurObjSpec->_fMoveSpeedRun = msc;
  //}
  //else if (lcmp(tokens[0], "OB_MOT1", 2)) {
  //    parseGetSetMotion(tokens[0], getCleanTokenInd(tokens, 1), _pCurObjSpec->_pBaseMotion);
  //}
  //else if (lcmp(tokens[0], "OB_MOTF", 2)) {
  //    parseGetSetMotion(tokens[0], getCleanTokenInd(tokens, 1), _pCurObjSpec->_pMoveUp);
  //}
  //else if (lcmp(tokens[0], "OB_MOTA", 2)) {
  //    parseGetSetMotion(tokens[0], getCleanTokenInd(tokens, 1), _pCurObjSpec->_pMoveDown);
  //}
  //else if (lcmp(tokens[0], "OB_MOTL", 2)) {
  //    parseGetSetMotion(tokens[0], getCleanTokenInd(tokens, 1), _pCurObjSpec->_pMoveLeft);
  //}
  //else if (lcmp(tokens[0], "OB_MOTR", 2)) {
  //    parseGetSetMotion(tokens[0], getCleanTokenInd(tokens, 1), _pCurObjSpec->_pMoveRight);
  //}
  //else if (lcmp(tokens[0], "OB_MASS", 2)) {
  //    string_t str = getCleanToken(tokens, iind);
  //    if(StringUtil::equalsi(str, "static")){
  //        _pCurObjSpec->_fMass = PHY_STATIC_MASS;
  //    }
  //    else {
  //        float msc = TypeConv::strToFloat(str);
  //        _pCurObjSpec->_fMass = msc;
  //    }
  //}
  //else if (lcmp(tokens[0], "OB_DRAW", 2)) {
  //    string_t drawType = getCleanToken(tokens, iind);
  //    if(StringUtil::equalsi(drawType, "box")){
  //        _pCurObjSpec->_eDrawType = DrawType::e::Box5;
  //    }
  //    else if (StringUtil::equalsi(drawType, "sprite")) {
  //        _pCurObjSpec->_eDrawType = DrawType::e::Sprite;
  //    }
  //    else if (StringUtil::equalsi(drawType, "mesh")) {
  //        _pCurObjSpec->_eDrawType = DrawType::e::Mesh;
  //    }
  //    else if (StringUtil::equalsi(drawType, "model")) {
  //        _pCurObjSpec->_eDrawType = DrawType::e::Model;
  //    }
  //    else {
  //        BRLogWarn(_pCurObjSpec->_strName, ":", tokens[0], 
  //            "  ERROR - invalid Draw type for object");
  //        _bCurSpecValid = false;
  //    }
  //}
  //else if (lcmp(tokens[0], "OBE", 1)) {
  //    if (_pCurObjSpec == nullptr) {
  //        BRThrowException("invalid OBE delcaration - Tried to add a null ob spec.");
  //    }

  //    //Validate
  //    for (size_t i = 0; i< _vecPixObjSpecs.size(); ++i) {
  //        if (_vecPixObjSpecs[i]->_iType == _pCurObjSpec->_iType){
  //            BRLogWarn("  ERROR - Duplicate Pix Obj Type ID ", _pCurObjSpec->_iType, " was found.  Ignoring.. Already:",
  //                _vecPixObjSpecs[i]->_strName, " Other: ", _pCurObjSpec->_strName);
  //            _bCurSpecValid = false;
  //        }
  //    }

  //    if(_bCurSpecValid == true) {
  //        _vecPixObjSpecs.push_back(_pCurObjSpec);
  //    }
  //    else {
  //        BRLogWarn(" Invalid Ob Spec - spec was not added.");
  //        DEL_MEM(_pCurObjSpec);
  //    }
  //    _pCurObjSpec = nullptr;
  //    _bCurSpecValid = true;
  //}
  //else if (lcmp(tokens[0], "OB_PERSISTENT", 2)) {
  //    bool bs = TypeConv::strToBool(getCleanToken(tokens, iind));
  //    _pCurObjSpec->_bPersistent = bs;
  //}
  //else {
  //    int iMinArgCount = 8; // all args including SPR, and one of the images at the end
  //}
}
void ObFile::parseMorphTiles(std::vector<string_t>& tokens) {
  int iind = 1;

  if (lcmp(tokens[0], "MT_BEG", 2)) {
    _bCurSpecValid = true;
    string_t name = getCleanToken(tokens, iind);
    if (_pCurMorphTile != nullptr) {
      BRLogError(tokens[0] + ": 'OBB' Failed to read MT: Format Error - already begun an object desc.");
    }

    _pCurMorphTile = new MorphTile(name);
  }
  else  if (lcmp(tokens[0], "MT_TIL", 2)) {
    if (_bCurSpecValid == true) {
      string_t name = getCleanToken(tokens, iind);

      Tile25Spec* pt = getTileSpecByName(name);

      if (pt == nullptr) {
        BRLogError(tokens[0] + ": couldn't find Tile '" + name + "' for morph tile '" + _pCurMorphTile->getName() + "'.");
        _bCurSpecValid = false;
      }
      else {
        _pCurMorphTile->getTileSpecs().insert(pt);
      }
    }
    _pCurWalkerSpec = nullptr;
  }
  else  if (lcmp(tokens[0], "MT_END", 1)) {
    if (_bCurSpecValid == true) {
      _vecMorphTiles.push_back(_pCurMorphTile);

    }
    _pCurWalkerSpec = nullptr;
    _pCurMorphTile = nullptr;
  }

}
void ObFile::parseWalkers(std::vector<string_t>& tokens) {
  int iind = 1;

  if (lcmp(tokens[0], "WK_BEG", 2)) {
    _bCurSpecValid = true;

    string_t name = getCleanToken(tokens, iind);
    if (_pCurWalkerSpec != nullptr) {
      BRThrowException(tokens[0] + ": 'OBB' Failed to read Objects: Format Error - already begun an object desc.");
    }
    Hash32 nh = STRHASH(name);
    for (WalkerSpec* sp : _vecWalkers) {
      if (sp->getNameHashed() == nh) {
        BRLogWarn(tokens[0] + ": Duplicate walker name found, ignoring: " + name);
        _bCurSpecValid = false;
      }
    }
    if (_bCurSpecValid) {
      _pCurWalkerSpec = new WalkerSpec(name);
    }
  }
  else if (lcmp(tokens[0], "WK_TYPE", 2)) {
    if (_bCurSpecValid == true) {
      if (_pCurWalkerSpec == nullptr) {
        BRLogError(tokens[0] + ":Walker was null.");
        _bCurSpecValid = false;
      }
      string_t type = getCleanToken(tokens, iind);

      WalkerType::e eType = WalkerType::e::Cave;
      if (StringUtil::equalsi(type, "cave")) {
        eType = WalkerType::e::Cave;
      }
      else if (StringUtil::equalsi(type, "tile")) {
        eType = WalkerType::e::Tile;
      }

      if (_bCurSpecValid) {
        _pCurWalkerSpec->_eWalkerType = eType;
      }
    }
  }
  else if (lcmp(tokens[0], "WK_LOCALE", 2)) {
    if (_bCurSpecValid == true) {
      if (_pCurWalkerSpec == nullptr) {
        BRLogError(tokens[0] + ":Walker was null.");
        _bCurSpecValid = false;
      }
      string_t sLocality = getCleanToken(tokens, iind);

      LairGenLocale::e locale = parseGenLocale(sLocality);

      if (_bCurSpecValid) {
        _pCurWalkerSpec->_eLocality = locale;
      }
    }
  }
  else if (lcmp(tokens[0], "WK_BRANCH", 3)) {
    if (_bCurSpecValid == true) {
      if (_pCurWalkerSpec == nullptr) {
        BRLogError("Walker was null.");
        _bCurSpecValid = false;
      }
      MpUint ui = parse_mxui2(tokens, _pCurWalkerSpec->getName(), iind);
      if (_bCurSpecValid) {
        _pCurWalkerSpec->_mxBranches = ui;
      }
    }
  }
  else if (lcmp(tokens[0], "WK_TILE", 6)) {
    if (_bCurSpecValid == true) {
      if (_pCurWalkerSpec == nullptr) {
        BRLogError("Walker was null.");
        _bCurSpecValid = false;
      }
      LairTile* lt = parseLairTile(tokens, _pCurWalkerSpec->getName(), iind);
      if (lt != nullptr) {
        _pCurWalkerSpec->getLairTiles().push_back(lt);
      }

    }
  }
  else if (lcmp(tokens[0], "WK_HOP ", 3)) {
    if (_bCurSpecValid == true) {
      if (_pCurWalkerSpec == nullptr) {
        BRLogError("Walker was null.");
        _bCurSpecValid = false;
      }
      MpUint ui = parse_mxui2(tokens, _pCurWalkerSpec->getName(), iind);
      if (_bCurSpecValid) {
        _pCurWalkerSpec->_mxBranches = ui;
      }
    }
  }
  else if (lcmp(tokens[0], "WK_END", 1)) {
    if (_bCurSpecValid == true) {
      if (_pCurWalkerSpec == nullptr) {
        BRLogError(tokens[0] + ":Walker was null.");
        _bCurSpecValid = false;

      }

      if (_bCurSpecValid) {
        _vecWalkers.push_back(_pCurWalkerSpec);
      }
    }
    _pCurWalkerSpec = nullptr;
  }
}
void ObFile::parseLairs(std::vector<string_t>& tokens) {
  int iind = 1;

  if (lcmp(tokens[0], "LA_BEG", 2)) {
    _bCurSpecValid = true;
    string_t name = getCleanToken(tokens, iind);
    if (_pCurLairSpec != nullptr) {
      BRThrowException(tokens[0] + ": Lair Object already begun");
    }
    Hash32 nh = STRHASH(name);
    for (LairSpec* sp : _vecLairs) {
      if (sp->getLairId() == nh) {
        BRLogWarn(tokens[0] + ": Duplicate lair name found, ignoring: " + name);
        _bCurSpecValid = false;
      }
    }
    if (_bCurSpecValid) {
      _pCurLairSpec = new LairSpec(name);
    }

  }
  else if (lcmp(tokens[0], "LA_END", 1)) {
    if (_bCurSpecValid) {
      _vecLairs.push_back(_pCurLairSpec);
    }
    _pCurLairSpec = nullptr;
  }
  else if (lcmp(tokens[0], "LA_DEFAULT", 2)) {
    if (_bCurSpecValid == true) {
      bool bDef = TypeConv::strToBool(getCleanToken(tokens, iind));
      _pCurLairSpec->_bDefault = bDef;

      if (bDef == true) {
        for (LairSpec* ls : _vecLairs) {
          if (ls->getDefault() == true) {
            BRLogWarn(tokens[0] + ": " + _pCurLairSpec->getName() + " and " + ls->getName() + ": Default lair spec already set. Ignoring");
            _pCurLairSpec->_bDefault = false;
          }
        }
      }
    }
  }
  else if (lcmp(tokens[0], "LA_PRIORITY", 2)) {
    if (_bCurSpecValid == true) {
      int pri = TypeConv::strToInt(getCleanToken(tokens, iind));
      for (LairSpec* ls : _vecLairs) {
        if (ls->getPriority() == pri) {
          BRLogWarn(tokens[0] + ": " + _pCurLairSpec->getName() + ": Priority is already used.");
          break;
        }
      }

      if (_bCurSpecValid) {
        _pCurLairSpec->_iPriority = pri;
      }
    }
  }
  else if (lcmp(tokens[0], "LA_AREA", 7)) {
    if (_bCurSpecValid == true) {
      ivec3 a, b;
      a.x = TypeConv::strToInt(getCleanToken(tokens, iind));
      a.y = TypeConv::strToInt(getCleanToken(tokens, iind));
      a.z = TypeConv::strToInt(getCleanToken(tokens, iind));
      b.x = TypeConv::strToInt(getCleanToken(tokens, iind));
      b.y = TypeConv::strToInt(getCleanToken(tokens, iind));
      b.z = TypeConv::strToInt(getCleanToken(tokens, iind));

      _pCurLairSpec->_mxGlobBounds.setMin(std::move(a));
      _pCurLairSpec->_mxGlobBounds.setMax(std::move(b));
      if ((a.x > b.x) || (a.y > b.y) || (a.z > b.z)) {
        BRLogError(tokens[0] + ":Lair area had a minimum value greater than a maximum.");
        _bCurSpecValid = false;
      }
    }
  }
  else if (lcmp(tokens[0], "LA_TILE", 5)) {
    if (_bCurSpecValid == true) {
      LairTile* lt = parseLairTile(tokens, _pCurLairSpec->getName(), iind);

      if (lt != nullptr) {
        _pCurLairSpec->getLairTiles().push_back(lt);
      }

    }
  }
  else if (lcmp(tokens[0], "LA_FUNC_SOLID", 5)) {
    if (_bCurSpecValid == true) {
      int octaves = TypeConv::strToInt(getCleanToken(tokens, iind));
      float off = TypeConv::strToFloat(getCleanToken(tokens, iind));
      float pers = TypeConv::strToFloat(getCleanToken(tokens, iind));
      float scale = TypeConv::strToFloat(getCleanToken(tokens, iind));

      if (octaves > 7) {
        BRLogWarn("Noise func has a large number of octaves (" + octaves + ").  World may generate slowly!");
      }

      _pCurLairSpec->getNoiseFunc()->_fOffset = off;
      _pCurLairSpec->getNoiseFunc()->_nOctaves = octaves;
      _pCurLairSpec->getNoiseFunc()->_fPersistence = pers;
      _pCurLairSpec->getNoiseFunc()->_fScale = scale;

    }
  }
  else if (lcmp(tokens[0], "LA_CLIMATE", 9)) {
    if (_bCurSpecValid == true) {
      ClimateSpec cp = parseClimate(tokens, _pCurLairSpec->getName(), iind);
      if (_bCurSpecValid) {
        *(_pCurLairSpec->getClimateParams()) = cp;
      }
    }
  }
  else if (lcmp(tokens[0], "LA_WALKER", 4)) {
    if (_bCurSpecValid == true) {
      string_t name = getCleanToken(tokens, iind);

      MpFloat temp = parse_mxf2(tokens, _pCurLairSpec->getName(), iind);

      WalkerSpec* ps = getWalkerSpecByName(name);

      if (ps == nullptr) {
        BRLogError(tokens[0] + ": While parsing lair, '" + _pCurLairSpec->getName() + "' Could not find the walker spec '" + name + "'.");

      }

      if (_bCurSpecValid) {
        LairWalker* lw = new LairWalker(ps, _pCurLairSpec);
        lw->_mxPct = temp;
        _pCurLairSpec->getLairWalkers().push_back(lw);
      }
    }
  }


}
void ObFile::parseGetSetMotion(string_t& tok, string_t& msName, std::shared_ptr<SpriteSpec>& out_ms) {
  //   Motion25Spec* mots = getMot(msName);
  //   if (mots == nullptr) {
  //       BRLogWarn(_pCurObjSpec->_strName, " ", tok, " --> ERROR - Motion ", msName, " not found.");
  //       _bCurSpecValid = false;
  //   }
  //   else {
  //       out_ms = mots;
  //   }
}

std::shared_ptr<SpriteSpec> ObFile::getMot(string_t& name) {
  for (size_t iMot = 0; iMot < _vecMotionSpecs.size(); ++iMot) {
    if (StringUtil::equalsi(_vecMotionSpecs[iMot]->getName(), name)) {
      return _vecMotionSpecs[iMot];
    }
  }
  return nullptr;
}


void ObFile::processMotion(string_t name, std::vector<string_t>& files, float fDuration, bool bLoop, bool bRandomSel) {
  std::shared_ptr<SpriteSpec> ps = std::make_shared<SpriteSpec>(_pBucket, name, files, fDuration, bLoop, bRandomSel);
  _vecMotionSpecs.push_back(ps);
}
//void ObFile::removeMotion(Motion25Spec* ps){
//    for(int mi=0; mi<_vecMotionSpecs.size(); ++mi){
//        if(_vecMotionSpecs[mi] == ps){
//            _vecMotionSpecs.erase(_vecMotionSpecs.begin()+mi);
//            break;
//        }
//    }
//}
void ObFile::processTile(string_t name, int32_t index, GridMeshLayer::e eMatterMode,
  std::shared_ptr<SpriteSpec> pTop, std::shared_ptr<SpriteSpec> pSide, std::shared_ptr<SpriteSpec> pBot, ClimateSpec& cp, float rarity) {

  Tile25Spec* ps = new Tile25Spec(name, index, eMatterMode, pTop, pSide, pBot, cp, rarity);
  _vecTileSpecs.push_back(ps);

  if (eMatterMode == GridMeshLayer::e::Opaque) {
  }
  else if (eMatterMode == GridMeshLayer::e::Transparent/* || eMatterMode == MatterMode::e::Gas*/) {
    if (pTop->getDefaultDuration() > 0) {
      processShiftMotionImage(pTop, true, true);
    }
    if (pSide->getDefaultDuration() > 0) {
      processShiftMotionImage(pSide, false, true);
    }
    if (pBot->getDefaultDuration() > 0) {
      processShiftMotionImage(pBot, true, true);
    }
  }
}
void ObFile::processShiftMotionImage(std::shared_ptr<SpriteSpec> ps, bool shiftH, bool shiftV) {
  if (ps->getIsGenerated() == true) {
  }
  else if (ps->getFrames().size() > 1) {
    BRLogWarn(ps->getName() + ": Motion to shift had more than 1 frame.  We can only process if they are 1 frame.");
    Gu::debugBreak();
  }
  else if (ps->getFrames().size() == 0) {
    BRLogWarn(ps->getName() + ": Motion to shift had no frames!");
    Gu::debugBreak();
  }
  else {
    SpriteFrame* pFrame = ps->getFrames()[0];
    string_t baseImgName = pFrame->getImageName();
    BRLogInfo("Processing " + baseImgName);

    std::shared_ptr<Img32> bi = nullptr;
    try {
      bi = Gu::loadImage(pFrame->getImageName());
    }
    catch (Exception * ex) {
      BRLogError("Failed to Open image.:\r\n" + ex->what());
      return;
    }

    //  _CrtSetDbgFlag(_CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_CRT_DF);

      //Remove the "loaded" frame.
    DEL_MEM(pFrame);
    ps->getFrames().clear();

    //Now create a temp image so we can do a recurrance copy on the image.
    std::shared_ptr<Img32> temp = std::make_shared<Img32>();
    temp->create(bi->getWidth(), bi->getHeight());

    //Assuming the image is square/ do a shift/h/v
    for (int ii = 0; ii < bi->getWidth(); ++ii) {
      temp->getData()->copyFrom(bi->getData());

      std::shared_ptr<Img32> pNew = std::make_shared<Img32>();
      pNew->create(bi->getWidth(), bi->getHeight());
      pNew->getData()->copyFrom(bi->getData());

      if (shiftH) {
        temp->shiftH(ii, *pNew);
        temp->getData()->copyFrom(pNew->getData());
      }
      if (shiftV) {
        temp->shiftV(ii, *pNew);
        temp->getData()->copyFrom(pNew->getData());
      }

      string_t genName = baseImgName + "-gen-" + ii;
      ps->addGeneratedFrame(genName, pNew);
    }

    Gu::freeImage(bi);

    //**IMPORTANT set this.
    ps->setIsGenerated(true);
  }

}

Tile25Spec* ObFile::getTileSpecByName(string_t name) {
  Hash32 h = STRHASH(name);
  for (Tile25Spec* pt : _vecTileSpecs) {
    if (pt->getHashName() == h) {
      return pt;
    }
  }
  return nullptr;
}
MorphTile* ObFile::getMorphTileGroupByName(string_t name) {
  Hash32 h = STRHASH(name);
  for (MorphTile* pt : _vecMorphTiles) {
    if (pt->getNameHashed() == h) {
      return pt;
    }
  }
  return nullptr;
}
LairGenLocale::e ObFile::parseGenLocale(string_t str) {
  LairGenLocale::e ret = LairGenLocale::e::Glob;
  if (StringUtil::equalsi(str, "G")) {
    ret = LairGenLocale::e::Glob;
  }
  else if (StringUtil::equalsi(str, "L")) {
    ret = LairGenLocale::e::Lair;
  }
  else if (StringUtil::equalsi(str, "W")) {
    ret = LairGenLocale::e::World;
  }
  else {
    if (_bCurSpecValid) {
      BRLogError("For Lair '" + _pCurLairSpec->getName() + "', Unrecognized Tile Gen Locality: " + str);
      _bCurSpecValid = false;
    }
  }
  return ret;
}

LairTile* ObFile::parseLairTile(std::vector<string_t>& tokens, string_t specName, int& iind) {
  LairTile* ts = nullptr;

  string_t name = getCleanToken(tokens, iind);
  bool bDefault = TypeConv::strToBool(getCleanToken(tokens, iind));
  float fMinPct = TypeConv::strToFloat(getCleanToken(tokens, iind));
  float fMaxPct = TypeConv::strToFloat(getCleanToken(tokens, iind));

  if (StringUtil::equalsi(name, "none")) {
    return nullptr;
  }

  //  LairGenLocale::e eGenLocale = parseGenLocale(sLocality);

  if (fMinPct < 0.0f || fMinPct > 100.0f) {
    BRLogError(tokens[0] + ":For  '" + specName + "', Invalid Min Pct: " + fMinPct);
    _bCurSpecValid = false;
  }
  if (fMaxPct < 0.0f || fMaxPct > 100.0f) {
    BRLogError(tokens[0] + ":For  '" + specName + "', Invalid Max Pct: " + fMaxPct);
    _bCurSpecValid = false;
  }
  if (fMinPct > fMaxPct) {
    BRLogError(tokens[0] + ":For  '" + specName + "', Min Pct Is greater than max Pct");
    _bCurSpecValid = false;
  }


  MorphTile* sp = getMorphTileGroupByName(name);
  if (sp == nullptr) {
    BRLogError(tokens[0] + ":For '" + specName + "' Morph Tile '" + name +
      "' was not found.  Make sure that the LA_BEG comes after all tiles (TIL) and that it's defined in MT_BEG.");
    _bCurSpecValid = false;
  }

  if (_bCurSpecValid) {
    ts = new LairTile(sp, _pCurLairSpec);
    ts->_mxPct.setMin(std::move(fMinPct));
    ts->_mxPct.setMax(std::move(fMaxPct));
  }

  return ts;
}

float ObFile::parseNbr_float(std::vector<string_t>& tokens, int& iind) {
  float ret;
  string_t nbr = getCleanToken(tokens, iind);
  if (StringUtil::equalsi(nbr, "-i")) {
    ret = -FLT_MAX;
  }
  else if (StringUtil::equalsi(nbr, "i")) {
    ret = FLT_MAX;
  }
  else {
    ret = TypeConv::strToInt(nbr);
  }
  return ret;
}
int32_t ObFile::parseNbr_int(std::vector<string_t>& tokens, int& iind) {
  int32_t ret;
  string_t nbr = getCleanToken(tokens, iind);
  if (StringUtil::equalsi(nbr, "-i")) {
    ret = -INT_MAX;
  }
  else if (StringUtil::equalsi(nbr, "i")) {
    ret = INT_MAX;
  }
  else {
    ret = TypeConv::strToInt(nbr);
  }
  return ret;
}
uint32_t ObFile::parseNbr_uint(std::vector<string_t>& tokens, int& iind) {
  uint32_t ret;
  string_t nbr = getCleanToken(tokens, iind);
  if (StringUtil::equalsi(nbr, "-i")) {
    BRLogWarn(tokens[0] + ": -i (infinity) is invalid for an unsigned int parameter.");
    ret = 0;
  }
  else if (StringUtil::equalsi(nbr, "i")) {
    ret = UINT_MAX;
  }
  else {
    ret = TypeConv::strToInt(nbr);
  }
  return ret;
}
MpVec3i ObFile::parse_mxv3(std::vector<string_t>& tokens, string_t specName, int& iind) {

  MpVec3i uv;

  uv.setMin(ivec3(parseNbr_int(tokens, iind),
    parseNbr_int(tokens, iind),
    parseNbr_int(tokens, iind)));

  uv.setMax(ivec3(parseNbr_int(tokens, iind),
    parseNbr_int(tokens, iind),
    parseNbr_int(tokens, iind)));

  if ((uv.getMin().x > uv.getMax().x) || (uv.getMin().y > uv.getMax().y) || (uv.getMin().z > uv.getMax().z)) {
    BRLogError(tokens[0] + " For Spec, '" + specName + "' a min component was greater than a max component '" +
      uv.getMin().toString() + " , " + uv.getMax().toString() + "'");
    _bCurSpecValid = false;
  }

  return uv;
}
MpUint ObFile::parse_mxui2(std::vector<string_t>& tokens, string_t specName, int& iind) {

  MpUint ui;

  int32_t cMin = parseNbr_uint(tokens, iind);
  int32_t cMax = parseNbr_uint(tokens, iind);

  if (cMin < 0) {
    BRLogError(tokens[0] + " For Spec, '" + specName + "' invalid min value '" + cMin + "'");
    _bCurSpecValid = false;
  }
  if (cMax < 0) {
    BRLogError(tokens[0] + " For Spec, '" + specName + "' invalid max value '" + cMax + "'");
    _bCurSpecValid = false;
  }
  if (cMin > cMax) {
    BRLogError(tokens[0] + " For Spec, '" + specName + "' min was greater than max '" + cMin + " , " + cMax + "'");
    _bCurSpecValid = false;
  }

  if (_bCurSpecValid) {
    ui.setMin(std::move(cMin));
    ui.setMax(std::move(cMax));
  }

  return ui;
}
MpFloat ObFile::parse_mxf2(std::vector<string_t>& tokens, string_t specName, int& iind) {

  MpFloat ui;

  ui.setMin(parseNbr_float(tokens, iind));
  ui.setMax(parseNbr_float(tokens, iind));

  if (ui.getMin() > ui.getMax()) {
    BRLogError(tokens[0] + ": For Spec, '" + specName + "' min was greater than max '" + ui.getMin() + " , " + ui.getMax() + "'");
    _bCurSpecValid = false;
  }

  return ui;
}
ClimateSpec ObFile::parseClimate(std::vector<string_t>& tokens, string_t specName, int& iind) {
  ClimateSpec cp;
  cp.getTemp() = parse_mxf2(tokens, specName, iind);
  cp.getPrecip() = parse_mxf2(tokens, specName, iind);
  cp.getLight() = parse_mxf2(tokens, specName, iind);
  cp.getDepth() = parse_mxf2(tokens, specName, iind);

  return cp;
}
WalkerSpec* ObFile::getWalkerSpecByName(string_t n) {
  Hash32 h = STRHASH(n);
  for (WalkerSpec* ws : _vecWalkers) {
    if (ws->getNameHashed() == h) {
      return ws;
    }
  }
  return nullptr;
}


}//ns Game
