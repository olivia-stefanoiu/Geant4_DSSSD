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
    // ── get the hits collection ────────────────────────────────────────────
    // look up the collection ID once, then cache it
    if (fHCID < 0)
        fHCID = G4SDManager::GetSDMpointer()
                    ->GetCollectionID("dE_strips/MyHitsCollection");
        // "dE_strips" is the SD name you pass in ConstructSDandField()

    //gets ALL hits from the event
    auto *hitsCollection =
        static_cast<MyHitsCollection*>(
            event->GetHCofThisEvent()->GetHC(fHCID));

    if (!hitsCollection) return;

    G4int nHits = hitsCollection->entries();
    if (nHits == 0) return;

    G4int eventID = event->GetEventID();

    G4AnalysisManager *man = G4AnalysisManager::Instance();

    // ── loop over ALL hits in this event and write each one ────────────────
    for (G4int i = 0; i < nHits; i++)
    {
        MyHit *hit = (*hitsCollection)[i];

        G4ThreeVector pos = hit->GetPosition();

        man->FillNtupleIColumn(0, 0, eventID);
        man->FillNtupleIColumn(0, 1, hit->GetCopyNo());
        man->FillNtupleDColumn(0, 2, pos.x());
        man->FillNtupleDColumn(0, 3, pos.y());
        man->FillNtupleDColumn(0, 4, pos.z());
        man->FillNtupleDColumn(0, 5, hit->GetEdep());
        man->FillNtupleDColumn(0, 6, hit->GetTime());
        man->FillNtupleIColumn(0, 7, hit->GetTrackID());
        man->AddNtupleRow(0);
    }

}