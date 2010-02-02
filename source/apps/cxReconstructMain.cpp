//===========================================================================
//
// File: cxReconstructMain.cpp
//
// Created: 06.05.2008
//
/// \file
///       Command line PNN 3D ultrasound reconstruction.
/// \author Ole Vegard Solberg
//===========================================================================

#include "cxUsreconstruction.h"
#include "cxImport2DUS.h"

#include <iostream>
#include <exception>
#include <time.h>

int main(int argc, char *argv[])
{ 
  float mVoxelSize = 0.5;
  bool maxRange = false;
  bool oldData = false;
  int interpolationSteps = 2;
  double timeDelay = 0;
  bool alignMiddle = true;
  bool interpolationSet = false;
  std::string mTargetVolumeTransformFile = "";
  std::string mSettingsFile = "";
  std::string rmsFile = "";
  std::string rndFile = "";
  unsigned int removeFromRecord = 0;
  UsReconstruction::ReconstructionTypeEnum mType = UsReconstruction::PNN;
  
  /** The volume */
  vtkImageData* imagedata = NULL;
  
  try {
    if (argc < 2)
      throw ("Input file missing!");
    else if (argc < 3)
      throw ("Output file missing!");
    
    const char* mInputFile = argv[1];
    std::string mOutputFile = argv[2];
    
    for (int i=2; i < argc; i++)
    {
      if (argv[i][0] == '-')
        if (argv[i][1] == 'v') //Voxel size
        {
          //mVoxelSize = argv[i+1];
          //if (EOF == sscanf(argv[i+1], "%f", mVoxelSize))
          //  throw ("Voxel size must be float");
          mVoxelSize = atof(argv[i+1]);
          //std::cout << "voxelSize: " << mVoxelSize << std::endl;
        }
        else if ((argv[i][1] == 'r') && // Use rms output file
                 (argv[i][2] == 'm') && 
                 (argv[i][3] == 's')) 
        {
          i++;
          if (argc < i)
            throw ("rms file missing!");
          rmsFile = argv[i];
          //std::cout << "rmsFile: " << rmsFile << std::endl;
        }
        else if ((argv[i][1] == 'r') && // Use random numbers input file
                 (argv[i][2] == 'n') && 
                 (argv[i][3] == 'd')) 
        {
          i++;
          if (argc < i)
            throw ("File with random numbers missing!");
          rndFile = argv[i];
          //std::cout << "rndFile: " << rndFile << std::endl;
        }
        else if ((argv[i][1] == 't') && (argv[i][2] == 'c')) // Time delay
        {
          i++;
          timeDelay = atof(argv[i]);
          //std::cout << "tc: " << argv[i] << std::endl;
        }
        else if ((argv[i][1] == 'd') && (argv[i][2] == 'a')) // Don't align with middle slice
        {
          alignMiddle = false;
        }
        else if ((argv[i][1] == 'p') && (argv[i][2] == 'n') && (argv[i][3] == 'n')) 
        {
          mType = UsReconstruction::PNN;
        }
        else if ((argv[i][1] == 'd') && (argv[i][2] == 'w')) 
        {
          mType = UsReconstruction::kernel;
        }
        else if ((argv[i][1] == 't') && (argv[i][2] == 'r')) // Target volume transform
        {
          i++;
          if (argc < i)
            throw ("Target volume transform file missing!");
          mTargetVolumeTransformFile = argv[i];
        }
        else if (argv[i][1] == 's') // Settings file
        {
          i++;
        }
        else if ((argv[i][1] == 'r') && (argv[i][2] == 'f')) // "Remove From" Remove data from record, for RMS tests
        {
          i++;
          removeFromRecord = atoi(argv[i]);
          std::cout << "rf: " << argv[i] << std::endl;
        }
        else if (argv[i][1] == 'r') //Use max range
          maxRange = true;
        else if (argv[i][1] == 'o') //Read old data without timetags
          oldData = true;
        else if (argv[i][1] == 'i') //number of interpolation steps
        {
          interpolationSteps = atoi(argv[i+1]);
          interpolationSet = true;
        }
        /*else if (argv[i][1] == 's') // Settings file
        {
          i++;
          if (argc < i)
            throw ("Settings file missing!");
          mSettingsFile = argv[i];
        }*/
        /*else if (argv[i][1] == 't') // Reconstruction type
        {
          if (argv[i][2] == 'p') // PNN
            mType = UsReconstruction::PNN;
          else if (argv[i][2] == 'k' || argv[i][2] == 's' ) // 3D Kernel
            mType = UsReconstruction::kernel;
          else
            throw ("Unknown reconstruction type"); 
        }*/
        else
        {
          std::cout << "Unknown switch: " << argv[i][1] << std::endl;
          throw ("Unknown switch");  
        }
    }
    
    // Only use interpolation for kernel reconstruction if asked for
    if (mType == UsReconstruction::kernel)
      if (!interpolationSet)
        interpolationSteps = 0;
    
    Import2DUS *reader = new Import2DUS();
    reader->importmhd(mInputFile, oldData, timeDelay);
    
    
    // Read multiple setting file(s) after 
    for (int i=2; i < argc; i++)
    {
      if (argv[i][1] == 's') // Settings file
      {
        i++;
        if (argc < i)
          throw ("Settings file missing!");
        mSettingsFile = argv[i];
        reader->readSettings(reader->getUsData(), mSettingsFile);
      }
    }
    
    // rms file from the command line takes precedence
    if (!rmsFile.empty())
      reader->getUsData()->rmsFile = rmsFile;
    // random number file from the command line takes precedence
    if (!rndFile.empty())
      reader->getUsData()->removeFile = rndFile;
    
    // Read random numbers from file if a file is supplied
    // either from the command line (-rnd) of the a setting file (RemoveFile)
    if (!reader->getUsData()->removeFile.empty())
      reader->readRandomNumbersFile(reader->getUsData());
    
    
    // test import
    imagedata = reader->getOutput();
    
    //usreconstruction test code
    //--------------------------
    
    //file->finalizePositions();//already called in exportPosFile();
    
    UsReconstruction *usrec = new UsReconstruction();
    
    //Test writer
//    imagedata->SetSpacing(2.0,2.0,2.0);
//    usrec->writeFile(imagedata, mOutputFile);
    //usrec->writeFile( usrec->createPNNVolume(dynamic_cast<EchopacTemplate<char>* >(g_echopac)) );
    usrec->writeFile( usrec->reconstructVolume(reader->getUsData(),
                                               mOutputFile, 
                                               mType,
                                               mVoxelSize, 
                                               interpolationSteps,
                                               alignMiddle,
                                               mTargetVolumeTransformFile,
                                               //mSettingsFile,
                                               maxRange,
                                               removeFromRecord),
                      mOutputFile);
 
    delete usrec;
    //--------------------------
    
    
 /*   clock_t t1, t2, t3, t4;
    t1 = clock();
    
    PNN* reconstruction = new PNN(mVoxelSize);
    
    //printf("inputfile \n ");
    reconstruction->readInputFile(mInputFile);
    //printf("posfile \n ");
    reconstruction->readPosStackFile();
    
    t2 = clock();
    
    //printf("calfile \n ");
    reconstruction->readCalFile();
    
    //printf("initRcImage \n ");
    reconstruction->initRcImage();
    
    //printf("getVolume \n ");
    imagedata = reconstruction->getFastVolume(true, interpolate, maxRange);
    
    t3 = clock();
    double dif = (t3 - t2);
    dif = dif / CLOCKS_PER_SEC;
    std::cout << "Total reconstruction time with interpolation: " << dif << " sec" << std::endl;
*/    
/*    printf("Write volume \n ");
    // Write .raw file
    vtkImageWriter* imgw = vtkImageWriter::New();
    std::string rawFileName = mOutputFile + ".raw";
    
    imgw->SetFileName(rawFileName.c_str());
    imgw->SetInput(imagedata);
    imgw->SetFileDimensionality(3);
    imgw->Write();
    
    //std::cout << "write .mhd file" << std::endl;
    const unsigned short US = 0x0011;
    const unsigned char *UC = (const unsigned char*)(&US);
    const bool isLittleEndian = (UC[1] == 0x11);
    std::string mhdFileName = mOutputFile + ".mhd";
    FILE* f = fopen(mhdFileName.c_str(), "w");
    fprintf(f, "NDims = 3\n"
            "DimSize = %i %i %i\n"
            "ElementType = MET_UCHAR\n"
            "ElementSpacing = %f %f %f\n"
            "ElementByteOrderMSB = %s\n"
            //"Position = %f %f %f\n"
            "ElementDataFile = %s\n",
            imagedata->GetDimensions()[0],
            imagedata->GetDimensions()[1],
            imagedata->GetDimensions()[2],
            imagedata->GetSpacing()[0],
            imagedata->GetSpacing()[1],
            imagedata->GetSpacing()[2],
            //ddims.w, ddims.h, ddims.d,
            //sspc.x, sspc.y, sspc.z,
            isLittleEndian ? "True" : "False",
            //m[0], m[1], m[2],
            rawFileName.c_str());
    fclose(f);
*/
    
/*    t4 = clock();
    dif = (t4 - t1);
    dif = dif / CLOCKS_PER_SEC;
    std::cout << "Total time with file reading and writing: " << dif 
      << " sec" << std::endl;
*/    
    
    
  } catch (char const * str) {
		std::cout << std::endl << "*** Error: " << str << " ***" 
    << std::endl << std::endl;
    std::cout << " ----------------------------------------------------------"
    << std::endl;
    std::cout << "Usage: reconstruct inputfile outputfile (-v voxel_size) "
    << "(-i interp_steps) (-tc ms) (-tr transformfile) (-pnn/-dw)" << std::endl;
    std::cout << " ----------------------------------------------------------"
    << std::endl;
    std::cout << "| inputfile   (with ending: .mha or .mhd)                  |" 
    << std::endl;
    std::cout << "| outputfile  (without ending .mha and .raw are created)   |" 
    << std::endl;
    std::cout << "| -v voxel_size (default 0.5)                              |"
    << std::endl;
    std::cout << "| -s settingfile                                           |"
    << std::endl;
    std::cout << "| -r (Use max char range - default: not enabled)           |" 
    << std::endl;
    std::cout << "| -i Number of interpolation steps (default 2)             |" 
    << std::endl;
    std::cout << "|    Use 0 for no interpolation                            |" 
    << std::endl;
    std::cout << "| -pnn (default) PNN reconstruction,                       |"
    << std::endl;
    std::cout << "| -dw kernel reconstruction                                |"
    << std::endl;
    std::cout << "| -tc time calibration value in ms (default 0.0)           |"
    << std::endl;
    std::cout << "| -da Don't align with middle slice                        |"
    << std::endl;
    std::cout << "| -tr Target volume transform file                         |"
    << std::endl;
    std::cout << "| -o Import data in old format, without time tags          |"
    << std::endl;
    std::cout << "| The old format reconstruction will need the following    |"
    << std::endl;
    std::cout << "| files: .mhd .vol .cal .msk .pos                          |" 
    << std::endl;
    std::cout << " ----------------------------------------------------------"
    << std::endl;
	}   

}