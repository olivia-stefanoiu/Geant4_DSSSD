#include "hit.hh"

// define the thread-local allocator
G4ThreadLocal G4Allocator<MyHit>* MyHitAllocator = nullptr;

MyHit::MyHit() {}
MyHit::~MyHit() {}
