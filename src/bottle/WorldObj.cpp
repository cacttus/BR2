#include "../base/BaseAll.h"
#include "../base/Gu.h"
#include "../model/ModelCache.h"
#include "../model/Model.h"
#include "../world/PhysicsWorld.h"

#include "../bottle/WorldObj.h"
#include "../bottle/World25.h"
#include "../bottle/BottleUtils.h"

namespace Game {

std::shared_ptr<WorldObj> WorldObj::create(std::string mobFolder, uint32_t typeID, std::string friendlyName, std::string strBox, std::string strPlace) {
    std::shared_ptr<WorldObj> ob = std::make_shared<WorldObj>();
    ob->_iTypeId = typeID;
    ob->_strMobName = mobFolder;
    ob->_placeOptions = parsePlacementOptions(strPlace);
    ob->_vBoxFit = parseBoxFit(strBox);

    return ob;
}
std::shared_ptr<ModelNode> WorldObj::createInstance(std::shared_ptr<World25> pWorld, vec3& r3Pos) {
    std::shared_ptr<ModelSpec> ms = getOrLoadModel();

    vec3 vScale = boxFit(ms, _vBoxFit);

    vec3 vPos;
    vec4 vRot;
    place(r3Pos, vPos, vRot);

    return nullptr;// pWorld->makeObj(ms, vPos, vRot, vScale, std::string(""));
}
void WorldObj::place(const vec3& r3, vec3& outPos, vec4& outRot) {
    //what's this do, well, we have a "grid" of positions on each "minecraft cube" 
    //snaps the given point to that grid BASED on the points we've configured for this.

    //Create a static array of positions mapped to a placement position.


    //Perform the placement finding routine.
    //Find the closest "snap" point on the cube

    //This is actually even harder, because we need to take into account hidden sides.
    //int32_t iClosest = -1;
    //float fnear = FLT_MAX;
    //for (auto p : _mapPlaceOptions) {
    //    auto it = _posMap.find(p.first);
    //    if (it != _posMap.end()) {
    //        float dist2 = r3.distance2(it->second);
    //        if (dist2 < fnear) {
    //            fnear = dist2;
    //            iClosest = p.first;
    //        }
    //    }
    //}

    //if (iClosest != -1) {
    //    outPos = _posMap.find(iClosest)->second;
    //    //TODO: handle the orientation
    //       //   outMode = _mapPlaceOptions.find(iClosest)->second;
    //}
    //else {
    //    outPos.construct(0, 0, 0);
    //    outRot.construct(0, 1, 0, 0);
    //    //failure
    //    Gu::debugBreak();
    //}
}

std::shared_ptr<ModelSpec> WorldObj::getOrLoadModel() {
    if (_pModelSpec == nullptr) {
        std::shared_ptr<ModelSpec> ms = Gu::getModelCache()->getOrLoadModel(_strMobName, true);
        _pModelSpec = ms;
    }
    return _pModelSpec;
}
vec3 WorldObj::boxFit(std::shared_ptr<ModelSpec> ms, vec3& vBoxFit) {
    //Perform the "box fit" routine.
    //Returns a scale that we apply to the model.
    vec3 vR3siz;
    vR3siz.x = ms->getBoundBoxObject()->getWidth();
    vR3siz.y = ms->getBoundBoxObject()->getHeight();
    vR3siz.z = ms->getBoundBoxObject()->getDepth();
    vec3 vBf;
    vBf.x = (vBoxFit.x < 0) ? (1.0f) : ((float)vBoxFit.x);
    vBf.y = (vBoxFit.y < 0) ? (1.0f) : ((float)vBoxFit.y);
    vBf.z = (vBoxFit.z < 0) ? (1.0f) : ((float)vBoxFit.z);
    vec3 vGSiz;
    vGSiz.x = vBf.x * BottleUtils::getCellWidth();
    vGSiz.y = vBf.y * BottleUtils::getCellHeight();//I'm not sure if we should use "height"
    vGSiz.z = vBf.z * BottleUtils::getCellWidth();

    vec3 vScale = vGSiz / vR3siz;

    return vScale;
}
vec3 WorldObj::parseBoxFit(std::string strBox) {
    // prase the box fit
    vec3 vBoxFit(-1, -1, -1);
    float tmp = 0;
    bool err = false;
    std::vector<std::string> s = StringUtil::split(strBox, std::vector<char>{','});
    if (s.size() != 3) {
        err = true;
    }
    else {
        for (int i = 0; i < 3; ++i) {
            s[i] = StringUtil::trim(s[i]);
        }
        if (!StringUtil::equalsi(s[0], "x")) {
            if (TypeConv::strToFloat(s[0], tmp)) {
                vBoxFit.x = tmp;
            }
            else {
                err = true;
            }
        }
        if (!StringUtil::equalsi(s[1], "x")) {
            if (TypeConv::strToFloat(s[2], tmp)) {
                vBoxFit.y = tmp;
            }
            else {
                err = true;
            }
        }
        if (!StringUtil::equalsi(s[2], "x")) {
            if (TypeConv::strToFloat(s[2], tmp)) {
                vBoxFit.z = tmp;
            }
            else {
                err = true;
            }
        }
    }

    if (err) {
        BroLogError("Failed to parse boxfit" + strBox);
        Gu::debugBreak();
    }
    return vBoxFit;
}
std::array<std::vector<PlaceMode::e>, W25SidePlace::e::Count> WorldObj::parsePlacementOptions(std::string strPlace) {
    //Muckass parse routine, but whatever
    std::array<std::vector<PlaceMode::e>, W25SidePlace::e::Count> options;
    PlaceMode::e eMode = PlaceMode::e::None;
    std::vector<std::string> sPlace = StringUtil::split(strPlace, std::vector<char>{','});
    W25SidePlace::e place = W25SidePlace::e::Top;

    //s0-3, e0-3, v0-3, c
    for (std::string s : sPlace) {
        bool bValid = true;

        s = StringUtil::trim(s);
        if (s.length() > 0) {

            if (s[0] == 'n') { eMode = PlaceMode::e::None; }
            else if (s[0] == 'o') { eMode = PlaceMode::e::Orient; }

            else if (s[0] == 'T') { place = W25SidePlace::e::Top; }
            else if (s[0] == 'B') { place = W25SidePlace::e::Bot; }
            else if (s[0] == 'S') { place = W25SidePlace::e::Side; }
            else if (s[0] == 'X') { place = W25SidePlace::e::Cross; }

            else if (s[0] == 's') {
                //0-3, sub-squares

            }
            else if (s[0] == 'e') {
                //0-3, 4 edge verts (along edges, vetex is on center)
            }
            else if (s[0] == 'c') {
                //center of tile
            }
            else if (s[0] == 'v') {
                //corners 0-3
            }
            ////Look for ranges in the form like 'v0-v10'
            //std::string range = s.substr(1);//remove 'v' or 's'
            //std::string si0 = "", si1 = "";
            //size_t ioff = range.find('-');
            //if (ioff != std::string::npos) {
            //    si0 = range.substr(0, ioff);
            //    si1 = range.substr(ioff + 1, range.length() - ioff - 1);
            //}
            //else {
            //    si0 = range;
            //}

            ////Convert index range
            //int32_t i0 = -1, i1 = -1;
            //if (si0 != "") {
            //    if (TypeConv::strToInt(si0, i0) == false) {
            //        bValid = false;
            //    }
            //}
            //if (si1 != "") {
            //    if (TypeConv::strToInt(si1, i1) == false) {
            //        bValid = false;
            //    }
            //}

            ////Add 26 to the placement option if we're a square side (total of 50)
            //if (s[0] == 's') {
            //    if (i0 != -1) {
            //        if (i0 >= 0 && i0 < 24) {
            //            i0 += 26;
            //        }
            //        else {
            //            bValid = false;
            //        }
            //    }
            //    if (i1 != -1) {
            //        if (i1 >= 0 && i1 < 24) {
            //            i1 += 26;
            //        }
            //        else {
            //            bValid = false;
            //        }
            //    }
            //}
            //else if (s[0] == 'v') {
            //    if (i0 != -1) {
            //        if (i0 >= 0 && i0 < 26) {
            //        }
            //        else {
            //            bValid = false;
            //        }
            //    }
            //    if (i1 != -1) {
            //        if (i1 >= 0 && i1 < 26) {
            //        }
            //        else {
            //            bValid = false;
            //        }
            //    }
            //}

            //if (bValid) {
            //    if (i0 != -1 && i1 != -1) {
            //        //Add the range i0 -  i1
            //        if (i0 < i1) {
            //            for (int ind = i0; ind < i1; ind++) {
            //                options.insert(std::make_pair(ind, eMode));
            //            }
            //        }
            //        else {
            //            bValid = false;
            //        }
            //    }
            //    else if (i0 != -1) {
            //        options.insert(std::make_pair(i0, eMode));
            //    }
            //    else {
            //        bValid = false;
            //    }
            //}

        }
        else {
            bValid = false;
        }
        if (bValid == false) {
            BroLogError("Invalid placement position '" + s + "'");
            Gu::debugBreak();
        }
    }

    return options;
}





}//ns Game
