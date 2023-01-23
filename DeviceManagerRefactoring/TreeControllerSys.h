/* ----------------------------------------------------------------------------------------
Class for scan system information by WinAPI.
This class builds system information tree as linked list of nodes descriptors.
At application debug, this class not used.
At real system information show, this is child class of TreeController class.
---------------------------------------------------------------------------------------- */

#pragma once
#ifndef TREECONTROLLERSYS_H
#define TREECONTROLLERSYS_H

#include "TreeController.h"
#include "Enumerator.h"

#define SYSTEM_TREE_MEMORY_MAX 1024*1024*2

class TreeControllerSys :
    public TreeController
{
public:
    TreeControllerSys();
    ~TreeControllerSys();
    PTREENODE BuildTree();
    void ReleaseTree();
private:
    static LPCSTR MAIN_SYSTEM_NAME;
    static int MAIN_SYSTEM_ICON_INDEX;
    static GROUPSORT sortControl[];
    static const UINT SORT_CONTROL_LENGTH;
    static LPSTR pEnumBase;
};

#endif  // TREECONTROLLERSYS_H