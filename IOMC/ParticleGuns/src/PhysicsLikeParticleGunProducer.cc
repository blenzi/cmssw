#include <ostream>

#include "IOMC/ParticleGuns/interface/PhysicsLikeParticleGunProducer.h"

#include "SimDataFormats/GeneratorProducts/interface/HepMCProduct.h"
#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

#include "DataFormats/Math/interface/Vector3D.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/RandomNumberGenerator.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Units/GlobalSystemOfUnits.h"
#include "CLHEP/Units/GlobalPhysicalConstants.h"
#include "CLHEP/Random/RandFlat.h"
#include "CLHEP/Random/RandExponential.h"

using namespace edm;
using namespace std;

PhysicsLikeParticleGunProducer::PhysicsLikeParticleGunProducer(const ParameterSet& pset) :
   BaseFlatGunProducer(pset)
{

    ParameterSet pgun_params = pset.getParameter<ParameterSet>("PGunParameters") ;

    fMaxEt = pgun_params.getParameter<double>("MaxEt");
    fMinEt = pgun_params.getParameter<double>("MinEt");
    fMeanEt = pgun_params.getParameter<double>("MeanEt");

    fMinEta     = pgun_params.getParameter<double>("MinEta");
    fMaxEta     = pgun_params.getParameter<double>("MaxEta");
    fMinPhi     = pgun_params.getParameter<double>("MinPhi");
    fMaxPhi     = pgun_params.getParameter<double>("MaxPhi");

    fNParticlesMin = pgun_params.getParameter<int>("NParticlesMin");
    fNParticlesMax = pgun_params.getParameter<int>("NParticlesMax");
    fPartIDs = pgun_params.getParameter< vector<int> >("PartID");

    f_atECal = pgun_params.getParameter<bool>("atECal");
    fR_ECal = pgun_params.getParameter<double>("R_ECal");
    fZ_ECal = pgun_params.getParameter<double>("Z_ECal");

    produces<HepMCProduct>("unsmeared");
    produces<GenEventInfoProduct>();

}

PhysicsLikeParticleGunProducer::~PhysicsLikeParticleGunProducer()
{
   // no need to cleanup GenEvent memory - done in HepMCProduct
}

void PhysicsLikeParticleGunProducer::produce(Event &e, const EventSetup& es)
{
    edm::Service<edm::RandomNumberGenerator> rng;
    CLHEP::HepRandomEngine* engine = &rng->getEngine(e.streamID());

    if ( fVerbosity > 0 )
    {
       LogDebug("PhysicsLikeParticleGunProducer") << " PhysicsLikeParticleGunProducer : Begin New Event Generation" << endl ;
    }
    fEvt = new HepMC::GenEvent() ;

    // loop over particles
    //
    int barcode = 1 ;
    int numParticles = CLHEP::RandFlat::shoot(engine, fNParticlesMin, fNParticlesMax);

    for(int i=0; i<numParticles; i++)
    {
        int pID_index = CLHEP::RandFlat::shoot(engine, 0, fPartIDs.size());
        int PartID = fPartIDs[pID_index];
        double Et = std::min(CLHEP::RandExponential::shoot(engine, fMeanEt - fMinEt) + fMinEt, fMaxEt);
        double eta = CLHEP::RandFlat::shoot(engine, fMinEta, fMaxEta);
        double phi = CLHEP::RandFlat::shoot(engine, fMinPhi, fMaxPhi);

//         const HepPDT::ParticleData *PData = fPDGTable->particle(HepPDT::ParticleID(abs(PartID))) ;
//         double mass   = PData->mass().value();
        double energy = Et*cosh(eta);
        double px     = Et*cos(phi);
        double py     = Et*sin(phi);
        double pz     = Et*sinh(eta);

        // Compute Vertex Position
        double x=0, y=0, z=0;
        if (f_atECal)
        {
            double R = fabs(eta) < 1.5 ? fR_ECal : fZ_ECal / sinh(eta);
            x = R * cos(phi);
            y = R * sin(phi);
            z = fabs(eta) < 1.5 ? fZ_ECal : fR_ECal * sinh(eta);
        }

        constexpr double c= 2.99792458e+1; // cm/ns
        double timeOffset = sqrt(x*x + y*y + z*z)/c*ns*c_light;
        HepMC::GenVertex* Vtx = new HepMC::GenVertex(HepMC::FourVector(x*cm,y*cm,z*cm,timeOffset));

        HepMC::FourVector p(px,py,pz,energy);
        HepMC::GenParticle* Part = new HepMC::GenParticle(p,PartID,1);
        Part->suggest_barcode( barcode );
        barcode++;

        Vtx->add_particle_out(Part);

        if (fVerbosity > 0) {
            Vtx->print();
            Part->print();
        }
        fEvt->add_vertex(Vtx);
    }

    fEvt->set_event_number(e.id().event());
    fEvt->set_signal_process_id(20);

    if ( fVerbosity > 0 )
    {
      fEvt->print();
    }

    unique_ptr<HepMCProduct> BProduct(new HepMCProduct());
    BProduct->addHepMCData( fEvt );
    e.put(std::move(BProduct), "unsmeared");

    unique_ptr<GenEventInfoProduct> genEventInfo(new GenEventInfoProduct(fEvt));
    e.put(std::move(genEventInfo));

    if ( fVerbosity > 0 )
     {
       LogDebug("PhysicsLikeParticleGunProducer") << " PhysicsLikeParticleGunProducer : Event Generation Done " << endl;
     }
}
