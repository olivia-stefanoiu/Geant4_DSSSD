#ifndef DETECTOR_HH
#define DETECTOR_HH

// NEW VERSION — uses G4THitsCollection instead of writing ntuples directly
#include "G4VSensitiveDetector.hh"
#include "hit.hh"

class MySensitiveDetector : public G4VSensitiveDetector {

public:
    MySensitiveDetector(G4String name);
    ~MySensitiveDetector() override;

    // called at the beginning of each event — creates a fresh collection
    void Initialize(G4HCofThisEvent *hitsCE) override;

    // called for every step inside a sensitive volume
    //ROhist = read out history, it can segment your domain
    G4bool ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist) override;

    // called at end of event — nothing special needed here, but available
    void EndOfEvent(G4HCofThisEvent *hitsCE) override;

    //i will have 2 HC one for each sensitive detector
private:
    MyHitsCollection *fHitsCollection = nullptr;
    G4int fHCID = -1;   // hits collection ID (assigned once by the SDManager)
};

#endif
