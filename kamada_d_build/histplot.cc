#include "TROOT.h"
#include "TH1.h"
#include "TH2.h"
#include "TTree.h"
#include "TFile.h"
#include <vector>

int evID;
double fEnergyAbs;
std::vector<double> *neutronEnergy;
std::vector<double> *protonEnergy;
std::vector<double> *gammaEnergy;
std::vector<double> *secondEnergy;

void histplot()
{
    gROOT->Reset();
    gROOT->SetStyle("Plain");

    // Create a canvas and divide it into 2x2 pads
    TCanvas *c1 = new TCanvas("c1", "c1", 20, 20, 1000, 1000);
    c1->Divide(3, 2);

    TFile *f = new TFile("test.root");
    TTree *tree = (TTree *)f->Get("tree");

    int NumEvents = tree->GetEntries();

    tree->SetBranchAddress("eveno", &evID);
    tree->SetBranchAddress("pi_edep", &fEnergyAbs);
    tree->SetBranchAddress("neutron_E", &neutronEnergy);
    tree->SetBranchAddress("proton_E", &protonEnergy);
    tree->SetBranchAddress("gamma_E", &gammaEnergy);
    tree->SetBranchAddress("secoP_E", &secondEnergy);

    int e_NumBin = 160;
    int e_MinBin = 0;
    int e_MaxBin = 160;

    TH1D *event_n_h = new TH1D("event_n_h", "event_n_h", e_NumBin, e_MinBin, e_MaxBin);
    TH1D *pi_edep_h = new TH1D("pi_edep_h", "pi_edep_h", e_NumBin, e_MinBin, e_MaxBin);
    TH1D *neutron_energy_h = new TH1D("neutron_energy_h", "neutron_energy_h", e_NumBin, e_MinBin, e_MaxBin);
    TH1D *proton_energy_h = new TH1D("proton_energy_h", "proton_energy_h", e_NumBin, e_MinBin, e_MaxBin);
    TH1D *gamma_energy_h = new TH1D("gamma_energy_h", "gamma_energy_h", e_NumBin, e_MinBin, e_MaxBin);
    TH1D *second_energy_h = new TH1D("second_energy_h", "second_energy_h", e_NumBin, e_MinBin, e_MaxBin);

    for (size_t i = 0; i < NumEvents; i++)
    {
        tree->GetEntry(i);
        event_n_h->Fill(evID);
        pi_edep_h->Fill(fEnergyAbs);

        size_t v_n_size = neutronEnergy->size();    // Get vector size
        double *a_neutron_E = new double[v_n_size]; // define array for size(1イベントごと要素分の配列を用意する)
        for (size_t j = 0; j < v_n_size; j++) //配列分だけloopを回す
        {
            a_neutron_E[j] = (*neutronEnergy)[j]; // vectorから配列に要素をコピー
            neutron_energy_h->Fill(a_neutron_E[j]);
        }
        delete[] a_neutron_E; // メモリ解放

        size_t v_p_size = protonEnergy->size();
        double *a_proton_E = new double[v_p_size];
        for (size_t j = 0; j < v_p_size; j++)
        {
            a_proton_E[j] = (*protonEnergy)[j]; // vectorから配列に要素をコピー
            proton_energy_h->Fill(a_proton_E[j]);
        }
        delete[] a_proton_E; // メモリ解放

        size_t v_g_size = gammaEnergy->size();
        double *a_gamma_E = new double[v_g_size];
        for (size_t j = 0; j < v_g_size; j++)
        {
            a_gamma_E[j] = (*gammaEnergy)[j]; // vectorから配列に要素をコピー
            gamma_energy_h->Fill(a_gamma_E[j]);
        }
        delete[] a_gamma_E; // メモリ解放

        size_t v_s_size = secondEnergy->size();
        double *a_second_E = new double[v_s_size];
        for (size_t j = 0; j < v_s_size; j++)
        {
            a_second_E[j] = (*secondEnergy)[j]; // vectorから配列に要素をコピー
            second_energy_h->Fill(a_second_E[j]);
        }
        delete[] a_second_E; // メモリ解放
    }

    // Draw hist
    c1->cd(1);
    event_n_h->Draw();

    c1->cd(2);
    pi_edep_h->Draw();

    c1->cd(3);
    neutron_energy_h->Draw();

    c1->cd(4);
    proton_energy_h->Draw();

    c1->cd(5);
    gamma_energy_h->Draw();

    c1->cd(6);
    second_energy_h->Draw();

}