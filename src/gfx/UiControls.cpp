#include "../base/Base.h"
#include "../base/Fingers.h"
#include "../base/Img32.h"

#include "../math/Box2x.h"
#include "../math/Vec4x.h"

#include "../gfx/Gui2d.h"
#include "../gfx/TexCache.h"
#include "../gfx/CameraNode.h"
#include "../gfx/WindowViewport.h"
#include "../gfx/FrustumBase.h"
#include "../gfx/ShaderBase.h"
#include "../gfx/Picker.h"
#include "../gfx/ShaderBase.h"
#include "../gfx/ShaderMaker.h"
#include "../gfx/RenderUtils.h" 
#include "../gfx/UiControls.h"
#include "../gfx/MegaTex.h"
#include "../gfx/RenderSettings.h"

#include "../model/MeshNode.h"
#include "../model/MeshSpec.h"
#include "../model/UtilMeshInline.h"
#include "../model/TextBufferMesh.h"
#include "../model/VaoDataGeneric.h"
#include "../model/VboData.h"
#include "../model/IboData.h"


namespace Game {

//////////////////////////////////////////////////////////////////////////
std::shared_ptr<UiEventFunc> UiEventFunc::create(std::function<void(UiEventId::e eventId, void*)> f) {
    std::shared_ptr<UiEventFunc> ef = std::make_shared<UiEventFunc>();
    ef->_func = f;
    return ef;
}
//////////////////////////////////////////////////////////////////////////
#pragma region UIElement
std::shared_ptr<UiElement> UiElement::create() {
    std::shared_ptr<UiElement> ui = std::make_shared<UiElement>();
    ui->init();
    return ui;
}
UiElement::UiElement() {
}
UiElement::~UiElement() {
}
void UiElement::init() {
    setName("UiElement");

}
vec4 UiElement::makeClipRectForRender(const Box2f& b2ClipRect) {
    vec4 clipRect;

    //we have a parent, and parent hides stuff
    clipRect.x = b2ClipRect._p0.x;
    clipRect.y = b2ClipRect._p1.y;//Note the swap of y here, for OpenGl texture coords
    clipRect.z = b2ClipRect._p1.x;
    clipRect.w = b2ClipRect._p0.y;

    return clipRect;
}
bool UiElement::removeEvent(UiEventId::e evId, std::shared_ptr<UiEventFunc> f) {
    //Layman's: For each Mouse Button State we have a list of functions that get called when it is active (like clicked)
    EventMap::iterator e = _events.find(evId);
    if (e == _events.end()) {
        return false;
    }
    size_t i;
    for (i = 0; i < e->second.size(); ++i) {
        if (e->second[i] == f) {
            break;
        }
    }
    if (i < e->second.size()) {
        e->second.erase(e->second.begin() + i);
        return true;
    }
    return false;
}
void UiElement::addEvent(UiEventId::e evId, std::shared_ptr<UiEventFunc> f) {
    //Layman's: For each Mouse Button State we have a list of functions that get called when it is active (like clicked)
    EventMap::iterator e = _events.find(evId);
    if (e == _events.end()) {
        EventFuncList newmap = EventFuncList();
        _events.insert(std::make_pair(evId, newmap));
        e = _events.find(evId);
    }
    e->second.push_back(f);
}
void UiElement::doMouseEvents(std::shared_ptr<Fingers> pFingers) {
    //This seems like the moste fficient way to do this.
    ButtonState::e eLmb = pFingers->getLmbState();
    ButtonState::e eRmb = pFingers->getRmbState();
    ButtonState::e eMmb = pFingers->getMmbState();

    EventMap::iterator it;

    //This caching thing doesn't work because for hover states we need to call teh event every fraem
    //**This is also probably why scrollbar was sluggish
   // if (eLmb != _eLastLmb || eLmb == ButtonState::e::Up) {
  //  _eLastLmb = eLmb;
    if (eLmb == ButtonState::e::Up) {
        if ((it = _events.find(UiEventId::e::Mouse_Lmb_Up)) != _events.end()) {
            for (std::shared_ptr<UiEventFunc> ef : it->second) {
                ef->call(it->first, nullptr);
            }
        }
    }
    else if (eLmb == ButtonState::e::Down) {
        if ((it = _events.find(UiEventId::e::Mouse_Lmb_Down)) != _events.end()) {
            for (std::shared_ptr<UiEventFunc> ef : it->second) {
                ef->call(it->first, nullptr);
            }
        }
    }
    else if (eLmb == ButtonState::e::Press) {
        if ((it = _events.find(UiEventId::e::Mouse_Lmb_Press)) != _events.end()) {
            for (std::shared_ptr<UiEventFunc> ef : it->second) {
                ef->call(it->first, nullptr);
            }
        }
    }
    else if (eLmb == ButtonState::e::Release) {
        if ((it = _events.find(UiEventId::e::Mouse_Lmb_Release)) != _events.end()) {
            for (std::shared_ptr<UiEventFunc> ef : it->second) {
                ef->call(it->first, nullptr);
            }
        }
    }
    //  }

    // if (eRmb != _eLastRmb) {
  //  _eLastRmb = eRmb;
    if (eRmb == ButtonState::e::Up) {
        if ((it = _events.find(UiEventId::e::Mouse_Rmb_Up)) != _events.end()) {
            for (std::shared_ptr<UiEventFunc> ef : it->second) {
                ef->call(it->first, nullptr);
            }
        }
    }
    else if (eRmb == ButtonState::e::Down) {
        if ((it = _events.find(UiEventId::e::Mouse_Rmb_Down)) != _events.end()) {
            for (std::shared_ptr<UiEventFunc> ef : it->second) {
                ef->call(it->first, nullptr);
            }
        }
    }
    else if (eRmb == ButtonState::e::Press) {
        if ((it = _events.find(UiEventId::e::Mouse_Rmb_Press)) != _events.end()) {
            for (std::shared_ptr<UiEventFunc> ef : it->second) {
                ef->call(it->first, nullptr);
            }
        }
    }
    else if (eRmb == ButtonState::e::Release) {
        if ((it = _events.find(UiEventId::e::Mouse_Rmb_Release)) != _events.end()) {
            for (std::shared_ptr<UiEventFunc> ef : it->second) {
                ef->call(it->first, nullptr);
            }
        }
    }
    //   }

       //if (eMmb != _eLastMmb) {
       //    _eLastMmb = eMmb;
       //    if (eMmb == ButtonState::e::Up) {
       //        if ((it = _events.find(UiEventId::e::Mouse_Mmb_Up)) != _events.end()) {
       //            for (EventFunc ef : it->second) {
       //                ef(it->first, nullptr);
       //            }
       //        }
       //    }
       //    else if (eMmb == ButtonState::e::Down) {
       //        if ((it = _events.find(UiEventId::e::Mouse_Mmb_Down)) != _events.end()) {
       //            for (EventFunc ef : it->second) {
       //                ef(it->first, nullptr);
       //            }
       //        }
       //    }
       //    else if (eMmb == ButtonState::e::Press) {
       //        if ((it = _events.find(UiEventId::e::Mouse_Mmb_Press)) != _events.end()) {
       //            for (EventFunc ef : it->second) {
       //                ef(it->first, nullptr);
       //            }
       //        }
       //    }
       //    else if (eMmb == ButtonState::e::Release) {
       //        if ((it = _events.find(UiEventId::e::Mouse_Mmb_Release)) != _events.end()) {
       //            for (EventFunc ef : it->second) {
       //                ef(it->first, nullptr);
       //            }
       //        }
       //    }
       //}

}
void UiElement::update(std::shared_ptr<Fingers> pFingers) {
    if (getLayoutVisible() == false || getRenderVisible() == false) {
        return;
    }
    _bPickedLastFrame = false;
    _eMouseState = ButtonState::e::Up;//Reset mouse state

    //Update various events.  Dragging
    if (_pDragInfo != nullptr) {
        _pDragInfo->update(pFingers);
    }

    //Recur
    for (auto p : getChildren()) {
        std::shared_ptr<UiElement> ele = p.second;
        ele->update(pFingers);
    }
}
std::shared_ptr<UiElement> UiElement::cloneSelfOnly() {
    std::shared_ptr<UiElement> ret = std::make_shared<UiElement>(*this);
    ret->_pParent = nullptr;
    ret->_mapChildren.clear();// do not clone children
    return ret;
}
bool UiElement::pick(std::shared_ptr<Fingers> fingers) {
    bool bPicked = false;
    const Box2f& q = getLayoutQuad();
    if (getLayoutVisible() == true) {
        if (getRenderVisible() == true) {
            if (getIsPickEnabled() == true) {
                if (q.containsPointInclusive(fingers->getMousePos())) {
                    _iPickedFrameId = Gu::getFrameNumber();

                    if (getPickRoot()) {
                        //Pick root items pick by boundbox
                        bPicked = true;
                    }
                    else {
                        //Else pick all children to find the pick root
                        for (auto it = getChildren().rbegin(); it != getChildren().rend(); it++) {
                            std::shared_ptr<UiElement> ele = it->second;
                            if (ele->pick(fingers) == true) {
                                //We pick the first child in the TOP level sort order
                                //If we hit it, break out because we don't want to pick children underneath
                                bPicked = true;
                                break;
                            }
                        }
                    }

                    //Capture the bubble-up pick event and process events
                    if (bPicked) {
                        doMouseEvents(fingers);
                    }
                }
            }
        }
    }
    _bPickedLastFrame = bPicked;
    return bPicked;
}
void UiElement::computepad(float& pt, float& pr, float& pb, float& pl) {
    float rl = (getParent()->right().px() - getParent()->left().px());
    float bt = (getParent()->bottom().px() - getParent()->top().px());

    computepad_unit(pt, bt, _padTop);
    computepad_unit(pb, bt, _padBottom);
    computepad_unit(pr, rl, _padRight);
    computepad_unit(pl, rl, _padLeft);
}
void UiElement::computepad_unit(float& pt, float dim, uDim& ud) {
    if (ud.getDimUnit() == UiDimUnit::e::Pixel) {
        pt = ud.px();
    }
    else if (ud.getDimUnit() == UiDimUnit::e::Percent) {
        float bt = (getParent()->bottom().px() - getParent()->top().px());
        pt = ud.pctOf(bt);
    }
    else {
        Gui2d::error("Invalid value for pad");
        pt = 0;
    }
}
void UiElement::performLayout(bool bForce) {
    //thinking of ditching generic layout and going for a manual approach
    //each element has its own performlayout, and we just do it based on that.
    //remove display(), position() 
    //remove uDim = replace with float. make it a fixed size.
    // width, height and position is all a fixed size.
    // we compute them in the performlaout of parent containers.

    //all elements start out as block then we collapse them to be inline
    //1 if computed, expand computed quad to w/h of parent container, else leave fixed
    //2 compute children recursive taking "full size" into account
    //3 collapse computed quad if inline

    ////initially fill to parent. Then after computing children, we'll collapse inline elements
    ////conversely why not initially set to zero, then grow? i don't know!
    //if(getParent() != nullptr) {
    //    if (width().getDimUnit() == UiDimUnit::e::Auto) {
    //        if (position() == UiPositionMode::e::Relative) {
    //            //sub from our position
    //            //right() = uDim()
    //        }
    //        else if (position() == UiPositionMode::e::Static) {
    //            if(display()==UiDisplayMode::e::InlineNoWrap || display()==UiDisplayMode::e::InlineWrap){

    //            }
    //            else if(display() == UiDisplayMode::e::Block) {

    //            }
    //            //otherwise our width fills parent, however..
    //            //what about layouts?
    //        }
    //    }
    //}
    //

    //3/4 - adding "auto" autosize
    positionChildren(bForce);
    performLayoutChildren(bForce);

    //finalize Child Quad
    for (auto p : getChildren()) {
        std::shared_ptr<UiElement> ele = p.second;
        if (ele->getLayoutVisible()) {
            layoutEleQuad(ele);
        }
    }

    computeContentQuad();
}
void UiElement::performLayoutChildren(bool bForce){

    for (auto p : getChildren()) {
        std::shared_ptr<UiElement> ele = p.second;

        if (ele->getLayoutVisible()) {

            ele->performLayout(bForce);
            layoutEleQuad(ele);
        }
    }

}
void UiElement::positionChildren(bool bForce) {
    //Naive caching - scrolling is still slow
    float wCur = _b2ComputedQuad.right() - _b2ComputedQuad.left();
    float hCur = _b2ComputedQuad.bottom() - _b2ComputedQuad.top();
    float lCur = _b2ComputedQuad.left();
    float tCur = _b2ComputedQuad.top();

    //Cache Method 2 - Positional 2/25/18 Here - if the element w/h is the same, then just move it and move its children. This skips expensive layouts while also allowing dragging to be quick.
    static bool dbg_bDisableCaching = false;
    float eps = 0.1f;
    bool dbg_bEnteredCachedRoutine = false;

    bool dbg_bEnableCache = false;

    if (dbg_bEnableCache && 
        MathUtils::fuzzyEquals(wCur, _fLastWidth, eps) && 
        MathUtils::fuzzyEquals(hCur, _fLastHeight, eps) &&
        _bLayoutChanged == false && 
        bForce == false && 
        dbg_bDisableCaching == false
        ) {
        //width/height hasn't changed, but statics need to be repositioned if position has changed.
        dbg_bEnteredCachedRoutine = true;
        if (MathUtils::fuzzyEquals(lCur, _fLastLeft, eps) && MathUtils::fuzzyEquals(tCur, _fLastTop, eps)) {
            //Left/Top has not changed
            //So this element genrally has not changed so don't change children
            //note: if we support auto-resizing parents in the future, all this logic is useless
        }
        else {
            //Position has changed, so we need to update position of all children
            float dt = _top.px() - _fLastTop;
            float dl = _left.px() - _fLastLeft;
            for (auto p : getChildren()) {
                if (p.second->getLayoutVisible()) {
                    std::shared_ptr<UiElement> ee = p.second;
                    ee->_b2ComputedQuadLast = ee->_b2ComputedQuad;
                    //Pos Changed
                    if (ee->position() == UiPositionMode::e::Relative) {
                        computePositionalElement(ee);
                    }
                    else if (ee->position() == UiPositionMode::e::Static) {
                        //Do nothing, this is automatically computed
                    }
                }
            }
        }
    }
    else {
        //W/h has changed.  We need to update everything.
        //Sort elements into layers by sort ID, and process each layer
        uint32_t uiLast = UINT_MAX;
        std::vector<std::shared_ptr<UiElement>> bucket;
        for (auto p : getChildren()) {
            std::shared_ptr<UiElement> ele = p.second;

            ele->validateDims();

            if (ele->getLayoutVisible()) {
                ele->_b2ComputedQuadLast = ele->_b2ComputedQuad;

                if (ele->position() == UiPositionMode::e::Static) {
                    //Static elements - Have a Flow, and computed position
                    if (p.first != uiLast) {
                        uiLast = p.first;
                        if (bucket.size() > 0) {
                            layoutLayer(bucket);
                            bucket.clear();
                        }
                    }
                    bucket.push_back(p.second);
                }
                else if (ele->position() == UiPositionMode::e::Relative) {
                    //Calc positioned elements (does not follow flow), static position
                    computePositionalElement(ele);
                }
            }
        }
        if (bucket.size() > 0) {
            layoutLayer(bucket);
            bucket.clear();
        }
    }



    //if (StringUtil::contains(_strName, "ContentContainer")) {
    //    if (_b2ContentQuad.width() <= 0.001) {
    //        int n = 0;
    //        n++;
    //    }

    //}
    //if (StringUtil::contains(_strName, "Label")) {
    //    int n = 0;
    //    n++;
    //}
    //Cache Values
    _fLastWidth = wCur;
    _fLastHeight = hCur;
    _fLastTop = tCur;
    _fLastLeft = lCur;
    _bLayoutChanged = false;
}
void UiElement::computeContentQuad() {
    //Reset content quad to 0,0
    _b2ContentQuad._p0 = _b2ContentQuad._p1 = vec2(left().px(), top().px());

    float dbgwidth = width().px();
    float dbgheight = height().px();
    //Recur and compute bounds for all children
    float fright = -FLT_MAX;
    float fbottom = -FLT_MAX;
    for (auto p : getChildren()) {
        std::shared_ptr<UiElement> ele = p.second;

        if (ele->getLayoutVisible()) {
            //Add padding for static elements
            float effR = ele->right().px();
            float effT = ele->bottom().px();
            if (ele->position() == UiPositionMode::e::Static) {
                effR -= (ele->padLeft().px() + ele->padRight().px());
            }
            if (ele->position() == UiPositionMode::e::Static) {
                effT -= (ele->padTop().px() + ele->padBottom().px());
            }

            //w/h adjust
            effR -= 1;
            effT -= 1;

            fright = MathUtils::broMax(fright, left().px() + effR);
            fbottom = MathUtils::broMax(fbottom, top().px() + effT);


            //expand contenet quad
            _b2ContentQuad.genExpandByPoint(vec2(fright, fbottom));

            if (_b2ContentQuad.width() > dbgwidth) {
                int nnn = 0;
                nnn++;
            }
            if (_b2ContentQuad.height() > dbgheight) {
                int nnn = 0;
                nnn++;
            }

        }
    }

}
void UiElement::validateDims() {
    //this is needed in case you set any of the fixed position attributes to something like a percent
    //these must be pixel
    //Validate Element dims.
    if (top().getDimUnit() != UiDimUnit::e::Pixel) {
        Gui2d::error("Ui Element Top was not a pixel unit");
        top() = uDim(0, UiDimUnit::e::Pixel);
    }
    if (bottom().getDimUnit() != UiDimUnit::e::Pixel) {
        Gui2d::error("Ui Element bottom was not a pixel unit");
        bottom() = uDim(0, UiDimUnit::e::Pixel);
    }
    if (left().getDimUnit() != UiDimUnit::e::Pixel) {
        Gui2d::error("Ui Element left was not a pixel unit");
        left() = uDim(0, UiDimUnit::e::Pixel);
    }
    if (right().getDimUnit() != UiDimUnit::e::Pixel) {
        Gui2d::error("Ui Element right was not a pixel unit");
        right() = uDim(0, UiDimUnit::e::Pixel);
    }


}
void UiElement::layoutLayer(std::vector<std::shared_ptr<UiElement>> stats) {
    //Calc width with all static blocks using 0 width for autos (expandable blocks).
    float fTotalHeight = 0;
    std::vector<UiElement::UiLine> vecLines;
    vecLines.push_back(UiLine());
    for (std::shared_ptr<UiElement> ele : stats) {
        calcStaticElement(ele, vecLines, 0.0f, 0.0f);
        fTotalHeight += vecLines[vecLines.size() - 1]._height;
    }

    //auto heights are confusing.  So the problem is that the height of a line is indeterminate.  We build the form by widths of elements and 
    //wrap when we reach the end.   What's the actual height of the line?  What we say is *Any line that has at least one
    //*auto height is an auto height line, thus the whole line's height gets auto*
    //3/4 technically auto height should always shrink whether we are inline, or block element
    int32_t nAutoHLines = 0;
    for (size_t iLine = 0; iLine < vecLines.size(); iLine++) {
        UiLine& line = vecLines[iLine];
        for (std::shared_ptr<UiElement> ele : line._eles) {
            if (ele->height().getDimUnit() == UiDimUnit::e::Auto) {
                nAutoHLines++;
                break;// Line height is auto for this line
            }
        }
    }

    //Expand Autos
    //in css block elements expand "grow" to 100% of parent
    //inline elements shrink to smallest size.  our auto is somewhat broken
    //Hmm..Auto size = grow would cause autos to expand to be the size of the max width..which defaults to 99999
    std::vector<UiElement::UiLine> vecLines2;//Technically this dummy variable should be '1'
    vecLines2.push_back(UiLine());
    for (size_t iLine = 0; iLine < vecLines.size(); iLine++) {
        UiLine& line = vecLines[iLine];

        //Sum Autos and average widths
        int32_t nAutoWsLine = 0;
        for (std::shared_ptr<UiElement> ele : line._eles) {
            if (ele->width().getDimUnit() == UiDimUnit::e::Auto) {
                nAutoWsLine++;
            }
        }
        float auto_width = 0;
        float auto_height = 0;
        if (nAutoWsLine > 0) {
            float myWidth = right().px() - left().px();//Container width
            auto_width = MathUtils::broMax(myWidth - line._width, 0.0f) / (float)nAutoWsLine;
            //So we're going to "sprinkle the width" across other autos
            // for each element whose min width > the computed auto
            //subtract from the computed_auto the remaining width minus the auto  (min_width - computed_auto) / remaining_autos4
        }
        if (nAutoHLines > 0) {
            float myHeight = bottom().px() - top().px();//Container height
            auto_height = MathUtils::broMax(myHeight - fTotalHeight, 0.0f) / (float)nAutoHLines;
        }

        //run calculation again, this time with autos
        for (std::shared_ptr<UiElement> ele : line._eles) {
            calcStaticElement(ele, vecLines2, auto_width, auto_height);
        }

    }

}
void UiElement::computePositionalElement(std::shared_ptr<UiElement> ele) {
    //*No padding
    //*No auto sizing
    //Needs to be separate becasue cursor/windwos needs special updating.
    //Comp static wh.
    //**PADDING IS NOT APLIED TO POSITIONAL ELEMENTS**
    float wpx, hpx;
    ele->computewh(wpx, hpx);//Cannot be auto
    ele->applyMinMax(wpx, hpx);
    ele->right() = ele->left().px() + wpx;
    ele->bottom() = ele->top().px() + hpx;
    ele->validateQuad();
}
void UiElement::calcStaticElement(std::shared_ptr<UiElement> ele, std::vector<UiElement::UiLine>& vecLines, float fAutoWidth, float fAutoHeight) {
    if (vecLines.size() == 0) {
        BroThrowException("GUI error - tried to run calc algorithm without any UILines created");
    }
    float myWidth = right().px() - left().px();
    UiLine* line = &vecLines[vecLines.size() - 1];

    //Autos get zero first so we an compute the fixed 
    //statics then we go throguh here again and assign them computed values

    float wpx = 0, hpx = 0;
    ele->computewh(wpx, hpx);//also applies min/max
    if (ele->width().getDimUnit() == UiDimUnit::e::Auto) {
        wpx = fAutoWidth;
    }
    if (ele->height().getDimUnit() == UiDimUnit::e::Auto) {
        hpx = fAutoHeight;
    }
    ele->applyMinMax(wpx, hpx);

    //Remove unnecessary padding to prevent auto and % widths from growing
   // float parent_w = getParent()->right().px() - getParent()->left().px();
   // float parent_h = getParent()->bottom().px() - getParent()->top().px();
   // 
   // wpx + pl + pr

    //*Padding
    float pl = 0, pr = 0, pb = 0, pt = 0;
    ele->computepad(pt, pr, pb, pl);

    float wpx_pad, hpx_pad;
    wpx_pad = wpx + pl + pr;
    hpx_pad = hpx + pb + pt;

    //**Line break
    bool bLineBreak = false;
    if (ele->display() == UiDisplayMode::e::InlineWrap) {
        if (wpx_pad + line->_width > myWidth) {
            bLineBreak = true;
        }
    }
    else if (ele->display() == UiDisplayMode::e::Block) {
        bLineBreak = true;
    }
    else if (ele->display() != UiDisplayMode::e::InlineNoWrap) {
        bLineBreak = false;
    }

    if (bLineBreak) {
        //new line
        UiLine line2;
        line2._top = line->_top + line->_height;
        vecLines.push_back(line2);
        line = &vecLines[vecLines.size() - 1];
    }

    ele->left() = line->_left + line->_width + pl;
    ele->right() = ele->left().px() + wpx;//wpx, not wpx_pad
    line->_width += wpx_pad;
    ele->top() = line->_top + pt;
    ele->bottom() = ele->top().px() + hpx; //hpx, not hpx_pad

    ele->validateQuad();

    //Increse line height WITH PAD
    line->_height = MathUtils::broMax(line->_height, hpx_pad);

    line->_eles.push_back(ele);
}
void UiElement::computewh(float& __out_ wpx, float& __out_ hpx) {

    //Width
    wpx = 0;
    // float effPadLR = padLeft().px() + padRight().px();
    if (width().getDimUnit() == UiDimUnit::e::Pixel) {
        wpx = width().px();
    }
    else if (width().getDimUnit() == UiDimUnit::e::Percent) {
        if (getParent() != nullptr) {
            float rl = (getParent()->right().px() - getParent()->left().px());

            wpx = width().pctOf(rl);
        }
    }
    else if (width().getDimUnit() == UiDimUnit::e::Auto) {
      //  if(position() == UiPositionMode::e::Relative) {
            //Error - relative elements can't be auto
      //      Gui2d::error(TStr(_strName, ": Tried to autosize relative (fixed size) element width."));
     //   }
     //   else {
            wpx = getAutoWidth();
     //   }
    }
    else {
        Gui2d::error("Invalid enum");
    }

    //Height
    hpx = 0;
    // float effPadTB = padBottom().px() + padTop().px();
    if (height().getDimUnit() == UiDimUnit::e::Pixel) {
        hpx = height().px();
    }
    else if (height().getDimUnit() == UiDimUnit::e::Percent) {
        if (getParent() != nullptr) {
            float rl = (getParent()->bottom().px() - getParent()->top().px());
            hpx = height().pctOf(rl);
        }
    }
    else if (height().getDimUnit() == UiDimUnit::e::Auto) {
     //   if (position() == UiPositionMode::e::Relative) {
            //Error - relative elements can't be auto
       //     Gui2d::error(TStr(_strName, ": Tried to autosize relative (fixed size) element height."));
    //    }
    //    else {
            hpx = getAutoHeight();
    //    }
    }
    else {
        Gui2d::error("Invalid enum");
    }
    //Make sure it's an actual heght
    if (hpx < 0.0f) {
        hpx = 0.0f;
    }
    if (wpx < 0.0f) {
        wpx = 0.0f;
    }
}
float UiElement::getAutoWidth() {
    if(display() == UiDisplayMode::e::Block) {
        if(position() == UiPositionMode::e::Relative) {

        }
        //fill 100% of parent
      //  float w = getParent()->getComputedQuad()->width()
    }
    else if(display() == UiDisplayMode::e::InlineNoWrap || display() == UiDisplayMode::e::InlineWrap){
        //shrink to smallest size
        return 0;
    }
    else {
        Gui2d::error("Auto w/h invlaid");
    }
    return 0;//?
}
float UiElement::getAutoHeight() {
    return 0;
}
void UiElement::validateQuad() {
    if (_right.px() < _left.px()) {
        static int nn = 0;
        if (nn == 0) {
            Gui2d::error(Stz "Quad is invalid, rtbl= "+ _right.px()+ ","+ _top.px()+ ","+ _bottom.px()+ ","+ _left.px());
        }
        _right = _left;
    }
    if (_bottom.px() < _top.px()) {
        static int nn = 0;
        if (nn == 0) {
            Gui2d::error(Stz "Quad is invalid, rtbl= "+ _right.px()+ ","+ _top.px()+ ","+ _bottom.px()+ ","+ _left.px());
        }
        _bottom = _top;
    }
}
void UiElement::layoutEleQuad(std::shared_ptr<UiElement> ele) {
    float fr, fl, ft, fb;
    fl = getComputedQuad().left() + ele->left().px();
    ft = getComputedQuad().top() + ele->top().px();
    fr = getComputedQuad().left() + ele->right().px();
    fb = getComputedQuad().top() + ele->bottom().px();

    if (fl > fr || ft > fb) {
        int nnn = 0;
        nnn++;
    }

    ele->computeQuads(fr, fl, ft, fb);
}
void UiElement::computeQuads(float final_r, float final_l, float final_t, float final_b) {
    //Design space quad
    _b2ComputedQuad.left() = final_l;
    _b2ComputedQuad.top() = final_t;
    _b2ComputedQuad.right() = final_r;
    _b2ComputedQuad.bottom() = final_b;

    //Layout Quad (for picking, debug)
    float w1 = 1.0f, h1 = 1.0f;
    w1 = Gui2d::getDesignMultiplierW();
    h1 = Gui2d::getDesignMultiplierH();

    //Set to false if we're controllig coordinates of this element (cursor, or window position)
    if (getShouldScalePositionToDesign() == true) {
        _b2LayoutQuad._p0.x = final_l * w1;
        _b2LayoutQuad._p0.y = final_t * h1;
        _b2LayoutQuad._p1.x = final_r * w1;
        _b2LayoutQuad._p1.y = final_b * h1;
    }
    else {
        //We are an absolute position (cursor)
        _b2LayoutQuad._p0.x = final_l;//These are alerady set
        _b2LayoutQuad._p0.y = final_t;
        _b2LayoutQuad._p1.x = final_l + (final_r - final_l) * w1; // Still Scale the width / height
        _b2LayoutQuad._p1.y = final_t + (final_b - final_t) * h1;
    }

    //Raster Quad (for drawing)
    _b2RasterQuad = _b2LayoutQuad;
    Gu::guiQuad2d(_b2RasterQuad, Gu::getCamera()->getViewport());
}

void UiElement::applyMinMax(float& wpx, float& hpx) {
    //TODO: so this should allow for % width/height of parent to be
    //min/max but that's fucking confusing.  Is it needed? 
    float minw = minWidth().px();
    float maxw = maxWidth().px();
    float minh = minHeight().px();
    float maxh = maxHeight().px();

    //apply min/max to box (not in parent space)
    if (wpx > maxw) {
        wpx = maxw;
    }
    if (wpx < minw) {
        wpx = minw;
    }
    if (hpx > maxh) {
        hpx = maxh;
    }
    if (hpx < minh) {
        hpx = minh;
    }

}
void UiElement::setLayoutChanged(bool bChildren) {
    _bLayoutChanged = true;
    if (bChildren) {
        //if bChildren is true, we go in reverse **this is expensive and should only be used by gui2d
        for (auto c : getChildren()) {
            c.second->setLayoutChanged(true);
        }
    }
    else {
        if (getParent()) {
            //*Recursive.
            getParent()->setLayoutChanged();
        }
    }
}
bool UiElement::isFullyClipped(const Box2f& b2ClipRect) {
    //This simple test saves us a ton of GPU pixel tests
    const Box2f& b = getGLRasterQuad();
    if (b._p1.x < b2ClipRect._p0.x) {
        return true;
    }
    if (b._p1.y > b2ClipRect._p0.y) { //**Norte we flipped Y's </> here - because GL min/max runs the oppostie direction in the Y axis
        return true;
    }
    if (b._p0.x > b2ClipRect._p1.x) {
        return true;
    }
    if (b._p0.y < b2ClipRect._p1.y) {//**Norte we flipped Y's </> here - because GL min/max runs the oppostie direction in the Y axis
        return true;
    }
    return false;
}

Box2f UiElement::shrinkClipRect(const Box2f& b2ClipRect) {
    //Hide all children that go beyond this container.
    //Must be called in the loop so we reset it with every child.
    const Box2f& b = getGLRasterQuad();
    Box2f ret = b2ClipRect;
    if (overflow() == UiOverflowMode::e::Hide) {
        //Shrink the box
        if (b._p0.x > ret._p0.x) {
            ret._p0.x = b._p0.x;
        }
        if (b._p0.y < ret._p0.y) { //**Norte we flipped Y's </> here - because GL min/max runs the oppostie direction in the Y axis
            ret._p0.y = b._p0.y;
        }
        if (b._p1.x < ret._p1.x) {
            ret._p1.x = b._p1.x;
        }
        if (b._p1.y > ret._p1.y) {//**Norte we flipped Y's </> here - because GL min/max runs the oppostie direction in the Y axis
            ret._p1.y = b._p1.y;
        }
        //Make sure it's valid
        if (ret._p0.x > ret._p1.x) {
            ret._p0.x = ret._p1.x;
        }
        if (ret._p1.y > ret._p0.y) {//**Norte we flipped Y's </> here - because GL min/max runs the oppostie direction in the Y axis
            ret._p1.y = ret._p0.y;
        }
    }
    return ret;
}

void UiElement::drawForward(RenderParams& rp, Box2f& b2ClipRect) {
    //Recur to all childs
    for (auto p : getChildren()) {
        std::shared_ptr<UiElement> ele = p.second;
        if (ele->getLayoutVisible()) {
            if (ele->getRenderVisible()) {
                if (ele->isFullyClipped(b2ClipRect) == false) {
                    Box2f clip = shrinkClipRect(b2ClipRect);
                    ele->drawForward(rp, clip);
                }
            }
        }
    }

}
void UiElement::drawBoundBox(std::shared_ptr<UtilMeshInline2d> mi, vec4& color, bool bPickedOnly) {
    if (bPickedOnly && (_iPickedFrameId != Gu::getFpsMeter()->getFrameNumber())) {
        //return bc only picked gets showed
        return;
    }

    for (auto p : getChildren()) {
        std::shared_ptr<UiElement> ele = p.second;
        if (ele->getLayoutVisible()) {
            ele->drawBoundBox(mi, color, bPickedOnly);
        }
    }

    if (color.x > 0 && color.z > 0) {
        //Pick Gui help
    }
    else {
        if (color.x > 0) {
            color.construct(0, 1, 0, 1);
        }
        else if (color.y > 0) {
            color.construct(0, 0, 1, 1);
        }
        else if (color.z > 0) {
            color.construct(1, 0, 0, 1);
        }
    }
    mi->setDefaultColor(color);

    const Box2f& cpyPos = getGLRasterQuad();
    vec2 p0 = cpyPos._p0;
    vec2 p1 = cpyPos._p1;
    mi->vt2(vec2(p0.x, p0.y), vec2(p1.x, p0.y));
    mi->vt2(vec2(p1.x, p0.y), vec2(p1.x, p1.y));
    mi->vt2(vec2(p1.x, p1.y), vec2(p0.x, p1.y));
    mi->vt2(vec2(p0.x, p1.y), vec2(p0.x, p0.y));
}
std::shared_ptr<UiElement> UiElement::addChild(std::shared_ptr<UiElement> c, uint32_t uiSort, bool bUpdateLayout, bool bCheckDupes) {
    if (c == nullptr) {
        //Allow this to simply fail this may happen if you fail to specify certain skin requirements.
        Gui2d::error(Stz "Tried to add a null child to UI element "+ _strName);
        return nullptr;
    }

    bool bCheckDupesResult = false;
    if (bCheckDupes) {
        bCheckDupesResult = checkDupes(c);
    }
    //We can only have 1 child in the tree, becasue we update their contents individually.
    if (bCheckDupesResult == false) {
        //We're good... add it
        _mapChildren.insert(std::make_pair(uiSort, c));
        c->_pParent = getThis<UiElement>();
        if (bUpdateLayout == true) {
            setLayoutChanged();
        }
    }
    return c;
}
bool UiElement::checkDupes(std::shared_ptr<UiElement> childTree) {
    std::shared_ptr<UiElement> rp = nullptr;
    getRootParent(rp);

    std::vector<std::shared_ptr<UiElement>> nodes;
    childTree->getTreeAsList(nodes);
    for (std::shared_ptr<UiElement> e : nodes) {
        bool b = false;;
        rp->findElement(e, b);
        if (b == true) {
            Gui2d::error(Stz "Gui2d: child '"+ e->_strName+ "' found multiple times in GUI, please create new GUI element then add.");
            return true;
        }
    }

    return false;
}
void UiElement::findElement(std::shared_ptr<UiElement> e, bool& b) {
    if (b == true) {
        return;
    }
    for (auto p : getChildren()) {
        std::shared_ptr<UiElement> ele = p.second;
        if (ele == e) {
            b = true;
            return;
        }
    }
}
void UiElement::getRootParent(std::shared_ptr<UiElement>& rp) {
    if (_pParent == nullptr) {
        rp = getThis<UiElement>();
    }
    else {
        _pParent->getRootParent(rp);
    }
}
void UiElement::getTreeAsList(std::vector<std::shared_ptr<UiElement>>& __out_ children) {
    children.push_back(getThis<UiElement>());
    for (auto p : getChildren()) {
        std::shared_ptr<UiElement> ele = p.second;
        ele->getTreeAsList(children);
    }
}

void UiElement::setSort(uint32_t uisort) {
    if (getParent() != nullptr) {
        getParent()->removeChild(getThis<UiElement>());
    }
    getParent()->addChild(getThis<UiElement>(), uisort);
    setLayoutChanged();
}
bool UiElement::removeChild(std::shared_ptr<UiElement> ele, bool bUpdateLayout) {
    std::multimap<uint32_t, std::shared_ptr<UiElement>>::iterator it = _mapChildren.begin();
    for (; it != _mapChildren.end(); it++) {
        if (it->second == ele) {
            _mapChildren.erase(it);
            if (bUpdateLayout == true) {
                setLayoutChanged();
            }
            return true;
        }
    }
    return false;
}
bool UiElement::hasChild(std::shared_ptr<UiElement> ele) {
    std::multimap<uint32_t, std::shared_ptr<UiElement>>::iterator it = _mapChildren.begin();
    for (; it != _mapChildren.end(); it++) {
        if (it->second == ele) {
            return true;
        }
    }
    return false;
}
bool UiElement::setLayoutVisible(bool b, bool bLayoutChanged) {
    bool bChanged = false;
    if (_bVisible != b) {
        bChanged = true;
        _bVisible = b;
        //Honestly, I think we should just remove setLayoutChanged from here and update it manuallly
        if (bLayoutChanged == true) {
            setLayoutChanged();
        }
    }
    return bChanged;
}
bool UiElement::hide() {
    return setLayoutVisible(false);
}
bool UiElement::show() {
    return setLayoutVisible(true);
}
void UiElement::hideLayerRender(uint32_t iLayer) {
    for (auto x : _mapChildren) {
        if (x.first == iLayer) {
            //Don't use || - eraly out
            x.second->hideRender();
        }
    }
}
void UiElement::showLayerRender(uint32_t iLayer) {
    for (auto x : _mapChildren) {
        if (x.first == iLayer) {
            //Don't use || - eraly out
            x.second->showRender();
        }
    }
}
void UiElement::enableDrag(UiDragInfo::DragFunc func) {
    _pDragInfo = std::make_shared<UiDragInfo>();
    _pDragInfo->_func = func;
    std::weak_ptr<UiElement> winw = getThis<UiElement>();
    addEvent(UiEventId::e::Mouse_Lmb_Press, UiEventFunc::create(
        [winw](UiEventId::e ev, void* pv) {
        if (std::shared_ptr<UiElement> w = winw.lock()) {
            w->_pDragInfo->_bDragStart = true;
            w->_pDragInfo->_vDragStart = Gu::getFingers()->getMousePos();
            // w->_pDragInfo->_b2StartBox = w->getComputedQuad();
        }
    }));
    addEvent(UiEventId::e::Mouse_Lmb_Release, UiEventFunc::create(
        [winw](UiEventId::e ev, void* pv) {
        if (std::shared_ptr<UiElement> w = winw.lock()) {
            w->_pDragInfo->_bDrag = false;
        }
    }));
    addEvent(UiEventId::e::Mouse_Lmb_Up, UiEventFunc::create(
        [winw](UiEventId::e ev, void* pv) {
        if (std::shared_ptr<UiElement> w = winw.lock()) {
            w->_pDragInfo->_bDrag = false;
        }
    }));
}
void UiElement::clearChildren() {
    _mapChildren.clear();
}
void UiElement::regenMesh(std::vector<v_GuiVert>& verts, std::vector<v_index32>& inds, Box2f& b2ClipRect) {
    for (auto p : getChildren()) {
        std::shared_ptr<UiElement> ele = p.second;
        if (ele->getLayoutVisible()) {
            if (ele->getRenderVisible()) {
                if (ele->isFullyClipped(b2ClipRect) == false) {
                    Box2f clip = shrinkClipRect(b2ClipRect);
                    ele->regenMesh(verts, inds, clip);
                }
            }
        }
    }

}
void UiElement::setClick(MouseFunc cc) {
    //Makei t so we can only have 1 click event
    if (_releaseEvt == nullptr) {
        std::weak_ptr<UiElement> weak = getThis<UiElement>();
        _releaseEvt = UiEventFunc::create(
            [weak](UiEventId::e evId, void* pv) {
            if (std::shared_ptr<UiElement> bt = weak.lock()) {
                //Press or down - same thing here
                bt->_eMouseState = ButtonState::e::Release;
                if (bt->_clickFunc != nullptr) {
                    bt->_clickFunc();
                }
            }
        }
        );
        addEvent(UiEventId::e::Mouse_Lmb_Release, _releaseEvt);
    }
    _clickFunc = cc;
}
void UiElement::setDown(MouseFunc cc) {
    //Makei t so we can only have 1 click event
    if (_downEvt == nullptr) {
        std::weak_ptr<UiElement> weak = getThis<UiElement>();
        _downEvt = UiEventFunc::create(
            [weak](UiEventId::e evId, void* pv) {
            if (std::shared_ptr<UiElement> bt = weak.lock()) {
                //Press or down - same thing here
                bt->_eMouseState = ButtonState::e::Down;
                if (bt->_downFunc != nullptr) {
                    bt->_downFunc();
                }
            }
        }
        );
        addEvent(UiEventId::e::Mouse_Lmb_Down, _downEvt);
    }
    _downFunc = cc;
}
void UiElement::setPress(MouseFunc cc) {
    //Makei t so we can only have 1 click event
    if (_pressEvt == nullptr) {
        std::weak_ptr<UiElement> weak = getThis<UiElement>();
        _pressEvt = UiEventFunc::create(
            [weak](UiEventId::e evId, void* pv) {
            if (std::shared_ptr<UiElement> bt = weak.lock()) {
                //Press or down - same thing here
                bt->_eMouseState = ButtonState::e::Press;
                if (bt->_pressFunc != nullptr) {
                    bt->_pressFunc();
                }
            }
        }
        );
        addEvent(UiEventId::e::Mouse_Lmb_Press, _pressEvt);
    }
    _pressFunc = cc;
}
void UiElement::setHover(MouseFunc cc) {
    //Makei t so we can only have 1 click event
    if (_upEvt == nullptr) {
        std::weak_ptr<UiElement> weak = getThis<UiElement>();
        _upEvt = UiEventFunc::create(
            [weak](UiEventId::e evId, void* pv) {
            if (std::shared_ptr<UiElement> bt = weak.lock()) {
                //Press or down - same thing here
                bt->_eMouseState = ButtonState::e::Up;
                if (bt->_hoverFunc != nullptr) {
                    bt->_hoverFunc();
                }
            }
        }
        );
        addEvent(UiEventId::e::Mouse_Lmb_Up, _upEvt);
    }
    _hoverFunc = cc;
}
void UiElement::drawDebug() {
    //ONLY CALL ON Gui2d!!
    //Debug Rendering
    if (Gu::getRenderSettings()->getDebug()->getShowGuiBoxesAndDisableClipping()) {
        //draws all children
        Gu::getGraphicsContext()->setLineWidth(1.0f);
        std::shared_ptr<UtilMeshInline2d> mi = std::make_shared<UtilMeshInline2d>(Gu::getGraphicsContext());
        mi->setDefaultColor(vec4(1, 1, 0, 1));
        mi->begin(GL_LINES);
        {
            vec4 c4(1, 0, 0, 1);
            drawBoundBox(mi, c4, false);
        }
        mi->end();
    }

    if (Gu::getRenderSettings()->getDebug()->getPickGui()) {
        //draws all children
        Gu::getGraphicsContext()->setLineWidth(2.0f);
        std::shared_ptr<UtilMeshInline2d> mi = std::make_shared<UtilMeshInline2d>(Gu::getGraphicsContext());
        mi->setDefaultColor(vec4(1, 0, 1, 1));
        mi->begin(GL_LINES);
        {
            vec4 c4(1, 0, 1, 1);
            drawBoundBox(mi, c4, true);
        }
        mi->end();
    }
}
void UiElement::bringToFront(std::shared_ptr<UiElement> child, bool bCreateNewLayer) {
    //CreateNewLayer - brings the child to the front of all Layers in addition to it's own layer.
    //set to false - child will become in the front of just it's own layer.

    uint32_t iMaxLayer = 0;
    if (bCreateNewLayer == false) {
        for (auto p : getChildren()) {
            if (p.second == child) {
                iMaxLayer = p.first;
                break;
            }
        }
    }
    removeChild(child);
    if (bCreateNewLayer == true) {
        for (auto p : getChildren()) {
            iMaxLayer = MathUtils::broMax(iMaxLayer, p.first);
        }
        iMaxLayer++;
    }
    addChild(child, iMaxLayer);
}

#pragma endregion
//////////////////////////////////////////////////////////////////////////
#pragma region UiImage
std::shared_ptr<UiImage> UiImage::create(std::shared_ptr<UiTex> tex, UiImageSizeMode::e eSizeX, UiImageSizeMode::e eSizeY) {
    return UiImage::create(tex, eSizeX, eSizeY, (float)tex->getWidth(), (float)tex->getHeight());
}
std::shared_ptr<UiImage> UiImage::create(std::shared_ptr<UiTex> tex, UiImageSizeMode::e eSizeX, UiImageSizeMode::e eSizeY, float fWidthPx, float fHeightPx) {
    std::shared_ptr<UiImage> pImg = std::make_shared<UiImage>();
    pImg->init();
    pImg->width() = "100%";
    pImg->height() = "100%";
    pImg->_pTexture = tex;
    pImg->_fTileWidthPx = fWidthPx;
    pImg->_fTileHeightPx = fHeightPx;
    pImg->setSizeMode(eSizeX, eSizeY);

    pImg->_iPickId = Gu::getPicker()->genPickId();
    return pImg;
}
UiImage::UiImage() {
}
UiImage::~UiImage() {
}
void UiImage::init() {
    UiElement::init();
    setName("UiImage");
}
void UiImage::getQuadVerts(std::vector<v_GuiVert>& verts, std::vector<v_index32>& inds, Box2f&  b2ClipRect) {
    if (getLayoutVisible() == false) {
        return;
    }
    bool bHasTex = (_pTexture != nullptr && _pTexture->tex() != nullptr);
    bool bComputedTex = !bHasTex && (_eSizeModeX == UiImageSizeMode::e::Computed && _eSizeModeY == UiImageSizeMode::e::Computed);
    if (!bHasTex && !bComputedTex) {
        return;
    }



    if (_eSizeModeX == UiImageSizeMode::e::Expand) {
        _q2Tex._p0.x = _pTexture->tex()->uv0().x;
        _q2Tex._p1.x = _pTexture->tex()->uv1().x;
    }
    else if (_eSizeModeX == UiImageSizeMode::e::Tile) {
        float wPx = getComputedQuad().right() - getComputedQuad().left();
        _q2Tex._p0.x = _pTexture->tex()->uv0().x;

        if (_fTileWidthPx > 0) {
            _q2Tex._p1.x = _pTexture->tex()->uv0().x + wPx / _fTileWidthPx;
        }
        else {
            _q2Tex._p1.x = _pTexture->tex()->uv1().x;
            static int x = 0;
            if (x == 0) { Gu::debugBreak(); }
        }
    }
    else if (_eSizeModeX == UiImageSizeMode::e::Computed) {
        //Tex coords are computed (used for UiGlyph)
    }
    else {
        Gui2d::error("Invalid layout X image size mode.");
    }

    if (_eSizeModeY == UiImageSizeMode::e::Expand) {
        _q2Tex._p0.y = _pTexture->tex()->uv1().y;
        _q2Tex._p1.y = _pTexture->tex()->uv0().y;
    }
    else if (_eSizeModeY == UiImageSizeMode::e::Tile) {
        float hPx = getComputedQuad().bottom() - getComputedQuad().top();
        _q2Tex._p0.y = _pTexture->tex()->uv1().y;

        if (_fTileHeightPx > 0) {
            _q2Tex._p1.y = _pTexture->tex()->uv1().y + hPx / _fTileHeightPx;
        }
        else {
            _q2Tex._p1.y = _pTexture->tex()->uv0().y;
            static int x = 0;
            if (x == 0) { Gu::debugBreak(); }//Texture height was zero
        }
    }
    else if (_eSizeModeY == UiImageSizeMode::e::Computed) {
        //Tex coords are computed (used for UiGlyph)
    }
    else if (_eSizeModeY == UiImageSizeMode::e::Proportion) {
        //proportion the Y to the X
        _q2Tex._p0.y = _pTexture->tex()->uv1().y;
        float fw = _q2Tex._p1.x - _q2Tex._p0.x;
        float fr = _pTexture->tex()->getSizeRatio();
        float fh = fw * fr;
        _q2Tex._p1.y = _q2Tex._p0.y + fh;
    }
    else {
        Gui2d::error("Invalid layout size mode.");
    }

    //if(_pQuad != nullptr) {
    //Add the vertexes of this element tot he given buffers
    //Creates an image out of this element.
    const Box2f& cpyPos = getGLRasterQuad();
    v_GuiVert v;
    v._rect.x = cpyPos._p0.x;
    v._rect.y = cpyPos._p0.y;
    v._rect.z = cpyPos._p1.x;
    v._rect.w = cpyPos._p1.y;

    //Clip Rect.  For discard
    v._clip = makeClipRectForRender(b2ClipRect);
    if (Gu::getRenderSettings()->getDebug()->getShowGuiBoxesAndDisableClipping()) {
        //Disable clip
        v._clip.x = -9999;
        v._clip.y = -9999;
        v._clip.z = 9999;
        v._clip.w = 9999;
    }

    //Texs
    v._tex.x = _q2Tex._p0.x;//GL - bottom left
    v._tex.y = _q2Tex._p0.y;
    v._tex.z = _q2Tex._p1.x;//GL - top right *this essentially flips it upside down
    v._tex.w = _q2Tex._p1.y;

    if (_pTexture != nullptr) {
        v._texsiz.x = fabsf(_pTexture->tex()->uv1().x - _pTexture->tex()->uv0().x);
        v._texsiz.y = fabsf(_pTexture->tex()->uv0().y - _pTexture->tex()->uv1().y);//Uv0 - uv1 - because we flipped coords bove
    }
    else {
        //*this is for "Fixed" textures, e.g. fonts and text.  We compute them in UiLabel first
        v._texsiz.x = fabsf(v._tex.z - v._tex.x);
        v._texsiz.y = fabsf(v._tex.w - v._tex.y);//y is flipfloped again
    }


    //**Texture Adjust - modulating repeated textures causes seaming issues, especially with texture filtering
    //adjust the texture coordinates by some pixels to account for that.  0.5f seems to work well.
    static float pixAdjust = 0.51f;  // # of pixels to adjust texture by
//#ifdef _DEBUG
//    if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
//        pixAdjust -= 0.005;
//    }
//    if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
//        pixAdjust += 0.005;
//    }
//#endif
    float w1px = 0; // 1 pixel subtract from the u/v to prevent creases during texture modulation
    float h1px = 0;
    if (_pTexture != nullptr) {
        if (_pTexture->tex()->getWidth() > 0 && v._texsiz.x > 0) {
            w1px = 1.0f / _pTexture->tex()->getWidth();
            w1px *= v._texsiz.x;
            w1px *= pixAdjust;
        }
        if (_pTexture->tex()->getHeight() > 0 && v._texsiz.y > 0) {
            h1px = 1.0f / _pTexture->tex()->getHeight();
            h1px *= v._texsiz.y;
            h1px *= pixAdjust;
        }
    }
    v._texsiz.x -= w1px*2.0f;
    v._texsiz.y -= h1px*2.0f;
    v._tex.x += w1px;
    v._tex.y += h1px;
    v._tex.z -= w1px;
    v._tex.w -= h1px;

    //**End texture adjust

    //Pick Color
    v._pick_color.x = _iPickId;

    //Display Color
    vec4 vc = getColor();
    v._pick_color.y = //    0xFFFFFFFF; //White
        ((int)(vc.x * 255.0f) << 24) |
        ((int)(vc.y * 255.0f) << 16) |
        ((int)(vc.z * 255.0f) << 8) |
        ((int)(vc.w * 255.0f) << 8);
    inds.push_back((v_index32)verts.size());
    verts.push_back(v);

}
bool UiImage::pick(std::shared_ptr<Fingers> fingers) {
    if (getLayoutVisible()) {
        if (getRenderVisible()) {
            if (_iPickId > 0) {
                uint32_t pixid = Gu::getPicker()->getSelectedPixelId();
                if (pixid != 0) {
                    if (pixid == _iPickId) {

                        _iPickedFrameId = Gu::getFrameNumber();

                        return true;
                    }
                }
                else {
                    int nnn = 0;
                    nnn++;
                }
            }
        }
    }
    return false;
}
float UiImage::getAutoWidth() {
    if (_pTexture != nullptr) {
        //Assuming height is set
        return height().px() * _pTexture->getSizeRatio();
    }
    return 0.0f;
}
float UiImage::getAutoHeight() {
    if (_pTexture != nullptr) {
        if (_pTexture->getSizeRatio() != 0.0f) {
            return width().px() * (1.0f / _pTexture->getSizeRatio());
        }
    }
    return 0.0f;
}
void UiImage::setSizeMode(UiImageSizeMode::e eSizeX, UiImageSizeMode::e eSizeY) {
    _eSizeModeY = eSizeY;
    _eSizeModeX = eSizeX;
    TexWrap::e wu = TexWrap::Clamp, wv = TexWrap::Clamp;
    if (eSizeX == UiImageSizeMode::e::Tile) { wu = TexWrap::Repeat; }
    if (eSizeY == UiImageSizeMode::e::Tile) { wv = TexWrap::Repeat; }

    setWrapMode(wu, wv);

    setLayoutChanged();
}
void UiImage::setWrapMode(TexWrap::e u, TexWrap::e v) {
    if (_pTexture != nullptr) {
        //Null for UiGlyph
        _pTexture->setWrapU(u);
        _pTexture->setWrapV(v);
        setLayoutChanged();
    }
}

void UiImage::performLayout(bool bForce) {
    UiElement::performLayout(bForce);
}

void UiImage::regenMesh(std::vector<v_GuiVert>& verts, std::vector<v_index32>& inds, Box2f&  b2ClipRect) {
    getQuadVerts(verts, inds, b2ClipRect);
    UiElement::regenMesh(verts, inds, b2ClipRect);

}

#pragma endregion
//////////////////////////////////////////////////////////////////////////
#pragma region UiGlyph
std::shared_ptr<UiGlyph> UiGlyph::create(uint32_t iChar) {
    std::shared_ptr<UiGlyph> g = std::make_shared<UiGlyph>();
    //Avoid any init crap becuase we're SUPER SLOW

    //g->init();
    //g->setName("G");//**Save space?
    return g;
}
void UiGlyph::performLayout(bool bForce) {
    //Skips UiImage PerformLayout
    UiElement::performLayout(bForce);
}
void UiGlyph::regenMesh(std::vector<v_GuiVert>& verts, std::vector<v_index32>& inds, Box2f&  b2ClipRect) {
    getQuadVerts(verts, inds, b2ClipRect);
}
#pragma endregion
//////////////////////////////////////////////////////////////////////////
#pragma region UiLabel
std::shared_ptr<UiLabel> UiLabel::create(std::string text, std::shared_ptr<UiLabelSkin> inf) {
    std::shared_ptr<UiLabel> lbl = std::make_shared<UiLabel>();
    lbl->_pFontInfo = inf;
    lbl->_strText = text;
    // lbl->_strText = "UiLabel";
    lbl->init();

    return lbl;
}
void UiLabel::init() {
    UiElement::init();
    setName("UiLabel");

}
void UiLabel::setText(std::string s) {
    _strText = s;
}

void UiLabel::performLayout(bool bForce) {

    if (_pFontInfo != nullptr) {
        //glyph creation is in performlayout because the HEIGHT of the label needs to be
        //calculated first
#ifdef _DEBUG
        if (StringUtil::contains(_strText, "Apple")) {
            int n = 0;
            n++;
        }
        if (StringUtil::contains(_strText, "DOF")) {
            int n = 0;
            n++;
        }
#endif


        // This has to come before performlayout in order to get the correct width.
        createGlyphs();
    }
    //Use the system to flow the layout.
    t_timeval us0 = Gu::getMicroSeconds();
    UiElement::performLayout(bForce);
    t_timeval us1 = Gu::getMicroSeconds() - us0;
    int nn = 0;
    nn++;
}
void UiLabel::update(std::shared_ptr<Fingers> pFingers) {

    if (StringUtil::equals(_strTextLast, _strText) == false) {
        _strTextLast = _strText;
        setLayoutChanged();
    }

    UiElement::update(pFingers);
}
void UiLabel::regenMesh(std::vector<v_GuiVert>& verts, std::vector<v_index32>& inds, Box2f&  b2ClipRect) {
    UiElement::regenMesh(verts, inds, b2ClipRect);
}
void UiLabel::createGlyphs() {
    //Easy out - if text is the same.
    //However, we still need to send the mesh becasue we use absolute coordinates instead of a positional uniform.

    //Hide glyphs - this is an optimization because adding/removing children was causing major slowdown
    for (auto c : getChildren()) {
        c.second->setLayoutVisible(false, false);
    }

    float labelHeight = bottom().px() - top().px();

    //Font height is the height of the label.
    float fontHeight;
    uDim& d = _pFontInfo->getFontSize();
    if (d.getDimUnit() == UiDimUnit::e::Pixel) {
        fontHeight = d.px();
    }
    else if (d.getDimUnit() == UiDimUnit::e::Percent) {
        fontHeight = d.pctOf(labelHeight);
    }
    else if (d.getDimUnit() == UiDimUnit::e::Auto) {
        fontHeight = labelHeight;
    }
    std::shared_ptr<MtFont> ft = _pFontInfo->getFont();
    /*
    lineheight - in the layout the height of space between lines
    padding
    text-align - top, center, bottom
    */
    //Add glyphs
    float curX = 0, curY = 0;//setting curY = labelHeight SORT OF fixes the issue.  It's not right
    Box2f outQuad;
    Box2f outTexs;
    float outW, outH, outPadT, outPadR, outPadB, outPadL;
    bool bNextWrap = false;
    int nCh = 0;
    float labelWidth = 0;
    float labelWidthCur = 0;
    //float labelHeight = 0;
    //float labelHeightCur = 0;
    int32_t cCodePrev = -1;//Previous code for kerning
    for (int32_t c : _strText) {
        ft->getCharQuad(c, cCodePrev, fontHeight, outW, outH, outTexs, outPadT, outPadR, outPadB, outPadL);
        cCodePrev = c;

        std::shared_ptr<UiGlyph> g;

        if (nCh < getChildren().size()) {
            auto it = getChildren().begin();
            std::advance(it, nCh);
            g = std::dynamic_pointer_cast<UiGlyph>(it->second);
            g->setLayoutVisible(true, false);
        }
        else {
            g = std::make_shared<UiGlyph>();//UiGlyph::create(c);
            addChild(g, 0, false, false);
            g->setSizeMode(UiImageSizeMode::e::Computed, UiImageSizeMode::e::Computed);
        }

        g->position() = UiPositionMode::Static;
        g->width().setDimUnit(UiDimUnit::e::Pixel);
        g->height().setDimUnit(UiDimUnit::e::Pixel);

        g->width() = outW;
        g->height() = outH;

        g->getTexs() = outTexs;
        g->getColor() = getColor(); //Copy color over Note: font color? I do't know I think we should use gui color

      //  g->padRight() = outPadR;
        //g->padBottom() = outPadB;
        g->padTop() = outPadT;//fontHeight - outH;    //this should never be greater
        g->padRight() = outPadR;//fontHeight - outH;    //this should never be greater
        g->padBottom() = outPadB;//fontHeight - outH;    //this should never be greater
        g->padLeft() = outPadL;//fontHeight - outH;    //this should never be greater

        labelWidthCur += g->right().px() - g->left().px() + g->padRight().px() + g->padLeft().px();
        ////Don't alter texcoords
        if (_bWordWrap) {
            //This won't work it'll let chars go past their container.
            //I think the only method to make this work is to encapsulate Words within UiElements (simply)
            g->display() = UiDisplayMode::e::InlineNoWrap;
            if (bNextWrap) {
                labelWidth = MathUtils::broMax(labelWidth, labelWidthCur);
                labelWidthCur = 0;
                g->display() = (UiDisplayMode::e::Block);
                bNextWrap = false;
            }

            if (c == '\r' || c == '\n') {
                bNextWrap = true;
            }
            else if (StringUtil::isWsExceptNewline(c)) {
                g->display() = (UiDisplayMode::e::InlineWrap);
            }
            else {
                g->display() = (UiDisplayMode::e::InlineNoWrap);
            }
        }
        else {
            g->display() = (UiDisplayMode::e::InlineNoWrap);//Default
        }

        //Top-Left origin conversion (STB uses bottom left of quad, place our quads at the bottom of the labe.
        //*We could even have centering, and other jargon as to where to place the text.  For now just place it at the bottom.
       // g->top() = g->top().px() + fontHeight; // add max height to the top element
       // g->bottom() = g->bottom().px() + fontHeight; // add max height to the top element


        nCh++;
    }

    if (labelWidth < 0.0f) {
        labelWidth = 0.0f;//hasn't happened yet
    }
    minWidth() = uDim(labelWidth, UiDimUnit::e::Pixel);



}

#pragma endregion
//////////////////////////////////////////////////////////////////////////
#pragma region UiButtonBase
void UiButtonBase::init() {
    std::weak_ptr<UiButtonBase> weak_p = getThis<UiButtonBase>();
    setClick([weak_p]() {
        if (std::shared_ptr<UiButtonBase> bt = weak_p.lock()) {
            bt->showUp();
        }
    });
    setDown([weak_p]() {
        if (std::shared_ptr<UiButtonBase> bt = weak_p.lock()) {
            bt->showDown();
        }
    });
    setPress([weak_p]() {
        if (std::shared_ptr<UiButtonBase> bt = weak_p.lock()) {
            bt->showUp();
        }
    });
    setHover([weak_p]() {
        if (std::shared_ptr<UiButtonBase> bt = weak_p.lock()) {

            bt->showHover();
        }
    });
}
void UiButtonBase::update(std::shared_ptr<Fingers> pFingers) {

    if (getPickedLastFrame() == false) {
        showUp();
    }

    UiElement::update(pFingers);
}
int UiButtonBase::layerIdUp() { return  Gui2d::sortLayer(0); }
int UiButtonBase::layerIdHover() { return  Gui2d::sortLayer(1); }
int UiButtonBase::layerIdDown() { return  Gui2d::sortLayer(2); }
#pragma endregion
//////////////////////////////////////////////////////////////////////////
#pragma region UiImageButton
std::shared_ptr<UiImageButton> UiImageButton::create(std::shared_ptr<UiButtonSkin> skin) {
    std::shared_ptr<UiImageButton> bt = std::make_shared<UiImageButton>();
    bt->_pSkin = skin;

    bt->init();
    bt->setUp(UiImage::create(skin->_pUp, UiImageSizeMode::Expand, UiImageSizeMode::Expand));
    bt->setHover(UiImage::create(skin->_pHv, UiImageSizeMode::Expand, UiImageSizeMode::Expand));
    bt->setDown(UiImage::create(skin->_pDn, UiImageSizeMode::Expand, UiImageSizeMode::Expand));

    return bt;
}
void UiImageButton::init() {
    UiButtonBase::init();
    //Use weak ptr to avoid circular reference
}
void UiImageButton::setHover(std::shared_ptr<UiImage> img) {
    _pHover = img;
    addChild(img, UiButtonBase::layerIdHover());//The index is a different layer
}
void UiImageButton::setUp(std::shared_ptr<UiImage> img) {
    _pUp = img;
    addChild(img, UiButtonBase::layerIdUp());//The index is a different layer
}
void UiImageButton::setDown(std::shared_ptr<UiImage> img) {
    _pDown = img;
    addChild(img, UiButtonBase::layerIdDown());//The index is a different layer
}
void UiImageButton::showHover() {
    if (_eImageState != ImageState::e::Hover) {
        _eImageState = ImageState::e::Hover;
        if (_pUp != nullptr)    _pUp->hideRender();//Don't use || here - early out breaks the logic
        if (_pHover != nullptr) _pHover->showRender();
        if (_pDown != nullptr)  _pDown->hideRender();
    }
}
void UiImageButton::showUp() {
    if (_eImageState != ImageState::e::Up) {
        _eImageState = ImageState::e::Up;
        if (_pUp != nullptr)    _pUp->showRender();
        if (_pHover != nullptr) _pHover->hideRender();
        if (_pDown != nullptr)  _pDown->hideRender();
    }
}
void UiImageButton::showDown() {
    if (_eImageState != ImageState::e::Down) {
        _eImageState = ImageState::e::Down;
        if (_pUp != nullptr)    _pUp->hideRender();
        if (_pHover != nullptr) _pHover->hideRender();
        if (_pDown != nullptr)  _pDown->showRender();
    }
}
void UiImageButton::update(std::shared_ptr<Fingers> pFingers) {
    UiButtonBase::update(pFingers);
}
#pragma endregion
//////////////////////////////////////////////////////////////////////////
#pragma region UiGridRow
void UiGridRow::init() {
    UiElement::init();
    setName("UiGridRow");
}
std::shared_ptr<UiElement> UiGridRow::getCol(size_t iCol) {
    if (iCol >= _cols.size()) {
        Gui2d::error(Stz "UiGridRow: Error: tried to access grid column outside of bounds: "+ iCol);
        return nullptr;
    }
    return _cols[iCol];
}
std::shared_ptr<UiElement> UiGridRow::addCol(int nCount, bool bAutoSizeCols, uint32_t iSort) {
    std::shared_ptr<UiElement> c = nullptr;
    for (int i = 0; i < nCount; ++i) {
        c = UiElement::create();
        c->setName("UiElement(UiGridCol)");
        addChild(c, iSort);
        _cols.push_back(c);
    }
    if (bAutoSizeCols) {
        resizeCols();
    }

    return c;
}
void UiGridRow::resizeCols() {
    if (_cols.size() == 0) {
        return;
    }
    std::string w = Stz 100.0f / (float)(_cols.size())+ "%";
    for (std::shared_ptr<UiElement> c : _cols) {
        c->width() = w;
        c->height() = "100%";
    }
}
void UiGridRow::performLayout(bool bForce) {
    UiElement::performLayout(bForce);
}
#pragma endregion
//////////////////////////////////////////////////////////////////////////
#pragma region UiGrid
std::shared_ptr<UiGrid> UiGrid::create(int nr, int nc) {
    std::shared_ptr<UiGrid> g = std::make_shared<UiGrid>();
    g->init();
    g->addRow(nr, nc);
    g->setName("UiGrid");
    return g;
}
std::shared_ptr<UiElement> UiGrid::getCell(size_t iRow, size_t iCol) {
    if (iRow >= _rows.size()) {
        Gui2d::error( Stz "UiGrid: Error: tried to access grid cell outside of bounds tried, ("+ iRow+ ","+ iCol+ ") ");
        return nullptr;
    }
    return  _rows[iRow]->getCol(iCol);
}
void UiGrid::performLayout(bool bForce) {
    UiElement::performLayout(bForce);
}
std::shared_ptr<UiGridRow> UiGrid::addRow(int nr, int nc, bool bAutoSizeRows, uint32_t iSort) {
    std::vector<std::shared_ptr<UiGridRow>> added;
    std::shared_ptr<UiGridRow> r;
    for (int ir = 0; ir < nr; ++ir) {
        r = std::make_shared<UiGridRow>();
        r->init();
        addChild(r, iSort);
        _rows.push_back(r);
        added.push_back(r);
    }

    if (bAutoSizeRows) {
        resizeRows();
    }

    if (nc > 0) {
        for (std::shared_ptr<UiGridRow> r2 : added) {
            r2->addCol(nc);
        }
    }

    return r;
}
void UiGrid::resizeRows() {
    std::string h = Stz 100.0f / (float)(_rows.size())+ "%";
    for (std::shared_ptr<UiGridRow> r : _rows) {
        r->height() = h;
        r->width() = "100%";
    }
}
std::shared_ptr<UiGrid> UiGrid::createImageStack(std::shared_ptr<Ui3Tex> tex, uDim uWorHPx, Orientation::e eOr) {
    if (uWorHPx.px() == 0) {
        Gui2d::error(Stz "Image stack width or hieght was zero, errors will occur orientation = "+ Orientation::toString(eOr));
        return nullptr;
    }
    std::shared_ptr<UiGrid> pg = UiGrid::create(0, 0);

    std::shared_ptr<UiElement> c0;
    std::shared_ptr<UiElement> c1;
    std::shared_ptr<UiElement> c2;
    std::shared_ptr<UiImage> imgTopOrLeft;
    std::shared_ptr<UiImage> imgMid;
    std::shared_ptr<UiImage> imgBotOrRight;

    uint32_t iSortLayer = Gui2d::sortLayer(0);

    if (eOr == Orientation::e::Vertical) {
        pg->setName("UiGrid(UiImage3Stack_Vertical)");
        uDim hR0 = uWorHPx.autoHeight(tex->get(0));
        uDim hR1 = uWorHPx.autoHeight(tex->get(1));
        uDim hR2 = uWorHPx.autoHeight(tex->get(2));
        std::shared_ptr<UiGridRow> r0 = pg->addRow(1, 0, false, iSortLayer);
        r0->setName("IMAGE_3STACK_ROW0_V");
        r0->width() = "100%";
        r0->height() = hR0;
        std::shared_ptr<UiGridRow> r1 = pg->addRow(1, 0, false, iSortLayer);
        r1->setName("IMAGE_3STACK_ROW1_V");
        r1->width() = "100%";
        r1->height() = "auto";
        std::shared_ptr<UiGridRow> r2 = pg->addRow(1, 0, false, iSortLayer);
        r2->setName("IMAGE_3STACK_ROW2_V");
        r2->width() = "100%";
        r2->height() = hR1;
        //COLS
        c0 = r0->addCol(1, false, iSortLayer);
        c0->setName("IMAGE_3STACK_COL0_V");
        c0->width() = "100%";
        c0->height() = hR0;
        c1 = r1->addCol(1, false, iSortLayer);
        c1->setName("IMAGE_3STACK_COL1_V");
        c1->width() = "100%";
        c1->height() = "auto";
        c2 = r2->addCol(1, false, iSortLayer);
        c2->setName("IMAGE_3STACK_COL2_V");
        c2->width() = "100%";
        c2->height() = hR2;

        imgTopOrLeft = UiImage::create(tex->get(0), UiImageSizeMode::e::Expand, UiImageSizeMode::e::Expand, uWorHPx.px(), hR0.px());
        imgMid = UiImage::create(tex->get(1), UiImageSizeMode::e::Expand, UiImageSizeMode::e::Tile, uWorHPx.px(), hR1.px());
        imgBotOrRight = UiImage::create(tex->get(2), UiImageSizeMode::e::Expand, UiImageSizeMode::e::Expand, uWorHPx.px(), hR2.px());
    }
    else if (eOr == Orientation::e::Horizontal) {
        pg->setName("UiGrid(UiImage3Stack_Horizontal)");
        uDim hR0 = uWorHPx.autoWidth(tex->get(0));
        uDim hR1 = uWorHPx.autoWidth(tex->get(1));
        uDim hR2 = uWorHPx.autoWidth(tex->get(2));

        std::shared_ptr<UiGridRow> r0 = pg->addRow(1, 0, false, iSortLayer);
        r0->setName("IMAGE_3STACK_ROW0_H");

        c0 = r0->addCol(1, false, iSortLayer);
        c0->setName("IMAGE_3STACK_COL0_H");
        c0->height() = uWorHPx;
        c0->width() = hR0;
        c0->display() = (UiDisplayMode::e::InlineNoWrap);
        c1 = r0->addCol(1, false, iSortLayer);
        c1->setName("IMAGE_3STACK_COL1_H");
        c1->height() = uWorHPx;
        c1->width() = "auto";
        c1->display() = (UiDisplayMode::e::InlineNoWrap);
        c2 = r0->addCol(1, false, iSortLayer);
        c2->setName("IMAGE_3STACK_COL2_H");
        c2->height() = uWorHPx;
        c2->width() = hR2;
        c2->display() = (UiDisplayMode::e::InlineNoWrap);

        imgTopOrLeft = UiImage::create(tex->get(0), UiImageSizeMode::e::Expand, UiImageSizeMode::e::Expand, hR0.px(), uWorHPx.px());
        imgMid = UiImage::create(tex->get(1), UiImageSizeMode::e::Tile, UiImageSizeMode::e::Expand, hR1.px(), uWorHPx.px());
        imgBotOrRight = UiImage::create(tex->get(2), UiImageSizeMode::e::Expand, UiImageSizeMode::e::Expand, hR2.px(), uWorHPx.px());
    }
    c0->addChild(imgTopOrLeft, Gui2d::sortLayer(0));
    c1->addChild(imgMid, Gui2d::sortLayer(0));
    c2->addChild(imgBotOrRight, Gui2d::sortLayer(0));

    return pg;
}
#pragma endregion
//////////////////////////////////////////////////////////////////////////
#pragma region Ui9Grid
std::shared_ptr<Ui9Grid> Ui9Grid::create(std::shared_ptr<UiBorderDim> dims) {
    std::shared_ptr<Ui9Grid> g = std::make_shared<Ui9Grid>();
    g->_pBorder = dims;
    g->init();
    return g;
}
void Ui9Grid::init() {
    UiElement::init();

    setName("UiImage9Grid");

    std::shared_ptr<UiGridRow> r0 = addRow();
    std::shared_ptr<UiGridRow> r1 = addRow();
    std::shared_ptr<UiGridRow> r2 = addRow();
    r0->addCol(3);
    r1->addCol(3);
    r2->addCol(3);

    setBorderWidth(_pBorder);

    //Add content area that fills the container AND is above the background
    _pContentContainer = UiElement::create();
    _pContentContainer->setName("UiElement(UiContentContainer)");
    _pContentContainer->position() = UiPositionMode::e::Static;
    _pContentContainer->overflow() = (UiOverflowMode::e::Hide);
    _pContentContainer->width() = "100%";
    _pContentContainer->height() = "100%";
    std::shared_ptr<UiElement> c = getCell(1, 1);
    c->addChild(_pContentContainer, Gui2d::sortLayer(Ui9Grid::getContentSortLayer()));
}
void Ui9Grid::set9GridImages(std::shared_ptr<Ui9Tex> texs, uint32_t iLayer) {

    for (int j = 0; j < 3; ++j) {
        for (int i = 0; i < 3; ++i) {
            size_t ioff = Ui9Grid::cellOff(i, j);
            float wpx = (float)texs->get(ioff)->getWidth();
            float hpx = (float)texs->get(ioff)->getHeight();

            if (wpx == 0 || hpx == 0) {
                static int xx = 0;
                if (xx == 0) {
                    //**If this hits, likely you created a UiTex AFTER you called the MegaTex::loadImages().. 
                    // we need image sizes when craeating the gui, after the skin.
                    //The order 100% NEEEDED - but it's more sane to do it this way to vet out visual bugs down the road
                    Gu::debugBreak();
                }
            }

            bool LFix = (ioff == 0 || ioff == 3 || ioff == 6);
            bool TFix = (ioff == 0 || ioff == 1 || ioff == 2);
            bool RFix = (ioff == 2 || ioff == 5 || ioff == 8);
            bool BFix = (ioff == 6 || ioff == 7 || ioff == 8);

            //Expand corners, tile borders, tile edges along w/h
            UiImageSizeMode::e wSizeMode = UiImageSizeMode::e::Tile;
            UiImageSizeMode::e hSizeMode = UiImageSizeMode::e::Tile;
            if (LFix) {
                wpx = _pBorder->get(GridBorder::e::Left).px();
                wSizeMode = UiImageSizeMode::e::Expand;
            }
            if (RFix) {
                wpx = _pBorder->get(GridBorder::e::Right).px();
                wSizeMode = UiImageSizeMode::e::Expand;
            }

            if (BFix) {
                hpx = _pBorder->get(GridBorder::e::Bot).px();
                hSizeMode = UiImageSizeMode::e::Expand;
            }
            if (TFix) {
                hpx = _pBorder->get(GridBorder::e::Top).px();
                hSizeMode = UiImageSizeMode::e::Expand;
            }
            //So setting this to _fBorderWidth.px() worked for the window not for the button!
            //The problem is that image tiling isn't working for the fixed width elements (window corners).  Although it should be because they're fixed
            std::shared_ptr<UiImage> img = UiImage::create(texs->get(ioff), UiImageSizeMode::e::Tile, UiImageSizeMode::e::Tile, wpx, hpx);

            img->setSizeMode(wSizeMode, hSizeMode);

            getCell(j, i)->addChild(img, iLayer);
        }
    }

}
void Ui9Grid::iterateCells(std::function<void(std::shared_ptr<UiElement>)> f) {
    for (int j = 0; j < 3; ++j) {
        for (int i = 0; i < 3; ++i) {
            size_t ioff = Ui9Grid::cellOff(i, j);
            std::shared_ptr<UiElement> c = getCell(j, i);
            f(c);
        }
    }
}
void Ui9Grid::setBorderWidth(std::shared_ptr<UiBorderDim> dims) {
    *_pBorder = *dims;

    for (int i = 0; i < 4; ++i) {
        if (_pBorder->get((GridBorder::e)i).getDimUnit() != UiDimUnit::e::Pixel) {
            Gui2d::error("Border width for window was not specified in pixels.  Errors will occur");
            _pBorder->get((GridBorder::e)i).setDimUnit(UiDimUnit::e::Pixel);
        }
    }

    std::shared_ptr<UiGridRow> r0 = _rows[0];
    std::shared_ptr<UiGridRow> r1 = _rows[1];
    std::shared_ptr<UiGridRow> r2 = _rows[2];

    r0->width() = "100%";
    r0->height() = dims->get(GridBorder::e::Top);
    r1->width() = "100%";
    r1->height() = "auto";
    r2->width() = "100%";
    r2->height() = dims->get(GridBorder::e::Bot);

    r0->getCol(0)->width() = dims->get(GridBorder::e::Left);
    r0->getCol(0)->height() = dims->get(GridBorder::e::Top);
    r0->getCol(1)->width() = "auto";
    r0->getCol(1)->height() = dims->get(GridBorder::e::Top);
    r0->getCol(2)->width() = dims->get(GridBorder::e::Right);
    r0->getCol(2)->height() = dims->get(GridBorder::e::Top);

    r1->getCol(0)->width() = dims->get(GridBorder::e::Left);
    r1->getCol(0)->height() = "auto";
    r1->getCol(1)->width() = "auto";
    r1->getCol(1)->height() = "auto";
    r1->getCol(2)->width() = dims->get(GridBorder::e::Right);
    r1->getCol(2)->height() = "auto";

    r2->getCol(0)->width() = dims->get(GridBorder::e::Left);
    r2->getCol(0)->height() = dims->get(GridBorder::e::Bot);
    r2->getCol(1)->width() = "auto";
    r2->getCol(1)->height() = dims->get(GridBorder::e::Bot);
    r2->getCol(2)->width() = dims->get(GridBorder::e::Right);
    r2->getCol(2)->height() = dims->get(GridBorder::e::Bot);

    //Min Size
    minWidth().px(dims->get(GridBorder::e::Left).px() + dims->get(GridBorder::e::Right).px());
    minHeight().px(dims->get(GridBorder::e::Top).px() + dims->get(GridBorder::e::Bot).px());
}
void Ui9Grid::update(std::shared_ptr<Fingers> pFingers) {
    UiGrid::update(pFingers);
}

#pragma endregion
//////////////////////////////////////////////////////////////////////////
#pragma region UiFlexButton
std::shared_ptr<UiFlexButton> UiFlexButton::create(std::shared_ptr<UiFlexButtonSkin> skin) {

    std::shared_ptr<UiFlexButton> ret = std::make_shared<UiFlexButton>();
    ret->_pGrid = Ui9Grid::create(skin->_border);
    ret->_pGrid->width() = "100%";
    ret->_pGrid->height() = "100%";
    ret->_pLabelSkin = skin->_pLabelFont;
    ret->init();
    ret->setButtonImages(skin->_pUp, skin->_pHv, skin->_pDn);

    ret->addChild(ret->_pGrid);
    ret->setPickRoot();

    return ret;
}
void UiFlexButton::init() {
    UiButtonBase::init();
    setName("UiFlexButton");
}
void UiFlexButton::setButtonImages(std::shared_ptr<Ui9Tex> vecUp, std::shared_ptr<Ui9Tex> vecHv, std::shared_ptr<Ui9Tex> vecDn) {

    _pGrid->set9GridImages(vecUp, UiButtonBase::layerIdUp());
    _pGrid->set9GridImages(vecHv, UiButtonBase::layerIdHover());
    _pGrid->set9GridImages(vecDn, UiButtonBase::layerIdDown());

    //Expand all center images (don't tile.
    for (auto x : _pGrid->getCell(1, 1)->getChildren()) {
        std::shared_ptr<UiImage> img = std::dynamic_pointer_cast<UiImage>(x.second);
        if (img != nullptr) {
            img->setSizeMode(UiImageSizeMode::e::Expand, UiImageSizeMode::e::Expand);
        }
    }
}
void UiFlexButton::showHover() {
    if (_eImageState != ImageState::e::Hover) {
        _eImageState = ImageState::e::Hover;
        _pGrid->iterateCells([&](std::shared_ptr<UiElement> c) {
            c->hideLayerRender(UiButtonBase::layerIdUp());
            c->showLayerRender(UiButtonBase::layerIdHover());
            c->hideLayerRender(UiButtonBase::layerIdDown());
        });
    }
}
void UiFlexButton::showUp() {
    if (_eImageState != ImageState::e::Up) {
        _eImageState = ImageState::e::Up;
        _pGrid->iterateCells([&](std::shared_ptr<UiElement> c) {
            c->showLayerRender(UiButtonBase::layerIdUp());
            c->hideLayerRender(UiButtonBase::layerIdHover());
            c->hideLayerRender(UiButtonBase::layerIdDown());
        });
    }
}
void UiFlexButton::showDown() {
    if (_eImageState != ImageState::e::Down) {
        _eImageState = ImageState::e::Down;
        _pGrid->iterateCells([&](std::shared_ptr<UiElement> c) {
            c->hideLayerRender(UiButtonBase::layerIdUp());
            c->hideLayerRender(UiButtonBase::layerIdHover());
            c->showLayerRender(UiButtonBase::layerIdDown());
        });
    }
}
std::shared_ptr<UiElement> UiFlexButton::getContentContainer() {
    return _pGrid->getContentContainer();
    //return _pPadGrid->getContentContainer();
}
void UiFlexButton::setLabel(std::string strText, std::shared_ptr<UiLabelSkin> labelFont) {
    if (_pLabelSkin == nullptr && labelFont == nullptr) {
        Gui2d::error(("No font specified when calling setLabel on flexbutton"));
        return;
    }
    if (_pLabel == nullptr) {
        std::shared_ptr<UiElement> topPad = UiElement::create();
        topPad->width() = "100%";
        topPad->height() = "auto"; //100%
        getContentContainer()->addChild(topPad);


        if (labelFont != nullptr) {
            _pLabelSkin = labelFont;
        }

        _pLabel = UiLabel::create(strText, _pLabelSkin);
        _pLabel->width() = "100%";
        _pLabel->height() = "auto";
        getContentContainer()->addChild(_pLabel);

        std::shared_ptr<UiElement> botPad = UiElement::create();
        botPad->width() = "100%";
        botPad->height() = "auto"; //100%
        getContentContainer()->addChild(botPad);
    }
    else {
        _pLabel->setText(strText);
    }
}

#pragma endregion
//////////////////////////////////////////////////////////////////////////
#pragma region UiScrollGenThumb
void UiScrubGenThumb::init() {
    //basically copying from UiVSCrollbarThumv
    position() = UiPositionMode::e::Relative;
    left() = "0px";
    top() = "0px";

    std::weak_ptr<UiScrubGenThumb> wp = getThis<UiScrubGenThumb>();
    UiDragInfo::DragFunc scrubFunc = [wp](vec2& dxy) {
        if (std::shared_ptr<UiScrubGenThumb> sb = wp.lock()) {
            sb->_bPlayerDrag = true;//FOrces this to update when the player drags
            if (sb->getOrientation() == Orientation::e::Vertical) {
                sb->_fBarLeftOrTopY += dxy.y;
            }
            if (sb->getOrientation() == Orientation::e::Horizontal) {
                sb->_fBarLeftOrTopY += dxy.x;
            }
            sb->setLayoutChanged();
        }
    };
    enableDrag(scrubFunc);
}
void UiScrubGenThumb::update(std::shared_ptr<Fingers> pFingers) {
    //Generic UPdate
    UiElement::update(pFingers);

    float fMaxWOrH = 1.0f;
    float plt = 0.0f;
    float prb = 1.0f;
    if (getParent() != nullptr) {
        if (getOrientation() == Orientation::e::Vertical) {
            prb = getParent()->getComputedQuad().bottom();
            plt = getParent()->getComputedQuad().top();
        }
        else {
            prb = getParent()->getComputedQuad().right();
            plt = getParent()->getComputedQuad().left();
        }
    }
    fMaxWOrH = prb - plt;
    float fBarWorH = fMaxWOrH * _fBarSizePct;

    uDim* pTorL = nullptr;
    uDim* pWorH = nullptr;
    if (getOrientation() == Orientation::e::Vertical) {
        pTorL = &top();
        pWorH = &height();
    }
    else {
        pTorL = &left();
        pWorH = &width();
    }

    *pTorL = uDim(_fBarLeftOrTopY, UiDimUnit::e::Pixel);
    if (pTorL->px() + fBarWorH > fMaxWOrH) {
        //Scrolled past bottom
        *pTorL = uDim(fMaxWOrH - fBarWorH, UiDimUnit::e::Pixel);
    }
    if (pTorL->px() < 0) {
        //Scrollbar is too big for container
        *pTorL = "0px";
    }
    if (pTorL->px() + fBarWorH > fMaxWOrH) {
        //Scrollbar is too big for container
        fBarWorH = fMaxWOrH;
    }

    //Set final height
    *pWorH = uDim(fBarWorH, UiDimUnit::e::Pixel);

    //Send this value to the window'        
    float fTopOrLeft = pTorL->px();
    float diff = (fMaxWOrH - fBarWorH);
    if (diff != 0) {
        _fScrollPct = fTopOrLeft / diff;
    }
    else {
        _fScrollPct = 0.0f;
    }

}
void UiScrubGenThumb::setBarSizePct(float pos01) {
    if (pos01 < 0.0f || pos01 > 1.0f) {
        pos01 = MathUtils::broClamp(pos01, 0.0f, 1.0f);
        Gui2d::error("invalid scroll position sent to scrollbar, clamping");
    }
    _fBarSizePct = pos01;
}
void UiScrubGenThumb::performLayout(bool bForce) {
    UiElement::performLayout(bForce);
}
float UiScrubGenThumb::getScrollPct() {
    return _fScrollPct;
}
#pragma endregion
//////////////////////////////////////////////////////////////////////////
#pragma region UiScrollGen
uint32_t UiScrubGen::getThumbSortLayer() {
    return Gui2d::sortLayer(1);
}
uint32_t UiScrubGen::getBackgroundSortLayer() {
    return Gui2d::sortLayer(0);
}
void UiScrubGen::setBarSizePct(float pos01) {
    _pThumb->setBarSizePct(pos01);
}
void UiScrubGen::init() {
    UiElement::init();

    if (_pThumb == nullptr) {
        _pThumb = createThumb(); // return a thumb to the fiv
        addChild(_pThumb, UiScrubGen::getThumbSortLayer());

        //Force the scroll func to get called, initializing the scrollbar
        _pThumb->setScrubChanged();
    }
}
void UiScrubGen::update(std::shared_ptr<Fingers> pFingers) {
    UiElement::update(pFingers);

    //Call the registered scroll, and barSize events
    float bw = 1.0;
    if (_barWidthFunc != nullptr) {
        bw = _barWidthFunc(getThis<UiScrubGen>());
    }
    else {
        bw = 0.1f; // Trackbars don't have a bar width func  Width of the bar is thus "zero"
    }
    if (bw >= 1.0 && _bAutoHideWhenMaxed) {
        hide();
    }
    else {
        show();
        setBarSizePct(bw);
        if (_pThumb != nullptr) {
            if (_pThumb->scrubChanged()) {
                _pThumb->resetScrubChanged();
                if (_scrollFunc != nullptr) {
                    float fMax = maxVal();
                    float fMin = minVal();
                    if (fMax < fMin) {
                        Gui2d::error(Stz "Max '"+ fMax+ "' was greater than min '"+ fMin+ "' for scrollgen.");
                        maxVal() = minVal();
                    }
                    float fVal = fMin + (fMax - fMin) * _pThumb->getScrollPct();
                    _scrollFunc(getThis<UiScrubGen>(), fVal);
                }
            }
        }
    }
}
#pragma endregion
//////////////////////////////////////////////////////////////////////////
#pragma region UiScrollbarThumb
//Test of VScrollbarThumb
std::shared_ptr<UiScrollbarThumb> UiScrollbarThumb::create(std::shared_ptr<UiScrollbarSkin> pSkin) {//std::vector<std::shared_ptr<Texture2DSpec>> imgs, uDim widthPx, Orientation::e eOr) {
    std::shared_ptr<UiScrollbarThumb> sbt = std::make_shared<UiScrollbarThumb>();

    sbt->_eOrientation = pSkin->_eOrientation;
    sbt->init();
    sbt->setName("UiScrollbarThumb");
    sbt->_pImage = UiGrid::createImageStack(pSkin->_pThumbImgs,
        pSkin->_uWidthOrHeightPx,
        pSkin->_eOrientation
    );
    sbt->_pImage->width() = "100%";
    sbt->_pImage->height() = "100%";
    sbt->addChild(sbt->_pImage);

    return sbt;
}
void UiScrollbarThumb::init() {
    UiScrubGenThumb::init();
}
void UiScrollbarThumb::update(std::shared_ptr<Fingers> pFingers) {
    UiScrubGenThumb::update(pFingers);
}
void UiScrollbarThumb::performLayout(bool bForce) {
    UiScrubGenThumb::performLayout(bForce);
}
#pragma endregion
//////////////////////////////////////////////////////////////////////////
#pragma region UiScrollbar
std::shared_ptr<UiScrollbar> UiScrollbar::create(std::shared_ptr<UiScrollbarSkin> pSkin, UiScrubGen::BarWidthFunc barFunc,
    UiScrubGen::ScrollFunc scrollFunc) {
    std::shared_ptr<UiScrollbar> sb = std::make_shared<UiScrollbar>();

    sb->_eOrientation = pSkin->_eOrientation;
    sb->_pSkin = pSkin;
    sb->_bAutoHideWhenMaxed = pSkin->_bAutoHideWhenMaxed;
    sb->init();
    sb->setName("UiScrollbar");
    sb->setBarWidthFunc(barFunc);
    sb->setScrollFunc(scrollFunc);

    std::shared_ptr<Texture2DSpec> img[3];
    if (pSkin->_eOrientation == Orientation::e::Vertical) {
        sb->width() = pSkin->_uWidthOrHeightPx;
        sb->height() = "auto";
    }
    else if (pSkin->_eOrientation == Orientation::e::Horizontal) {
        sb->height() = pSkin->_uWidthOrHeightPx;
        sb->width() = "100%";
    }
    sb->_pImage = UiGrid::createImageStack(
        pSkin->_pBackImgs,
        pSkin->_uWidthOrHeightPx,
        pSkin->_eOrientation
    );
    sb->_pImage->width() = "100%";
    sb->_pImage->height() = "100%";
    sb->addChild(sb->_pImage, UiScrubGen::getBackgroundSortLayer());

    return sb;
}
void UiScrollbar::init() {
    UiScrubGen::init();
}
void UiScrollbar::update(std::shared_ptr<Fingers> pFingers) {
    UiScrubGen::update(pFingers);
}
void UiScrollbar::performLayout(bool bForce) {
    UiScrubGen::performLayout(bForce);
}
std::shared_ptr<UiScrubGenThumb> UiScrollbar::createThumb() {
    std::shared_ptr<UiScrollbarThumb> thumb = UiScrollbarThumb::create(_pSkin);
    return thumb;
}
#pragma endregion
//////////////////////////////////////////////////////////////////////////
#pragma region UiCheckbox
std::shared_ptr<UiCheckbox> UiCheckbox::create(std::shared_ptr<UiCheckboxSkin> pSkin, std::string strlabel) {
    if (UiSkinElement::setAndValid(pSkin) == false) {
        return nullptr;
    }

    std::shared_ptr<UiCheckbox> chk = std::make_shared<UiCheckbox>();
    chk->init();
    chk->setName("UiCheckbox");
    chk->width() = "100%";
    chk->height() = "32px";

    std::shared_ptr<UiFlexButton> bt = UiFlexButton::create(pSkin->_pFlexButtonSkin);
    bt->display() = (UiDisplayMode::e::InlineNoWrap);
    bt->width() = "32px";
    bt->padRight() = "20px";
    bt->height() = "100%";
    chk->addChild(bt);

    chk->_pCheckImg = UiImage::create(pSkin->_pCheckImg, UiImageSizeMode::e::Expand, UiImageSizeMode::e::Expand);
    chk->_pCheckImg->setRenderVisible(false);//Initially don't show check
    chk->_pCheckImg->disablePick();
    chk->_pCheckImg->width() = "100%";
    chk->_pCheckImg->height() = "100%";
    bt->getContentContainer()->addChild(chk->_pCheckImg);

    std::shared_ptr<UiLabel> label = UiLabel::create(strlabel, pSkin->_pLabelFont);
    label->display() = (UiDisplayMode::e::InlineNoWrap);
    label->width() = "auto";
    label->height() = "100%";
    chk->addChild(label);

    std::weak_ptr<UiCheckbox> chk_weak = chk->getThis<UiCheckbox>();
    chk->setClick([chk_weak]() {
        if (std::shared_ptr<UiCheckbox> chk = chk_weak.lock()) {
            chk->doCheck();
        }
    });
    //make the label &c of the checkbox activate it
    chk->setPickRoot();

    return chk;
}
void UiCheckbox::doCheck() {
    setChecked(!_bChecked);
}
void UiCheckbox::init() {
    UiElement::init();
}
void UiCheckbox::update(std::shared_ptr<Fingers> pFingers) {
    UiElement::update(pFingers);
}
void UiCheckbox::performLayout(bool bForce) {
    UiElement::performLayout(bForce);
}
void UiCheckbox::setChecked(bool b) {
    _bChecked = b;
    if (_checkFunc) {
        _checkFunc(_bChecked);
    }
    _pCheckImg->setRenderVisible(_bChecked);
}
#pragma endregion
//////////////////////////////////////////////////////////////////////////
#pragma region UiSliderThumb

std::shared_ptr<UiSliderThumb> UiSliderThumb::create(std::shared_ptr<UiSliderSkin> pSkin) {
    std::shared_ptr<UiSliderThumb> sbt = std::make_shared<UiSliderThumb>();

    sbt->_eOrientation = pSkin->_eOrientation;
    sbt->init();
    sbt->setName("UiTrackbarThumb");
    sbt->_pImage = UiGrid::createImageStack(pSkin->_pThumbImgs,
        pSkin->_uWidthOrHeightPx,
        //Swap orientation for the thumb since it's a vertical image.
        (pSkin->_eOrientation == Orientation::e::Horizontal) ? (Orientation::e::Vertical) : (Orientation::e::Horizontal)
    );
    sbt->_pImage->width() = "100%";
    sbt->_pImage->height() = "100%";
    sbt->addChild(sbt->_pImage);

    return sbt;
}
void UiSliderThumb::init() {
    UiScrubGenThumb::init();
}
void UiSliderThumb::update(std::shared_ptr<Fingers> pFingers) {

    //Update height of TB thumb to match parent height.
    if (_eOrientation == Orientation::e::Horizontal) {
        float h = height().px();
        float ph = 10;
        if (getParent() != nullptr) {
            ph = getParent()->getComputedQuad().height();
        }
        if (h != ph) {
            height() = uDim(ph, UiDimUnit::e::Pixel);
        }
    }
    else { throw 0; }


    UiScrubGenThumb::update(pFingers);
}
void UiSliderThumb::performLayout(bool bForce) {
    UiScrubGenThumb::performLayout(bForce);
}
#pragma endregion 
//////////////////////////////////////////////////////////////////////////
#pragma region UiSlider
std::shared_ptr<UiSlider> UiSlider::create(std::shared_ptr<UiSliderSkin> pSkin, UiScrubGen::ScrollFunc sf) {
    std::shared_ptr<UiSlider> tb = std::make_shared<UiSlider>();
    tb->_eOrientation = pSkin->_eOrientation;
    tb->_pSkin = pSkin;
    tb->init();
    tb->setName("UiTrackbar");

    std::weak_ptr<UiSlider> w_s = tb;
    UiScrubGen::ScrollFunc s2 = [sf, w_s](std::shared_ptr<UiScrubGen> pScrollbar, float scroll) {
        //Update Label
        if (std::shared_ptr<UiSlider> s = w_s.lock()) {
            if (s->getValueLabel() != nullptr) {
                s->getValueLabel()->setText(StringUtil::format("%.2f", scroll));
            }
        }

        //Pass through to user defined function
        sf(pScrollbar, scroll);
    };
    tb->setScrollFunc(s2);
    tb->_pImage = UiGrid::createImageStack(
        pSkin->_pBackImgs,
        pSkin->_uWidthOrHeightPx,//updated later
        pSkin->_eOrientation
    );

    //This is the "container" for the image.  The images are manually sized
    tb->addChild(tb->_pImage, UiScrubGen::getBackgroundSortLayer());
    return tb;
}
void UiSlider::init() {
    UiScrubGen::init();
}
void UiSlider::update(std::shared_ptr<Fingers> pFingers) {
    UiScrubGen::update(pFingers);

    //Update height of TB thumb to match parent height.
    if (_eOrientation == Orientation::e::Horizontal) {
        float h = _pImage->height().px();
        float ph = 10;
        ph = getComputedQuad().height();
        if (h != ph) {
            _pImage->height() = uDim(ph, UiDimUnit::e::Pixel);
        }
    }
    else { throw 0; }
}
void UiSlider::performLayout(bool bForce) {

    // a little padding for the left/right of the track makes it look better
    if (_eOrientation == Orientation::e::Horizontal) {
        float wh = getComputedQuad().width();
        float imagePad = getThumb()->width().px() * 0.5f;
        _pImage->width() = uDim(wh - imagePad * 2.0f, UiDimUnit::e::Pixel);
        _pImage->height() = "100%";
        _pImage->padLeft() = uDim(imagePad, UiDimUnit::e::Pixel);
        _pImage->padRight() = uDim(imagePad, UiDimUnit::e::Pixel);
    }
    else {
        float wh = getComputedQuad().height();
        float imagePad = getThumb()->height().px() * 0.5f;
        _pImage->height() = uDim(wh - imagePad * 2.0f, UiDimUnit::e::Pixel);
        _pImage->width() = "100%";
        _pImage->padTop() = uDim(imagePad, UiDimUnit::e::Pixel);
        _pImage->padBottom() = uDim(imagePad, UiDimUnit::e::Pixel);
    }

    UiScrubGen::performLayout(bForce);


}
std::shared_ptr<UiScrubGenThumb> UiSlider::createThumb() {
    std::shared_ptr<UiSliderThumb> thumb = UiSliderThumb::create(_pSkin);
    return thumb;
}
#pragma endregion
//////////////////////////////////////////////////////////////////////////
#pragma region UiDropdown
std::shared_ptr<UiDropdown> UiDropdown::create(std::shared_ptr<UiDropdwonSkin> pSkin) {
    std::shared_ptr<UiDropdown> dd = std::make_shared<UiDropdown>();
    dd->_pSkin = pSkin;
    dd->_pBorder = pSkin->_pBorder;
    dd->init();

    return dd;
}
void UiDropdown::init() {
    Ui9Grid::init();
    setName("UiDropdown");
    set9GridImages(_pSkin->_pBorderImgs);

    height() = "32px";//just show so we can see
    //std::shared_ptr<UiWindowSkin> sk = std::make_shared<UiWindowSkin>();
    //sk->_pBorder = _pSkin->_pBorder;
    //sk->_pBorderImgs = _pSkin->_pBorderImgs;

    _pListContainer = Ui9Grid::create(_pSkin->_pBorder);
    //_pListContainer->enableVScrollbar();
    _pListContainer->maxHeight() = "200px";
    _pListContainer->set9GridImages(_pSkin->_pBorderImgs);
    _pListContainer->position() = UiPositionMode::e::Relative;

    //Just testing..
    _pListContainer->top() = "40px";
    _pListContainer->bottom() = "40px";
    _pListContainer->width() = "200px";
    _pListContainer->height() = "200px";
    _pListContainer->hide();

    _pArrowButton = UiFlexButton::create(_pSkin->_pArrowButtonSkin);
    _pArrowButton->getContentContainer()->addChild(UiImage::create(_pSkin->_arrow, UiImageSizeMode::e::Expand, UiImageSizeMode::e::Expand));
    _pArrowButton->position() = UiPositionMode::e::Static;
    _pArrowButton->display() = (UiDisplayMode::e::InlineNoWrap);
    _pArrowButton->width() = "32px";
    _pArrowButton->height() = "32px";
    _pArrowButton->padRight() = "7px";
    getContentContainer()->addChild(_pArrowButton);

    _pSelectedContainer = UiElement::create();
    _pSelectedContainer->position() = UiPositionMode::e::Static;
    _pSelectedContainer->display() = (UiDisplayMode::InlineNoWrap);
    _pSelectedContainer->width() = "auto";
    _pSelectedContainer->height() = "100%";
    getContentContainer()->addChild(_pSelectedContainer);

    //**This will have errors because until now we assueme gui2d has only windows as children
    Gu::getGui()->addChild(_pListContainer);

    std::weak_ptr<Ui9Grid> cont_w = _pListContainer;
    std::weak_ptr<UiDropdown> dd_w = getThis<UiDropdown>();
    uDim* contW = &_uListContainerWidth;

    //You can click anything, anything you like.
    setPickRoot();
    setPress([cont_w, dd_w, contW]() {
        //Show container
        std::shared_ptr<Ui9Grid> cont = cont_w.lock();
        std::shared_ptr<UiDropdown> dd = dd_w.lock();
        if (cont && dd) {
            cont->left() = dd->getComputedQuad().left();
            if (contW->getDimUnit() == UiDimUnit::e::Auto) {
                cont->width() = dd->getComputedQuad().right() - dd->getComputedQuad().left();
            }
            else if (contW->getDimUnit() == UiDimUnit::e::Percent) {
                Gui2d::error("Percentage width on dropdown container... is invalid");
            }
            else {
                cont->width() = contW->px();
            }
            cont->top() = dd->getComputedQuad().bottom();
            Gu::getGui()->bringToFront(cont, true);
            cont->overflow() = (UiOverflowMode::e::Hide);
            cont->show();
        }
    });
    setClick([cont_w]() {
        //Show container
        if (std::shared_ptr<Ui9Grid> cont = cont_w.lock()) {
            cont->hide();
        }
    });
    //TODO: global release
}
void UiDropdown::setSelectedItem(std::shared_ptr<UiElement> item) {
    if (_pSelected == item) {
        return;
    }
    if (_pListContainer->getContentContainer()->hasChild(item) == false) {
        Gui2d::error( Stz "Dropdown didn't have the item you treid to select: "+ item->getName());
        return;
    }

    bool b;
    if (_pSelected != nullptr) {
        b = _pSelectedContainer->removeChild(_pSelected);
        _pListContainer->getContentContainer()->addChild(_pSelected);
    }
    b = _pListContainer->getContentContainer()->removeChild(item);
    _pSelectedContainer->addChild(item);

    _pSelected = item;
}
void UiDropdown::addItem(std::shared_ptr<UiElement> item, bool bFitHeight) {

    item->position() = UiPositionMode::e::Static;
    if(bFitHeight) {
        //*For labels..
        //**Set fittheight to false if you're making an image or other dropdown.
        //
        item->height() = height();
    }
    std::weak_ptr<UiDropdown> dd_w = getThis<UiDropdown>();
    std::weak_ptr<Ui9Grid> cont_w = _pListContainer;
    std::weak_ptr<UiElement> item_w = item;

    item->setPickRoot();
    item->setClick([cont_w, dd_w, item_w]() {
        std::shared_ptr<UiDropdown> dd = dd_w.lock();
        std::shared_ptr<Ui9Grid> cont = cont_w.lock();
        std::shared_ptr<UiElement> item = item_w.lock();
        if (cont && dd && item) {
            dd->setSelectedItem(item);
            cont->hide();
        }
    });
    _pListContainer->getContentContainer()->addChild(item);

    if (_pSelected == nullptr) {
        setSelectedItem(item);
    }
}
void  UiDropdown::performLayout(bool bForce) {
    Ui9Grid::performLayout(bForce);

}
#pragma endregion
//////////////////////////////////////////////////////////////////////////
#pragma region UiContainer
std::shared_ptr<UiContainer> UiContainer::create(){
    std::shared_ptr<UiContainer> pc = std::make_shared<UiContainer>();
    pc->init();
    return pc;
}
void UiContainer::init(){
    UiGrid::init();
    /*
    +-----------------+
    |     0       | 1 |
    |             |   |
    +-----------------+
    |  2          | 3 |
    +-----------------+
    */
    
    //I think we need uigrid here..3/3/18s
    std::shared_ptr<UiGridRow> r0 = addRow();
    r0->width() = "100%";
    r0->height() = "auto";
    std::shared_ptr<UiGridRow> r1 = addRow();
    r1->width() = "100%";
    r1->height() = uDim(_fBorderWidthPx, UiDimUnit::e::Pixel);
    std::shared_ptr<UiElement> c0 = r0->addCol();
    c0->width() = "auto";
    c0->height() = "100%";
    std::shared_ptr<UiElement> c1 = r0->addCol();//HSB
    c1->width() = uDim(_fBorderWidthPx, UiDimUnit::e::Pixel);
    c1->height() = "100%";

    std::shared_ptr<UiElement> c2 = r1->addCol();//VSB
    c0->width() = "auto";
    c0->height() = uDim(_fBorderWidthPx, UiDimUnit::e::Pixel);
    std::shared_ptr<UiElement> c3 = r1->addCol();//Corner
    c1->width() = uDim(_fBorderWidthPx, UiDimUnit::e::Pixel);
    c1->height() = uDim(_fBorderWidthPx, UiDimUnit::e::Pixel);

}
std::shared_ptr<UiElement> UiContainer::getContentContainer() {
    return getCell(0,0);
}
void UiContainer::enableScrollbar(std::shared_ptr<UiScrollbarSkin> pSkin) {
    //if (pSkin == nullptr) {
    //    Gui2d::error("Could not enable scroll bar on window, the skin didn't have one");
    //    return;
    //}
    //detachContentContainer();

    //**HACK this is not generic. scale stupid bar to make room on the right
    uDim d0w;
    uDim d2w;
    uDim d0h;
    uDim d2h;
    uDim d1;
    std::shared_ptr<UiElement> pCell = nullptr;
    if (pSkin->_eOrientation == Orientation::e::Vertical) {
        //float borderRem = MathUtils::broMax(_pBorder->get(GridBorder::e::Right).px() - pSkin->_uWidthOrHeightPx.px(), 0.0f);

        d0w = _fBorderWidthPx;//uDim(borderRem, UiDimUnit::e::Pixel);//Left pad %
        d0h = "100%";
      //  d1 = uDim(_pBorder->get(GridBorder::e::Right).px() * 0.3f, UiDimUnit::e::Pixel);
        //  d2w = uDim(_pBorder->get(GridBorder::e::Right).px() * 0.60f, UiDimUnit::e::Pixel);//Right pad %
        //  d2h = "auto";
        pCell = getCell(0,1);
    }
    else if (pSkin->_eOrientation == Orientation::e::Horizontal) {
        //float borderRem = MathUtils::broMax(_pBorder->get(GridBorder::e::Bot).px() - pSkin->_uWidthOrHeightPx.px(), 0.0f);

        d0w = "100%";
        d0h = _fBorderWidthPx; //uDim(borderRem, UiDimUnit::e::Pixel);//Left pad %
       // d1 = uDim(_pBorder->get(GridBorder::e::Bot).px() * 0.3f, UiDimUnit::e::Pixel);
        //   d2w = "100%";
        //   d2h = uDim(_pBorder->get(GridBorder::e::Bot).px() * 0.35f, UiDimUnit::e::Pixel);//Right pad %
        pCell = getCell(1, 0);
    }
    //*padding for the scrollbar to the left of the right column so that it pushes it over
    std::shared_ptr<UiElement> scrollLeftOrTopPad = UiElement::create();
    scrollLeftOrTopPad->width() = d0w;
    scrollLeftOrTopPad->height() = d0h; //100%
    pCell->addChild(scrollLeftOrTopPad, Gui2d::sortLayer(2));

    std::weak_ptr<UiWindow> weak_win = getThis<UiWindow>();
    std::weak_ptr<UiElement> weakControlObject = getContentContainer();
    UiScrubGen::BarWidthFunc barWidthfunc = [weakControlObject](std::shared_ptr<UiScrubGen> pScrollbar) {
        float fBarSize = 0;
        if (std::shared_ptr<UiElement> cont = weakControlObject.lock()) {
            std::shared_ptr<UiElement> ccParent = cont->getParent();
            float b1wh;
            float b2wh;
            Box2f b1 = ccParent->getComputedQuad();//9Grid column
            Box2f b2 = cont->getContentQuad();
            if (pScrollbar->getOrientation() == Orientation::e::Vertical) {
                b1wh = b1.height();
                b2wh = b2.height();

            }
            else if (pScrollbar->getOrientation() == Orientation::e::Horizontal) {
                b1wh = b1.width();
                b2wh = b2.width();

            }
            //#1 set bar size
            //Calculate the width/hight of the thumb bar relative to the scroll percentage. (Zero for trackbar)
            if (b2wh > 0) {
                fBarSize = b1wh / (b2wh);
                fBarSize = MathUtils::broClamp(fBarSize, 0.0f, 1.0f);
            }
        }
        return fBarSize;
    };
    UiScrubGen::ScrollFunc scrollFunc = [weakControlObject, weak_win](std::shared_ptr<UiScrubGen> pScrollbar, float scrollPct) {
        if (std::shared_ptr<UiElement> cont = weakControlObject.lock()) {
            if (std::shared_ptr<UiWindow> win = weak_win.lock()) {
                //What this does: Move the content container of this window based on the scrollPct of the H or V scrollbar
                float ccWh;
                float ccWhMinusEof;
                float pwh = 0;
                uDim* cTorL = nullptr;
                uDim* cWorH = nullptr;
                if (pScrollbar->getOrientation() == Orientation::e::Vertical) {
                    ccWh = cont->getContentQuad().height();
                    if (cont->getParent()) {
                        pwh = cont->getParent()->getComputedQuad().height();
                    }
                    cTorL = &cont->top();
                    cWorH = &cont->height();
                }
                else if (pScrollbar->getOrientation() == Orientation::e::Horizontal) {

                    ccWh = cont->getContentQuad().width();
                    if (cont->getParent()) {
                        pwh = cont->getParent()->getComputedQuad().width();
                    }
                    cTorL = &cont->left();
                    cWorH = &cont->width();
                }

                //Update window contents offset
                ccWhMinusEof = ccWh;
                if (pScrollbar->getCanScrollPastEof() == false) {
                    ccWhMinusEof = ccWh - pwh;
                }

                float ccWorHPct = ccWhMinusEof * scrollPct;
                *cTorL = -ccWorHPct;
                *cWorH = uDim(ccWh, UiDimUnit::e::Pixel);

                win->setLayoutChanged();
                win->setLayoutChanged(true);
            }
        }
    };

    //add the scrollar
    _pVScrollBar = UiScrollbar::create(pSkin, barWidthfunc, scrollFunc);
    pCell->addChild(_pVScrollBar, Gui2d::sortLayer(2));

    //Right pad
    //  std::shared_ptr<UiElement> scrollRightOrBotPad = UiElement::create();
    //  scrollRightOrBotPad->width() = d2w;
    //  scrollRightOrBotPad->height() = d2h; //100%
    //  pCell->addChild(scrollRightOrBotPad, Gui2d::sortLayer(2));

    setLayoutChanged();
    setLayoutChanged(true);
}
void UiContainer::enableResize() {
    //Use weak ptr to avoid circular reference
    std::weak_ptr<UiWindow> win = getThis<UiWindow>();
    UiDragInfo::DragFunc sizeFunc = [win](vec2& dxy) {
        if (std::shared_ptr<UiWindow> ww = win.lock()) {
            ww->width() = ww->width().px() + dxy.x;
            ww->height() = ww->height().px() + dxy.y;
            ww->setLayoutChanged();
        }
    };
    getCell(1,1)->enableDrag(sizeFunc);
    setLayoutChanged();
}
#pragma endregion
//////////////////////////////////////////////////////////////////////////
#pragma region UIWindow
std::shared_ptr<UiElement> UiWindow::getContentContainer(){
    return _pContainer->getContentContainer();
}
std::shared_ptr<UiWindow> UiWindow::create(std::shared_ptr<UiWindowSkin> pSkin) {
    if (UiSkinElement::setAndValid(pSkin) == false) {
        return nullptr;
    }
    std::shared_ptr<UiWindow> ui = std::make_shared<UiWindow>();
    ui->_pWindowSkin = pSkin;
    ui->init();

    return ui;
}
void UiWindow::init() {
    UiElement::init();
    setName("UiWindow");

    position() = UiPositionMode::Relative;
    //Set some default position stuff so we can see it.
    top() = "100px";
    left() = "200px";
    width() = "300px";
    height() = "400px";
    

    //Create background
    _pBackground = Ui9Grid::create(_pWindowSkin->_pBorder);
    _pBackground->set9GridImages(_pWindowSkin->_pBorderImgs);
    addChild(_pBackground, Gui2d::sortLayer(0));

    _pTitleBar = UiElement::create();
    _pTitleBar->width() = "100%";
    _pTitleBar->height() = "32px";
    _pContentArea = UiElement::create();
    _pContentArea->width() = "100%";
    _pContentArea->height() = "auto";

    _pContainer = UiContainer::create();
    _pContainer->width() = "100%";
    _pContainer->height() = "100%";
    _pContentArea->addChild(_pContainer);

    addChild(_pTitleBar, Gui2d::sortLayer(1));
    addChild(_pContentArea, Gui2d::sortLayer(1));

    //Title Bar
    _lblTitle = UiLabel::create("Title", _pWindowSkin->_pTitleFont);
    _lblTitle->position() = UiPositionMode::Static;
    _lblTitle->width() = "100%";
    _lblTitle->height() = "30px";
    _lblTitle->padLeft() = "20px";
    _lblTitle->padTop() = "10px";
    _lblTitle->getColor().a() = 0.521f;

    _pTitleBar->addChild(_lblTitle);

    std::shared_ptr<UiImageButton> bt = UiImageButton::create(_pWindowSkin->_pCloseButton);
    //Close Event
    std::weak_ptr<UiWindow> winw = getThis<UiWindow>();
    bt->addEvent(UiEventId::e::Mouse_Lmb_Release, UiEventFunc::create(
        [winw](UiEventId::e evId, void*) {
        if (std::shared_ptr<UiWindow> win = winw.lock()) {
            win->hide();
        }
    }
    ));
    _pTitleBar->addChild(bt);

    //BTF
    std::weak_ptr<UiWindow> weak_win = getThis<UiWindow>();
    std::function<void()> bringToFront = [weak_win]() {
        if (std::shared_ptr<UiWindow> win = weak_win.lock()) {
            //assumes all windows are on the same layer.
            win->getParent()->bringToFront(win, false);
        }
    };
    setPress(bringToFront);

}
void UiWindow::setTitleLabel(std::string lbl){
    _lblTitle->setText(lbl);
}
void UiWindow::update(std::shared_ptr<Fingers> pFingers) {
    //Update Childs including VScrollbar
    UiElement::update(pFingers);

    //auto expand
    _pContainer->width() = uDim(getComputedQuad().right() - getComputedQuad().left(), UiDimUnit::e::Pixel);
    _pContainer->height() = uDim(getComputedQuad().bottom() - getComputedQuad().top(), UiDimUnit::e::Pixel);
}

void UiWindow::enableResize(){
    _pContainer->enableResize();
}

void UiWindow::enableDrag() {

    //Use weak ptr to avoid circular reference
    std::weak_ptr<UiWindow> win = getThis<UiWindow>();
    UiDragInfo::DragFunc moveFunc = [win](vec2& dxy) {
        if (std::shared_ptr<UiWindow> ww = win.lock()) {
            ww->left() = ww->left().px() + dxy.x;
            ww->top() = ww->top().px() + dxy.y;
            if (ww->getParent() != nullptr) {
                //change layout for parent, window's layout isn't changed
                ww->getParent()->setLayoutChanged();
            }
            else {
                ww->setLayoutChanged();
            }
        }
    };

    _pTitleBar->enableDrag(moveFunc);
    setLayoutChanged();
}
bool UiWindow::pick(std::shared_ptr<Fingers> fingers) {
    bool b = UiElement::pick(fingers);

    return b;
}
void UiWindow::performLayout(bool bForce) {
    UiElement::performLayout(bForce);
}
void UiWindow::enableVScrollbar() {
    _pContainer->enableScrollbar(_pWindowSkin->_pVScroll);
}
void UiWindow::enableHScrollbar() {
    _pContainer->enableScrollbar(_pWindowSkin->_pHScroll);
}

#pragma endregion
//////////////////////////////////////////////////////////////////////////
#pragma region UiToolbar
std::shared_ptr<UiToolbar> UiToolbar::create(std::shared_ptr<UiToolbarSkin> skin) {
    std::shared_ptr<UiToolbar> ui = std::make_shared<UiToolbar>();
    ui->_pToolbarSkin = skin;
    ui->_pWindowSkin = skin->_pWindowSkin;
   // ui->_pBorder = skin->_pWindowSkin->_pBorder;
    ui->init();
    return ui;

}
void UiToolbar::init() {
    UiWindow::init();
    setName("Toolbarf");

    _pTitleBar->hide();

    position() = UiPositionMode::Relative;
    display() = UiDisplayMode::InlineNoWrap;
    //Set some default position stuff so we can see it.
    top() = "0px";
    left() = uDim(Gu::getGui()->getDesignSize().getWidth() * 0.1f, UiDimUnit::e::Pixel);
    width() = "auto";//uDim(Gu::getGui()->getDesignSize().getWidth() * 0.8f, UiDimUnit::e::Pixel);
    height() = "auto";//uDim(Gu::getGui()->getDesignSize().getHeight() * 0.1f, UiDimUnit::e::Pixel);

}
void UiToolbar::update(std::shared_ptr<Fingers> pFingers) {
    UiWindow::update(pFingers);
}
void UiToolbar::performLayout(bool bForce) {
    UiElement::performLayout(bForce);
    for(auto p : getChildren()) {
        std::shared_ptr<UiElement> e = p.second;
    }
}
#pragma endregion
//////////////////////////////////////////////////////////////////////////
#pragma region UICursor
std::shared_ptr<UiCursor> UiCursor::create(std::shared_ptr<UiCursorSkin> ss) {
    std::shared_ptr<UiCursor> ret = std::make_shared<UiCursor>();
    ret->_pSkin = ss;
    ret->init();
    ret->setName("UiCursor");
    ret->setTexture(ss->_pTex);
    ret->setSizeMode(UiImageSizeMode::e::Expand, UiImageSizeMode::e::Expand);
    ret->setWrapMode(TexWrap::Clamp, TexWrap::Clamp);

    return ret;
}
void UiCursor::init() {
    UiImage::init();
}
void UiCursor::performLayout(bool bForce) {
    UiImage::performLayout(bForce);
}
#pragma endregion
//////////////////////////////////////////////////////////////////////////
#pragma region Gui2d
Gui2d::Gui2d() {
}
Gui2d::~Gui2d() {
}
void Gui2d::init() {
    //Note:this init() is lazy and happens way after Gui2d is allocated
    UiElement::init();

    right() = _designSize.getWidth() - 1;//Gu::getViewport()->getWidth();
    bottom() = _designSize.getHeight() - 1;//Gu::getViewport()->getHeight();
    top() = 0;
    left() = 0;
    minWidth() = maxWidth() = _designSize.getWidth() - 1;//right();
    minHeight() = maxHeight() = _designSize.getHeight() - 1;//bottom();
    setName("Gui2d");
    _pTex = std::make_shared<MegaTex>(Gu::getGraphicsContext());

    std::vector<v_GuiVert> verts;
    std::vector<v_index32> inds;
    _pMesh = MeshNode::create(
        std::make_shared<MeshSpec>(
            verts.data(), verts.size(),
            inds.data(), inds.size(),
            v_GuiVert::getVertexFormat(), nullptr)
    );
}

void Gui2d::update(std::shared_ptr<Fingers> pFingers) {
    UiElement::update(pFingers);//Note: due to buttons resetting themselves update() must come before pick()

   // if (Gu::getFpsMeter()->frameMod(2)) {
    updateLayout(pFingers);
    // }

     //Updating this per frame to indicate if the GUI is picked.
    _bIsPicked = pick(pFingers);
}
void Gui2d::updateLayout(std::shared_ptr<Fingers> pFingers) {
    _pCursor->left() = pFingers->getMousePos().x;
    _pCursor->top() = pFingers->getMousePos().y;

    // if(_updateFuture.valid() == false) {
       //  std::weak_ptr<Gui2d> gw = getThis<Gui2d>();

        // _updateFuture = std::async(std::launch::async, [gw] {
        //     if (std::shared_ptr<Gui2d> gui = gw.lock()) {
    if (getLayoutChanged()) {
        //Gui2d doesn't have a parent, so we have to compute the quads to create a valid clip region.
        computeQuads(right().px(), left().px(), top().px(), bottom().px());

        performLayout(_bDebugForceLayoutChange);
        _bDebugForceLayoutChange = false;
    }

    //Cursor
    computePositionalElement(_pCursor);
    UiElement::layoutEleQuad(_pCursor);
    _pCursor->performLayout(false);
    //        }
     //       return true;
     //   });
   // }
   // if (_updateFuture.valid()) {
   //    std::future_status status = _updateFuture.wait_for(std::chrono::milliseconds(0));
   //    if (status == std::future_status::ready) {
    updateMesh();
    //       _updateFuture.get();//Sets to invalid
    //   }
   // }

}
void Gui2d::updateMesh() {

    Box2f b = getGLRasterQuad();
    std::vector<v_GuiVert> verts;
    std::vector<v_index32> inds;
    UiElement::regenMesh(verts, inds, b);

    //CURSOR
    b = getGLRasterQuad();
    _pCursor->regenMeshExposed(verts, inds, b);
    //CURSOR

    _pMesh->getMeshSpec()->getVaoData()->getVbo()->allocate(verts.size());
    _pMesh->getMeshSpec()->getVaoData()->getVbo()->copyDataClientServer(verts.size(), verts.data());
    _pMesh->getMeshSpec()->getVaoData()->getIbo()->allocate(inds.size());
    _pMesh->getMeshSpec()->getVaoData()->getIbo()->copyDataClientServer(inds.size(), inds.data());

}
void Gui2d::screenChanged(uint32_t uiWidth, uint32_t uiHeight, bool bFullscreen) {
    //Mark all children as dirty
    setLayoutChanged(true);
}

void Gui2d::drawForward() {
    RenderParams rp;
    Box2f b = getGLRasterQuad();
    drawForward(rp, b);
}

void Gui2d::drawForward(RenderParams& rp, Box2f&  b2ClipRect) {
    rp.setShader(Gu::getShaderMaker()->getGuiShader());

    //Zero useless params.
    getTex()->bind(TextureChannel::e::Channel0, rp.getShader());
    rp.setMesh(_pMesh);
    rp.setDrawMode(GL_POINTS);
    rp.draw();

    //*Debug
    UiElement::drawDebug();
}
void Gui2d::error(std::string errMsg) {
    BroLogError(errMsg);
    Gu::debugBreak();
}
void Gui2d::hideCursor() { 
    _pCursor->setLayoutVisible(false); 
}
void Gui2d::showCursor() { 
    _pCursor->setLayoutVisible(true); 
}
void Gui2d::setCursor(std::shared_ptr<UiCursor> c) {
    //Do not add the cursor
    _pCursor = c;
    c->position() = UiPositionMode::Relative;
    //updateCursorPos(Gu::getFingers()->getMousePos());
}
float Gui2d::getDesignMultiplierW() {
    float dw = Gu::getGui()->getDesignSize().getWidth();
    float vw = (float)Gu::getCamera()->getViewport()->getWidth();
    float w1 = vw / dw;
    return w1;
}
float Gui2d::getDesignMultiplierH() {
    float dh = Gu::getGui()->getDesignSize().getHeight();
    float vh = (float)Gu::getCamera()->getViewport()->getHeight();
    float h1 = vh / dh;
    return h1;
}
void Gui2d::debugForceLayoutChanged() {
    setLayoutChanged(true);
    _bDebugForceLayoutChange = true;
}
void Gui2d::performForcedLayout(){
    performLayout(true);
}



#pragma endregion

}//ns Game
