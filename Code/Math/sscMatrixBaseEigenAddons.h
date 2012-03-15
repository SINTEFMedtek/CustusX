/*
 * sscVector3DEigenAddons.h
 *
 * Plugin extension for the eigen::Vector3d type
 *
 * Usage:
 * #define EIGEN_MATRIXBASE_PLUGIN "sscVector3DEigenAddons.h"
 *
 *  Created on: Apr 10, 2011
 *      Author: christiana
 */

/**
 * return the vector with length 1.
 */
inline PlainObject normal() const
{
	return this->normalized();
}

/**
 * return cartesian length of vector.
 */
inline RealScalar length() const
{
	return this->norm();
}


/**
 * Construct a vector from a string containing 3 whitespace-separated numbers
 */
static PlainObject fromString(const QString& text)
{
	QStringList v = text.split(QRegExp("\\s+"), QString::SkipEmptyParts);
	if (v.size() != 3)
		return PlainObject(0, 0, 0);
	return PlainObject(v[0].toDouble(), v[1].toDouble(), v[2].toDouble());
}

