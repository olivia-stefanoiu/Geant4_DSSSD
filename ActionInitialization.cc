#include "ActionInitialization.hh"

MyActionInitialization::MyActionInitialization() {}
MyActionInitialization::~MyActionInitialization() {}

void MyActionInitialization::BuildForMaster() const {
    // intentionally empty — sequential mode doesn't need this
}

void MyActionInitialization::Build() const {
    SetUserAction(new MyPrimaryGenerator());
    SetUserAction(new MyRunAction());
    SetUserAction(new MyEventAction());
}