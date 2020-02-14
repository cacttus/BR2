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
#include "../world/Component.h"

namespace BR2 {
/**
*  @class CSharpScript
*  @brief A minimally functional C Sharp script.
*/
class CSharpScript : public Component {
public:
  CSharpScript(std::shared_ptr<WorldObject> myObj);
  virtual ~CSharpScript() override;

protected:
  std::shared_ptr<WorldObject> getWorldObject() { return _worldObject; }
private:
  std::shared_ptr<WorldObject> _worldObject = nullptr;

};

}//ns Game



#endif
