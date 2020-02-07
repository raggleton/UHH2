#include <string>
#include <iostream>
#include <exception>
#include <vector>

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "Compression.h"


// Macro to split one Ntuple file into several output file
//
// Example usage:
//     root -b -q -l 'splitNtuple.C("Ntuple_1.root","Ntuple_1_split%d.root",3)'
//
// DO NOT PUT SPACES INBETWEEN THE ARGUMENTS
//
// Or with bash variables:
//     root -b -q -l 'splitNtuple.C("'${input}'","'${output}'",3)'
//
// Note that the output filename should have the %d pattern in it,
// which will be replaced by an index
//
void splitNtuple(const std::string & in_filename, const std::string & out_filename_template, int nfiles) {
    gROOT->ProcessLine("#include <vector>");

    if (out_filename_template.find("\%d") == std::string::npos) {
        throw std::runtime_error("out_filename_template must have a \%d");
    }

    TFile * f = new TFile(in_filename.c_str());
    if (!f || f->IsZombie()) {
        throw std::runtime_error("Cannot open input file");
    }
    TTree * tree = (TTree*)f->Get("AnalysisTree");
    if (!tree || tree->IsZombie()) {
        throw std::runtime_error("Cannot get TTree");
    }
    // Setup reading the triggerNames branch
    std::vector<std::string> *triggerNames = 0;
    tree->SetBranchAddress("triggerNames", &triggerNames);
    std::vector<std::string> *originalTriggerNames = 0;
    int run(-1);
    tree->SetBranchAddress("run", &run);

    int nentries = tree->GetEntriesFast();
    // figure out nentries per file, to get nfiles
    // we add on the modulus since normal division is floor
    int nentriesPerFile = (nentries + nentries%nfiles)/nfiles;
    cout << "Input file has " << nentries << " events" << endl;
    cout << "Each file will have " << nentriesPerFile <<  " events" << endl;
    int lastRun = -99;
    for (int i=0; i < nfiles; i++) {

        TFile * newfile = new TFile(TString::Format(out_filename_template.c_str(), i).Data(), "RECREATE");
        TTree * treeClone = tree->CloneTree(0);

        // We have to manually copy accross the triggerNames branch since it
        // is only filled in the original file every new run number
        // and all other runs have nothing
        std::vector<std::string> *newTriggerNames = 0;
        treeClone->SetBranchAddress("triggerNames", &newTriggerNames);

        for (int c=0; c<nentriesPerFile; c++) {
            int thisEvent = nentriesPerFile*i + c;
            if (thisEvent == nentries) break; // since it wont be an exact divisor
            tree->GetEntry(thisEvent);

            if (lastRun != run) {
                // Get the new original trigger names
                // Note that we only keep a copy whenever there's a new run
                // since the others in the original file all have empty vectors
                originalTriggerNames = new std::vector<std::string>(*triggerNames);
                lastRun = run;
                // use the new trigger names
                newTriggerNames = new std::vector<std::string>(*originalTriggerNames);
            } else {
                if (c == 0) {
                    // new file: write out last non-empty triggers
                    newTriggerNames = new std::vector<std::string>(*originalTriggerNames);
                }
                else {
                    // otherwise just use whatever the original tree has (which is empty)
                    // to save some space
                    newTriggerNames = new std::vector<std::string>(*triggerNames);
                }
            }

            treeClone->Fill();
        }
        newfile->Write();
        cout << "Written to " << newfile->GetName() << endl;
        newfile->Close();
    }
    f->Close();
}

