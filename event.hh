#ifndef EVENT_HH
#define EVENT_HH

#include "G4UserEventAction.hh"
#include "G4Event.hh"

class MyEventAction : public G4UserEventAction {
public:
    MyEventAction();
    ~MyEventAction() override;

    void BeginOfEventAction(const G4Event *) override;
    void EndOfEventAction(const G4Event *) override;

private:
    G4int fHCID_dE = -1;   // cached hits collection ID for dE strips
    G4int fHCID_E  = -1;   // cached hits collection ID for E sectors
};

#endif
