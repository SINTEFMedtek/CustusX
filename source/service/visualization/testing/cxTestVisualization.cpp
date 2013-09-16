#include "cxTestVisualization.h"

#include "sscMesh.h"
#include "sscImage.h"
#include "sscDataMetric.h"
#include "sscPointMetric.h"
#include "cxViewWrapper.h"
#include "sscTypeConversions.h"

TestVisualization::TestVisualization()
{
}

void TestVisualization::setUp()
{

}

void TestVisualization::tearDown()
{

}

void TestVisualization::testSortData()
{
	cx::MeshPtr mesh(new cx::Mesh("mesh1     "));
	cx::ImagePtr image_mr(new cx::Image("image1_mr ", vtkImageDataPtr()));
    image_mr->setModality("MR");
	cx::ImagePtr image_ct(new cx::Image("image1_ct ", vtkImageDataPtr()));
    image_ct->setModality("CT");
	cx::ImagePtr image_us(new cx::Image("image1_us ", vtkImageDataPtr()));
    image_us->setModality("US");
    image_us->setImageType("B-Mode");
	cx::ImagePtr image_usa(new cx::Image("image1_usa", vtkImageDataPtr()));
    image_usa->setModality("US");
    image_usa->setImageType("Angio");
	cx::PointMetricPtr point(new cx::PointMetric("point1    "));

	std::vector<cx::DataPtr> unsorted1;
    unsorted1.push_back(image_us);
    unsorted1.push_back(point);
    unsorted1.push_back(image_mr);
    unsorted1.push_back(mesh);
    unsorted1.push_back(image_usa);
    unsorted1.push_back(image_ct);
	std::vector<cx::DataPtr> unsorted2 = unsorted1;

	std::vector<cx::DataPtr> sorted;
    sorted.push_back(image_ct);
    sorted.push_back(image_mr);
    sorted.push_back(image_us);
    sorted.push_back(image_usa);
    sorted.push_back(mesh);
    sorted.push_back(point);

    // sanity check: should be entirely unsorted at start
    CPPUNIT_ASSERT(unsorted1.size()==sorted.size());
    for (unsigned i=0; i<sorted.size(); ++i)
        CPPUNIT_ASSERT(unsorted1[i]!=sorted[i]);

    std::sort(unsorted1.begin(), unsorted1.end(), &cx::dataTypeSort);

//    std::cout << std::endl;
//    for (unsigned i=0; i<sorted.size(); ++i)
//    {
//        std::cout << "pri\t" << cx::getPriority(sorted[i]) << "\tbase\t" << sorted[i]->getName() << "\tsort:\t" << unsorted1[i]->getName() << std::endl;
//    }

    // check sorting success
    CPPUNIT_ASSERT(unsorted1.size()==sorted.size());
    for (unsigned i=0; i<sorted.size(); ++i)
        CPPUNIT_ASSERT(unsorted1[i]==sorted[i]);

    // test cx::ViewGroupData::addDataSorted()
    cx::ViewGroupData vgData;
    for (unsigned i=0; i<unsorted2.size(); ++i)
        vgData.addDataSorted(unsorted2[i]);
	std::vector<cx::DataPtr> sorted2 = vgData.getData();

//    std::cout << std::endl;
//    std::cout << "sizes: " << sorted.size() << " " << sorted2.size() << std::endl;
//    for (unsigned i=0; i<sorted.size(); ++i)
//    {
//        std::cout << "pri\t" << cx::getPriority(sorted[i]) << "\tbase\t" << sorted[i]->getName()
//                   << "pri\t" << cx::getPriority(sorted2[i]) << "\tsort:\t" << sorted2[i]->getName() << std::endl;
//    }

    // check sorting success
    CPPUNIT_ASSERT(sorted2.size()==sorted.size());
    for (unsigned i=0; i<sorted.size(); ++i)
        CPPUNIT_ASSERT(sorted2[i]==sorted[i]);
}
