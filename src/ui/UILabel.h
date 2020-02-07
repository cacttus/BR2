///**
//*
//*    @file UILabel.h
//*    @date February 3, 2020
//*    @author MetalMario971
//*
//*    © 2020 
//*
//*
//*/
//#pragma once
//#ifndef __UILABEL_15807660013902850557_H__
//#define __UILABEL_15807660013902850557_H__
//
//#include "../ui/UIHeader.h"
//#include "../ui/UIImage.h"
//
//namespace Game {
///**
//*    @class UILabel
//*    @brief
//*
//*/
//class UiGlyph : public UiImage {
//  uint32_t _iChar = 0x20;
//public:
//  static std::shared_ptr<UiGlyph> UiGlyph::create(uint32_t iChar);
//  UiGlyph() {}
//  virtual ~UiGlyph() override {}
//  virtual void performLayout(bool bForce) override;
//  virtual void regenMesh(std::vector<v_GuiVert>& verts, std::vector<v_index32>& inds, Box2f& b2ClipRect) override;
//
//};
//class UiLabel : public UiElement {
//  std::vector<std::shared_ptr<UiGlyph>> _vecGlyphs;
//  std::string _strText = "";
//  std::string _strTextLast = "";
//  std::shared_ptr<MeshNode> _pMesh = nullptr;
//  std::shared_ptr<UiLabelSkin> _pFontInfo = nullptr;
//  bool _bWordWrap = false;
//  void createGlyphs();
//protected:
//  virtual void regenMesh(std::vector<v_GuiVert>& verts, std::vector<v_index32>& inds, Box2f& b2ClipRect) override;
//public:
//  static std::shared_ptr<UiLabel> create(std::string text, std::shared_ptr<UiLabelSkin> inf);
//  UiLabel() {}
//  virtual ~UiLabel() override {}
//  virtual void init() override;
//  std::string getText() { return _strText; }
//  void setText(std::string s);
//  virtual void performLayout(bool bForce) override;
//  void enableWordWrap() { _bWordWrap = true; }
//  virtual void update(std::shared_ptr<Fingers> pInput) override;
//};
//}//ns Game
//
//
//
//#endif
