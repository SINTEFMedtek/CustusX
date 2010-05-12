/*
 *  sscReconstructionWidget.cpp
 *  Created by Ole Vegard Solberg on 5/4/10.
 */


#include "sscReconstructionWidget.h"

namespace ssc 
{
ReconstructionWidget::ReconstructionWidget(QWidget* parent):
  QWidget(parent),
  mReconstructer(new Reconstructer())
{
  this->setWindowTitle("US Reconstruction");

/*#define input_set_mac "/Users/olevs/data/UL_thunder/Nevro_Spine/SpineData/ultrasoundSample5.mhd", \
"/Users/olevs/data/UL_thunder/Nevro_Spine/SpineData/ultrasoundSample5.pos", \
"/Users/olevs/data/UL_thunder/Nevro_Spine/SpineData/ultrasoundSample5.tim", \
"/Users/olevs/data/UL_thunder/Nevro_Spine/SpineData/ultrasoundSample5.msk", \
"/Users/olevs/data/UL_thunder/Nevro_Spine/SpineData/ultrasoundSample5.vol", \
"/Users/olevs/data/UL_thunder/Nevro_Spine/calibration_files/M12L.cal"
  //#define input_set_mac_origo_x 850.0f
  //#define input_set_mac_origo_y 10.0f
  //#define input_set_mac_origo_z 2800.0f
#define input_set_mac_origo_x 85.0f
#define input_set_mac_origo_y 1.0f
#define input_set_mac_origo_z 280.0f*/
  
  //QString path = "/Users/olevs/data/UL_thunder/test/";
  QString path = "/Users/christiana/workspace/sessions/us_acq_holger_data/";

  //mInputFile = "/Users/olevs/data/UL_thunder/Nevro_Spine/SpineData/ultrasoundSample5.mhd";
  mInputFile = path + "ultrasoundSample5.mhd";
  
  mReconstructer->reconstruct(mInputFile, path+"M12L.cal");
  
}
  
  
  
}//namespace
