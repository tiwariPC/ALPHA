// -*- C++ -*-
//
// Package:    Analysis/HHAnalyzer
// Class:      HHAnalyzer
// 
/**\class HHAnalyzer HHAnalyzer.cc Analysis/HHAnalyzer/plugins/HHAnalyzer.cc
 Description: [one line class summary]
 Implementation:     [Notes on implementation]
*/
//
// Original Author:  Alberto Zucchetta
// Created:  Thu, 28 Apr 2016 08:28:54 GMT
// Modified for HH analysis - Sept 2016
//

#include "HHAnalyzer.h"

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
HHAnalyzer::HHAnalyzer(const edm::ParameterSet& iConfig):
    GenPSet(iConfig.getParameter<edm::ParameterSet>("genSet")),
    PileupPSet(iConfig.getParameter<edm::ParameterSet>("pileupSet")),
    TriggerPSet(iConfig.getParameter<edm::ParameterSet>("triggerSet")),
    ElectronPSet(iConfig.getParameter<edm::ParameterSet>("electronSet")),
    MuonPSet(iConfig.getParameter<edm::ParameterSet>("muonSet")),
    JetPSet(iConfig.getParameter<edm::ParameterSet>("jetSet")),
    FatJetPSet(iConfig.getParameter<edm::ParameterSet>("fatJetSet")),
    WriteNJets(iConfig.getParameter<int>("writeNJets")),
    WriteNFatJets(iConfig.getParameter<int>("writeNFatJets")),
    HistFile(iConfig.getParameter<std::string>("histFile")),
    Verbose(iConfig.getParameter<bool>("verbose"))
{
    //now do what ever initialization is needed
    usesResource("TFileService");
    
    // Initialize Objects
    theGenAnalyzer      = new GenAnalyzer(GenPSet, consumesCollector());
    thePileupAnalyzer   = new PileupAnalyzer(PileupPSet, consumesCollector());
    theTriggerAnalyzer  = new TriggerAnalyzer(TriggerPSet, consumesCollector());
    theElectronAnalyzer = new ElectronAnalyzer(ElectronPSet, consumesCollector());
    theMuonAnalyzer     = new MuonAnalyzer(MuonPSet, consumesCollector());
    theJetAnalyzer      = new JetAnalyzer(JetPSet, consumesCollector());
    theFatJetAnalyzer   = new JetAnalyzer(FatJetPSet, consumesCollector());
    
    std::vector<std::string> TriggerList(TriggerPSet.getParameter<std::vector<std::string> >("paths"));
    for(unsigned int i = 0; i < TriggerList.size(); i++) TriggerMap[ TriggerList[i] ] = false;
        
    // ---------- Plots Initialization ----------
    TFileDirectory allDir=fs->mkdir("All/");
    TFileDirectory genDir=fs->mkdir("Gen/");
    TFileDirectory jetDir=fs->mkdir("Jets/");
    
    // Make TH1F
    std::vector<std::string> nLabels={"All (jets in Acc)", "Trigger", "# jets >3", "# med b-tag >0", "# med b-tag >1", "# med b-tag >2", "# med b-tag >3", "", "", ""};
    
    int nbins;
    float min, max;
    std::string name, title, opt;
    
    ifstream histFile(HistFile);
    if(!histFile.is_open()) {
        throw cms::Exception("HHAnalyzer Analyzer", HistFile + " file not found");
    }
    while(histFile >> name >> title >> nbins >> min >> max >> opt) {
        if(name.find('#')==std::string::npos) {
            while(title.find("~")!=std::string::npos) title=title.replace(title.find("~"), 1, " "); // Remove ~
            if(name.substr(0, 2)=="a_") Hist[name] = allDir.make<TH1F>(name.c_str(), title.c_str(), nbins, min, max); //.substr(2)
            if(name.substr(0, 2)=="g_") Hist[name] = genDir.make<TH1F>(name.c_str(), title.c_str(), nbins, min, max);
            if(name.substr(0, 2)=="j_") Hist[name] = jetDir.make<TH1F>(name.c_str(), title.c_str(), nbins, min, max);
            Hist[name]->Sumw2();
            Hist[name]->SetOption(opt.c_str());
            // Particular histograms
            if(name=="a_nEvents" || name=="e_nEvents" || name=="m_nEvents") for(unsigned int i=0; i<nLabels.size(); i++) Hist[name]->GetXaxis()->SetBinLabel(i+1, nLabels[i].c_str());
        }
    }
    histFile.close();

    std::cout << "---------- STARTING ----------" << std::endl;
}


HHAnalyzer::~HHAnalyzer() {
    // do anything here that needs to be done at desctruction time
    // (e.g. close files, deallocate resources etc.)
    std::cout << "---------- ENDING  ----------" << std::endl;
    
    delete theGenAnalyzer;
    delete thePileupAnalyzer;
    delete theTriggerAnalyzer;
    delete theElectronAnalyzer;
    delete theMuonAnalyzer;
    delete theJetAnalyzer;
    delete theFatJetAnalyzer;
}


//
// member functions
//

// ------------ method called for each event  ------------
void HHAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
    isMC = !iEvent.isRealData();
    EventNumber = iEvent.id().event();
    LumiNumber = iEvent.luminosityBlock();
    RunNumber = iEvent.id().run();
    
    EventWeight = StitchWeight = ZewkWeight = WewkWeight = PUWeight = TriggerWeight = LeptonWeight = 1.;
    FacWeightUp = FacWeightDown = RenWeightUp = RenWeightDown = ScaleWeightUp = ScaleWeightDown = 1.;
    nPV = nElectrons = nMuons = nJets = nFatJets = nBTagJets = -1.;
    nVetoElectrons = nLooseMuons = 0;
    MaxJetBTag = MaxFatJetBTag = -1.;
    MinJetMetDPhi = 10.;

    // Initialize types
    for(int i = 0; i < WriteNFatJets; i++) ObjectsFormat::ResetFatJetType(FatJets[i]);
    ObjectsFormat::ResetMEtType(MEt);
    
    Hist["a_nEvents"]->Fill(1., EventWeight);
    
    // -----------------------------------
    //           READ OBJECTS
    // -----------------------------------
    
    // Pu weight
    PUWeight = thePileupAnalyzer->GetPUWeight(iEvent);
    nPV = thePileupAnalyzer->GetPV(iEvent);
    Hist["a_nPVNoWeight"]->Fill(nPV, EventWeight);
    EventWeight *= PUWeight;
    Hist["a_nPVReWeight"]->Fill(nPV, EventWeight);
    
    // Trigger
    theTriggerAnalyzer->FillTriggerMap(iEvent, TriggerMap);
    EventWeight *= TriggerWeight;
    
    // Electrons
    std::vector<pat::Electron> ElecVect = theElectronAnalyzer->FillElectronVector(iEvent);
    nElectrons = ElecVect.size();
    for(unsigned int i =0; i<ElecVect.size(); i++){
        if(ElecVect.at(i).userInt("isVeto")==1) nVetoElectrons++;
    }
    // Muons
    std::vector<pat::Muon> MuonVect = theMuonAnalyzer->FillMuonVector(iEvent);
    nMuons = MuonVect.size();
    std::vector<pat::Muon> LooseMuonVect;
    for(unsigned int i =0; i<MuonVect.size(); i++){
        if(MuonVect.at(i).userInt("isLoose")==1){
	    LooseMuonVect.push_back(MuonVect.at(i));
            nLooseMuons++;
	}
    }
    // Jets
    std::vector<pat::Jet> JetsVect = theJetAnalyzer->FillJetVector(iEvent);
    theJetAnalyzer->CleanJetsFromMuons(JetsVect, MuonVect, 0.4);
    theJetAnalyzer->CleanJetsFromElectrons(JetsVect, ElecVect, 0.4);
    nJets = JetsVect.size();
    nBTagJets = theJetAnalyzer->GetNBJets(JetsVect);
    // Fat Jets
    std::vector<pat::Jet> FatJetsVect = theFatJetAnalyzer->FillJetVector(iEvent);
    //theFatJetAnalyzer->CleanJetsFromMuons(FatJetsVect, MuonVect, 1.); // Do NOT clean the fatjet now
    //theFatJetAnalyzer->CleanJetsFromElectrons(FatJetsVect, ElecVect, 1.); // Do NOT clean the fatjet now
    nFatJets = FatJetsVect.size();
    // Missing Energy
    pat::MET MET = theJetAnalyzer->FillMetVector(iEvent);
    pat::MET Neutrino(MET);
    float metNoMupx = MET.px();
    float metNoMupy = MET.py();
    for(unsigned int i=0; i<LooseMuonVect.size();i++){
      metNoMupx -= LooseMuonVect.at(i).px();
      metNoMupy -= LooseMuonVect.at(i).py();
    }
    reco::Particle::LorentzVector metNoMup4(metNoMupx, metNoMupy, 0, 0 );
    MET.addUserFloat("metNoMu",metNoMup4.px());
    MET.addUserFloat("phiNoMu",metNoMup4.phi());

    //theJetAnalyzer->ApplyRecoilCorrections(MET, &MET.genMET()->p4(), &theV.p4(), 0);
    
    // -----------------------------------
    //           GEN LEVEL
    // -----------------------------------
    
    // Gen weights
    std::map<std::string, float> GenWeight = theGenAnalyzer->FillWeightsMap(iEvent);
    EventWeight *= GenWeight["event"];
    if(GenWeight.find("2") != GenWeight.end()) FacWeightUp     = GenWeight["2"];
    if(GenWeight.find("3") != GenWeight.end()) FacWeightDown   = GenWeight["3"];
    if(GenWeight.find("4") != GenWeight.end()) RenWeightUp     = GenWeight["4"];
    if(GenWeight.find("7") != GenWeight.end()) RenWeightDown   = GenWeight["7"];
    if(GenWeight.find("5") != GenWeight.end()) ScaleWeightUp   = GenWeight["5"];
    if(GenWeight.find("9") != GenWeight.end()) ScaleWeightDown = GenWeight["9"];
    // LHE Particles
    std::map<std::string, float> LheMap = theGenAnalyzer->FillLheMap(iEvent);
    // MC Stitching
    StitchWeight = theGenAnalyzer->GetStitchWeight(LheMap);
    //EventWeight *= StitchWeight; // Not yet
    // Gen Particles
    std::vector<reco::GenParticle> GenPVect = theGenAnalyzer->FillGenVector(iEvent);
    // Gen candidates
    reco::Candidate* theGenZ = theGenAnalyzer->FindGenParticle(GenPVect, 23);
    reco::Candidate* theGenW = theGenAnalyzer->FindGenParticle(GenPVect, 24);
    // EWK corrections
    if(theGenZ) ZewkWeight = theGenAnalyzer->GetZewkWeight(theGenZ->pt());
    if(theGenW) WewkWeight = theGenAnalyzer->GetWewkWeight(theGenW->pt());
    
    EventWeight *= ZewkWeight * WewkWeight;

    std::vector<reco::GenParticle> GenHsPart;
    std::vector<reco::GenParticle> GenBFromHsPart = theGenAnalyzer->PartonsFromDecays({25}, GenHsPart);


    // --- Trigger selection ---
    // selection (if required) is made with HLT Analyzer (called before ntuple Analyzer)
    Hist["a_nEvents"]->Fill(2., EventWeight);
   
    // ---------- Event Variables ----------    
    // Max b-tagged jet in the event
    for(unsigned int i = 0; i < JetsVect.size(); i++) if(JetsVect[i].bDiscriminator(JetPSet.getParameter<std::string>("btag")) > MaxJetBTag) MaxJetBTag = JetsVect[i].bDiscriminator(JetPSet.getParameter<std::string>("btag"));
    // Max b-tagged jet in the event
    for(unsigned int i = 0; i < JetsVect.size(); i++) if(FatJetsVect.size() > 0 && JetsVect[i].bDiscriminator(JetPSet.getParameter<std::string>("btag")) > MaxFatJetBTag && deltaR(FatJetsVect.at(0), JetsVect[i])>0.8) MaxFatJetBTag = JetsVect[i].bDiscriminator(JetPSet.getParameter<std::string>("btag"));
    
    for(unsigned int i = 0; i < JetsVect.size(); i++) if(fabs(reco::deltaPhi(JetsVect[i].phi(), MET.phi())) < MinJetMetDPhi) MinJetMetDPhi = fabs(reco::deltaPhi(JetsVect[i].phi(), MET.phi()));
    
    // Jet variables
    theJetAnalyzer->AddVariables(JetsVect, MET);
    theFatJetAnalyzer->AddVariables(FatJetsVect, MET);
    // Leptons
    theElectronAnalyzer->AddVariables(ElecVect, MET);
    theMuonAnalyzer->AddVariables(MuonVect, MET);

    // ---------- Fill objects ----------
    Electrons.clear();
    for(unsigned int i = 0; i < ElecVect.size(); i++) {
      Electrons.emplace_back();
      ObjectsFormat::FillElectronType(Electrons[i], &ElecVect[i], isMC);
    }
    Muons.clear();
    for(unsigned int i = 0; i < MuonVect.size(); i++) {
      Muons.emplace_back();
      ObjectsFormat::FillMuonType(Muons[i], &MuonVect[i], isMC);
    }
    alp::convert(JetsVect, Jets);
    ObjectsFormat::FillMEtType(MEt, &MET, isMC);

    // fill sorting vectors
    j_sort_pt = std::vector<std::size_t>(Jets.size());
    std::iota(j_sort_pt.begin(), j_sort_pt.end(), 0);
    auto pt_comp = [&](std::size_t a, std::size_t b) {return Jets.at(a).pt() > Jets.at(b).pt();};
    std::sort(j_sort_pt.begin(), j_sort_pt.end(), pt_comp ); 

    j_sort_csv = std::vector<std::size_t>(Jets.size());
    std::iota(j_sort_csv.begin(), j_sort_csv.end(), 0);
    auto csv_comp = [&](std::size_t a, std::size_t b) {return Jets.at(a).CSV() > Jets.at(b).CSV();};
    std::sort(j_sort_csv.begin(), j_sort_csv.end(), csv_comp);

    j_sort_cmva = std::vector<std::size_t>(Jets.size());
    std::iota(j_sort_cmva.begin(), j_sort_cmva.end(), 0);
    auto cmva_comp = [&](std::size_t a, std::size_t b) {return Jets.at(a).CMVA() > Jets.at(b).CMVA();};
    std::sort(j_sort_cmva.begin(), j_sort_cmva.end(), cmva_comp);

    // fill b quarks from higgs
    GenBFromHs.clear();
    for(unsigned int i = 0; i < GenBFromHsPart.size(); i++) {
      GenBFromHs.emplace_back();
      ObjectsFormat::FillLorentzType(GenBFromHs[i], &GenBFromHsPart.at(i).p4());
    }

    // fill b quarks from higgs
    GenHs.clear();
    for(unsigned int i = 0; i < GenHsPart.size(); i++) {
      GenHs.emplace_back();
      ObjectsFormat::FillLorentzType(GenHs[i], &GenHsPart.at(i).p4());
    }

    // --- Fill nEvents histogram --- no effective selection applied
    // --- num jets selection ---
    if(JetsVect.size() > 3) {
      Hist["a_nEvents"]->Fill(3., EventWeight);

      // --- b-Tag selection ---
      if( Jets.at(j_sort_csv[0]).CSV() > 0.800) Hist["a_nEvents"]->Fill(4., EventWeight);
      if( Jets.at(j_sort_csv[1]).CSV() > 0.800) Hist["a_nEvents"]->Fill(5., EventWeight);
      if( Jets.at(j_sort_csv[2]).CSV() > 0.800) Hist["a_nEvents"]->Fill(6., EventWeight);
      if( Jets.at(j_sort_csv[3]).CSV() > 0.800) Hist["a_nEvents"]->Fill(7., EventWeight);      
    }
    
    // Fill tree
    tree->Fill();

}


// ------------ method called once each job just before starting event loop  ------------
void HHAnalyzer::beginJob() {
    
    // Object objects are created only one in the begin job. The reference passed to the branch has to be the same
    for(int i = 0; i < WriteNFatJets; i++) FatJets.push_back( FatJetType() );
    
    // Create Tree and set Branches
    tree=fs->make<TTree>("tree", "tree");
    tree->Branch("isMC", &isMC, "isMC/O");
    tree->Branch("EventNumber", &EventNumber, "EventNumber/L");
    tree->Branch("LumiNumber", &LumiNumber, "LumiNumber/L");
    tree->Branch("RunNumber", &RunNumber, "RunNumber/L");
    tree->Branch("EventWeight", &EventWeight, "EventWeight/F");
    tree->Branch("FacWeightUp", &FacWeightUp, "FacWeightUp/F");
    tree->Branch("FacWeightDown", &FacWeightDown, "FacWeightDown/F");
    tree->Branch("RenWeightUp", &RenWeightUp, "RenWeightUp/F");
    tree->Branch("RenWeightDown", &RenWeightDown, "RenWeightDown/F");
    tree->Branch("ScaleWeightUp", &ScaleWeightUp, "ScaleWeightUp/F");
    tree->Branch("ScaleWeightDown", &ScaleWeightDown, "ScaleWeightDown/F");
    tree->Branch("StitchWeight", &StitchWeight, "StitchWeight/F");
    tree->Branch("ZewkWeight", &ZewkWeight, "ZewkWeight/F");
    tree->Branch("WewkWeight", &WewkWeight, "WewkWeight/F");
    tree->Branch("PUWeight", &PUWeight, "PUWeight/F");
    tree->Branch("TriggerWeight", &TriggerWeight, "TriggerWeight/F");
    tree->Branch("LeptonWeight", &LeptonWeight, "LeptonWeight/F");
    
    // Set trigger branches
    for(auto it = TriggerMap.begin(); it != TriggerMap.end(); it++) tree->Branch(it->first.c_str(), &(it->second), (it->first+"/O").c_str());
    
    // Objects
    tree->Branch("nPV", &nPV, "nPV/I");
    tree->Branch("nElectrons", &nElectrons, "nElectrons/I");
    tree->Branch("nVetoElectrons", &nVetoElectrons, "nVetoElectrons/I");
    tree->Branch("nMuons", &nMuons, "nMuons/I");
    tree->Branch("nLooseMuons", &nLooseMuons, "nLooseMuons/I");
    tree->Branch("nJets", &nJets, "nJets/I");
    tree->Branch("nFatJets", &nFatJets, "nFatJets/I");
    tree->Branch("nBTagJets", &nBTagJets, "nBTagJets/I");    
    tree->Branch("MaxJetBTag", &MaxJetBTag, "MaxJetBTag/F");
    tree->Branch("MaxFatJetBTag", &MaxFatJetBTag, "MaxFatJetBTag/F");
    tree->Branch("MinJetMetDPhi", &MinJetMetDPhi, "MinJetMetDPhi/F");
  
    // Set Branches for objects
    // save vector of electron, muon and jets
    tree->Branch("Electrons", &(Electrons), 64000, 2);
    tree->Branch("Muons", &(Muons), 64000, 2);
    // save vector of jets
    tree->Branch("Jets", &(Jets), 64000, 2);
    for(int i = 0; i < WriteNFatJets; i++) tree->Branch(("FatJet"+std::to_string(i+1)).c_str(), &(FatJets[i].pt), ObjectsFormat::ListFatJetType().c_str());
    tree->Branch("MEt", &MEt.pt, ObjectsFormat::ListMEtType().c_str());
    tree->Branch("GenBFromHs", &(GenBFromHs), 64000, 2);
    tree->Branch("GenHs", &(GenHs), 64000, 2);

    tree->Branch("j_sort_pt", &(j_sort_pt), 64000, 2);
    tree->Branch("j_sort_csv", &(j_sort_csv), 64000, 2);
    tree->Branch("j_sort_cmva", &(j_sort_cmva), 64000, 2);
}

// ------------ method called once each job just after ending the event loop  ------------
void HHAnalyzer::endJob() {
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void HHAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    //The following says we do not know what parameters are allowed so do no validation
    // Please change this to state exactly what you do use, even if it is no parameters
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
}


//define this as a plug-in
DEFINE_FWK_MODULE(HHAnalyzer);
