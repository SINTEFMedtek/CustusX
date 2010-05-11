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
  
  //mInputFile = "/Users/olevs/data/UL_thunder/Nevro_Spine/SpineData/ultrasoundSample5.mhd";
  mInputFile = "/Users/olevs/data/UL_thunder/test/ultrasoundSample5.mhd";
  
  mReconstructer->reconstruct(mInputFile, "/Users/olevs/data/UL_thunder/test/M12L.cal");
  
}
  
  
  
}//namespace