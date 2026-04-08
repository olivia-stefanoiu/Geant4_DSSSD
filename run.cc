#include "run.hh"
#include <sstream>

MyRunAction::MyRunAction() {

    G4AnalysisManager *man = G4AnalysisManager::Instance();
    man->SetNtupleMerging(true);

    // Guard: only book the ntuple once.
    // In sequential mode, both BuildForMaster() and Build() create a
    // MyRunAction, so the constructor runs twice.  The AnalysisManager
    // is a singleton — a second CreateNtuple call creates a duplicate
    // that can silently break file output.
    if (man->GetNofNtuples() == 0) {
        man->CreateNtuple("Hits", "Hits");
        man->CreateNtupleIColumn("fEvent");   // col 0 - event ID
        man->CreateNtupleIColumn("fCopyNo");  // col 1 - detector ID (0-63 dE, 64-79 E)
        man->CreateNtupleDColumn("fX");       // col 2 - hit x position
        man->CreateNtupleDColumn("fY");       // col 3 - hit y position
        man->CreateNtupleDColumn("fZ");       // col 4 - hit z position
        man->FinishNtuple(0);
    }
}

MyRunAction::~MyRunAction() {}

void MyRunAction::BeginOfRunAction(const G4Run *run) {
    G4AnalysisManager *man = G4AnalysisManager::Instance();
    G4bool fileOpen = man->OpenFile("DSSSD_Sim_34Si.root");
    G4cout << "=== BeginOfRunAction: OpenFile returned " << fileOpen << " ===" << G4endl;
    
}

void MyRunAction::EndOfRunAction(const G4Run *) {
    G4cout << "=== EndOfRunAction called ===" << G4endl;
    G4AnalysisManager *man = G4AnalysisManager::Instance();
    man->Write();
    man->CloseFile();
}