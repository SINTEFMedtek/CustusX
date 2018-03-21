/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "catch.hpp"
#include "cxLogicManager.h"
#include "cxPluginFramework.h"

#include "cxPatientModelService.h"
#include "cxRegistrationService.h"
#include "cxRegistrationTransform.h"
#include "cxRegServices.h"
#include "cxImage.h"
#include "cxTransform3D.h"
#include "cxVector3D.h"
#include "cxLandmark.h"

#include "vtkImageData.h"
#include "vtkMatrix4x4.h"

#include "cxManualImage2ImageRegistrationWidget.h"


namespace cxtest
{

typedef boost::shared_ptr<class cx::ManualImage2ImageRegistrationWidget> registrationWidgetPtr;
void init()
{
    cx::LogicManager::initialize();
}

bool checkRegistrationHistoryEmpty(cx::RegistrationServicePtr regService,
                                   cx::PatientModelServicePtr patientModel)
{
    cx::DataPtr image1 = regService->getFixedData();
    cx::DataPtr image2 = regService->getMovingData();
    bool image1HistoryEmpty = image1->get_rMd_History()->getData().empty();
    bool image2HistoryEmpty = image2->get_rMd_History()->getData().empty();
    if(image1HistoryEmpty && image2HistoryEmpty)
        return true;
    else
        return false;
}

void checkAndAddDummyImagesToPatient(cx::RegistrationServicePtr regService,
                                     cx::PatientModelServicePtr patientModel)
{
    // Test image data
    vtkImageDataPtr dummyImageData = cx::Image::createDummyImageData(4, 1);

    /******************************/
    // Fixed image volume - image1
    /******************************/
    cx::ImagePtr image1 = cx::ImagePtr(new cx::Image("imageUid1",
                                                     dummyImageData, "imageName1"));
    /******************************/
    // Moving image volume - image2
    /******************************/
    cx::ImagePtr image2 = cx::ImagePtr(new cx::Image("imageUid2",
                                                     dummyImageData, "imageName2"));
    /******************************************/
    // Setup the Services Plugin data
    /******************************************/
    patientModel->insertData(image1);
    patientModel->insertData(image2);
    regService->setFixedData(image1);
    regService->setMovingData(image2);

    REQUIRE(checkRegistrationHistoryEmpty(regService, patientModel));
}

void define_rMd_AndFourLandmarksForFixedAndMovingImages(cx::RegistrationServicePtr regService)
{
    cx::DataPtr image1 = regService->getFixedData();
    cx::DataPtr image2 = regService->getMovingData();

    bool ok = false;
    /*************************/
    // Fixed image - image1
    /*************************/
    QString rMd_1_matrix= QString("0.999998    -1.57265e-06  0.00198575  -200.936 \
                                  -2.16633e-07     1         0.000901065  -245.095 \
                                  -0.00198575  -0.000901064  0.999998     -664.637 \
                                    0            0            0            1");

    cx::Transform3D rMd_1 = cx::Transform3D::fromString(rMd_1_matrix, &ok);
    REQUIRE(ok);
    image1->get_rMd_History()->setRegistration(rMd_1);
    int numberOfLandmarks = image1->getLandmarks()->getLandmarks().size();
    REQUIRE(numberOfLandmarks==0);
    // Define four known landmarks
    cx::Vector3D landmark1_img1_coord(145.676, 143.279, 52.5942);
    cx::Landmark landmark1_img1("1", landmark1_img1_coord);
    cx::Vector3D landmark2_img1_coord(216.527, 145.428, 42.9244);
    cx::Landmark landmark2_img1("2", landmark2_img1_coord);
    cx::Vector3D landmark3_img1_coord(138.119, 148.355, 69.2363);
    cx::Landmark landmark3_img1("3", landmark3_img1_coord);
    cx::Vector3D landmark4_img1_coord(220.452, 154.114, 59.0022);
    cx::Landmark landmark4_img1("4", landmark4_img1_coord);
    image1->getLandmarks()->setLandmark(landmark1_img1);
    image1->getLandmarks()->setLandmark(landmark2_img1);
    image1->getLandmarks()->setLandmark(landmark3_img1);
    image1->getLandmarks()->setLandmark(landmark4_img1);
    numberOfLandmarks = image1->getLandmarks()->getLandmarks().size();
    REQUIRE(numberOfLandmarks==4);

    /**************************/
    // Moving image - image2
    /**************************/
    QString rMd_2_matrix = QString(" 1        0        0 -149.707 \
                                     0        1        0 -249.707 \
                                     0        0        1  -1172.2 \
                                     0        0        0     1");

    cx::Transform3D rMd_2 = cx::Transform3D::fromString(rMd_2_matrix, &ok);
    REQUIRE(ok);
    image2->get_rMd_History()->setRegistration(rMd_2);

    numberOfLandmarks = image2->getLandmarks()->getLandmarks().size();
    REQUIRE(numberOfLandmarks==0);
    // Define four known landmarks
    cx::Vector3D landmark1_img2_coord(181.778, 143.559, 274.013);
    cx::Landmark landmark1_img2("1", landmark1_img2_coord);
    cx::Vector3D landmark2_img2_coord(111.144, 147.007, 284.916);
    cx::Landmark landmark2_img2("2", landmark2_img2_coord);
    cx::Vector3D landmark3_img2_coord(189.012,  149.21, 257.282);
    cx::Landmark landmark3_img2("3", landmark3_img2_coord);
    cx::Vector3D landmark4_img2_coord(106.915, 155.231, 268.812);
    cx::Landmark landmark4_img2("4", landmark4_img2_coord);
    image2->getLandmarks()->setLandmark(landmark1_img2);
    image2->getLandmarks()->setLandmark(landmark2_img2);
    image2->getLandmarks()->setLandmark(landmark3_img2);
    image2->getLandmarks()->setLandmark(landmark4_img2);
    numberOfLandmarks = image2->getLandmarks()->getLandmarks().size();
    REQUIRE(numberOfLandmarks==4);
}


void shutdown()
{
    cx::LogicManager::shutdown();
}

ctkPluginContext* getPluginContext()
{
    cx::LogicManager::getInstance()->getPluginFramework()->start();
    ctkPluginContext* context = cx::LogicManager::getInstance()->getPluginContext();
    return context;
}

TEST_CASE("RegistrationMethodManual: Verify correct operation when registration history is empty",
           "[unit][plugins][org.custusx.registration.method.manual]")
{
    // Registration should be correctly set even if no previous registration exists in history
    init();
    ctkPluginContext* context = getPluginContext();
    REQUIRE(context);
    cx::RegServicesPtr regServices = cx::RegServices::create(context);
    cx::RegistrationServicePtr registrationService = regServices->registration();
    cx::PatientModelServicePtr patientModelService = regServices->patient();
    REQUIRE(patientModelService);
    REQUIRE(registrationService);

    checkAndAddDummyImagesToPatient(registrationService, patientModelService);

    registrationWidgetPtr regWidget = registrationWidgetPtr(
                new cx::ManualImage2ImageRegistrationWidget(regServices, NULL,
                                                            QString("TestregistrationWidget")));
    REQUIRE(regWidget);

    // Check registration matrix when no previous registration exist
    // should return identity
    cx::Transform3D result = regWidget->getMatrixFromBackend();
    cx::Transform3D identity = cx::Transform3D::Identity();
    REQUIRE(cx::similar(result, identity));

    // Simulate changing GUI registration matrix input
    // Check that history is updated
    cx::DataPtr movingImage = registrationService->getMovingData();

    regWidget->setMatrixFromWidget(cx::Transform3D::Identity());
    REQUIRE(movingImage->get_rMd_History()->getData().size() == 1);

    shutdown();
}

TEST_CASE("RegistrationMethodManual: Verify Image-to-Image registration with known Fixed and Moving image data - \
No landmarks defined",
           "[unit][plugins][org.custusx.registration.method.manual]")
{
    // This test recreates an image-to-image registration with confirmed match where we know the
    // resulting fMm matrix

    init();
    ctkPluginContext* context = getPluginContext();
    REQUIRE(context);
    cx::RegServicesPtr regServices = cx::RegServices::create(context);
    cx::RegistrationServicePtr registrationService = regServices->registration();
    cx::PatientModelServicePtr patientModelService = regServices->patient();
    REQUIRE(patientModelService);
    REQUIRE(registrationService);

    checkAndAddDummyImagesToPatient(registrationService, patientModelService);
    define_rMd_AndFourLandmarksForFixedAndMovingImages(registrationService);

    bool ok = false;

    /************************************************************************/
    // Setup the image2image registration with known transformation matrix
    /************************************************************************/
    QString fMm_matrix = QString("-0.999946   0.00233145 0.0101763 -13.9823 \
                                   0.00212849 0.9998    -0.0199104 -15.1837 \
                                  -0.0102207 -0.0198877 -0.99975   -1511.4 \
                                      0          0         0        1");

    cx::Transform3D fMm = cx::Transform3D::fromString(fMm_matrix, &ok);
    REQUIRE(ok);

    registrationWidgetPtr regWidget = registrationWidgetPtr(
                new cx::ManualImage2ImageRegistrationWidget(regServices, NULL,
                                                            QString("TestregistrationWidget")));
    REQUIRE(regWidget);
    regWidget->setMatrixFromWidget(fMm);
    int numberOfLandmarks = 0;
    double accuracy = regWidget->getAverageAccuracy(numberOfLandmarks);
    REQUIRE(numberOfLandmarks == 0);
    REQUIRE(accuracy > 999);

    shutdown();

}


TEST_CASE("RegistrationMethodManual: Verify Image-to-Image registration with known Fixed and Moving image data - \
Four landmarks defined for accuracy verification",
          "[unit][plugins][org.custusx.registration.method.manual]")
{
    // This test recreates an image-to-image registration with confirmed match where we know the
    // resulting fMm matrix
    // The accuracy of the match is confirmed by defining 4 known landmarks in both volumes
    // and verify the accuracy measure calculated by CustusX.

    init();
    ctkPluginContext* context = getPluginContext();
    REQUIRE(context);
    cx::RegServicesPtr regServices = cx::RegServices::create(context);
    cx::RegistrationServicePtr registrationService = regServices->registration();
    cx::PatientModelServicePtr patientModelService = regServices->patient();
    REQUIRE(patientModelService);
    REQUIRE(registrationService);
    bool ok = false;

    checkAndAddDummyImagesToPatient(registrationService, patientModelService);
    define_rMd_AndFourLandmarksForFixedAndMovingImages(registrationService);

    patientModelService->setLandmarkActive("1", true);
    patientModelService->setLandmarkActive("2", true);
    patientModelService->setLandmarkActive("3", true);
    patientModelService->setLandmarkActive("4", true);


    /************************************************************************/
    // Setup the image2image registration with known transformation matrix
    /************************************************************************/
    QString fMm_matrix = QString("-0.999946   0.00233145 0.0101763 -13.9823 \
                                   0.00212849 0.9998    -0.0199104 -15.1837 \
                                  -0.0102207 -0.0198877 -0.99975   -1511.4 \
                                      0          0         0        1");

    cx::Transform3D fMm = cx::Transform3D::fromString(fMm_matrix, &ok);
    REQUIRE(ok);

    registrationWidgetPtr regWidget = registrationWidgetPtr(
                new cx::ManualImage2ImageRegistrationWidget(regServices, NULL,
                                                            QString("TestregistrationWidget")));
    REQUIRE(regWidget);
    regWidget->setMatrixFromWidget(fMm);
    int numberOfLandmarks = 0;
    double accuracy = regWidget->getAverageAccuracy(numberOfLandmarks);
    REQUIRE(numberOfLandmarks == 4);
    REQUIRE(accuracy < 1.30);

    shutdown();

}

TEST_CASE("RegistrationMethodManual: Verify Image-to-Image registration with known Fixed and Moving image data - \
One landmark defined for accuracy verification",
          "[unit][plugins][org.custusx.registration.method.manual]")
{
    // This test recreates an image-to-image registration with confirmed match where we know the
    // resulting fMm matrix
    // The accuracy of the match is confirmed by defining one known landmark in both volumes
    // and verify the accuracy measure calculated by CustusX.

    init();
    ctkPluginContext* context = getPluginContext();
    REQUIRE(context);
    cx::RegServicesPtr regServices = cx::RegServices::create(context);
    cx::RegistrationServicePtr registrationService = regServices->registration();
    cx::PatientModelServicePtr patientModelService = regServices->patient();
    REQUIRE(patientModelService);
    REQUIRE(registrationService);
    bool ok = false;

    checkAndAddDummyImagesToPatient(registrationService, patientModelService);
    define_rMd_AndFourLandmarksForFixedAndMovingImages(registrationService);
    patientModelService->setLandmarkActive("1", true);

    /************************************************************************/
    // Setup the image2image registration with known transformation matrix
    /************************************************************************/
    QString fMm_matrix = QString("-0.999946   0.00233145 0.0101763 -13.9823 \
                                   0.00212849 0.9998    -0.0199104 -15.1837 \
                                  -0.0102207 -0.0198877 -0.99975   -1511.4 \
                                      0          0         0        1");

    cx::Transform3D fMm = cx::Transform3D::fromString(fMm_matrix, &ok);
    REQUIRE(ok);

    registrationWidgetPtr regWidget = registrationWidgetPtr(
                new cx::ManualImage2ImageRegistrationWidget(regServices, NULL,
                                                            QString("TestregistrationWidget")));
    REQUIRE(regWidget);
    regWidget->setMatrixFromWidget(fMm);
    int numberOfLandmarks;
    double accuracy = regWidget->getAverageAccuracy(numberOfLandmarks);
    REQUIRE(numberOfLandmarks == 1);
    REQUIRE(accuracy < 2.0);

    shutdown();

}

} // cxtest
