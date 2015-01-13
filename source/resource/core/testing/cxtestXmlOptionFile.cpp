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
		filename = cx::DataLocations::getXmlSettingsFile();
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
