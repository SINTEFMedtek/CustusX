#ifndef CXTESTCGEOTESTFUNCTIONS_H
#define CXTESTCGEOTESTFUNCTIONS_H

#include "catch.hpp"

#include <QByteArray>
#include <QIODevice>
#include <QDataStream>

namespace cxtest
{

void checkCgeoByteArray(QByteArray byteArray)
{
	qint32 cgeoFirstValue, cgeo2nd, cgeo3rd;
	QDataStream stream(&byteArray, QIODevice::ReadOnly);
	stream.setByteOrder(QDataStream::LittleEndian);
	//	stream.setVersion(QDataStream::Qt_5_9);
	stream >> cgeoFirstValue;
	stream >> cgeo2nd;
	stream >> cgeo3rd;

	//.cgeo file should start with 12072001, then 0 and 1. All 32 bit int.
	CHECK(cgeoFirstValue == 12072001);
	CHECK(cgeo2nd == 0);
	CHECK(cgeo3rd == 1);
}

}//cxtest

#endif // CXTESTCGEOTESTFUNCTIONS_H
