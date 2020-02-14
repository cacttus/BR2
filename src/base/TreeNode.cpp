#include "../base/TreeNode.h"
#include "../base/Exception.h"
#include "../base/Gu.h"   

namespace BR2 {
TreeNode::TreeNode() :
  _pParent(NULL)
  , _iRecursionStamp(0)
  , _bUnloading(0) {
  _mapChildren = std::make_unique<std::vector<std::shared_ptr<TreeNode>>>();
}
TreeNode::~TreeNode() {
  //Remove removes from parent
 //So we can't call unlinkAllChildren because teh weak_ptr is destroyed in the dtor, however
  //becasue it's reference counted we can just not call it and hope that it destroys all child
  //references
  // unlinkAllChildren();
  //_mapChildren->clear();
  //DEL_MEM(_mapChildren);
}
void TreeNode::attachToParent(std::shared_ptr<TreeNode> pParent)  // calls insert()
{
  AssertOrThrow2(pParent != nullptr);
  pParent->insert(getThis<TreeNode>());
}
bool TreeNode::detachFromParent()  //calls remove()
{
  if (getParent() == nullptr) {
    return false;
  }
  getParent()->remove(getThis<TreeNode>());
  return true;
}
std::shared_ptr<TreeNode> TreeNode::attachChild(std::shared_ptr<TreeNode> pChild)  // calls insert()
{
  AssertOrThrow2(pChild != nullptr);
  pChild->detachFromParent();
  return this->insert(pChild);
}
bool TreeNode::detachChild(std::shared_ptr<TreeNode> pChild)  //calls remove()
{
  AssertOrThrow2(pChild != nullptr);
  return this->remove(pChild);
}
void TreeNode::internalAddChildNode(std::shared_ptr<TreeNode> pTreeNode) {
  //This is private
  AssertOrThrow2(pTreeNode != nullptr);
  _mapChildren->push_back(pTreeNode);
}
void TreeNode::internalRemoveChildNode(std::shared_ptr<TreeNode> pTreeNode) {
  //This is private
  AssertOrThrow2(pTreeNode != nullptr);
  for (size_t i = 0; i < _mapChildren->size(); ++i) {
    std::shared_ptr<TreeNode> c = _mapChildren->at(i);
    if (c == pTreeNode) {
      _mapChildren->erase(_mapChildren->begin() + i);
      return;
    }
  }
  //Failure
  Gu::debugBreak();

}
//void TreeNode::unlinkAllChildren(bool bRecursive)
//{
//    iterateDepthFirst<TreeNode>([](std::shared_ptr<TreeNode> tx) {
//        if (tx->_pParent != NULL)
//            tx->_pParent->detachChild(tx);
//        return true;
//    });
//}

void TreeNode::flattenBreadthFirst(TreeNode::NodeList& ret) {
  // - Gets a list of all items in breadth first
  // traversal.  This will give us a list of all dependencies first
  // and the exes / items last.
  if (_mapChildren->size() == 0) {
    return;
  }

  for (std::shared_ptr<TreeNode> item : (*_mapChildren)) {
    getBreadthFirstList_r(item, ret);
  }

}
//This returns the node you passed in, kind of pointlesss return value
std::shared_ptr<TreeNode> TreeNode::insert(std::shared_ptr<TreeNode> txChild, std::shared_ptr<TreeNode> txParent) {
  // If parent is not null then parent is assumed to be a sub-node of this node.
  // child cannot be null.

  VerifyOrThrow(txChild != shared_from_this(), "[ItemTree] Tried to insert node on itself (duplicate error).");
  VerifyOrThrow(txChild != nullptr, "[ItemTree] Child was null trying to insert tree node.");
  std::shared_ptr<TreeNode> tn = find(txChild);
  VerifyOrThrow(tn == nullptr, "[ItemTree] Tried to insert a duplicate node.");

  if (txParent == nullptr) {
    txParent = getThis<TreeNode>();
  }
  else {
    std::shared_ptr<TreeNode> pFound = find(txParent);
    //We must make sure that parent is a subnode of this node.
    VerifyOrThrow(pFound != nullptr, "[ItemTree] Failed to find parent node while inserting child.");
  }
  //Remove child from parent.
  if (txChild->_pParent != nullptr) {
    txChild->_pParent->remove(txChild);
  }

  // Add child to new parent.
  txChild->_pParent = txParent;
  txParent->internalAddChildNode(txChild);

  afterChildInserted(txChild);

  return txChild;
}
/**
*  @fn remove
*  @brief Remove a node from the tree
*  @return true if the node was found, false if it wasn't
*  if blnLinkChildren is true we will link the children of the
*  @param node - if null removes this node from its parent
*  removed node to the removed node's existing parent (or Root if it has none)
*  * bRecursive - when true we will attempt to search for the node hten remove it.
*/
bool TreeNode::remove(std::shared_ptr<TreeNode> node, bool blnSplice, bool bImmediateNodeOnly) {
  if (node == nullptr) {
    node = getThis<TreeNode>();
  }
  std::shared_ptr<TreeNode> pParent = node->_pParent;

  if (find(node) == nullptr) {
    return false;
  }
  //if(bImmediateNodeOnly==true && node != nullptr && node!=this)
  //    VerifyOrThrow(pParent==this, "[std::shared_ptr<TreeNode>] Node parent was not an immediate parent for removal.");

  if (pParent != nullptr) {
    pParent->internalRemoveChildNode(node);
  }
  node->_pParent = NULL;

  // Splice children with other parent.  This effectively removes the node but keeps the tree.
  if (blnSplice == true) {
    for (std::shared_ptr<TreeNode> c : *(node->_mapChildren)) {
      pParent->internalAddChildNode(c);
      c->_pParent = pParent;
    }
    node->_mapChildren->clear();
  }

  //Callback for subclasses.
  afterChildRemoved(node);

  return true;
}
//finds a descendent child from the given node.
std::shared_ptr<TreeNode> TreeNode::find(std::shared_ptr<TreeNode> bt) {
  std::shared_ptr<TreeNode> found = NULL;

  if (bt == nullptr) {
    BroThrowException("[ItemTree] Inserting, Child was null");
  }
  if (bt == getThis<TreeNode>()) {
    return getThis<TreeNode>();
  }

  for (std::shared_ptr<TreeNode> item : *(_mapChildren)) {
    find_r(bt, item, found);
  }

  return found;
}
size_t TreeNode::getNodeCountParentOnly() {
  AssertOrThrow2(_mapChildren != NULL);
  return _mapChildren->size();
}
size_t TreeNode::getNodeCountHierarchy() {
  size_t ret = 0;
  AssertOrThrow2(_mapChildren != NULL);
  getNodeCount_r(_mapChildren, ret);
  return ret;
}
void TreeNode::getNodeCount_r(std::unique_ptr<NodeList>& vecNodes, size_t& count) {
  for (std::shared_ptr<TreeNode> n : *(vecNodes)) {
    if (n->_mapChildren->size() > 0) {
      getNodeCount_r(n->_mapChildren, count);
    }
    //if ((predicate == nullptr) || (predicate(n->getItem()) == true))
    count++;
  }
}
void TreeNode::getBreadthFirstList_r(std::shared_ptr<TreeNode> parent, NodeList& outList) {
  for (std::shared_ptr<TreeNode> child : *(parent->_mapChildren)) {
    getBreadthFirstList_r(child, outList);
  }
  outList.push_back(parent);
}
void TreeNode::find_r(std::shared_ptr<TreeNode> nodeToFind, std::shared_ptr<TreeNode> parent, std::shared_ptr<TreeNode>& found) {
  if (found != nullptr)
    return;

  if (parent == nodeToFind) {
    found = parent;
    return;
  }

  for (std::shared_ptr<TreeNode> n : *(parent->_mapChildren)) {
    if (found != nullptr)
      break;

    find_r(nodeToFind, n, found);
  }

}
bool TreeNode::getIsLeaf() const {
  return _mapChildren->size() == 0;
}
void TreeNode::addNullChildren(int32_t count) {
  //Add null children to simulate a rigidly defined structure (quad/octree) with default NULL nodes.

  for (int32_t i = 0; i < count; ++i) {
    _mapChildren->push_back(NULL);
  }
}

//void TreeNode::delinkTree()
//{
//    iterateBreadthFirst<TreeNode>([](std::shared_ptr<TreeNode> pt) {
//        if (pt != NULL)
//            pt->detachFromParent();
//        return true;
//    });
//}



}