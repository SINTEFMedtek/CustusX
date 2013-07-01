/*
 * Branch.cpp
 *
 *  Created on: May 30, 2013
 *      Author: ehofstad
 */

#include "Branch.h"

namespace cx
{

Branch::Branch()
{
	// TODO Auto-generated constructor stub

}

void Branch::setPositions(Eigen::MatrixXd pos)
{
	positions = pos;
}

Eigen::MatrixXd Branch::getPositions()
{
	return positions;
}

void Branch::setOrientations(Eigen::MatrixXd orient)
{
	orientations = orient;
}

Eigen::MatrixXd Branch::getOrientations()
{
	return orientations;
}

void Branch::addChildBranch(Branch* child)
{
	childBranches.push_back(child);
}

void Branch::setChildBranches(branchVector children)
{
	childBranches = children;
}

void Branch::deleteChildBranches()
{
	childBranches.clear();
}

branchVector Branch::getChildBranches()
{
	return childBranches;
}

void Branch::setParentBranch(Branch* parent)
{
	parentBranch = parent;
}
Branch* Branch::getParentBranch()
{
	return parentBranch;
}


Branch::~Branch()
{

}

}//namespace cx
