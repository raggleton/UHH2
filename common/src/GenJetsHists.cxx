#include "UHH2/common/include/GenJetsHists.h"

using namespace uhh2;
using namespace std;

GenJetsHists::GenJetsHists(Context & ctx,
			   const std::string & dname,
			   const unsigned int NumberOfPlottedJets,
			   const std::string & collection_,
         bool useRapidity_):
      Hists(ctx, dname),
      collection(collection_),
      useRapidity(useRapidity_) {
    number = book<TH1F>("number","number of genjets",21, -.5, 20.5);
    alljets = book_ParticleHist("genjet","_genjet",20,1500);
    vector<double> minPt {0,0,0,0};
    vector<double> maxPt {1500,1000,500,350};
    vector<string> axis_suffix {"first jet","second jet","third jet","fourth jet"};
    for(unsigned int i =0; i<NumberOfPlottedJets; i++){
      if(i<4){
        single_ParticleHists.push_back(book_ParticleHist(axis_suffix[i],"_"+to_string(i+1),minPt[i],maxPt[i]));
      }
      else {
        single_ParticleHists.push_back(book_ParticleHist(to_string(i+1)+"-th jet","_"+to_string(i+1),20,500));
      }
    }
    if(!collection.empty()){
        h_jets = ctx.get_handle<std::vector<GenJetWithParts> >(collection);
    }

}
void GenJetsHists::fill_ParticleHist(const GenJetWithParts & jet, ParticleHist & particle_hist, double  weight){
  particle_hist.pt->Fill(jet.pt(), weight);
  if (useRapidity)
    particle_hist.eta->Fill(jet.eta(), weight);
  else
    particle_hist.eta->Fill(jet.Rapidity(), weight);
  particle_hist.phi->Fill(jet.phi(), weight);
  particle_hist.mass->Fill(jet.v4().M(), weight);
  particle_hist.ptVsEta->Fill(jet.eta(), jet.pt(), weight);
}
GenJetsHists::ParticleHist GenJetsHists::book_ParticleHist(const string & axisSuffix, const string & histSuffix, double minPt, double maxPt){
  ParticleHist particle_hist;
  particle_hist.pt = book<TH1F>("pt"+histSuffix,"p_{T} "+axisSuffix,200,minPt,maxPt);
  string etaLabel = (useRapidity) ? "y" : "#eta";
  particle_hist.eta = book<TH1F>("eta"+histSuffix,etaLabel+" "+axisSuffix,100,-5,5);
  particle_hist.phi = book<TH1F>("phi"+histSuffix,"#phi "+axisSuffix,50,-M_PI,M_PI);
  particle_hist.mass = book<TH1F>("mass"+histSuffix,"M^{ "+axisSuffix+"} [GeV/c^{2}]", 100, 0, 300);
  particle_hist.ptVsEta = book<TH2F>("ptVsEta"+histSuffix, etaLabel+" "+axisSuffix+";p_{T} "+axisSuffix, 100, -5, 5, 200, minPt, maxPt);
  return particle_hist;
}

void GenJetsHists::fill(const uhh2::Event & event){
  if(event.isRealData) return;
  auto w = event.weight;
  if (!collection.empty() && !event.is_valid(h_jets)){
    cerr<<collection<<" is invalid. Going to abort from GenJetsHists class"<<endl;
    assert(1==0);
  }
  vector<GenJetWithParts> jets = collection.empty() ?  *event.genjets : event.get(h_jets);
  number->Fill(jets.size(), w);
  for(unsigned int i = 0; i <jets.size(); i++){
    const auto & jet = jets[i];
    fill_ParticleHist(jet,alljets,w);
    if(i < single_ParticleHists.size()){
      fill_ParticleHist(jet, single_ParticleHists[i], w);
    }
  }
}
