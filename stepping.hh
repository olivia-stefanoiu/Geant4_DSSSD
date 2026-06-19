#ifndef STEPPING_HH
#define STEPPING_HH

#include "G4UserSteppingAction.hh"
#include "G4Step.hh"

class MySteppingAction : public G4UserSteppingAction
{
public:
    MySteppingAction() = default;
    ~MySteppingAction() override = default;

    void UserSteppingAction(const G4Step*) override;
};
#endif
