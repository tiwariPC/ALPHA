#ifndef OBJECTS_H
#define OBJECTS_H

struct LeptonType {
LeptonType(): pt(-1.), eta(-9.), phi(-9.), mass(-1.), energy(-1.), charge(0), pdgId(0), pfIso03(-1.), pfIso04(-1.), trkIso(-1.), miniIso(-1.), dxy(-99.), dz(-99.), ip3d(-99.), sip3d(-99.), nPixelHits(-1.), dPhi_met(-1.), isElectron(false), isMuon(false), isVeto(false), isLoose(false), isMedium(false), isTight(false), isHighPt(false), isTrackerHighPt(false), isMatched(false) {} // isHEEP(false), isMVANonTrigMedium(false), isMVANonTrigTight(false), isMVATrigMedium(false), isMVATrigTight(false)
    float pt;
    float eta;
    float phi;
    float mass;
    float energy;
    int charge;
    int pdgId;
    float pfIso03;
    float pfIso04;
    float trkIso;
    float miniIso;
    float dxy;
    float dz;
    float ip3d;
    float sip3d;
    float nPixelHits;
    float dPhi_met;
    bool isElectron;
    bool isMuon;
    bool isVeto;
    bool isLoose;
    bool isMedium;
    bool isTight;
//    bool isHEEP;
//    bool isMVANonTrigMedium;
//    bool isMVANonTrigTight;
//    bool isMVATrigMedium;
//    bool isMVATrigTight;
    bool isHighPt;
    bool isTrackerHighPt;
    bool isMatched;
};

struct PhotonType {
PhotonType(): pt(-1.), eta(-9.), phi(-9.), mass(-1.), energy(-1.), charge(0), pdgId(0), pfIso(-1.), dz(-99.), isLoose(false), isMedium(false), isTight(false), isMVANonTrigMedium(false), isMatched(false) {}
    float pt;
    float eta;
    float phi;
    float mass;
    float energy;
    int charge;
    int pdgId;
    float pfIso;
    float dz;
    bool isLoose;
    bool isMedium;
    bool isTight;
    bool isMVANonTrigMedium;
    bool isMatched;
};

struct TauType {
TauType(): pt(-1.), eta(-9.), phi(-9.), mass(-1.), energy(-1.), charge(0), pdgId(0), pfIso(-1.), dz(-99.), isLoose(false), isMedium(false), isTight(false), isMatched(false) {}
    float pt;
    float eta;
    float phi;
    float mass;
    float energy;
    int charge;
    int pdgId;
    float pfIso;
    float dz;
    bool isLoose;
    bool isMedium;
    bool isTight;
    bool isMatched;
};


struct JetType {
    JetType(): pt(-1.), eta(-9.), phi(-9.), mass(-1.), energy(-1.), ptRaw(-1.), ptUnc(-1.), dPhi_met(-1.), dPhi_jet1(-1.), puId(-1.), CSV(-99.), CSVR(-99.), chf(-1.), nhf(-1.), phf(-1.), elf(-1.), muf(-1.), chm(-1), npr(-1), flavour(0), mother(0), isLoose(false), isMedium(false), isTight(false), isTightLepVeto(false), isCSVL(false), isCSVM(false), isCSVT(false), isMatched(false), QGLikelihood(-1.) {}
    float pt;
    float eta;
    float phi;
    float mass;
    float energy;
    float ptRaw;
    float ptUnc;
    float dPhi_met;
    float dPhi_jet1;
    float puId;
    float CSV;
    float CSVR;
    float chf;
    float nhf;
    float phf;
    float elf;
    float muf;
    int chm;
    int npr;
    int flavour;
    int mother;
    bool isLoose;
    bool isMedium;
    bool isTight;
    bool isTightLepVeto;
    bool isCSVL;
    bool isCSVM;
    bool isCSVT;
    bool isMatched;
    float QGLikelihood;
};



struct FatJetType {
FatJetType(): pt(-1.), eta(-9.), phi(-9.), mass(-1.), energy(-1.), ptRaw(-1.), ptUnc(-1.), dPhi_met(-1.), dPhi_jet1(-1.), puId(-1.), CSV(-99.), CSVR(-99.), chf(-1.), nhf(-1.), phf(-1.), elf(-1.), muf(-1.), chm(-1), npr(-1), flavour(0), mother(0), isLoose(false), isMedium(false), isTight(false), isTightLepVeto(false), isMatched(false), prunedMass(-1.), softdropMass(-1.), softdropPuppiMass(-1.), prunedMassCorr(-1.), softdropMassCorr(-1.), softdropPuppiMassCorr(-1.), pt1(-1.), eta1(-9.), phi1(-9.), mass1(-1.), CSV1(-99.), CSVR1(-99.), flavour1(-1.), pt2(-1.), eta2(-9.), phi2(-9.), mass2(-1.), CSV2(-99.), CSVR2(-99.), flavour2(-1.), dR(-1.), tau21(-1.) {}
    float pt;
    float eta;
    float phi;
    float mass;
    float energy;
    float ptRaw;
    float ptUnc;
    float dPhi_met;
    float dPhi_jet1;
    float puId;
    float CSV;
    float CSVR;
    float chf;
    float nhf;
    float phf;
    float elf;
    float muf;
    int chm;
    int npr;
    int flavour;
    int mother;
    bool isLoose;
    bool isMedium;
    bool isTight;
    bool isTightLepVeto;
    bool isMatched;
    float prunedMass;
    float softdropMass;
    float softdropPuppiMass;
    float prunedMassCorr;
    float softdropMassCorr;
    float softdropPuppiMassCorr;
    float pt1;
    float eta1;
    float phi1;
    float mass1;
    float CSV1;
    float CSVR1;
    float flavour1;
    float pt2;
    float eta2;
    float phi2;
    float mass2;
    float CSV2;
    float CSVR2;
    float flavour2;
    float dR;
    float tau21;
};


//struct MEtType {
//    MEtType(): pt(-1.), eta(-9.), phi(-9.), sign(-1.) {}
//    float pt;
//    float eta;
//    float phi;
//    float sign;
//};

struct MEtType {
    MEtType(): pt(-1.), eta(-9.), phi(-9.), sign(-1.), ptRaw(-1.), phiRaw(-9.), ptType1(-1.), phiType1(-9.), ptGen(-1.), phiGen(-9.), ptScaleUp(-1.), ptScaleDown(-1.), ptResUp(-1.), ptResDown(-1.) {}
    float pt;
    float eta;
    float phi;
    float sign;
    float ptRaw;
    float phiRaw;
    float ptType1;
    float phiType1;
    float ptGen;
    float phiGen;
    float ptScaleUp;
    float ptScaleDown;
    float ptResUp;
    float ptResDown;
};

struct MEtFullType {
    MEtFullType(): pt(-1.), eta(-9.), phi(-9.), sign(-1.), ptRaw(-1.), phiRaw(-9.), ptGen(-1.), phiGen(-9.), ptJERUp(-1.), ptJERDown(-1.), ptJESUp(-1.), ptJESDown(-1.), ptMUSUp(-1.), ptMUSDown(-1.), ptELSUp(-1.), ptELSDown(-1.), ptTAUUp(-1.), ptTAUDown(-1.), ptUNCUp(-1.), ptUNCDown(-1.), ptPHOUp(-1.), ptPHODown(-1.), phf(-1.), nhf(-1.), elf(-1.), chf(-1.), muf(-1.) {}
    float pt;
    float eta;
    float phi;
    float sign;
    float ptRaw;
    float phiRaw;
    float ptGen;
    float phiGen;
    float ptJERUp;
    float ptJERDown;
    float ptJESUp;
    float ptJESDown;
    float ptMUSUp;
    float ptMUSDown;
    float ptELSUp;
    float ptELSDown;
    float ptTAUUp;
    float ptTAUDown;
    float ptUNCUp;
    float ptUNCDown;
    float ptPHOUp;
    float ptPHODown;
    float phf;
    float nhf;
    float elf;
    float chf;
    float muf;
};


struct CandidateType {
    CandidateType(): pt(-1.), eta(-9.), phi(-9.), mass(-1.), tmass(-1.), dR(-1.), dEta(-1.), dPhi(-1.), twist(-1.), isMatched(false) {} // angle(-1.), ptBalance(-9.), centrality(-1.), charge(0), 
    float pt;
    float eta;
    float phi;
    float mass;
    float tmass;
//    float tmassScaleUp;
//    float tmassScaleDown;
//    float tmassResUp;
//    float tmassResDown;
    float dR;
    float dEta;
    float dPhi;
    float twist;
  //  float angle;
  //  float ptBalance;
  //  float centrality;
  //  int charge;
    bool isMatched;
};





struct LorentzType {
  LorentzType(): pt(-1.), eta(-9.), phi(-9.), energy(-1.), mass(-1.) {}
  float pt;
  float eta;
  float phi;
  float energy;
  float mass;
};

struct EventType {
  EventType(): id1(-9), id2(-9), x1(-1.), x2(-1.), Q(-1.) {}
  int id1;
  int id2;
  float x1;
  float x2;
  float Q;
};

#endif

