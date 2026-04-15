#include "construction.hh"
#include "G4SDManager.hh"
#include <string>

MyDetectorConstruction::MyDetectorConstruction() {


    env_sizeXY   = 0.15 * m;
    env_sizeZ    = 1.0 * m;
    world_sizeXY = 1.2 * env_sizeXY;
    world_sizeZ  = 1.2 * env_sizeZ;

    fMessenger = new G4GenericMessenger(this, "/detector/", "Detector Construction");
}

MyDetectorConstruction::~MyDetectorConstruction() {
    delete fMessenger;
}

G4VPhysicalVolume *MyDetectorConstruction::Construct() {
    DefineMaterials();
    return DefineVolumes();
}

void MyDetectorConstruction::DefineMaterials() {
    G4NistManager *nist = G4NistManager::Instance();

    world_mat = nist->FindOrBuildMaterial("G4_AIR");
    env_mat   = nist->FindOrBuildMaterial("G4_AIR");
    silicon   = nist->FindOrBuildMaterial("G4_Si");
}

G4VPhysicalVolume *MyDetectorConstruction::DefineVolumes() {

    // ── World ──────────────────────────────────────────────────────────────
    solidWorld = new G4Box("World",
                           0.5 * world_sizeXY,
                           0.5 * world_sizeXY,
                           0.5 * world_sizeZ);

    logicWorld = new G4LogicalVolume(solidWorld, world_mat, "World");

    G4VPhysicalVolume *physWorld =
        new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World",
                          0, false, 0, checkOverlaps);

    // ── Envelope ───────────────────────────────────────────────────────────
    solidEnv = new G4Box("Envelope",
                         0.5 * env_sizeXY,
                         0.5 * env_sizeXY,
                         0.5 * env_sizeZ);

    logicEnv = new G4LogicalVolume(solidEnv, env_mat, "Envelope");

    new G4PVPlacement(0, G4ThreeVector(), logicEnv, "Envelope",
                      logicWorld, false, 0, checkOverlaps);

    // ── ΔE DSSSD (300 μm, 64 strips, 4 quadrants x 16) ───────────────────
    //
    // Strip numbering:
    //   Quadrant 0 (top-left,   phi 90->180):  strips  0-15,  ascending outward
    //   Quadrant 1 (top-right,  phi  0-> 90):  strips 16-31,  ascending outward
    //   Quadrant 2 (bot-right,  phi 270->360): strips 32-47,  DESCENDING outward
    //   Quadrant 3 (bot-left,   phi 180->270): strips 48-63,  DESCENDING outward
    //
    // For quadrants 2 & 3 (strips 32-63) the wiring is physically inverted:
    //   correctedIndex = 63 - globalIndex

    constexpr G4double dE_R0        = 24.0  * mm;
    constexpr G4double dE_H         = 1.505 * mm;
    constexpr G4double dE_Hactif    = 1.425 * mm;
    constexpr G4double dE_thickness = 0.300 * mm;
    constexpr G4double dE_posZ      = 0.0   * mm;
    constexpr G4double quadWidth    = 90.0  * deg;

    const G4double quadPhiStart[4] = {
        90.0 * deg,   // Q0: top-left
         0.0 * deg,   // Q1: top-right
       270.0 * deg,   // Q2: bottom-right
       180.0 * deg,   // Q3: bottom-left
    };

    fDeltaEStrips.clear();
    fDeltaEStrips.reserve(64);

    for (int q = 0; q < 4; q++)
    {
        for (int strip = 0; strip < 16; strip++)
        {
            int globalIndex = q * 16 + strip;

            int correctedIndex = globalIndex;
            if (globalIndex >= 32)
                correctedIndex = 63 - globalIndex;

            G4double stripInnerR = dE_R0 + (correctedIndex % 16) * dE_H;
            G4double stripOuterR = stripInnerR + dE_Hactif;

            G4String solidName = "solid_dE_q" + std::to_string(q) + "_s" + std::to_string(strip);
            G4String logicName = "logic_dE_q" + std::to_string(q) + "_s" + std::to_string(strip);
            G4String physName  = "phys_dE_q"  + std::to_string(q) + "_s" + std::to_string(strip);

            G4Tubs* solidStrip = new G4Tubs(
                solidName,
                stripInnerR,
                stripOuterR,
                dE_thickness / 2.0,
                quadPhiStart[q],
                quadWidth
            );

            G4LogicalVolume* logicStrip = new G4LogicalVolume(
                solidStrip,
                silicon,
                logicName
            );

            new G4PVPlacement(
                0,
                G4ThreeVector(0, 0, dE_posZ),
                logicStrip,
                physName,
                logicEnv,
                false,
                globalIndex,   // copy number = raw strip index (0-63)
                checkOverlaps
            );

            fDeltaEStrips.push_back(logicStrip);
        }
    }

    fScoringVolume = fDeltaEStrips[0];

    /* ── E DSSSD COMMENTED OUT ──────────────────────────────────────────────
    // ── E DSSSD (1500 μm, 16 sectors) ─────────────────────────────────────
    //
    // Same inner/outer radii as dE.
    // 16 sectors of 22.5 deg each, CCW from 0.
    // Copy number = sector index (0-15).

    constexpr G4double E_innerR     = dE_R0;                    // 24.0 mm
    constexpr G4double E_outerR     = dE_R0 + 16 * dE_H;        // 48.08 mm
    constexpr G4double E_thickness  = 1.500 * mm;
    constexpr G4double E_posZ       = 50.0  * mm;               // adjust to your beam-line
    constexpr G4double sectorWidth  = 22.5  * deg;              // 360/16

    fESectors.clear();
    fESectors.reserve(16);

    for (int sec = 0; sec < 16; sec++)
    {
        G4String solidName = "solid_E_sec_" + std::to_string(sec);
        G4String logicName = "logic_E_sec_" + std::to_string(sec);
        G4String physName  = "phys_E_sec_"  + std::to_string(sec);

        G4Tubs* solidSector = new G4Tubs(
            solidName,
            E_innerR,
            E_outerR,
            E_thickness / 2.0,
            sec * sectorWidth +0.1*deg,   // startPhi, CCW from 0
            sectorWidth-0.1*deg          // deltaPhi
        );

        G4LogicalVolume* logicSector = new G4LogicalVolume(
            solidSector,
            silicon,
            logicName
        );

        new G4PVPlacement(
            0,
            G4ThreeVector(0, 0, E_posZ),
            logicSector,
            physName,
            logicEnv,
            false,
            sec,           // copy number = sector index (0-15)
            checkOverlaps
        );

        fESectors.push_back(logicSector);
    }
    */ // ── END E DSSSD COMMENTED OUT ──────────────────────────────────────

    std::cout<<"CONSTRUCT.CC"<<'\n';
    return physWorld;
}

void MyDetectorConstruction::ConstructSDandField()
{
    std::cout<<"CONSTRUCTION.CC CONSTRUCTFIELD"<<'\n';
    // dE strips — one SD instance, copy number tells you which strip fired
    auto *sensDet_dE = new MySensitiveDetector("dE_strips");

    // OLD VERSION (missing this line — caused "is not found" crash):
    // auto *sensDet_dE = new MySensitiveDetector("dE_strips");
    // for (auto *lv : fDeltaEStrips)
    //     lv->SetSensitiveDetector(sensDet_dE);

    // NEW VERSION — register with G4SDManager so the hits collection
    // can be found by name in EndOfEventAction
    G4SDManager::GetSDMpointer()->AddNewDetector(sensDet_dE);

    for (auto *lv : fDeltaEStrips)
        lv->SetSensitiveDetector(sensDet_dE);

    // E sectors — commented out
    // auto *sensDet_E = new MySensitiveDetector("E_sectors");
    // for (auto *lv : fESectors)
    //     lv->SetSensitiveDetector(sensDet_E);
}