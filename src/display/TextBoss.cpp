//#include "../base/GLContext.h"
//#include "../base/Logger.h"
//#include "../base/Gu.h"
//#include "../base/EngineConfig.h"
//#include "../display/FontSpec.h"
//#include "../display/FontTextBuffer.h"
//#include "../display/TextBoss.h"
//#include "../display/Viewport.h"
//
//namespace Game {;
//TextBoss::TextBoss(std::shared_ptr<GLContext> pc) : _pContext(pc) {
//    _currentRenderColor = vec4(1, 1, 1, 1);
//    _fCachedBakedFontSize = (float)_pContext->getConfig()->getBakedCharSize();//float)Gu::getBakedFontCharSize();;
//}
//TextBoss::~TextBoss() {
//    _fontSpecs.clear();
//}
//void TextBoss::setColor(float r, float g, float b, float a) { _currentRenderColor = vec4(r, g, b, a); }
//void TextBoss::setColor(vec4& col) { _currentRenderColor = col; }
//vec4 TextBoss::getColor() { return _currentRenderColor; }
//void TextBoss::setFont(t_string& fontName) { _currentFontName = fontName; }
//void TextBoss::setFontSize(FontSize sz)
//{
//    _iCurrentFontSize = sz;
//}
//float TextBoss::fontSizeToFontScale(FontSize fontSize)
//{
//    float f = fontSize / _fCachedBakedFontSize;
//    return f;
//}
//void TextBoss::pstrb(t_string& format, int32_t X, int32_t Y, ...)
//{
//    //Note: text coordinates are bottom-left corner in OpenGL
//    //size_t size = _INTSIZEOF(format.c_str());
//    va_list args;
//    va_start(args, Y);
//    pstrb(args, format, X, Y);
//    va_end(args);
//}
//void TextBoss::pstrb(va_list args, t_string& format, int32_t X, int32_t Y) {
//    std::shared_ptr<FontSpec> fs = getFontSpec(_currentFontName);
//    if (fs == nullptr) {
//        BroLogErrorCycle(TStr("Font spec ", _currentFontName, " not found.  You may have not created the fonts in the mainroom!"), __LINE__, __FILE__);
//        return;
//    }
//
//    t_string theString = StringUtil::formatVa(format, args);
//
//    fs->getBufList()->push_back(
//        std::make_shared<FontTextBuffer>(
//            theString,
//            X,
//            Y,
//            _currentRenderColor,
//            _currentFontName,
//            _iCurrentFontSize
//        )
//    );
//}
//void TextBoss::addVec3(int32_t X, int32_t Y, t_string& format, vec3& v)
//{
//    t_string s1 = format;
//    t_string s2 = s1 + t_string("<%.2f,%.2f,%.2f>");
//    return pstrb(s2, X, Y, v.x, v.y, v.z);
//}
//void TextBoss::addVec3(int32_t X, int32_t Y, t_string& format, ivec3& v)
//{
//    t_string s1 = format;
//    t_string s2 = s1 + t_string("<%i,%i,%i>");
//    return pstrb(s2, X, Y, v.x, v.y, v.z);
//}
//std::shared_ptr<FontSpec> TextBoss::getFontSpec(t_string& name)
//{
//    FontSet::iterator ite = _fontSpecs.begin();
//    for (; ite != _fontSpecs.end(); ite++)
//        if (StringUtil::equals((*ite)->getFontName(), name))
//            return (*ite);
//
//    return NULL;
//}
//std::shared_ptr<FontSpec> TextBoss::getDefaultFont(){
//    if(_fontSpecs.size()==0){
//        return nullptr;
//    }
//    return *_fontSpecs.begin();
//}
//
//std::shared_ptr<FontSpec> TextBoss::getCurrentFont()
//{
//    return getFontSpec(_currentFontName);
//}
//void TextBoss::drawTextBuffer(RenderParams& rp)
//{
//
//    FontSet::iterator ite = _fontSpecs.begin();
//    for (; ite != _fontSpecs.end(); ite++) {
//        (*ite)->copyAndDrawFonts(rp);
//    }
//
//    clearTextBuffer();
//    //CheckAllErrorsDbg();
//}
//void TextBoss::clearTextBuffer()
//{
//    FontSet::iterator ite = _fontSpecs.begin();
//    for (; ite != _fontSpecs.end(); ite++)
//        (*ite)->clearBufList();
//}
//void TextBoss::createFont(t_string& userName, t_string& fileName)
//{
//    float cFontSize;
//
//    if (_currentFontName == StringUtil::empty()) {
//        _currentFontName = userName;
//    }
//
//    cFontSize = (float)_pContext->getConfig()->getBakedCharSize();//Gu::getBakedFontCharSize();
//
//    std::shared_ptr<FontSpec> fspec = std::make_shared<FontSpec>(
//        _pContext
//        , userName
//        , fileName
//        , cFontSize
//        );
//    fspec->create();
//    _fontSpecs.insert(fspec);
//
//}
//float TextBoss::getCenteredTextX(t_string text) {
//    return getCurrentFont()->getCenteredTextX(text, getFontSize());
//}
//float TextBoss::getCenteredTextY(t_string text) {
//    return getCurrentFont()->getCenteredTextY(text, getFontSize());
//}
//
//
//}//ns game
