/**
*
*    @file HierarchyString.h
*    @date March 26, 2012
*    @author Derek Page
*
*    © 2012 
*
*
*/
#pragma once
#pragma once
#ifndef __HIERARCHYSTRING_25178498830374329924750_H__
#define __HIERARCHYSTRING_25178498830374329924750_H__

#include "../base/BaseHeader.h"

namespace Game {;
/**
*    @class HierarchyString
*    @brief A string "flat file" hierarchy.  Sort of like an L system string it parses
*    data between nodes demarcated by (..) with values separated by commas
*    and push and pop operations are demarcated by [...]
*   Generally, all hierarchy strings look like:
*       (Node_Param1, Node_Param2,...,NodeParamn)[(NodeParam...n)
*
*   For Joints:
*        (root,52)[(r_hip,53)[(r_upper_leg,54)[(r_lower_leg,55)[(r_foot,56)]]]][(l_hip,60)[(l_upper_leg,61)[(l_lower_leg,62)[(l_foot,63)]]]][(pelvis,72)[(chest,73)[(r_shoulder,77)[(r_upper_arm,78)[(r_lower_arm,83)[(r_hand,84)]]]][(l_shoulder,86)[(l_upper_arm,87)[(l_lower_arm,88)[(l_hand,89)]]]][(neck,74)[(head,75)]]]]
*/
template <class Tx>
class HierarchyString : public VirtualMemory {
private:
    Tx _parse( int32_t& n, Tx parent, bool bThrowIfDupeFound);

    std::vector<std::string> _vecParms;

protected:
	std::string _h_str;
    std::vector<Tx> _vecTx;
    bool tryGetParam(int32_t pId, std::string& __out_ parm);
public:

    HierarchyString(std::string h_str) { _h_str = h_str; }
    OVERRIDES ~HierarchyString() OVERRIDE
    {
        //Not sure if we keep the joints or what.
        //DELETE_VECTOR_ELEMENTS(_vecTx);
        _vecTx.resize(0);
    }

    Tx _pRoot;
    void parse(bool bThrowIfDuplicateFound=FALSE);
    
    //TODO: va_list (i'm lazy)
    //OVERRIDE this function.  The parameters are whatever you put in the node (...) separated by ',' commas. 
    MUST_OVERRIDE Tx createNode()=0;
    //OVERRIDE
    MUST_OVERRIDE void addNode(Tx parent, Tx child)=0;
    
    // not implemented by default.
    virtual bool getIsDupe(Tx j1) { BroThrowNotImplementedException(); return FALSE; }    
};
template < class Tx >
bool HierarchyString<Tx>::tryGetParam(int32_t parmId, std::string& __out_ parm)
{
    parm = "";
	if (parmId >= _vecParms.count()) {
		return false;
	}
    parm = _vecParms[parmId];
    return true;
}
/**
*    @fn parse()
*    @brief Call this to parse the hierarchy.
*
*/
template < class Tx >
void HierarchyString<Tx>::parse(bool bThrowIfDuplicateFound) {
    int32_t n=0;
    _parse(n,0,bThrowIfDuplicateFound);
}

//param: n = 
// - Inner parse function - WARNING: recursive
template < class Tx >
Tx HierarchyString<Tx>::_parse( int32_t& n, Tx parent, bool bThrowIfDupeFound)
{
    t_char c;
    bool in_tok = false;    // - We are in a node definition token. Demarcated by (...)
    int32_t node_part;        // - Which part of the node parameters we are parsing
    //t_string p[16];            // - The node parameters
    Tx pLastNode=0;        // - Need this for when we push.

    while(n<((int32_t)_h_str.length()))
    {
        c=_h_str[n];
        if(c=='[')
        {
            n++;
            //push
            if(pLastNode.isNull())
            {
                BroThrowException("Could not parse hierarchy. Child node was null."); //error
            }
            _parse(n,pLastNode,bThrowIfDupeFound);
        }
        else if(c==']')
        {
            //pop
            break;
        }
        else if(c=='(')
        {
            //begin node
            if(in_tok==(bool)true) 
                BroThrowException("Could not parse node: there was an error in the hierarchy string[1]."); //error
                
            in_tok=true;
            node_part=0;    
            _vecParms.clear();

        }
        else if(c==')')
        {
            //end node
            if(in_tok==false) 
                BroThrowException("Could not parse node: there was an error in the hierarchy string[2]."); //error
                
            in_tok=false;
            
            Tx new_node = pLastNode = createNode();

            if(bThrowIfDupeFound==TRUE)
            {
                if(getIsDupe(new_node)==TRUE)
                {
                    BroThrowException("[Hierarchy string] Duplicate item found in hierarchy string.");
                }
            }

            //add to list.
            _vecTx.add(new_node);

            if(parent.isNull())
            {
                parent = _pRoot = new_node;
                addNode(0,parent);
            }
            else
                addNode(parent,new_node);
        }
        else if(in_tok)
        {
            if(c==',')
            {
                node_part++;
            }
            else
            {
                if(node_part>=_vecParms.count())
                    _vecParms.add("");
                _vecParms[node_part]+=c;
            }
        }

        n++;

    }

    return _pRoot;
}

}//ns game



#endif
