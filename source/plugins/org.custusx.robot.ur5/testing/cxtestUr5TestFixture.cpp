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

#include "cxtestUr5TestFixture.h"

#include <QFile>
#include <QDir>
#include <QTextStream>

namespace cxtest
{

Ur5TestFixture::Ur5TestFixture() :
    mUr5Robot(),
    mUr5Connection(),
    mTestDataFolderName("/CX/source/plugins/org.custusx.robot.ur5/testing/testdata/")
{

}

QByteArray Ur5TestFixture::getByteArrayFromTxt(QString filename)
{
    QString fullPath = this->getTestDataFolderPath() + filename;

    if(!(fullPath.isEmpty()))
    {
        QFile file(fullPath);
        if (!file.open(QIODevice::ReadOnly)) return QByteArray();
        return QByteArray::fromHex(file.readAll());
    }
    else
    {
        return QByteArray(0);
    }
}

void Ur5TestFixture::printMatrix(Eigen::MatrixXd matrix)
{
    for(int i=0;i<matrix.rows(); i++)
    {
        for(int j=0;j<matrix.cols(); j++)
        {
            std::cout << matrix(i,j) << "\t";
        }
        std::cout << std::endl;
    }
}

QString Ur5TestFixture::getSourcePath()
{
    QDir dir = QDir::current();
    dir.cdUp();
    dir.cdUp();

    return dir.path();
}

QString Ur5TestFixture::getTestDataFolderPath()
{
    return (this->getSourcePath()+mTestDataFolderName);
}

std::vector<cx::Transform3D> Ur5TestFixture::getMatrixVectorFromFile(QString filename)
{
    QString fullPath = this->getTestDataFolderPath() + filename;
    QFile inputFile(fullPath);
    std::vector<cx::Transform3D> matrices;

    if (inputFile.open(QIODevice::ReadOnly))
    {
        QTextStream  textFile(&inputFile);
        cx::Transform3D matrix = cx::Transform3D::Identity();

        int i = 0, j = 0;

        while ( !textFile.atEnd() )
        {
            double value;
            textFile >> value;
            if(j<3)
            {
                matrix(i,j) = value;
                j++;
            }
            else if(i<3)
            {
                matrix(i,j) = value;
                j = 0;
                i++;
            }
            else
            {
                matrices.push_back(matrix);
                matrix = cx::Transform3D::Identity();
                j = 0;
                i = 0;
            }
        }
    }
    inputFile.close();

    return matrices;
}

void Ur5TestFixture::printMatrices(std::vector<cx::Transform3D> matrices)
{
    for(int i=0; i<matrices.size(); i++)
    {
        std::cout << matrices.at(i) << std::endl << std::endl;
    }
}

} //cxtest
