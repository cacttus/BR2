#include "../gfx/GraphicsContext.h"
#include "../gfx/RenderSettings.h"
#include "../gfx/CameraNode.h"
#include "../gfx/ShaderMaker.h"
#include "../model/ModelCache.h"
#include "../base/FpsMeter.h"
#include "../base/FrameSync.h"
#include "../base/Fingers.h"
#include "../base/Package.h"
#include "../base/Logger.h"
#include "../base/Sequencer.h"
#include "../base/SoundCache.h"
#include "../base/FileSystem.h"
#include "../base/EngineConfig.h"
#include "../gfx/FlyCam.h"
#include "../model/VertexTypes.h"
#include "../model/VertexFormat.h"
#include "../gfx/Gui2d.h"   
#include "../gfx/Picker.h"   
#include "../gfx/Party.h"   
#include "../world/PhysicsWorld.h"


namespace Game {
GraphicsContext::GraphicsContext(){
    _fClearR = 0.02f;
    _fClearG = 0.02f;
    _fClearB = 0.021f;
    _fClearA = 1.0f;
}
GraphicsContext::~GraphicsContext(){

}


bool GraphicsContext::load(std::shared_ptr<AppBase> rb) {
    _bValid = true;
    setRoom(rb);

    BroLogInfo("GraphicsContext - Making Vtx Formats.");
    makeVertexFormats();

    BroLogInfo("GraphicsContext - Creating Managers.");
    createManagers(rb);

    //In the future we will replace this witht he active object.
    BroLogInfo("GraphicsContext - Creating Render View");
    _pFlyCam = std::make_shared<FlyCam>();

    return isValid();
}

void GraphicsContext::createManagers(std::shared_ptr<AppBase> rb) {
    _pRenderSettings = RenderSettings::create();
    BroLogInfo("GLContext - Building Package");
    _pPackage = std::make_shared<Package>();
    _pPackage->build(FileSystem::getExecutableFullPath());
    BroLogInfo("GLContext - Creating TexCache");
    _pTexCache = std::make_shared<TexCache>(shared_from_this());

    BroLogInfo("GLContext - Creating TextBoss");
//    _pTextManager = std::make_shared<TextBoss>(shared_from_this());
    BroLogInfo("GLContext - Creating Party");
    _pParty = std::make_shared<Party>(shared_from_this());
    BroLogInfo("GLContext - Creating Sequencer");
    _pSequencer = std::make_shared<Sequencer>();
    BroLogInfo("GLContext - Creating Fingers");
    _pFingers = std::make_shared<Fingers>();
    _pFingers->init();
    BroLogInfo("GLContext - Creating FpsMeter");
    _pFpsMeter = std::make_shared<FpsMeter>();
    BroLogInfo("GLContext - Creating FrameSync");
    _pFrameSync = std::make_shared<FrameSync>(shared_from_this());
    //This was commented out.  Why? 11/6
    BroLogInfo("GLContext - Creating SoundCache");
    _pSoundCache = std::make_shared<SoundCache>();
    BroLogInfo("GLContext - Creating ShaderMaker & base shaders");
    _pShaderMaker = std::make_shared<ShaderMaker>();
    _pShaderMaker->initialize(rb);
    BroLogInfo("GLContext -  Lights");
    _pLightManager = std::make_shared<LightManager>(shared_from_this());
    BroLogInfo("GLContext - Model Cache");
    _pModelCache = std::make_shared<ModelCache>(shared_from_this());

    BroLogInfo("GLContext - Picker");
    _pPicker = std::make_shared<Picker>(shared_from_this());

    BroLogInfo("GLContext - Gui");
    _pGui2d = std::make_shared<Gui2d>();
}
void GraphicsContext::update(float delta) {
    if (_pSequencer != nullptr) {
        _pSequencer->update();
    }
    if (_pParty != nullptr) {
        _pParty->update(delta);
    }
    if (_pFpsMeter != nullptr) {
        _pFpsMeter->update();
    }
    if (_pSoundCache != nullptr) {
        _pSoundCache->update();
    }
    if (_pPicker != nullptr) {
        _pPicker->update(_pFingers);
    }
    if (_pGui2d != nullptr) {
        _pGui2d->update(_pFingers);
    }

}
void GraphicsContext::makeVertexFormats() {

    v_v3c4x2::_pVertexFormat = std::make_shared<VertexFormat>(shared_from_this(), "v_v3c4x2");
    v_v3c4x2::_pVertexFormat->addComponent(VertexUserType::e::v3_01);
    v_v3c4x2::_pVertexFormat->addComponent(VertexUserType::e::c4_01);
    v_v3c4x2::_pVertexFormat->addComponent(VertexUserType::e::x2_01);

    v_v2c4x2::_pVertexFormat = std::make_shared<VertexFormat>(shared_from_this(), "v_v2c4x2");
    v_v2c4x2::_pVertexFormat->addComponent(VertexUserType::e::v2_01);
    v_v2c4x2::_pVertexFormat->addComponent(VertexUserType::e::c4_01);
    v_v2c4x2::_pVertexFormat->addComponent(VertexUserType::e::x2_01);

    v_v3n3x2::_pVertexFormat = std::make_shared<VertexFormat>(shared_from_this(), "v_v3n3x2");
    v_v3n3x2::_pVertexFormat->addComponent(VertexUserType::e::v3_01);
    v_v3n3x2::_pVertexFormat->addComponent(VertexUserType::e::n3_01);
    v_v3n3x2::_pVertexFormat->addComponent(VertexUserType::e::x2_01);

    v_v3x2::_pVertexFormat = std::make_shared<VertexFormat>(shared_from_this(), "v_v3x2");
    v_v3x2::_pVertexFormat->addComponent(VertexUserType::e::v3_01);
    v_v3x2::_pVertexFormat->addComponent(VertexUserType::e::x2_01);

    v_v3n3::_pVertexFormat = std::make_shared<VertexFormat>(shared_from_this(), "v_v3n3");
    v_v3n3::_pVertexFormat->addComponent(VertexUserType::e::v3_01);
    v_v3n3::_pVertexFormat->addComponent(VertexUserType::e::n3_01);

    v_v3::_pVertexFormat = std::make_shared<VertexFormat>(shared_from_this(), "v_v3");
    v_v3::_pVertexFormat->addComponent(VertexUserType::e::v3_01);

    v_v2x2::_pVertexFormat = std::make_shared<VertexFormat>(shared_from_this(), "v_v2x2");
    v_v2x2::_pVertexFormat->addComponent(VertexUserType::e::v2_01);
    v_v2x2::_pVertexFormat->addComponent(VertexUserType::e::x2_01);

    v_v2c4::_pVertexFormat = std::make_shared<VertexFormat>(shared_from_this(), "v_v2c4");
    v_v2c4::_pVertexFormat->addComponent(VertexUserType::e::v2_01);
    v_v2c4::_pVertexFormat->addComponent(VertexUserType::e::c4_01);

    //v_v3c3i4i4::_pVertexFormat = new VertexFormat(this, "v_v3c3i4i4");
    //v_v3c3i4i4::_pVertexFormat->addComponent(GL_FLOAT, 3, sizeof(vec3), VertexUserType::e::v3_01);
    //v_v3c3i4i4::_pVertexFormat->addComponent(GL_FLOAT, 3, sizeof(vec3), VertexUserType::e::c3_01);
    //v_v3c3i4i4::_pVertexFormat->addComponent(GL_INT, 4, sizeof(ivec4));
    //v_v3c3i4i4::_pVertexFormat->addComponent(GL_INT, 4, sizeof(ivec4));

    v_v3c3x2n3::_pVertexFormat = std::make_shared<VertexFormat>(shared_from_this(), "v_v3c3x2n3");
    v_v3c3x2n3::_pVertexFormat->addComponent(VertexUserType::e::v3_01);
    v_v3c3x2n3::_pVertexFormat->addComponent(VertexUserType::e::c3_01);
    v_v3c3x2n3::_pVertexFormat->addComponent(VertexUserType::e::x2_01);
    v_v3c3x2n3::_pVertexFormat->addComponent(VertexUserType::e::n3_01);

    v_v3i2n3::_pVertexFormat = std::make_shared<VertexFormat>(shared_from_this(), "v_v3i2n3");
    v_v3i2n3::_pVertexFormat->addComponent(VertexUserType::e::v3_01);
    v_v3i2n3::_pVertexFormat->addComponent(VertexUserType::e::i2_01);
    v_v3i2n3::_pVertexFormat->addComponent(VertexUserType::e::n3_01);

    v_v3c4::_pVertexFormat = std::make_shared<VertexFormat>(shared_from_this(), "v_v3c4");
    v_v3c4::_pVertexFormat->addComponent(VertexUserType::e::v3_01);
    v_v3c4::_pVertexFormat->addComponent(VertexUserType::e::c4_01);

    v_v3c4x2n3::_pVertexFormat = std::make_shared<VertexFormat>(shared_from_this(), "v_v3c4x2n3");
    v_v3c4x2n3::_pVertexFormat->addComponent(VertexUserType::e::v3_01);
    v_v3c4x2n3::_pVertexFormat->addComponent(VertexUserType::e::c4_01);
    v_v3c4x2n3::_pVertexFormat->addComponent(VertexUserType::e::x2_01);
    v_v3c4x2n3::_pVertexFormat->addComponent(VertexUserType::e::n3_01);

    v_GuiVert::_pVertexFormat = std::make_shared<VertexFormat>(shared_from_this(), "v_GuiVert");
    v_GuiVert::_pVertexFormat->addComponent(VertexUserType::e::v4_01);
    v_GuiVert::_pVertexFormat->addComponent(VertexUserType::e::v4_02);
    v_GuiVert::_pVertexFormat->addComponent(VertexUserType::e::v4_03);
    v_GuiVert::_pVertexFormat->addComponent(VertexUserType::e::v2_01);
    v_GuiVert::_pVertexFormat->addComponent(VertexUserType::e::u2_01);


}







}//ns Game
