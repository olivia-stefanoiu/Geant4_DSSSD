#ifndef EVENT_HH
#define EVENT_HH

// NEW VERSION — pulls the hits collection and writes all hits to ntuples
#include "G4UserEventAction.hh"
#include "G4Event.hh"

class MyEventAction : public G4UserEventAction {
public:
    MyEventAction();
    ~MyEventAction() override;

    void BeginOfEventAction(const G4Event *) override;
    void EndOfEventAction(const G4Event *) override;

private:
    G4int fHCID = -1;   // cached hits collection ID
};

#endif
