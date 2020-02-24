/**
*  @file OpenGLWindow.h
*  @date August 23, 2019
*  @author MetalMario971
*/
#pragma once
#ifndef __OPENGLWINDOW_15665854371549001192_H__
#define __OPENGLWINDOW_15665854371549001192_H__

#include "../base/GraphicsWindow.h"

namespace Game {
/**
*  @class OpenGLWindow
*  @brief
*/
class OpenGLWindow : public GraphicsWindow {
public:
  OpenGLWindow(bool is_main = false);
  virtual ~OpenGLWindow() override;

  virtual void create(t_string title) override;
  virtual void createRenderer();

protected:
  virtual void getDrawableSize(int* w, int* h) override;
  virtual void makeCurrent() override;
  virtual void swapBuffers() override;
};

}//ns Game



#endif
