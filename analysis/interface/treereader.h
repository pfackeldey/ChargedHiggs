#ifndef TREEREADER_H
#define TREEREADER_H

#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <map> 	

#include <thread>
#include <mutex>
#include <chrono>
#include <pthread.h>

#include <TROOT.h>
#include <TLorentzVector.h>
#include <TFile.h>
#include <TH1F.h>
#include <TTree.h>
#include <TChain.h>
#include <TTreeReader.h>
#include <TTreeReaderValue.h>

#include <ChargedHiggs/nanoAOD_processing/interface/quantities.h> 
#include <ChargedHiggs/nanoAOD_processing/interface/jet.h> 
#include <ChargedHiggs/nanoAOD_processing/interface/electron.h> 
#include <ChargedHiggs/nanoAOD_processing/interface/muon.h> 


class TreeReader {
    
    enum Processes {DY, QCD, TT, T, DATA, VV, WJ, SIGNAL};

    //Struct for reading out tree
    struct Event{ 
        //Initially given       
        std::vector<Electron> electrons;
        std::vector<Muon> muons;
        std::vector<Jet> jets;
        Quantities quantities;
        TLorentzVector MET;

        //Reconstructed during processing of the event
        TLorentzVector h1;
        TLorentzVector h2;
        TLorentzVector W;
        TLorentzVector Hc;
    };

    //typedef for pointer to member functions
    typedef float(TreeReader::*parameterFunc)(Event &event);
    typedef bool(TreeReader::*cut)(Event &event);

    //Struct for histogram configuration
    struct Hist{
        float nBins;
        float xMin;
        float xMax;
        std::string xLabel;
        parameterFunc parameterValue;
    };

    private:
        int progress = 0;
        int nCores;

        //Needed parameter set in the constructor
        std::string process;
        std::vector<std::string> parameters;
        std::vector<std::string> cutstrings;

        //Class for locking thread unsafe operation
        std::mutex mutex;

        //Final histograms
        std::vector<TH1F*> mergedHistograms;

        //Helper function
        std::map<std::string, Hist> histValues; 
        std::map<std::string, cut> cutValues; 
        std::map<std::string, Processes> procDic;                   

        //Setup fill in treereaderfunction.cc        
        void SetHistMap();
        void SetCutMap();
    
        //Helper function
        void ProgressBar(int progress);
        float GetWeight(Event &event, std::vector<float> &weights);

        //Loop for each thread
        void ParallelisedLoop(const std::vector<TChain*> &v, const int &entryStart, const int &entryEnd, const float &nGen);

        //Function for reconstruct objects
        void WBoson(Event &event);
        void Higgs(Event &event);

        //Functions for cuts 

        bool Baseline(Event &event);
        bool ZeroJet(Event &event);
        bool FourJet(Event &event);

        //Functions for calculating quantities
        float WBosonMT(Event &event);
        float WBosonPhi(Event &event);
        float WBosonPT(Event &event);
        float ElectronPT(Event &event);
        float ElectronPhi(Event &event);
        float ElectronEta(Event &event);
        float MuonPT(Event &event);
        float MuonPhi(Event &event);
        float MuonEta(Event &event);
        float Jet1PT(Event &event);
        float Jet1Phi(Event &event);
        float Jet1Eta(Event &event);
        float Jet2PT(Event &event);
        float Jet2Phi(Event &event);
        float Jet2Eta(Event &event);
        float nJet(Event &event);
        float nLooseBJet(Event &event);
        float nMediumBJet(Event &event);
        float nTightBJet(Event &event);
        float HT(Event &event);
        float MET(Event &event);
        float METPhi(Event &event);
        float higgs1Mass(Event &event);
        float higgs2Mass(Event &event);
        float higgs1PT(Event &event);
        float higgs2PT(Event &event);
        float cHiggsPT(Event &event);
        float cHiggsMT(Event &event);
        float dPhih1h2(Event &event);


    public:
        TreeReader();
        TreeReader(std::string &process, std::vector<std::string> &parameters, std::vector<std::string> &cutstrings);
        void EventLoop(std::vector<std::string> &filenames);
        void Write(std::string &outname);
};


#endif
