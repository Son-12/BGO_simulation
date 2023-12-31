#include "CalorHit.hh"

#include "G4UnitsTable.hh"
#include "G4VVisManager.hh"
#include "G4Circle.hh"
#include "G4Colour.hh"
#include "G4VisAttributes.hh"

#include "G4AttDefStore.hh"
#include "G4AttDef.hh"
#include "G4AttValue.hh"
#include "G4UIcommand.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"

#include <iomanip>



//G4ThreadLocal G4Allocator<CalorHit>* CalorHitAllocator = nullptr;
G4ThreadLocal G4Allocator<CalorHit>* CalorHitAllocator = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CalorHit::CalorHit()
:G4VHit(),
fEdep(0.),
fTrackLength(0.)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CalorHit::~CalorHit() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CalorHit::CalorHit(const CalorHit& right)
  : G4VHit()
{
  fEdep        = right.fEdep;
  fTrackLength = right.fTrackLength;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const CalorHit& CalorHit::operator=(const CalorHit& right)
{
  fEdep        = right.fEdep;
  fTrackLength = right.fTrackLength;

  return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// G4bool CalorHit::operator==(const CalorHit& right) const
// {
//   //G4cout << "///---G4bool CalorHit::operator==(const CalorHit& right) const---///" << G4endl;
//   return ( this == &right ) ? true : false;
// }

G4int CalorHit::operator==(const CalorHit& right) const
{
  //G4cout << "///---G4bool CalorHit::operator==(const CalorHit& right) const---///" << G4endl;
  return ( this == &right ) ? 1 : 0;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CalorHit::Print()
{
  G4cout
     << "Edep: "
     << std::setw(7) << G4BestUnit(fEdep,"Energy")
     << " track length: "
     << std::setw(7) << G4BestUnit( fTrackLength,"Length")
     << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
