#ifndef HIT_HH
#define HIT_HH

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"

class MyHit : public G4VHit
{
public:
    MyHit();
    ~MyHit() override;

    // memory management via G4Allocator (fast pool allocation for hits)
    inline void *operator new(size_t);
    inline void  operator delete(void *);

    // setters
    void SetEdep(G4double edep)           { fEdep = edep; }
    void SetPosition(G4ThreeVector pos)   { fPos  = pos;  }
    void SetCopyNo(G4int copyNo)          { fCopyNo = copyNo; }
    void SetTime(G4double time)           { fTime = time; }
    void SetParticleName(G4String name)   { fParticleName = name; }
    void SetTrackID(G4int id)             { fTrackID = id; }

    // getters
    G4double       GetEdep()         const { return fEdep; }
    G4ThreeVector  GetPosition()     const { return fPos;  }
    G4int          GetCopyNo()       const { return fCopyNo; }
    G4double       GetTime()         const { return fTime; }
    G4String       GetParticleName() const { return fParticleName; }
    G4int          GetTrackID()      const { return fTrackID; }

private:
    G4double      fEdep = 0.;
    G4ThreeVector fPos;
    G4int         fCopyNo = -1;
    G4double      fTime = 0.;
    G4String      fParticleName = "";
    G4int         fTrackID = -1;
};

// typedef for the hits collection
typedef G4THitsCollection<MyHit> MyHitsCollection;

// G4Allocator — declared extern here, defined in hit.cc
extern G4ThreadLocal G4Allocator<MyHit>* MyHitAllocator;

inline void* MyHit::operator new(size_t)
{
    if (!MyHitAllocator)
        MyHitAllocator = new G4Allocator<MyHit>;
    return (void *) MyHitAllocator->MallocSingle();
}

inline void MyHit::operator delete(void *hit)
{
    MyHitAllocator->FreeSingle((MyHit*) hit);
}

#endif
