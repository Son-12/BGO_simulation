/// \file B4/B4c/src/CalorimeterSD.cc
/// \brief Implementation of the B4c::CalorimeterSD class

#include "CalorimeterSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CalorimeterSD::CalorimeterSD(const G4String& name,
                             const G4String& hitsCollectionName)//,
                             //G4int nofCells)
 : G4VSensitiveDetector(name),
   fHitsCollection(nullptr)
   //fNofCells(nofCells)
{
  collectionName.insert(hitsCollectionName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CalorimeterSD::~CalorimeterSD() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CalorimeterSD::Initialize(G4HCofThisEvent* hce)
{
 // G4cout << "///---void CalorimeterSD::Initialize(G4HCofThisEvent* hce)----///" << G4endl;
  // Create hits collection
  fHitsCollection
    = new CalorHitsCollection(SensitiveDetectorName, collectionName[0]);

  // Add this collection in hce
  auto hcID
    = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection( hcID, fHitsCollection );

//G4cout << "///---fNofCells----/// " << fNofCells << G4endl;
  // Create hits
  // fNofCells for cells + one more for total sums
  for (G4int i=0; i</*fNofCells+*/1; i++ ) {
    //G4cout << "///---loop for fNofCells---///" << G4endl;
    fHitsCollection->insert(new CalorHit());
  }
 // G4cout << "///---void CalorimeterSD::Initialize(G4HCofThisEvent* hce)----/// initialize end" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool CalorimeterSD::ProcessHits(G4Step* step,
                                     G4TouchableHistory*)
{
  // energy deposit
  auto edep = step->GetTotalEnergyDeposit();

  // step length
  G4double stepLength = 0.;
  if ( step->GetTrack()->GetDefinition()->GetPDGCharge() != 0. ) {
    stepLength = step->GetStepLength();
  }

  if ( edep==0. && stepLength == 0. ) return false;

  auto touchable = (step->GetPreStepPoint()->GetTouchable());
  
  // Get calorimeter cell id
  //auto layerNumber = touchable->GetReplicaNumber(1);
  //auto hitID = どうしよう。

  // Get hit accounting data for this cell
  // auto hit = (*fHitsCollection)[layerNumber];
  // if ( ! hit ) {
  //   G4ExceptionDescription msg;
  //   msg << "Cannot access hit " << layerNumber;
  //   G4Exception("CalorimeterSD::ProcessHits()",
  //     "MyCode0004", FatalException, msg);
  // }

  // Get hit for total accounting
  //G4cout <<"///---fHitsCollection->entries()---///" << "==" <<fHitsCollection->entries()  <<G4endl;
  
  auto hit
       = (*fHitsCollection)[fHitsCollection->entries()-1];
    // = (*fHitsCollection)[fHitsCollection->entries()-1];

  // Add values
  // hit->Add(edep, stepLength);
  hit->Add(edep, stepLength);

  //G4cout << "///---G4bool CalorimeterSD::ProcessHits----/// end ///" << G4endl;

  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CalorimeterSD::EndOfEvent(G4HCofThisEvent*)
{
  if ( verboseLevel>1 ) {
     auto nofHits = fHitsCollection->entries();
     G4cout
       << G4endl
       << "-------->Hits Collection: in this event they are " << nofHits
       << " hits in the tracker chambers: " << G4endl;
     for ( std::size_t i=0; i<nofHits; ++i ) (*fHitsCollection)[i]->Print();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
