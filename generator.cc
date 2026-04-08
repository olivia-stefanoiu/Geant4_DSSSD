#include "generator.hh"

MyPrimaryGenerator::MyPrimaryGenerator()
{
    std::cout<<"MYPRIMARYGENERATOR.cc";
    fParticleGun = new G4GeneralParticleSource();
}

MyPrimaryGenerator::~MyPrimaryGenerator()
{
    delete fParticleGun;
}

void MyPrimaryGenerator::GeneratePrimaries(G4Event *anEvent)
{
    fParticleGun->GeneratePrimaryVertex(anEvent);
}