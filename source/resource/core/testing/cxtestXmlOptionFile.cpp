/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxXmlOptionItem.h"
#include "cxDataLocations.h"

#include <QFile>
#include <QFileInfo>
#include "catch.hpp"
#include "cxTypeConversions.h"


struct XmlOptionFileFixture
{
	XmlOptionFileFixture()
	{
		filename = cx::DataLocations::getRootConfigPath() + "/test.xml";
		value = "myvalue";
		uid = "myuid";
		node = "mynode";

	}
	void remove()
	{
		QFile(filename).remove();
	}
	void open()
	{
		file = cx::XmlOptionFile(filename).descend(node);
	}
	cx::XmlOptionItem item()
	{
		return cx::XmlOptionItem (uid, file.getElement());
	}

	QString filename;
	QString value;
	QString uid;
	QString node;

	cx::XmlOptionFile file;
};


TEST_CASE("XmlOptionFile: Write and read value in-memory", "[unit][resource][core]")
{
	cx::DataLocations::setTestMode();
	XmlOptionFileFixture file1;
	file1.remove();
	file1.open();

	cx::XmlOptionItem item = file1.item();
	CHECK(item.readValue("none") == "none"); // item doesnt exist at start
	item.writeValue(file1.value);

	cx::XmlOptionItem item2 = file1.item();
	CHECK(item2.readValue("none") == file1.value); //  second item contains same value as first
}

TEST_CASE("XmlOptionFile: Multiple instances share data", "[unit][resource][core]")
{
	cx::DataLocations::setTestMode();
	XmlOptionFileFixture file1;
	file1.remove();
	file1.open();

	cx::XmlOptionItem item = file1.item();
	CHECK(item.readValue("none") == "none"); // item doesnt exist at start
	item.writeValue(file1.value);

	XmlOptionFileFixture file2;
	file2.open();

	cx::XmlOptionItem item2 = file2.item();
	CHECK(item2.readValue("none") == file2.value); //  second item contains same value as first
}

TEST_CASE("XmlOptionFile: Writing to file using one instance, then loading using another", "[unit][resource][core]")
{
	cx::DataLocations::setTestMode();

	// write using file1
	{
		XmlOptionFileFixture file1;
		file1.remove();
		file1.open();

		cx::XmlOptionItem item = file1.item();
		CHECK(item.readValue("none") == "none"); // item doesnt exist at start
		item.writeValue(file1.value);

		file1.file.save();
	}

	// new scope, file1 is out of scope
	{
		XmlOptionFileFixture file3;
		file3.open();

		cx::XmlOptionItem item3 = file3.item();
		CHECK(item3.readValue("none") == file3.value); //  second item contains same value as first
	}
}

TEST_CASE("XmlOptionFile: Write and read QVariant-int", "[unit][resource][core]")
{
	cx::DataLocations::setTestMode();
	XmlOptionFileFixture file1;
	file1.remove();
	file1.open();

	int value = 42;

	cx::XmlOptionItem item = file1.item();
//	CHECK(item.readValue("none") == "none"); // item doesnt exist at start
	item.writeVariant(QVariant::fromValue<int>(value));

	cx::XmlOptionItem item2 = file1.item();
	CHECK(item2.readVariant("none").toInt() == value); //  second item contains same value as first
}

TEST_CASE("XmlOptionFile: Write and read QVariant-QColor", "[unit][resource][core]")
{
	cx::DataLocations::setTestMode();
	XmlOptionFileFixture file1;
	file1.remove();
	file1.open();

	QColor value("red");

	cx::XmlOptionItem item = file1.item();
//	CHECK(item.readValue("none") == "none"); // item doesnt exist at start
	item.writeVariant(QVariant::fromValue<QColor>(value));

	cx::XmlOptionItem item2 = file1.item();
	CHECK(item2.readVariant("none").value<QColor>() == value); //  second item contains same value as first
}
