#include "run.hh"
#include <sstream>

MyRunAction::MyRunAction() {

    G4AnalysisManager *man = G4AnalysisManager::Instance();
    man->SetNtupleMerging(true);

    if (man->GetNofNtuples() == 0) {

        // ── Single ntuple: all hits from both dE and E detectors ───────────
        // fDetType distinguishes them: 0 = dE strip, 1 = E sector
        man->CreateNtuple("Hits", "All detector hits");
        man->CreateNtupleIColumn("fEvent");       // col 0 - event ID
        man->CreateNtupleIColumn("fDetType");     // col 1 - detector type: 0=dE, 1=E
        man->CreateNtupleIColumn("fCopyNo");      // col 2 - strip (0-63) or sector (0-15)
        man->CreateNtupleDColumn("fX");           // col 3 - hit x position
        man->CreateNtupleDColumn("fY");           // col 4 - hit y position
        man->CreateNtupleDColumn("fZ");           // col 5 - hit z position
        man->CreateNtupleDColumn("fEdep");        // col 6 - energy deposited in this step
        man->CreateNtupleDColumn("fTime");        // col 7 - global time of hit
        man->CreateNtupleIColumn("fTrackID");     // col 8 - track ID (1=primary)
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