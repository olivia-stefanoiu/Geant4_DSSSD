#include "construction.hh"
#include <string>

MyDetectorConstruction::MyDetectorConstruction() {

    env_sizeXY   = 0.3 * m;
    env_sizeZ    = 0.5 * m;
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
        for (int strip = 0; strip < 16; strip++)  // renamed s -> strip to avoid shadowing CLHEP::s
        {
            int globalIndex = q * 16 + strip;

            int correctedIndex = globalIndex;
            if (globalIndex >= 32)
                correctedIndex = 63 - globalIndex;

            G4double stripInnerR = dE_R0 + (correctedIndex%16) * dE_H;
            G4double stripOuterR = stripInnerR + dE_Hactif;

            G4String solidName  = "solid_dE_q"  + std::to_string(q) + "_s" + std::to_string(strip);
            G4String logicName  = "logic_dE_q"  + std::to_string(q) + "_s" + std::to_string(strip);
            G4String physName   = "phys_dE_q"   + std::to_string(q) + "_s" + std::to_string(strip);

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

    return physWorld;
}

void MyDetectorConstruction::ConstructSDandField()
{
    auto *sensDet = new MySensitiveDetector("dE_strips");

    for (auto *lv : fDeltaEStrips)
        lv->SetSensitiveDetector(sensDet);
}