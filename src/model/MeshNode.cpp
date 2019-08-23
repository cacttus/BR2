#include "../base/Base.h"
#include "../base/OglErr.h"
#include "../app/AppBase.h"
#include "../base/Perf.h"
#include "../math/Random.h"
#include "../model/OBB.h"

#include "../gfx/GpuComputeSync.h"
#include "../gfx/ShaderMaker.h"
#include "../gfx/ShaderBase.h"
#include "../gfx/CameraNode.h"
#include "../gfx/RenderParams.h"
#include "../gfx/LightNode.h"
#include "../gfx/LightManager.h"
#include "../gfx/ShadowBoxSide.h"
#include "../gfx/Picker.h"
#include "../gfx/TexCache.h"
#include "../gfx/RenderSettings.h"
#include "../model/Material.h"
#include "../model/ModelCache.h"

#include "../model/ShaderStorageBuffer.h"
#include "../model/FragmentBufferData.h"
#include "../model/IndexBufferData.h"
#include "../model/MeshSpec.h"
#include "../model/Model.h"
#include "../model/MeshSpec.h"
#include "../model/VboData.h"
#include "../model/IboData.h"
#include "../model/VaoDataGeneric.h"
#include "../model/MeshNode.h"
#include "../world/PhysicsWorld.h"
#include "../world/RenderBucket.h"
#include "../gfx/Picker.h"

namespace Game {

///////////////////////////////////////////////////////////////////

MeshNode::MeshNode(std::shared_ptr<MeshSpec> ms) : MeshNode(ms, nullptr) {
   // _pVaoData = std::make_shared<VaoDataGeneric>(pContext, fmt);
}
MeshNode::MeshNode(std::shared_ptr<MeshSpec> ps, std::shared_ptr<ModelNode> mn) : BaseNode(ps) {
    _pModelNode = mn;
}

//MeshNode::MeshNode(
//    const void* cVerts, size_t vCount,
//    const void* cIndexes, size_t iCount,
//    std::shared_ptr<VertexFormat> fmt, std::shared_ptr<Material> pm, std::shared_ptr<MeshSpec> ps) : MeshNode(pContext, fmt, ps) {
//    //We could easily create the box by getting a pointer to the raw data and casting to vec3.
//    //getBox()->genResetLimits();
//
//    createDataBlock(cVerts, vCount, cIndexes, iCount);
//
//    if (pm != nullptr) {
//        _pMaterial = pm->makeCopy();
//    }
//}
MeshNode::~MeshNode()
{
  //  DEL_MEM(_pVaoData);
  //  DEL_MEM(_pMaterial);
  //  DEL_MEM(_pArmJoints);
    _vecBoneNodesOrdered.resize(0);
    _vecArmaturesOrdered.resize(0);
}

///////////////////////////////////////////////////////////////////
std::shared_ptr<MeshNode> MeshNode::create(std::shared_ptr<MeshSpec> ps, std::shared_ptr<ModelNode> mn){
    std::shared_ptr<MeshNode> m = std::make_shared<MeshNode>(ps, mn);
    m->init();
    return m;
}
std::shared_ptr<MeshNode> MeshNode::create(std::shared_ptr<MeshSpec> pd){
    std::shared_ptr<MeshNode> m = std::make_shared<MeshNode>(pd);
    m->init();
    return m;
}
void MeshNode::init() {
    BaseNode::init();

    //    _pVaoData = std::make_shared<VaoDataGeneric>(pContext, ps->getVertexFormat());
    setHidden(getMeshSpec()->getHideRender());

//#ifdef _DEBUG
//    setVisible(true);
//#endif

    if (getMeshSpec()->getMaterial() != nullptr) {
        _pMaterial = getMeshSpec()->getMaterial()->makeCopy();
    }

    if (getMeshSpec()->hasSkin()) {
        createSkin();
    }

    _iPickId = Gu::getPicker()->genPickId();
}

std::shared_ptr<MeshSpec> MeshNode::getMeshSpec() { return std::dynamic_pointer_cast<MeshSpec>(BaseNode::getSpec()); }

void MeshNode::printDataToStdout(){
    GpuAnimatedMeshWeightData* wdat = new GpuAnimatedMeshWeightData[getMeshSpec()->getWeightOffsetsGpu()->getNumElements()];
    getMeshSpec()->getWeightOffsetsGpu()->copyDataServerClient(getMeshSpec()->getWeightOffsetsGpu()->getNumElements(), wdat);
    std::cout << "WDAT" << std::endl;
    for (size_t i = 0; i < getMeshSpec()->getWeightOffsetsGpu()->getNumElements(); ++i) {
        std::cout << " (" << wdat[i]._offset << ", " << wdat[i]._count << ") ";
    }
    std::cout << std::endl;
    std::cout << std::endl;
    delete[] wdat;

    GpuAnimatedMeshWeight* weights = new GpuAnimatedMeshWeight[getMeshSpec()->getWeightsGpu()->getNumElements()];
    getMeshSpec()->getWeightsGpu()->copyDataServerClient(getMeshSpec()->getWeightsGpu()->getNumElements(), weights);
    std::cout << "Weihts" << std::endl;
    for (size_t i = 0; i < getMeshSpec()->getWeightsGpu()->getNumElements(); ++i) {
        std::cout << " (" << weights[i]._iArmJointOffset << ", " << weights[i]._weight << ") ";
    }
    std::cout << std::endl;
    std::cout << std::endl;
    delete[] weights;

    GpuAnimatedMeshSkinMatrix* arms = new GpuAnimatedMeshSkinMatrix[_pArmJoints->getNumElements()];
    _pArmJoints->copyDataServerClient(_pArmJoints->getNumElements(), arms);
    std::cout << "ARM MATS" << std::endl;
    for (size_t i = 0; i < _pArmJoints->getNumElements(); ++i) {
        std::cout << " (" << arms[i]._matrix.toString() << ") ";
    }
    std::cout << std::endl;
    std::cout << std::endl;
    delete[] arms;
}
void MeshNode::createSkin() {

    AssertOrThrow2(_pModelNode != nullptr);
    AssertOrThrow2(getMeshSpec() != nullptr);
    // AssertOrThrow2(getMeshSpec()->getArmatureMapOrdered().size() > 0);

     //Fill identity buffer of skin mats
    int32_t iOrdCount = 0;
    std::vector <mat4> idents;
    for (std::pair<Hash32, std::shared_ptr<Armature>> parm : _pModelNode->getModelSpec()->getArmatureMapOrdered()) {
        for (std::pair<Hash32, std::shared_ptr<BoneSpec>> pbone : *parm.second->getBoneCacheOrdered()) {
            iOrdCount++;
            idents.push_back(mat4::identity());
        }
    }

  //  _pSkinCompute = new GpuComputeSync(Gu::getGraphicsContext());

    _pArmJoints = std::make_shared<ShaderStorageBuffer>(Gu::getGraphicsContext(), sizeof(GpuAnimatedMeshSkinMatrix));
    _pArmJoints->allocate(iOrdCount);
    _pArmJoints->copyDataClientServer(iOrdCount, (void*)idents.data());

    orderBoneNodesForGpu();
}
void MeshNode::orderBoneNodesForGpu() {
    //Retrieve a list of all matrixes as appearing on the GPU - this si crucially ordered.
    //[arm, bone,bone, arm2, bone, bone...]
    //Armature order comes frist
   // int32_t iMaxArmId = -1;
    _vecBoneNodesOrdered.resize(0);
    _vecArmaturesOrdered.resize(0);

    //Build list of armatures, ordered by ID
    for (std::pair<Hash32, std::shared_ptr<Armature>> arm_pair : _pModelNode->getModelSpec()->getArmatureMapOrdered()) {
        for (std::shared_ptr<ArmatureNode> parm_node : _pModelNode->getArmatureNodes()) {
            if (arm_pair.second == parm_node->getSpec()) {
                _vecArmaturesOrdered.push_back(parm_node);
            }
        }
    }

    //Build list of bone nodes, ordered by armature, then bone id
    int32_t iMaxBoneId = -1;
    for (std::shared_ptr<ArmatureNode> parm_node : _vecArmaturesOrdered) {
        iMaxBoneId = -1;//Reset for each armature

        for (std::shared_ptr<BoneNode> bn : parm_node->getBonesOrdered()) {
            _vecBoneNodesOrdered.push_back(bn);

            //Check bone order is sequential.
            if (bn->getBoneSpec()->getBoneId() > iMaxBoneId) {
                iMaxBoneId = bn->getBoneSpec()->getBoneId();
            }
            else {
                BroLogError("'" + getMeshSpec()->getName() + "' : Node, Bone Cache is not ordered properly.");
                Gu::debugBreak();
            }
        }
    }
    //*If we have skin we have to have bone nodes, or lese there was an error somewhere.
    AssertOrThrow2(_vecBoneNodesOrdered.size() > 0);
}
void MeshNode::computeAndDispatchSkin() {
    if (getMeshSpec()->hasSkin()) {
      //  if (!_pSkinCompute->isDispatched() || _pSkinCompute->isComputeComplete()) {

            if (false) {
                printDataToStdout();
            }

            computeSkinFrame();
       // }
    }

}
void MeshNode::update(float delta, std::map<Hash32, std::shared_ptr<Animator>>& pAnimator) {
    BaseNode::update(delta, pAnimator);
}

void MeshNode::computeSkinFrame() {
    Perf::pushPerf();
    Gu::getGraphicsContext()->chkErrDbg();
    copyJointsToGpu();
    Perf::popPerf();
    // dispatchSkinCompute();
}
void MeshNode::copyJointsToGpu() {
    Perf::pushPerf();
    AssertOrThrow2(_pArmJoints != nullptr);
    std::vector<mat4> mats;
    std::vector<t_string> names; //TEST
    mat4 mtmp;
   // static bool copyLocal = false;
    for (std::shared_ptr<BoneNode> bn : _vecBoneNodesOrdered)
    {
        //It's all fucked up because of blender's ordering --skin meshes
        //should not be part of the scene graph.  Really, the bones should be part
        //of the scenegraph.
        // Here we're doing
        //  MESH_LOCAL * BONE * (ARMATURE * MODEL)
        mtmp = getSpec()->getBind() * bn->getLocal();
        if(getParent()){
            mtmp = mtmp * (std::dynamic_pointer_cast<BaseNode>(getParent()))->getLocal();
        }

        mats.push_back(mtmp);
        names.push_back(bn->getBoneSpec()->getName());//TEST
    }
    _pArmJoints->copyDataClientServer(mats.size(), mats.data());
    Perf::popPerf();
}
//void MeshNode::dispatchSkinCompute() {
//    Gu::getGraphicsContext()->chkErrDbg();
//
//    std::shared_ptr<ShaderBase> pSkinShader = Gu::getShaderMaker()->getSkinComputeShader();
//    AssertOrThrow2(pSkinShader != nullptr);
//    pSkinShader->bind();
//
//    AssertOrThrow2(getMeshSpec()->getWeightOffsetsGpu() != nullptr);
//    AssertOrThrow2(getMeshSpec()->getWeightsGpu() != nullptr);
//    AssertOrThrow2(_pArmJoints != nullptr);
//    AssertOrThrow2(getMeshSpec()->getVertsGpu() != nullptr);
//    AssertOrThrow2(_pVaoData->getVbo() != nullptr);
//    /*
//    okay so each GPU has BLOCKs and BINDING POINTS
//    binding poitns are particular to the program.  you specify them with layout(binding=..
//    BLOCKS are specific to the GPU.  only the GPU can specify a BLOCK for you.  And YOU have to map the
//    BINDIGN POINTS to the BLOCKS by calling glShaderSTorageBlockBinding
//    */
//    pSkinShader->bindSsbo(getMeshSpec()->getWeightOffsetsGpu(), "ssInWeightOffsets", 0);
//    //    Gu::checkErrorsDbg();
//    pSkinShader->bindSsbo(getMeshSpec()->getWeightsGpu(), "ssInJointWeights", 1);
//    //   Gu::checkErrorsDbg();
//    pSkinShader->bindSsbo(_pArmJoints, "ssInSkinMatrices", 2);
//    //   Gu::checkErrorsDbg();
//    pSkinShader->bindSsbo(getMeshSpec()->getVertsGpu(), "v_v3n3x2_buf_in", 3);
//    //  Gu::checkErrorsDbg();
//    pSkinShader->bindSsbo(_pVaoData->getVbo(), "v_v3n3x2_buf_out", 4);
//    // Gu::checkErrorsDbg();
//
//    pSkinShader->dispatchCompute(getMeshSpec()->getVertsGpu()->getNumElements(), 1, 1, _pSkinCompute);
//    _pSkinCompute->isComputeComplete();
//    //  Gu::checkErrorsDbg();
//      //while(!){
//      //    int nnn=0;
//      //    nnn++;
//      //}
//
//    if(false){
//        printFragmentsToStdout();
//    }
//}
void MeshNode::getMeshLocalMatrix(mat4& __out_ mat_mesh){
    //debug
    std::shared_ptr<MeshNode> mg = getThis<MeshNode>();
    if (false) {
        //Spec
        //mg->getMeshSpec()->printDataToStdout();
    }
    if (false) {
        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;
        //actual model.
      //  mg->printDataToStdout();
    }
    //debug


    //1/17/18 Node; removed model_world in order to prevent bullshit like having a unique model matrix

    //Add the mesh-local matrix
   // mat_mesh = mg->getLocal();
    if (mg->getMeshSpec()->hasSkin()) {
        int nnn = 0;
        nnn++;
          mat_mesh = mat4::identity();
    }
    if (mg->getSpec()->getParentType() == ParentType::e::Bone) {
        static int n = 0;
        if (n == 0)
            mat_mesh = mg->getLocal();;// * mat_model_world;
        if (n == 1)
            mat_mesh = mat4::identity();;// * mat_model_world;
    }
    else {
        mat_mesh = mg->getLocal();
    }
    //This is a hack due to our using compute shader for skin! really this should all happen all at once by 
    //multiply the bone skin matrices **(see copyJointsToGpu())**.  Either way we'd have to handle skin and
    //static meshes different.  The optimal method would use a skin shader and we could do all this on the draw.
    // mat_mesh = mat4::identity();
    //HACK DEBUG
    //  mat_mesh.setTranslation(0,0,0);
    //  }
    //  else

    //  else {
    //      //For parent type of BONE we treat the mesh as a bone vertex
    //      mat_mesh = mg->getLocal();// * mat_model_world;
    //      //  mat_mesh = mat_model_world;
    //  }
}

void MeshNode::drawDeferred(RenderParams& rp) {
    //Children.
    BaseNode::drawDeferred(rp);
    draw(rp,false);
}
void MeshNode::drawTransparent(RenderParams& rp){
    BaseNode::drawTransparent(rp);
    draw(rp, true);
}
void MeshNode::draw(RenderParams& rp, bool bTransparent) {
    //This is surprisingly a huge thing
    mat4 mat_mesh;
    getMeshLocalMatrix(mat_mesh);

    //New "multiple vertex types" system. 1/12/18
    std::shared_ptr<VertexFormat> meshFmt = getMeshSpec()->getVertexFormat();

    if (_pMaterial == nullptr) {
        _pMaterial = Gu::getModelCache()->getDefaultMaterial();
        //showNoMaterialError();
        //meshFmt = v_v3::getVertexFormat();
    }

    //Bind a new shader based on format.
    std::shared_ptr<ShaderBase> pShader;
    if (_pMaterial && _pMaterial->getEnableTransparency() && bTransparent) {
        pShader = Gu::getShaderMaker()->getGlassShader(meshFmt);
    }
    else {
        pShader = Gu::getShaderMaker()->getDiffuseShader(meshFmt);
    }
    if(pShader==nullptr) {
        BroLogWarnCycle("Could not find shader for mesh '" + (getSpec() ? getSpec()->getName() : "") + "'");
    }
    else {
        pShader->bind();
        rp.setShader(pShader);

        //Camera
        std::shared_ptr<CameraNode> bc = Gu::getCamera();
        rp.getShader()->setCameraUf(bc, &mat_mesh);

        //Pick ID
        uint32_t pickid = getPickId();
        rp.getShader()->setUf("_ufPickId", (void*)&pickid);

        //Skin
        bindSkin(rp.getShader());

        //Material
        if (_pMaterial != nullptr) {
            _pMaterial->bind(rp.getShader(), true, true);
        }

        //Mesh
        rp.setMesh(getThis<MeshNode>());

        rp.draw();
    }
}
void MeshNode::bindSkin(std::shared_ptr<ShaderBase> shader){
    int wc = 0;
    if (getMeshSpec()->hasSkin()) {
        if (false) {
            printDataToStdout();
        }
        shader->bindSsbo(getMeshSpec()->getWeightOffsetsGpu(), "ssInWeightOffsets", 0);
        //    Gu::checkErrorsDbg();
        shader->bindSsbo(getMeshSpec()->getWeightsGpu(), "ssInJointWeights", 1);
        //   Gu::checkErrorsDbg();
        shader->bindSsbo(_pArmJoints, "ssInSkinMatrices", 2);
        wc = 1;
    }
    shader->setUf("_ufWeightCount", (void*)&wc);
}
void MeshNode::drawForward(RenderParams& rp) {
    BaseNode::drawForward(rp);
    if (Gu::getRenderSettings()->getDebug()->getShowNormals()) {

        //Draw Normals
        std::shared_ptr<MeshNode> mg = getThis<MeshNode>();
        mat4 mat_mesh;
        getMeshLocalMatrix(mat_mesh);
        std::shared_ptr<ShaderBase> pShader = Gu::getShaderMaker()->getNormalsShader_v3n3();
        pShader->bind();
        rp.setShader(pShader);
        std::shared_ptr<CameraNode> bc = Gu::getCamera();
        rp.getShader()->setCameraUf(bc, &mat_mesh);
        vec4 vColor(1, 0, 1, 1);
        float fLen = 0.3f;
        vec3 vCamPos = bc->getPos();
        rp.getShader()->setUf("_ufNormalColor", (void*)&vColor, -1, true);
        rp.getShader()->setUf("_ufNormalLength", (void*)&fLen, -1, true);
        rp.getShader()->setUf("_ufCamPos", (void*)&vCamPos, -1, true);

        bindSkin(rp.getShader());

        rp.setMesh(mg);
        rp.draw();
    }
}
void MeshNode::drawShadow(RenderParams& rp){
    mat4 mModel;
    getMeshLocalMatrix(mModel);
    rp.getShader()->setUf("_ufModel", (void*)&mModel);
    bindSkin(rp.getShader());
    
    if(_pMaterial==nullptr) {
        _pMaterial = Gu::getModelCache()->getDefaultMaterial();
       // showNoMaterialError();
    }

    _pMaterial->bind(rp.getShader(), true, false);
    rp.getShader()->draw(getThis<MeshNode>());
}

void MeshNode::calcBoundBox(Box3f& __out_ pBox, const vec3& obPos, float extra_pad)  {
    pBox.genResetLimits();
    if (getSpec()) {
        std::shared_ptr<MeshNode> mn = getThis<MeshNode>();
        if (mn->getMeshSpec()->hasSkin()) {
            //Mesh + skin - compute box by the bone boxes.
            for (std::shared_ptr<BoneNode> bn : mn->getBoneNodesOrdered()) {
                pBox.genExpandByBox(bn->getBoundBoxObject());
            }
            //Make the OBB be the same as the box
            //bone node boxes are already transformed to armature space
            getOBB()->calc(mat4::identity(), &pBox);
        }
        else {
            //Simple mesh.  No skin.
            getOBB()->calc(getLocal(), getSpec()->getBoundBoxObject());
            for (int i = 0; i < 8; ++i) {
                pBox.genExpandByPoint(getOBB()->getVerts()[i]);
            }
        }
    }
    BaseNode::calcBoundBox(pBox, obPos, extra_pad);

}
void MeshNode::showNoMaterialError(){
    //Check that textured meshes actually have texture.  If they don't swap the vertex format for something we can draw
    //if (_pMaterial == nullptr){
    //  //  if(getMeshSpec()->getVertexFormat()->getComponentForUserType(VertexUserType::x2_01) != nullptr) {
    //    BroLogError("Mesh '", getSpec()->getName(), "' did not have a material. All meshes must have a material in the",
    //        "new system. Note: if the object is",
    //        "to be rendered colored, make sure to delete all UV Maps in blender in Data -> UV Maps");
    //  //  }
    //}
}



}//ns Game
