/* ----------------------------------------------------------------------------------------
Class for build constant tree as linked list of nodes descriptors.
Version with extended nodes nesting level.
At application debug, this class used as tree extended version builder for data emulation.
At real system information show, this class not used.
---------------------------------------------------------------------------------------- */

#pragma once
#ifndef TREECONTROLLEREXT_H
#define TREECONTROLLEREXT_H

#include "TreeController.h"

class TreeControllerExt :
    public TreeController
{
public:
    TreeControllerExt();
    ~TreeControllerExt();
    PTREENODE BuildTree();
};

#endif  // TREECONTROLLEREXT_H
