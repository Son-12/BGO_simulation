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
/// \file B4DetectorConstruction.cc
/// \brief Implementation of the B4DetectorConstruction class

#include "B4DetectorConstruction.hh"
#include "CalorimeterSD.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4SDManager.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal
    G4GlobalMagFieldMessenger *B4DetectorConstruction::fMagFieldMessenger = nullptr;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4DetectorConstruction::B4DetectorConstruction()
    : G4VUserDetectorConstruction(),
      fAbsorberPV(nullptr),
      fGapPV(nullptr),
      fCheckOverlaps(true)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B4DetectorConstruction::~B4DetectorConstruction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume *B4DetectorConstruction::Construct()
{
  // Define materials
  DefineMaterials();

  // Define volumes
  return DefineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4DetectorConstruction::DefineMaterials()
{
  // Lead material defined using NIST Manager
  auto nistManager = G4NistManager::Instance();
  nistManager->FindOrBuildMaterial("G4_Pb");
  nistManager->FindOrBuildMaterial("G4_BGO"); // 必要??
  nistManager->FindOrBuildMaterial("G4_AIR"); // 必要??

  // Liquid argon material
  G4double a; // mass of a mole;
  G4double z; // z=mean number of protons;
  G4double density;
  new G4Material("liquidArgon", z = 18., a = 39.95 * g / mole, density = 1.390 * g / cm3);
  // The argon by NIST Manager is a gas with a different density

  // Vacuum
  new G4Material("Galactic", z = 1., a = 1.01 * g / mole, density = universe_mean_density,
                 kStateGas, 2.73 * kelvin, 3.e-18 * pascal);

  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume *B4DetectorConstruction::DefineVolumes()
{
  // Geometry parameters
  G4int nofLayers = 1; // 230727これ後で消すべき。
  G4double absoThickness = 1. * mm;
  //G4double absoThickness = 200. * mm;
  // G4double gapThickness =  5.*mm;
  G4double calorSizeXY = 1. * mm;
  G4double calorThickness = 1. * mm;
  // G4double calorSizeXY = 50. * mm;
  // G4double calorThickness = 50. * mm;
  auto worldSizeXY = 100. * cm;
  auto worldSizeZ = 100. * cm;

  // Get materials
  auto defaultMaterial = G4Material::GetMaterial("Galactic");
  auto absorberMaterial = G4Material::GetMaterial("G4_BGO");
  auto gapMaterial = G4Material::GetMaterial("liquidArgon");
  auto air = G4Material::GetMaterial("G4_AIR");
  auto bgo = G4Material::GetMaterial("G4_BGO");

  // if ( ! defaultMaterial || ! absorberMaterial || ! gapMaterial ) {
  //   G4ExceptionDescription msg;
  //   msg << "Cannot retrieve materials already defined.";
  //   G4Exception("B4DetectorConstruction::DefineVolumes()",
  //     "MyCode0001", FatalException, msg);
  // }

  /////////////////
  //    World    //
  /////////////////
  auto worldS = new G4Box("World",                                           // its name
                          worldSizeXY / 2, worldSizeXY / 2, worldSizeZ / 2); // its size

  auto worldLV = new G4LogicalVolume(
      worldS,          // its solid
      defaultMaterial, // its material
      "World");        // its name

  auto worldPV = new G4PVPlacement(
      0,               // no rotation
      G4ThreeVector(), // at (0,0,0)
      worldLV,         // its logical volume
      "World",         // its name
      0,               // its mother  volume
      false,           // no boolean operation
      0,               // copy number
      fCheckOverlaps); // checking overlaps

  /////////////////////
  //   Calorimeter   //
  /////////////////////
  auto calorimeterS = new G4Box("Calorimeter",                                         // its name
                                calorSizeXY / 2, calorSizeXY / 2, calorThickness / 2); // its size

  auto calorLV = new G4LogicalVolume(
      calorimeterS,    // its solid
      defaultMaterial, // its material
      "Calorimeter");  // its name

  new G4PVPlacement(
      0,               // no rotation
      G4ThreeVector(), // at (0,0,0)
      calorLV,         // its logical volume
      "Calorimeter",   // its name
      worldLV,         // its mother  volume
      false,           // no boolean operation
      0,               // copy number
      fCheckOverlaps); // checking overlaps

  

   /////////////////////
  // Absorber of BGO //
  /////////////////////
  auto absorberS = new G4Box("Abso",                                               // its name
                             calorSizeXY / 2, calorSizeXY / 2, absoThickness / 2); // its size

  auto absorberLV = new G4LogicalVolume(
      absorberS,        // its solid
      absorberMaterial, // its material
      "Abso");          // its name

  fAbsorberPV = new G4PVPlacement(
      0,                                        // no rotation
      G4ThreeVector(),                          // its position
      absorberLV,                               // its logical volume
      "Abso",                                   // its name
      calorLV,                                  // its mother  volume
      false,                                    // no boolean operation
      0,                                        // copy number
      fCheckOverlaps);                          // checking overlaps

  
  //
  // print parameters
  //
  G4cout
      << G4endl
      << "------------------------------------------------------------" << G4endl
      << "---> The calorimeter is " << nofLayers << " layers of: [ "
      << absoThickness / mm << "mm of " << absorberMaterial->GetName()
      << " + "
      // << gapThickness / mm << "mm of " << gapMaterial->GetName() << " ] " << G4endl
      << "------------------------------------------------------------" << G4endl;

  //
  // Visualization attributes
  //
  worldLV->SetVisAttributes(G4VisAttributes::GetInvisible());

  auto simpleBoxVisAtt = new G4VisAttributes(G4Colour(1.0, 1.0, 1.0));
  simpleBoxVisAtt->SetVisibility(true);
  calorLV->SetVisAttributes(simpleBoxVisAtt);

  //
  // Always return the physical World
  //
  return worldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B4DetectorConstruction::ConstructSDandField()
{

  //
  // Sensitive detectors
  //
  auto absoSD = new CalorimeterSD("AbsorberSD", "AbsorberHitsCollection");//, fNofLayers); //fNofLayrsの引数絶対要らない。
  G4SDManager::GetSDMpointer()->AddNewDetector(absoSD);
  SetSensitiveDetector("Abso",absoSD);

  // Create global magnetic field messenger.
  // Uniform magnetic field is then created automatically if
  // the field value is not zero.
  G4ThreeVector fieldValue;
  fMagFieldMessenger = new G4GlobalMagFieldMessenger(fieldValue);
  fMagFieldMessenger->SetVerboseLevel(1);

  // Register the field messenger for deleting
  G4AutoDelete::Register(fMagFieldMessenger);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
