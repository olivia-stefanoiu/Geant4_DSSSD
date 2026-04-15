#include "detector.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4TouchableHistory.hh"

MySensitiveDetector::MySensitiveDetector(G4String name)
    : G4VSensitiveDetector(name)
{
    // register the collection name — this is how Geant4 knows
    // this SD produces a collection called "<SDname>/MyHitsCollection"
    collectionName.insert("MyHitsCollection");
}

MySensitiveDetector::~MySensitiveDetector() {}

//──────────────────────────────────────────────────────────────────────────────
// Initialize — called once per event, before any ProcessHits
//──────────────────────────────────────────────────────────────────────────────
void MySensitiveDetector::Initialize(G4HCofThisEvent *hitsCE)
{
    // create a fresh, empty hits collection for this event
    fHitsCollection = new MyHitsCollection(SensitiveDetectorName, collectionName[0]);

    // get the collection ID (only once — cached after the first event)
    if (fHCID < 0)
        fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection);

    // attach the collection to the event so EventAction can retrieve it
    //current event
    hitsCE->AddHitsCollection(fHCID, fHitsCollection);
}

//──────────────────────────────────────────────────────────────────────────────
// ProcessHits — called for every step in a sensitive volume
//──────────────────────────────────────────────────────────────────────────────
G4bool MySensitiveDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *)
{
    // NEW VERSION — create a hit object and store it in the collection.
    // No ntuple writing here; that happens in EndOfEventAction.

    G4double edep = aStep->GetTotalEnergyDeposit();
    if (edep == 0.) return false;

    G4StepPoint *preStepPoint = aStep->GetPreStepPoint();

    MyHit *hit = new MyHit();
    //astep what is between pre and post
    hit->SetEdep(edep);
    hit->SetPosition(preStepPoint->GetPosition());
    hit->SetCopyNo(preStepPoint->GetTouchable()->GetCopyNumber());
    hit->SetTime(preStepPoint->GetGlobalTime());
    hit->SetParticleName(aStep->GetTrack()->GetParticleDefinition()->GetParticleName());
    hit->SetTrackID(aStep->GetTrack()->GetTrackID());

    fHitsCollection->insert(hit);

    return true;
}

//──────────────────────────────────────────────────────────────────────────────
// EndOfEvent — optional, collection is already attached to the event
//──────────────────────────────────────────────────────────────────────────────
void MySensitiveDetector::EndOfEvent(G4HCofThisEvent *)
{
    // nothing needed — EventAction will pull the collection
}