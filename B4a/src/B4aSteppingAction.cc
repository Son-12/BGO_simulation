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
/// \file B4aSteppingAction.cc
/// \brief Implementation of the B4aSteppingAction class

#include "B4aSteppingAction.hh"
#include "B4aEventAction.hh"
#include "B4DetectorConstruction.hh"

#include "G4Step.hh"
#include "G4RunManager.hh"

#include "G4SystemOfUnits.hh"

#include "TROOT.h"
#include "TH1.h"
#include "TTree.h"
#include "TFile.h"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4aSteppingAction::B4aSteppingAction(
    const B4DetectorConstruction *detectorConstruction,
    B4aEventAction *eventAction)
    : G4UserSteppingAction(),
      fDetConstruction(detectorConstruction),
      fEventAction(eventAction)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4aSteppingAction::~B4aSteppingAction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4aSteppingAction::UserSteppingAction(const G4Step *step)
{

  // Collect energy and track length step by step

  // get volume of the current step
  auto volume = step->GetPreStepPoint()->GetTouchableHandle()->GetVolume();

  // make track object, in order to confirm the tracking status
  G4Track *track = step->GetTrack();

  // initialize particle id and particle name
  int p_id = 0;
  p_id = track->GetParticleDefinition()->GetPDGEncoding(); //GetParticleDefinitionID();
  G4String p_name = track->GetParticleDefinition()->GetParticleName();

  //-------------- Get the all paticle edep, length -----------------//
  // energy deposit
  auto edep = step->GetTotalEnergyDeposit();

  // step length
  G4double stepLength = 0.;

  // //-------------- Get the primary particle edep, length ------------//
  if (step->GetTrack()->GetTrackID() == 1) // select primary particle
  {

    if (step->GetTrack()->GetDefinition()->GetPDGCharge() != 0.)
    {
      stepLength = step->GetStepLength();
    }

    if (volume == fDetConstruction->GetAbsorberPV()) // GetAbsorberPVはAbsoPVのポインタを返すだけ。
    {
      // confirm the pi- is existing in the absorber(GetNameをするとAbsoPVポインタの名前が返ってくる)
      //  G4cout << "pi- is existing in " << fDetConstruction->GetAbsorberPV()->GetName() << G4endl;
      fEventAction->AddAbs(edep, stepLength); // +=edep
    }
  }
  // //-------------- Get the primary particle edep, length ------------//

  // Get the vector of secondary particles in the current step
  const std::vector<const G4Track *> *secondaries = step->GetSecondaryInCurrentStep(); // stepの終了時に生成された2次粒子を含むG4Track型のvectorを返す。
  
  // G4int second_p_n = secondaries->size();
  // G4cout << "second particle number(second_p_n) is " << second_p_n <<G4endl;

  // // get the secondary particle //
  // if (track->GetTrackID() != 1)
  //   return;

  // //pi- stop events
  // if (track->GetTrackStatus() == fStopAndKill)
  // {

    G4double second_P_E = 0 * MeV; // secondary particle energies
    G4ThreeVector neutrondirection;
    neutrondirection = G4ThreeVector(0 * mm, 0 * mm, 0 * mm);

    // Loop through the secondary particles
    G4int secosize = secondaries->size(); // 2次粒子の数(vectorであるsecondariesの要素数を返す)
    //G4cout << "number of secondary particles (secondaries->size) ==== " << secosize << G4endl;

    for (size_t i = 0; i < secosize; ++i)
    {
      // Get the G4Track object for each secondary particle
      const G4Track *secondaryTrack = (*secondaries)[i];
      G4int parentID = secondaryTrack->GetParentID();

      // Get the G4ParticleDefinition object for the secondary particle
      const G4ParticleDefinition *particleDef = secondaryTrack->GetDefinition();

      G4int s_p_id = particleDef->GetPDGEncoding(); //second particle id 

      // Get the information of the secondary particle
      G4String seco_particleName = particleDef->GetParticleName();
      p_id = particleDef->GetParticleDefinitionID(); //これはsecond particleの外でloop回しているもの。
      second_P_E = secondaryTrack->GetKineticEnergy();
      
      fEventAction->AddSecondEnergy(second_P_E); //void AddSecondEnergy(G4double energy) { secondEnergy->push_back(energy); }

      G4cout << "////parentID///// " << parentID << G4endl;
      G4cout << "secondry particleName is " << seco_particleName << " " <<"kinetic Enenrgy = " << second_P_E << " MeV" << G4endl;
      G4cout << "////----- particle id is -----////" << p_id << "      /////s_p_id/////" << s_p_id << G4endl;
      G4cout << G4endl;

      // case of secondary particle is neutron
      if (seco_particleName == "neutron")
      {
        // G4cout << "secondry particleName is " << seco_particleName << G4endl;
        neutrondirection = secondaryTrack->GetMomentumDirection();
        fEventAction->AddNeutronEnergy(second_P_E);
      }

      // case of secondary particle is proton
      if (seco_particleName == "proton")
      {
        // G4cout << "secondry particleName is " << seco_particleName << G4endl;
        fEventAction->AddProtonEnergy(second_P_E);
      }

      // case of secondary particle is gamma
      if (seco_particleName == "gamma")
      {
        fEventAction->AddGammaEnergy(second_P_E);
      }

    } // loop for number of second particle
  //  }   // if track is kill

  // G4cout << "///////////////Usersteppingaction end////////////////" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
