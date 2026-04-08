#ifndef CONSTRUCTION_HH
#define CONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "G4SystemOfUnits.hh"

#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4NistManager.hh"
#include "G4GenericMessenger.hh"
#include "G4Material.hh"
#include "G4MultiUnion.hh"

#include "detector.hh"

#include <iostream>
#include <vector>

using namespace std;


class MyDetectorConstruction : public G4VUserDetectorConstruction {

public:
    MyDetectorConstruction();
    ~MyDetectorConstruction() override;

    G4LogicalVolume *GetScoringVolume() const { return fScoringVolume; }

    G4VPhysicalVolume *Construct() override;
    void ConstructSDandField() override;
    void BuildStrips();

protected:

    //ca sa nu mai inundam terminalul sunt ok :)
    G4bool checkOverlaps = false;

    // logical volumes
    G4LogicalVolume *fScoringVolume = nullptr;
    G4LogicalVolume *logicWorld     = nullptr;
    G4LogicalVolume *logicEnv       = nullptr;

    // solid geometry
    G4Box  *solidWorld = nullptr;
    G4Box  *solidEnv   = nullptr;

    // all 64 dE strip logical volumes
    std::vector<G4LogicalVolume*> fDeltaEStrips;

    // all 16 E sector logical volumes
    std::vector<G4LogicalVolume*> fESectors;

    G4GenericMessenger *fMessenger;

    G4double env_sizeXY, env_sizeZ;
    G4double world_sizeXY, world_sizeZ;

    // materials
    G4Material *world_mat = nullptr;
    G4Material *env_mat   = nullptr;
    G4Material *silicon   = nullptr;

    void DefineMaterials();
    G4VPhysicalVolume *DefineVolumes();
};

#endif