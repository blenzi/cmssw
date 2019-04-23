#include "OnlineDB/EcalCondDB/interface/EcalCondDBInterface.h"
#include "OnlineDB/EcalCondDB/interface/all_lmf_types.h"

#include <iostream>
#include <string>

using namespace std;
int main (int argc, char* argv[]) {
  if (argc < 2) return 0;
  string db_pass = argv[1];
  string db_user = "CMS_ECAL_LASER_COND";
  string db_sid  = "CMS_OMDS_LB";

  RunTag  rt;


  cout << "Connecting to OMDS" << endl;
  EcalCondDBInterface econn( db_sid, db_user, db_pass );
  LocationDef my_locdef;
  my_locdef.setLocation("P5_Co");
  RunTypeDef my_rundef;
  my_rundef.setRunType("PHYSICS");


  rt.setLocationDef(my_locdef);
  rt.setRunTypeDef(my_rundef);
  rt.setGeneralTag("GLOBAL");  

  const std::vector<RunIOV> & tmp_rl = econn.fetchNonEmptyGlobalRunList(rt, 290000 + 1, 999999).getRuns();
  cout << " Number of Run IOV with Ecal" << tmp_rl.size() << endl;
  return 1;
}
