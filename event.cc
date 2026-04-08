#include "event.hh"

MyEventAction::MyEventAction() {std::cout<<"EVENT.cc";}

MyEventAction::~MyEventAction() {}

void MyEventAction::BeginOfEventAction(const G4Event *) {}

void MyEventAction::EndOfEventAction(const G4Event *) {}