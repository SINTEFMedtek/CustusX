/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

/*
 * sscPlainObjectEigenAddons.h
 *
 * Plugin extension for the eigen::PlainObject type
 *
 *
 *  Created on: Apr 10, 2011
 *      Author: christiana
 */

Scalar* begin()
{
	return this->data();
}

const Scalar* begin() const
{
	return this->data();
}

Scalar* end()
{
	return this->data() + this->size();
}

const Scalar* end() const
{
	return this->data() + this->size();
}

