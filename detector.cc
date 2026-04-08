#include "detector.hh"

MySensitiveDetector::MySensitiveDetector(G4String name) : G4VSensitiveDetector(name) {}

MySensitiveDetector::~MySensitiveDetector() {}

G4bool MySensitiveDetector::ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist) {


     G4cout << "=== ProcessHits called! edep = " 
           << aStep->GetTotalEnergyDeposit() << " ===" << G4endl;

    // skip steps with no energy deposition
    //any touch counts as an interaction, but we can pass through the volume without energy depositon
    G4double edep = aStep->GetTotalEnergyDeposit();
    if (edep == 0.) return false;

    //aStep is just naming convention
    G4StepPoint *preStepPoint = aStep->GetPreStepPoint();

    // actual hit position in global coordinates
    G4ThreeVector hitPos = preStepPoint->GetPosition();

    // which detector was hit (0-63 = dE strips, 100-115 = E sectors)
    const G4VTouchable *touchable = aStep->GetPreStepPoint()->GetTouchable();
    G4int copyNo = touchable->GetCopyNumber();

    // event ID
    G4int evt = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();

    G4AnalysisManager *man = G4AnalysisManager::Instance();

    man->FillNtupleIColumn(0, 0, evt);
    man->FillNtupleIColumn(0, 1, copyNo);
    man->FillNtupleDColumn(0, 2, hitPos[0]);
    man->FillNtupleDColumn(0, 3, hitPos[1]);
    man->FillNtupleDColumn(0, 4, hitPos[2]);
    man->AddNtupleRow(0);

    return true;
}