#ifndef IOMC_ParticleGun_PhysicsLikeParticleGunProducer_H
#define IOMC_ParticleGun_PhysicsLikeParticleGunProducer_H

#include "IOMC/ParticleGuns/interface/BaseFlatGunProducer.h"
#include <cmath>

namespace edm
{

  class PhysicsLikeParticleGunProducer : public BaseFlatGunProducer
  {

  public:
    PhysicsLikeParticleGunProducer(const ParameterSet &);
    ~PhysicsLikeParticleGunProducer() override;

  private:

    void produce(Event & e, const EventSetup& es) override;

  protected :

    // data members
    double fMinEt, fMaxEt, fMeanEt;
//     double fMinEta, fEtaMax, fPhiMin=-M_PI, fPhiMax=M_PI;

    int fNParticlesMin, fNParticlesMax;
    bool f_atECal = false; // shoot at ECal front face instead of (0,0,0)
    double fR_ECal, fZ_ECal; // ECal front face in cm (R for barrel, z for endcap)
    std::vector<int> fPartIDs;
  };
}

#endif
