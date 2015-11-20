/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#ifndef CXTREENODE_H
#define CXTREENODE_H

#include <vector>
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <QString>
#include <QObject>
#include "cxForwardDeclarations.h"

namespace cx
{

class TreeNode;
typedef boost::weak_ptr<TreeNode> TreeNodeWeakPtr;
typedef boost::shared_ptr<TreeNode> TreeNodePtr;
typedef boost::weak_ptr<class TreeRepository> TreeRepositoryWeakPtr;
typedef boost::shared_ptr<class TreeRepository> TreeRepositoryPtr;


class TreeNode : public QObject
{
  Q_OBJECT
public:
	virtual ~TreeNode() {}
	virtual std::vector<TreeNodePtr> getChildren() const = 0;
	virtual QString getUid() const = 0;
	virtual QString getName() const = 0;
//	virtual QString getType() const = 0;
	virtual TreeNodePtr getParent() const = 0;
	virtual void activate() {}
//	virtual void getIcon() const = 0;
//	virtual bool getViewGroupVisibility(int index) const = 0;
//	virtual void setViewGroupVisibility(int index, bool value) = 0;
//	virtual bool visible() const = 0; // if need be, might get away simply populating from a root node
//	QWidget* getPropertiesWidget() const = 0; // later

signals:
  void changed();
};

} // namespace cx

#endif // CXTREENODE_H
