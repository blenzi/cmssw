#include "Rtypes.h"
#include "OnlineDB/EcalCondDB/interface/RunIOV.h"
#include "OnlineDB/EcalCondDB/interface/LMFSeqDat.h"
#include "OnlineDB/EcalCondDB/interface/LMFRunIOV.h"
#include "OnlineDB/EcalCondDB/interface/EcalCondDBInterface.h"
#include "OnlineDB/Oracle/interface/Oracle.h"

namespace OnlineDB_EcalCondDB
{
  struct dictionary
  {
    RunIOV r1;
    LMFSeqDat r2;
    LMFRunIOV r3;
    EcalCondDBInterface r4;
  };
}
