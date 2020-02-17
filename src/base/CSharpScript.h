/**
*  @file CSharpScript.h
*  @date August 23, 2019
*  @author MetalMario971
*/
#pragma once
#ifndef __CSHARPSCRIPT_15665746341154937548_H__
#define __CSHARPSCRIPT_15665746341154937548_H__

#include "../base/BaseHeader.h"
#include "../world/WorldHeader.h"
#include "../base/Component.h"

namespace BR2 {
/**
*  @class CSharpScript
*  @brief A minimally functional C Sharp script.
*/
class CSharpScript : public Component {
public:
  CSharpScript(std::shared_ptr<WorldObject> myObj);
  virtual ~CSharpScript() override;

  virtual void start() = 0;
  virtual void update(float delta) = 0;

};

}//ns BR2



#endif
