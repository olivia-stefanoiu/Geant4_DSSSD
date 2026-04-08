#include "physics.hh"
#include "G4ParticleDefinition.hh"
#include "G4ProcessManager.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"

#include "G4SystemOfUnits.hh"


MyPhysicsList::MyPhysicsList() {

    //ma omoara cu lista aia de 1000 de fenomene
       SetVerboseLevel(0);

   //how long must a secondary particle live to detect it
    defaultCutValue = 1.0*mm;
    std::cout<<"PHYSICS.cc"<<'\n';
    RegisterPhysics(new G4EmStandardPhysics(0));  // pass 0 = silent
    RegisterPhysics(new G4DecayPhysics(0));
    RegisterPhysics(new G4RadioactiveDecayPhysics(0));
    RegisterPhysics(new G4HadronPhysicsQGSP_BERT(0));
}

MyPhysicsList::~MyPhysicsList() {}

/*
void MyPhysicsList::ConstructParticle()
{

    ConstructLeptons();
    G4GenericIon::GenericIonDefinition();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MyPhysicsList::ConstructBosons()
{
    G4Gamma::GammaDefinition();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MyPhysicsList::ConstructLeptons()
{
    G4Electron::ElectronDefinition();
    G4Positron::PositronDefinition();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MyPhysicsList::ConstructProcess()
{
    AddTransportation();
    ConstructEM();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4PhysicsListHelper.hh"

#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4PhotoElectricEffect.hh"

#include "G4eMultipleScattering.hh"
#include "G4eIonisation.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MyPhysicsList::ConstructEM()
{
    G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();

    auto particleIterator=GetParticleIterator();
    particleIterator->reset();
    while( (*particleIterator)() ){
        G4ParticleDefinition* particle = particleIterator->value();
        G4String particleName = particle->GetParticleName();

        if (particleName == "gamma") {

            ph->RegisterProcess(new G4PhotoElectricEffect, particle);
            ph->RegisterProcess(new G4ComptonScattering,   particle);
            ph->RegisterProcess(new G4GammaConversion,     particle);

        }
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void MyPhysicsList::SetCuts()
{

    SetCutValue(defaultCutValue, "gamma");

}
*/