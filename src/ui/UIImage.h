///**
//*
//*    @file UIImage.h
//*    @date February 3, 2020
//*    @author Derek Page
//*
//*    © 2020 
//*
//*
//*/
//#pragma once
//#ifndef __UIIMAGE_1580766352512917830_H__
//#define __UIIMAGE_1580766352512917830_H__
//
//namespace Game {
//class UiImage : public UiElement {
//  // std::shared_ptr<MeshNode> _pMesh = nullptr;
//  std::shared_ptr<UiTex> _pTexture = nullptr;
//  UiImageSizeMode::e _eSizeModeX = UiImageSizeMode::e::Expand;//tile = GL_REPEAT, Clamp = GL_CLAMP, Expand - expand tex coords.
//  UiImageSizeMode::e _eSizeModeY = UiImageSizeMode::e::Expand;//tile = GL_REPEAT, Clamp = GL_CLAMP, Expand - expand tex coords.
//  float _fTileWidthPx = 32;//Determines image size  for tiling
//  float _fTileHeightPx = 32; //Determines image size for tiling
//  Box2f _q2Tex = Box2f(vec2(0, 0), vec2(1, 1));
//  uint32_t _iPickId = 0;
//
//protected:
//  virtual float getAutoWidth()  override;
//  virtual float getAutoHeight() override;
//  virtual void regenMesh(std::vector<v_GuiVert>& verts, std::vector<v_index32>& inds, Box2f& b2ClipRect) override;
//  //virtual void setQuad() override;
//
//public:
//  UiImage();
//  virtual ~UiImage() override;
//  virtual void init() override;
//
//  // static std::shared_ptr<UiImage> create(t_string path, UiImageSizeMode::e eSizeX, UiImageSizeMode::e eSizeY);
//  static std::shared_ptr<UiImage> create(std::shared_ptr<UiTex> tex, UiImageSizeMode::e eSizeX, UiImageSizeMode::e eSizeY);
//  static std::shared_ptr<UiImage> create(std::shared_ptr<UiTex> tex, UiImageSizeMode::e eSizeX, UiImageSizeMode::e eSizeY, float fWidthPx, float fHeightPx);
//  virtual void performLayout(bool bForce) override;
//  void getQuadVerts(std::vector<v_GuiVert>& verts, std::vector<v_index32>& inds, Box2f& b2ClipRect);
//  //  std::shared_ptr<MeshNode> getMesh() { return _pMesh; }
//  void setTexture(std::shared_ptr<UiTex> ps) { _pTexture = ps; }
//  virtual bool getPickable() override { return true; }
//  void setSizeMode(UiImageSizeMode::e eSizeX, UiImageSizeMode::e eSizeY);
//  void setWrapMode(TexWrap::e u, TexWrap::e v);
//  virtual bool pick(std::shared_ptr<Fingers> fingers) override;
//  Box2f& getTexs() { return  _q2Tex; }
//  void regenMeshExposed(std::vector<v_GuiVert>& verts, std::vector<v_index32>& inds, Box2f& b2ClipRect) { regenMesh(verts, inds, b2ClipRect); }
//
//};
//
//}//ns Game
//
//
//
//#endif
