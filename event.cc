#include "event.hh"
#include "hit.hh"

#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"


MyEventAction::MyEventAction() {}
MyEventAction::~MyEventAction() {}

void MyEventAction::BeginOfEventAction(const G4Event *) {}

void MyEventAction::EndOfEventAction(const G4Event *event)
{
    G4AnalysisManager *man = G4AnalysisManager::Instance();
    G4int eventID = event->GetEventID();

    G4HCofThisEvent *hce = event->GetHCofThisEvent();
    if (!hce) return;

    // ── dE strips → ntuple 0, fDetType = 0 ────────────────────────────────
    if (fHCID_dE < 0)
        fHCID_dE = G4SDManager::GetSDMpointer()
                       ->GetCollectionID("dE_strips/MyHitsCollection");

    auto *dE_hits = static_cast<MyHitsCollection*>(hce->GetHC(fHCID_dE));

    if (dE_hits) {
        G4int nHits = dE_hits->entries();
        for (G4int i = 0; i < nHits; i++)
        {
            MyHit *hit = (*dE_hits)[i];
            G4ThreeVector pos = hit->GetPosition();

            man->FillNtupleIColumn(0, 0, eventID);
            man->FillNtupleIColumn(0, 1, 0);              // fDetType = 0 (dE)
            man->FillNtupleIColumn(0, 2, hit->GetCopyNo());
            man->FillNtupleDColumn(0, 3, pos.x());
            man->FillNtupleDColumn(0, 4, pos.y());
            man->FillNtupleDColumn(0, 5, pos.z());
            man->FillNtupleDColumn(0, 6, hit->GetEdep());
            man->FillNtupleDColumn(0, 7, hit->GetTime());
            man->FillNtupleIColumn(0, 8, hit->GetTrackID());
            man->AddNtupleRow(0);
        }
    }

    // ── E sectors → ntuple 0, fDetType = 1 ────────────────────────────────
    if (fHCID_E < 0)
        fHCID_E = G4SDManager::GetSDMpointer()
                      ->GetCollectionID("E_sectors/MyHitsCollection");

    auto *E_hits = static_cast<MyHitsCollection*>(hce->GetHC(fHCID_E));

    if (E_hits) {
        G4int nHits = E_hits->entries();
        for (G4int i = 0; i < nHits; i++)
        {
            MyHit *hit = (*E_hits)[i];
            G4ThreeVector pos = hit->GetPosition();

            man->FillNtupleIColumn(0, 0, eventID);
            man->FillNtupleIColumn(0, 1, 1);              // fDetType = 1 (E)
            man->FillNtupleIColumn(0, 2, hit->GetCopyNo());
            man->FillNtupleDColumn(0, 3, pos.x());
            man->FillNtupleDColumn(0, 4, pos.y());
            man->FillNtupleDColumn(0, 5, pos.z());
            man->FillNtupleDColumn(0, 6, hit->GetEdep());
            man->FillNtupleDColumn(0, 7, hit->GetTime());
            man->FillNtupleIColumn(0, 8, hit->GetTrackID());
            man->AddNtupleRow(0);
        }
    }

    // optional: periodic status print
    if (eventID % 1000 == 0) {
        G4int n_dE = dE_hits ? dE_hits->entries() : 0;
        G4int n_E  = E_hits  ? E_hits->entries()  : 0;
        G4cout << "Event " << eventID
               << ": " << n_dE << " dE hits, "
               << n_E << " E hits" << G4endl;
    }
}