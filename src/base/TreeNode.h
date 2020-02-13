/**
*  @file TreeNode.h
*  @date August 6, 2015
*  @author MetalMario971
*/
#pragma once
#ifndef __TREENODE_2292922995327663144523481_H__
#define __TREENODE_2292922995327663144523481_H__

#include "../base/BaseHeader.h"
#include "../gfx/GfxHeader.h"

namespace BR2 {
/**
*  @class TreeNode
*  @brief Represents generic node of a DAG tree. *Not sorted
*/
class TreeNode : public Drawable {
public:
  //DOn't change this to MAP because the vecotr must be ordered by index
  typedef std::vector<std::shared_ptr<TreeNode>> NodeList;
  typedef std::function<bool(std::shared_ptr<TreeNode>)> NodeLambda;

public:
  TreeNode();
  virtual ~TreeNode()  override;

  template < class Tx >
  bool iterateBreadthFirst(std::function<bool(std::shared_ptr<Tx>)> fn);
  template < class Tx >
  bool iterateDepthFirst(std::function<bool(std::shared_ptr<Tx>)> fn);
  template < class TBase, class TCopy >
  std::shared_ptr<TCopy> duplicateBreadthFirst(std::function<std::shared_ptr<TCopy>(std::shared_ptr<TBase>)> fn);
  void flattenBreadthFirst(NodeList& outVec);
  std::shared_ptr<TreeNode> find(std::shared_ptr<TreeNode> bt);
  virtual std::shared_ptr<TreeNode> attachChild(std::shared_ptr<TreeNode> pChild);  // calls insert()
  bool detachChild(std::shared_ptr<TreeNode> pChild);  //calls remove()
  bool detachFromParent();  //calls remove()
  size_t getNodeCountHierarchy();
  size_t getNodeCountParentOnly();
  bool getHasParent() { return _pParent != nullptr; }
  std::shared_ptr<TreeNode> getParent() { return _pParent; }
  std::unique_ptr<NodeList>& getChildren() { return _mapChildren; }
  virtual bool getIsLeaf() const;
  virtual bool canDelete() { return true; }    //override to check whether the node can be deleted.

protected:
  void addNullChildren(int32_t count);
  virtual void afterChildInserted(std::shared_ptr<TreeNode>) {}    //Override thsi - Called after a node is appended.
  virtual void afterChildRemoved(std::shared_ptr<TreeNode>) {}    //Override this - Called after a node is appended.
  void attachToParent(std::shared_ptr<TreeNode> pParent);  // calls insert()

private:
  int32_t _iRecursionStamp = 0;
  std::shared_ptr<TreeNode> _pParent = nullptr;
  std::unique_ptr<NodeList> _mapChildren = nullptr;
  bool _bUnloading = false;

  void getNodeCount_r(std::unique_ptr<NodeList>& vecNodes, size_t& count);
  void getBreadthFirstList_r(std::shared_ptr<TreeNode> parent, NodeList& outList);
  void find_r(std::shared_ptr<TreeNode> bt, std::shared_ptr<TreeNode> parent, std::shared_ptr<TreeNode>& found);
  void internalAddChildNode(std::shared_ptr<TreeNode> pTreeNode);
  void internalRemoveChildNode(std::shared_ptr<TreeNode> pTreeNode);

  //protected in favor of attach()  etach()
  std::shared_ptr<TreeNode> insert(std::shared_ptr<TreeNode> txChild, std::shared_ptr<TreeNode> txParent = NULL);
  bool remove(std::shared_ptr<TreeNode> node = NULL, bool blnSplice = false, bool bImmediateNodeOnly = true);// Return true if the node was removed
};


template < class Tx >
bool TreeNode::iterateBreadthFirst(std::function<bool(std::shared_ptr<Tx>)> fn) {
  //**Return FALSE to stop iterating**
  //Same as other lambda, but instead we use a temp1late and try to cast it
  // when iterating so we don't have to repeat that code.
  std::shared_ptr<TreeNode> pTreeNode = shared_from_this();
  std::shared_ptr<Tx> pt = std::dynamic_pointer_cast<Tx>(pTreeNode);
  //We need the other type here.

//Tx pt = DirectCast(pTreeNode, Tx);
  if (pt == nullptr) {
    return true;
  }

  if (fn(pt) == false) {
    return false;
  }

  for (std::shared_ptr<TreeNode> r : (*_mapChildren)) {
    if (r->iterateBreadthFirst<Tx>(fn) == false) {
      return false;
    }
  }

  return true;
}
template < class Tx >
bool TreeNode::iterateDepthFirst(std::function<bool(std::shared_ptr<Tx>)> fn) {
  //**Return FALSE to stop iterating**

  //Same as other lambda, but instead we use a temp1late and try to cast it
  // when iterating so we don't have to repeat that code.
  //Tx pt = DirectCast(this, Tx);

  std::shared_ptr<TreeNode> trThis = getThis<TreeNode>();

  //TODO: figure out a way to reference this.  Or else do some kind of reference controlled shit under the hood.
  std::shared_ptr<Tx> pt = std::dynamic_pointer_cast<Tx>(trThis);
  if (pt == nullptr) {
    return true;
  }

  for (std::shared_ptr<TreeNode> r : (*_mapChildren)) {
    if (r->iterateDepthFirst<Tx>(fn) == false) {
      return false;
    }
  }

  if (fn(pt) == false) {
    return false;
  }
  return true;
}
/**
*    @fn duplicateBreadthFirst()
*    @brief Duplicate a tree structure.
*    @details This is a bit confusing:
*    TBase is the type of node we're copying.
*    TCopy is the type of node being copied to.
*    This basically only copies the Parent/Child structure.  Your lambda
*    (fn) should copy the required node information from TBase to TCopy and return TCopy.
*    If you return NULL from fn() the copy process stops for that node.  This effectively
*    prunes nodes in the resulting structure.
*/
template < class TBase, class TCopy>
std::shared_ptr<TCopy> TreeNode::duplicateBreadthFirst(std::function<std::shared_ptr<TCopy>(std::shared_ptr<TBase>)> fn) {
  std::shared_ptr<TreeNode> trThis = this;

  TBase* pt = std::make_shared<TBase*>(trThis);
  if (pt == nullptr) {
    return nullptr;
  }

  //fn should return a copy of a node of type TCopy
  TCopy* pParentCopy = fn(pt);

  if (pParentCopy == nullptr) {
    return nullptr;
  }

  for (std::shared_ptr<TreeNode> r : (*_mapChildren)) {
    TCopy* pChildCopy = r->duplicateBreadthFirst<TBase, TCopy>(fn);

    pParentCopy->attachChild((std::shared_ptr<TreeNode>)pChildCopy);
  }

  return pParentCopy;
}


}//ns game



#endif
