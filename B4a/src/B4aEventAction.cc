//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// 
/// \file B4aEventAction.cc
/// \brief Implementation of the B4aEventAction class

#include "B4aEventAction.hh"
#include "B4RunAction.hh"
#include "B4Analysis.hh"

#include "CalorimeterSD.hh"
#include "CalorHit.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"
#include <iomanip>

#include "TROOT.h"
#include "TTree.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4aEventAction::B4aEventAction()
 : G4UserEventAction(),
   edep_hit(0.),
   fEnergyAbs(0.),
   fEnergyGap(0.),
   fTrackLAbs(0.),
   fTrackLGap(0.),
   neutronEnergy(new std::vector<G4double>()),
   protonEnergy(new std::vector<G4double>()),
   gammaEnergy(new std::vector<G4double>()),
   secondEnergy(new std::vector<G4double>())

   
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4aEventAction::~B4aEventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CalorHitsCollection*
B4aEventAction::GetHitsCollection(G4int hcID,
                                  const G4Event* event) const
{
  auto hitsCollection
    = static_cast<CalorHitsCollection*>(
        event->GetHCofThisEvent()->GetHC(hcID));

  if ( ! hitsCollection ) {
    G4ExceptionDescription msg;
    msg << "Cannot access hitsCollection ID " << hcID;
    G4Exception("EventAction::GetHitsCollection()",
      "MyCode0003", FatalException, msg);
  }

  return hitsCollection;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4aEventAction::BeginOfEventAction(const G4Event* /*event*/)
{  
 // G4cout << "void B4aEventAction::BeginOfEventAction(const G4Event* /*event*/)" << G4endl;
  // initialisation per event
  evID = 0;
  edep_hit = 0. ;
  fEnergyAbs = 0.;
  fEnergyGap = 0.;
  fTrackLAbs = 0.;
  fTrackLGap = 0.;
  etot = 0.;

  //vector clear 
  neutronEnergy->clear();
  protonEnergy->clear();
  gammaEnergy->clear();
  secondEnergy->clear();
  

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4aEventAction::EndOfEventAction(const G4Event* event)
{
//G4cout << "void B4aEventAction::EndOfEventAction(const G4Event* event)" << G4endl;

  if ( fAbsHCID == -1 ) {
    fAbsHCID
      = G4SDManager::GetSDMpointer()->GetCollectionID("AbsorberHitsCollection");
  }

  // Get hits collections
  auto absoHC = GetHitsCollection(fAbsHCID, event);

  // Get hit with total values
  auto absoHit = (*absoHC)[absoHC->entries()-1];
  edep_hit = absoHit->GetEdep();

  

  TTree* tree = (TTree*)gROOT-> FindObject("tree");
  tree->SetBranchAddress("eveno"  ,&evID);

  tree->SetBranchAddress("pi_edep"  ,&fEnergyAbs);
  tree->SetBranchAddress("pi_edep_hit"  ,&edep_hit);

  evID = event->GetEventID();
  
  // neutronEnergyとprotonEnergyに保存されているエネルギーをヒストグラムにFill

  
    for (int i = 0; i < neutronEnergy->size(); i++)
    {
      tree->SetBranchAddress("neutron_E"	, &neutronEnergy);
    }

    for (int i = 0; i < protonEnergy->size(); i++)
    {
        tree->SetBranchAddress("proton_E"	, &protonEnergy);
    }

    for (int i = 0; i < gammaEnergy->size(); i++)
    {
        tree->SetBranchAddress("gamma_E"	, &gammaEnergy);
    }

    for (int i = 0; i < secondEnergy->size(); i++)
    {
        tree->SetBranchAddress("secoP_E"	, &secondEnergy);
    }

    tree->Fill();

  //Check the event end
  if( evID%10000 == 0 ){
    G4cout << "End of Event# : " << evID << G4endl;
    //getchar();
    }
  //delete neutronEnergy;
 
  // Accumulate statistics
  //

  // get analysis manager
  // auto analysisManager = G4AnalysisManager::Instance();

  // // fill histograms
  // analysisManager->FillH1(0, fEnergyAbs);
  // analysisManager->FillH1(1, fEnergyGap);
  // analysisManager->FillH1(2, fTrackLAbs);
  // analysisManager->FillH1(3, fTrackLGap);
  
  // // fill ntuple
  // analysisManager->FillNtupleDColumn(0, fEnergyAbs);
  // analysisManager->FillNtupleDColumn(1, fEnergyGap);
  // analysisManager->FillNtupleDColumn(2, fTrackLAbs);
  // analysisManager->FillNtupleDColumn(3, fTrackLGap);
  // analysisManager->AddNtupleRow();  
  
  // // Print per event (modulo n)
  // //
  // auto eventID = event->GetEventID();
  // auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
  // if ( ( printModulo > 0 ) && ( eventID % printModulo == 0 ) ) {
  //   G4cout << "---> End of event: " << eventID << G4endl;     

  //   G4cout
  //      << "   Absorber: total energy: " << std::setw(7)
  //                                       << G4BestUnit(fEnergyAbs,"Energy")
  //      << "       total track length: " << std::setw(7)
  //                                       << G4BestUnit(fTrackLAbs,"Length")
  //      << G4endl
  //      << "        Gap: total energy: " << std::setw(7)
  //                                       << G4BestUnit(fEnergyGap,"Energy")
  //      << "       total track length: " << std::setw(7)
  //                                       << G4BestUnit(fTrackLGap,"Length")
  //      << G4endl;
  // }
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
