#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dctag.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmimgle/dcmimage.h"

#include "dcmtk/dcmimage/diregist.h"   /* include to support color images */ 
#include "dcmtk/dcmdata/dcrledrg.h"      /* for DcmRLEDecoderRegistration */ 
#include "dcmtk/dcmjpeg/djdecode.h"      /* for dcmjpeg decoders */ 
#include "dcmtk/dcmjpeg/dipijpeg.h"      /* for dcmimage JPEG plugin */

int main(int argc, char **argv)
{
  if (argc < 2) { printf("Usage: %s <file>\n", argv[0]); exit(1); }

  DcmRLEDecoderRegistration::registerCodecs(); 
  DJDecoderRegistration::registerCodecs();
  DicomImage dicomimage( argv[1], CIF_AcrNemaCompatibility );
  EI_Status status = dicomimage.getStatus();

#if 0
  E_TransferSyntax xfer = dicomimage.getTransferSyntax();
  
  if (!DcmXfer(dicomimage.getTransferSyntax()).isNotEncapsulated())
  {
    printf(" This is an encapsulated image.\n");
  }

  case (xfer)
  {
    case EXS_LittleEndianImplicit: printf("EXS_LittleEndianImplicit\n"); break;
    case EXS_BigEndianImplicit: printf("EXS_BigEndianImplicit\n"); break;
    case EXS_LittleEndianExplicit: printf("EXS_LittleEndianExplicit\n"); break;
    case EXS_BigEndianExplicit: printf("EXS_BigEndianExplicit\n"); break;
    default: printf("Unknown pixel transfer syntax\n"); break;
  }
#endif

  if ( status != EIS_Normal )
  {
    printf( "Error from DCMTK: %s\n", dicomimage.getString( status ) );
  }
  assert( status == EIS_Normal );

  return 0;
}
