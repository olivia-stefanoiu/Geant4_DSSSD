#include "run.hh"
#include <sstream>

MyRunAction::MyRunAction() {

    G4AnalysisManager *man = G4AnalysisManager::Instance();
    man->SetNtupleMerging(true);

    if (man->GetNofNtuples() == 0) {

        man->CreateNtuple("Hits", "Hits");
        man->CreateNtupleIColumn("fEvent");       // col 0 - event ID
        man->CreateNtupleIColumn("fCopyNo");      // col 1 - strip index (0-63)
        man->CreateNtupleDColumn("fX");           // col 2 - hit x position
        man->CreateNtupleDColumn("fY");           // col 3 - hit y position
        man->CreateNtupleDColumn("fZ");           // col 4 - hit z position
        man->CreateNtupleDColumn("fEdep");        // col 5 - energy deposited in this step
        man->CreateNtupleDColumn("fTime");        // col 6 - global time of hit: in order to check the coincidence window perhaps
        man->CreateNtupleIColumn("fTrackID");     // col 7 - track ID: it helps me determine whether we have a primary or sec 1
        man->FinishNtuple(0);
    }
}

MyRunAction::~MyRunAction() {}

void MyRunAction::BeginOfRunAction(const G4Run *run) {
    G4AnalysisManager *man = G4AnalysisManager::Instance();
    G4bool fileOpen = man->OpenFile("DSSSD_Sim_34Si_OBJECTS.root");
    G4cout << "=== BeginOfRunAction: OpenFile returned " << fileOpen << " ===" << G4endl;
}

void MyRunAction::EndOfRunAction(const G4Run *) {
    G4cout << "=== EndOfRunAction called ===" << G4endl;
    G4AnalysisManager *man = G4AnalysisManager::Instance();
    man->Write();
    man->CloseFile();
}