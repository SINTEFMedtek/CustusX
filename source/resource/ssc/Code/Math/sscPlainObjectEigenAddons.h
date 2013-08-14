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

