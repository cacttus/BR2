//#include "../model/ModFile.h"
//#include "../model/ModelLoadInfo.h"
//#include "../base/Logger.h"
//
//namespace Game {
//
//ModelLoadInfo* ModFile::getModelLoadInfo()
//{
//    return _pModelLoadInfo;
//}
//
//void ModFile::pkp(std::vector<t_string>& tokens)
//{
//    gatherArgs();
//    
//    t_string tok0,tok1;
//    if(tokens.size()>0)
//        tok0 = tokens[0];
//    if(tokens.size()>1)
//        tok1 = tokens[1];
//
//    // - Parse the given token buffer
//    if(tkModelPart(tok0,tok1))
//        ;
//    else if(lcmp(getTempArg(0),"mod",2))
//    {
//        _pModelLoadInfo->_strModName = tok1;
//    }
//    else if(StringUtil::lowercase(tok0)=="skn")//TODO: replace all the crap with lcmp
//    {
//        if(_pModelLoadInfo->_sknFileName.length()>0)
//            BroThrowException(" [MOD LOADER] Skin file already specified.");
//        _pModelLoadInfo->_sknFileName = tok1;
//    }
//    else if(StringUtil::lowercase(tok0)=="mpt_beg")//TODO: replace all the crap with lcmp
//    {
//        addPart();
//
//        _pCurPart = new ModelLoadInfoPart();
//        _pCurPart->_partName = tok1;
//    }
//    //else if(StringUtil::lowercase(tok0)=="amp")//TODO: replace all the crap with lcmp
//    //{
//    //    if(!_pCurPart)
//    //        BroThrowException(" [MOD Loader] No current model part defined.");
//    //    
//    //    std::vector<t_string> args = StringUtil::split(tok1,',');
//
//    //    if(!(args.size()==4))
//    //        BroThrowException(" [MOD loader] Animation map invalid number of arguments.");
//
//    //    ModelLoadInfoAmap am;
//    //    am._modelAnimName = args[0];
//    //    am._sysAnimName = args[1];
//    //    am._speed = TypeConv::strToFloat(args[2]);
//    //    am._blendWrap = TypeConv::strToBool(args[3]);
//    //    
//    //    // Validate that the FSM state exists for the given anim name
//    //   // AiManager::strToFsmState(am._sysAnimName);
//
//    //    _pModelLoadInfo->_amap.push_back( am);
//    //}
//    else if(StringUtil::lowercase(tok0)=="motions")
//    {
//        //Motion count - do nothing
//    }
//    else if(lcmp(getTempArg(0),"mot",6))
//    {
//        // - An actual animation.  Range of key frame motions
//        //SknFileAnimation* raw = new SknFileAnimation();
//        //raw->_systemName = tokens[1];
//        //raw->_name = tokens[2];
//        //raw->_startkey = strToInt(tokens[3]);
//        //raw->_endkey = strToInt(tokens[4]);
//        //raw->_iTimeMillis = strToInt(tokens[5]);
//
//        //_pModelLoadInfo->_vecRawAnimations.push_back(raw);
//        
//    }
//    else if(lcmp(getTempArg(0),"cbx",8))
//    {
//        //Cube3f* pc = new Cube3f();
//
//        ////Name = getTempArg(1)
//        //pc->_min = rdVec3f(getTempArgs(), 2);
//        //pc->_max = rdVec3f(getTempArgs(), 4);
//        //
//        //_pModelLoadInfo->_vecContactBoxes.add(pc);
//    }    
//    else if(lcmp(getTempArg(0),"phyv",8))
//    {
//        //Cube3f* pc = new Cube3f();
//
//        ////Name = getTempArg(1)
//        //pc->_min = rdVec3f(getTempArgs(), 2);
//        //pc->_max = rdVec3f(getTempArgs(), 4);
//        //
//        //_pModelLoadInfo->_vecPhysicsVolumes.add(pc);
//    }    
//    else if(lcmp(getTempArg(0),"beh",2))
//    {
//        //AI Behavior.
//        _pModelLoadInfo->_strBehaviorType = getTempArg(1);
//    }    
//    else 
//    {
//        BroLogDebug(" [Mod loader] Warning: invalid token encountered: ", tok0);
//    }
//
//}
///**
//*    @fn 
//*    @brief 
//*    @return true if we ate the tokens.
//*/
//bool ModFile::tkModelPart(t_string tok0, t_string tok1)
//{
//    //TODO: replace all the crap with lcmp
//    if( StringUtil::lowercase(tok0)=="tmd" )//TODO: replace all the crap with lcmp
//    {
//        if(!_pCurPart)
//            throw new Exception(" [MOD Loader] No current model part defined.",__LINE__,__FILE__);
//        _pCurPart->_texDiffuse = tok1;
//        return true;
//    }
//    else if(StringUtil::lowercase(tok0)=="tmn")//TODO: replace all the crap with lcmp
//    {
//        if(!_pCurPart)
//            throw new Exception(" [MOD Loader] No current model part defined.",__LINE__,__FILE__);
//        _pCurPart->_texNormal = tok1;
//        return true;
//    }
//    else if(StringUtil::lowercase(tok0)=="obj")
//    {
//        if(!_pCurPart)
//            BroLogError("Error; no part specified");//_pModelLoadInfo->_globalObjFileName = tok1;
//        else
//            _pCurPart->_objFileName = tok1;
//        return true;
//    }
//    else if(StringUtil::lowercase(tok0)=="mtl")
//    {
//        if(!_pCurPart)
//            throw new Exception(" [MOD Loader] No current model part defined.",__LINE__,__FILE__);
//        _pCurPart->_mtlFileName = tok1;
//        return true;
//    }
//    else if(StringUtil::lowercase(tok0)=="shr")
//    {
//        if(!_pCurPart)
//            throw new Exception(" [MOD Loader] No current model part defined.",__LINE__,__FILE__);
//        _pCurPart->_shaderName = tok1;
//        return true;
//    }
//    else if(StringUtil::lowercase(tok0)=="flip_triangles")
//    {
//        if(!_pCurPart)
//            BroThrowException(" [MOD Loader] No current model part defined.");
//        _pCurPart->_flipTris = TypeConv::strToBool(tok1);
//        return true;
//    }
//    else if(StringUtil::lowercase(tok0)=="agc")
//    {
//        _pCurPart->_autoGenerateContactBoxes=TypeConv::strToBool(tok1);
//        return true;
//    }
//    else if(StringUtil::lowercase(tok0)=="redo_normals")
//    {
//        _pCurPart->_bRedoNormals = TypeConv::strToBool(tok1);
//        return true;
//    }
//
//
//    return false;
//}
//
////ModelLoadInfo* ModFile::parse(char* buf)
////{
////
////    int nKd=0;
////    char c;
////    ptr = buf;
////    std::vector<t_string> tokens;
////    t_string token;
////    while(ptr)
////    {
////        c=*ptr;
////        if(c=='#')
////        { 
////            if(tokens.size() >=2)
////                pkp(tokens[0],tokens[1],nKd);// - Parse the given token buffer
////            
////            tokens.clear();
////            eatLine();
////        }
////        else if(c=='\n')
////        {
////            if(tokens.size() >=2)
////                pkp(tokens[0],tokens[1],nKd);// - Parse the given token buffer
////            
////            tokens.clear();
////            eatLine();
////        }
////        else if(IOFile::isWs(c))
////        {
////            if(token.length())
////                tokens.push_back( t_string(token) );
////            token="";
////
////            inc();
////        }
////        else
////        {
////            if(b_eof)
////            {
////                if(tokens.size() >=2)
////                    pkp(tokens[0],tokens[1],nKd);// - Parse the given token buffer
////            
////                break;
////            }
////            token+=c;
////            inc();
////        }
////        
////    }
////
////}
//void ModFile::addPart()
//{
//    if(_pCurPart)
//    {
//        _pModelLoadInfo->_vecParts.push_back(_pCurPart);
//        _pCurPart=NULL;
//    }
//}
//void  ModFile::preLoad()
//{
//    _pModelLoadInfo = new ModelLoadInfo();
//
//}
//void  ModFile::postLoad()
//{
//
//    // - Add final part
//    addPart();
//}
////ModelLoadInfo* ModFile::load(DiskLoc fileLoc)
////{
////    char* buf;
////    std::fstream in;
////    in.open(fileLoc.c_str(), std::ios::in|std::ios::binary);
////    if(!in.good())
////    {
////        in.close();
////        throw new Exception(FILE_NOTFOUND,__LINE__,__FILE__);
////    }
////    in.seekg(0,std::ios::end);
////    size_t x = in.tellg();
////    in.seekg(0,std::ios::beg);
////    buf = new char[x];
////    in.read(buf,x);
////    in.close();
////
////
////    // - Parse file contents
////    _pModelLoadInfo = parse(buf);
////
////    return _pModelLoadInfo;
////}
//
////+-- CTOR/DTOR --+
//ModFile::ModFile() : 
//_pModelLoadInfo(NULL),
//_pCurPart(NULL)
//{
//}
//
//ModFile::~ModFile()
//{
//    if(_pModelLoadInfo)
//        delete _pModelLoadInfo;
//    _pModelLoadInfo=NULL;
//}
//
//}//ns game
