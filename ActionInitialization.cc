#include "ActionInitialization.hh"

MyActionInitialization::MyActionInitialization() {}

MyActionInitialization::~MyActionInitialization() {}

//needed for MT mode
void MyActionInitialization::BuildForMaster() const {
    auto *runAction = new MyRunAction();
    SetUserAction(runAction);
}

//When running in MT we need to instantiate each individual build
//step->event->Run
//Adds edep - total edep per event - run
void MyActionInitialization::Build() const {

    auto *generator = new MyPrimaryGenerator();
    SetUserAction(generator);

    auto *runAction = new MyRunAction();
    SetUserAction(runAction);

    auto *eventAction = new MyEventAction(runAction);
    SetUserAction(eventAction);

    auto *steppingAction = new MySteppingAction(eventAction);
    SetUserAction(steppingAction);

}
