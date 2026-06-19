#include "stepping.hh"
#include "G4Track.hh"
#include "G4SystemOfUnits.hh"

void MySteppingAction::UserSteppingAction(const G4Step *step)
{
    G4Track *track = step->GetTrack();

    if (track->GetParticleDefinition()->GetParticleName() != "Si34") return;
    if (track->GetTrackStatus() != fStopAndKill) return;   // last step only

    G4ThreeVector pos = step->GetPostStepPoint()->GetPosition();
    G4cout << "34Si died at " << pos/mm << " mm   KE="
           << step->GetPostStepPoint()->GetKineticEnergy()/MeV << " MeV" << G4endl;
}
