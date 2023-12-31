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
/// \file B4aEventAction.hh
/// \brief Definition of the B4aEventAction class

#ifndef B4aEventAction_h
#define B4aEventAction_h 1

#include "G4UserEventAction.hh"
#include "globals.hh"
#include <vector>

#include "CalorHit.hh"

// #include "caloconstant.hh" //なんか定数みたいなのを入れると便利ぽい。

/// Event action class
///
/// It defines data members to hold the energy deposit and track lengths
/// of charged particles in Absober and Gap layers:
/// - fEnergyAbs, fEnergyGap, fTrackLAbs, fTrackLGap
/// which are collected step by step via the functions
/// - AddAbs(), AddGap()

class B4aEventAction : public G4UserEventAction
{
public:
  B4aEventAction();
  virtual ~B4aEventAction();

  virtual void BeginOfEventAction(const G4Event *event);
  virtual void EndOfEventAction(const G4Event *event);

  void AddAbs(G4double de, G4double dl);                                      // used steppingaction
  void AddGap(G4double de, G4double dl);                                      // used steppingaction
  //void AddNeutronEnergy(G4double energy) { if(neutronEnergy){neutronEnergy->push_back(energy);} } // used steppingaction
  void AddNeutronEnergy(G4double energy) {neutronEnergy->push_back(energy);} // used steppingaction
  void AddProtonEnergy(G4double energy) { protonEnergy->push_back(energy); }   // used steppingaction
  void AddGammaEnergy(G4double energy) { gammaEnergy->push_back(energy); }     // used steppingaction
  void AddSecondEnergy(G4double energy) { secondEnergy->push_back(energy); }   // used steppingaction

  G4double etot;
  std::vector<G4double> *neutronEnergy;
  std::vector<G4double> *protonEnergy;
  std::vector<G4double> *gammaEnergy;
  std::vector<G4double> *secondEnergy;

private:
  CalorHitsCollection* GetHitsCollection(G4int hcID,
                                            const G4Event* event) const;

  // data members
    G4int fAbsHCID = -1;
    G4int fGapHCID = -1;

  G4int evID;

  G4double edep_hit;

  G4double fEnergyAbs;
  G4double fEnergyGap;
  G4double fTrackLAbs;
  G4double fTrackLGap;

  
  

};

// inline functions

inline void B4aEventAction::AddAbs(G4double de, G4double dl)
{
  fEnergyAbs += de;
  fTrackLAbs += dl;
}

inline void B4aEventAction::AddGap(G4double de, G4double dl)
{
  fEnergyGap += de;
  fTrackLGap += dl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
