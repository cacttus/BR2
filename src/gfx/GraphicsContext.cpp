
#include "../base/FpsMeter.h"
#include "../base/Delta.h"
#include "../base/FrameSync.h"
#include "../base/InputManager.h"
#include "../base/Package.h"
#include "../base/Logger.h"
#include "../base/Sequencer.h"
#include "../base/SoundCache.h"
#include "../base/FileSystem.h"
#include "../base/EngineConfig.h"
#include "../base/GraphicsWindow.h"
#include "../gfx/UiControls.h"   
#include "../gfx/Picker.h"   
#include "../gfx/ParticleManager.h"   
#include "../gfx/GraphicsContext.h"
#include "../gfx/RenderSettings.h"
#include "../gfx/CameraNode.h"
#include "../gfx/ShaderMaker.h"
#include "../gfx/GraphicsApi.h"
#include "../model/VertexTypes.h"
#include "../model/VertexFormat.h"
#include "../model/ModelCache.h"
#include "../world/PhysicsWorld.h"
#include "../world/Scene.h"

namespace BR2 {
GraphicsContext::GraphicsContext(){
    _fClearR = 0.02f;
    _fClearG = 0.02f;
    _fClearB = 0.021f;
    _fClearA = 1.0f;
}
GraphicsContext::~GraphicsContext(){
}
bool GraphicsContext::init() {
    _bValid = true;

    BRLogInfo("GraphicsContext - Making Vtx Formats.");
    makeVertexFormats();

    return isValid();
}
std::shared_ptr<CameraNode> GLContext::getActiveCamera() {
  return getGraphicsWindow()->setScene()->getActiveCamera();
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
