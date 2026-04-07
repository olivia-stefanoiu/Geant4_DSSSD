#include <iostream>

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisManager.hh"

#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "construction.hh"
#include "physics.hh"
#include "ActionInitialization.hh"


int main(int argc, char **argv) {

    // construct the default run manager
    auto *runManager = new G4RunManager();

    // set mandatory initialization classes
    runManager->SetUserInitialization(new MyDetectorConstruction());
    runManager->SetUserInitialization(new MyPhysicsList());
    runManager->SetUserInitialization(new MyActionInitialization()); //use particle init.

    // initialize G4 kernel
    runManager->Initialize();

   //G4UImanager is the command interpreter. It processes Geant4 commands like /run/beamOn 1000 or /control/execute run.mac
    G4UIExecutive *ui = nullptr;
    if(argc == 1)
    {
        ui = new G4UIExecutive(argc, argv);
    }

    //G4VisManager is purely the graphics engine. It handles OpenGL rendering, drawing the detector geometry, drawing particle tracks, setting colors and visibility of volumes etc. It needs the UImanager to receive its commands (like /vis/open OGL
    G4VisManager *visManager= new G4VisExecutive();
    visManager->Initialize();
    G4UImanager *UImanager = G4UImanager::GetUIpointer();

    if(ui)
    {
        UImanager->ApplyCommand("/control/execute vis.mac");
        ui->SessionStart();
    }

    else
    {
        G4String command = "/control/execute";
        G4String fileName = argv[1];
        UImanager->ApplyCommand(command + fileName);
    }

    delete visManager;
    delete runManager;
    return 0;
}

