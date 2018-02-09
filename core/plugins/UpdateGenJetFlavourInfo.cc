/**
  \class    
  \brief    Update JetFlavourInfo in reco::GenJet collection from a JetFlavourInfoMatchingCollection
            
  \author   Robin Aggleton, robin.aggleton@cern.ch
*/


#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DataFormats/Common/interface/Association.h"
#include "DataFormats/Common/interface/RefToPtr.h"

#include "DataFormats/JetReco/interface/GenJet.h"
#include "SimDataFormats/JetMatching/interface/JetFlavourInfoMatching.h"



class UpdateGenJetFlavourInfo : public edm::stream::EDProducer<> {
  public:
    explicit UpdateGenJetFlavourInfo(const edm::ParameterSet & iConfig);
    ~UpdateGenJetFlavourInfo() override { }
    
    void produce(edm::Event & iEvent, const edm::EventSetup & iSetup) override;
    
  private:
    const edm::EDGetTokenT<edm::View<reco::GenJet> > genJetsToken_;
    const edm::EDGetTokenT<edm::View<reco::Jet> > recoJetsToken_;
    const edm::EDGetTokenT<reco::JetFlavourInfoMatchingCollection> jetFlavourInfosToken_;
};


UpdateGenJetFlavourInfo::UpdateGenJetFlavourInfo(const edm::ParameterSet & iConfig) :
    genJetsToken_(consumes<edm::View<reco::GenJet> >(iConfig.getParameter<edm::InputTag>("jetSrc"))),
    recoJetsToken_(consumes<edm::View<reco::Jet> >(iConfig.getParameter<edm::InputTag>("jetSrc"))),
    jetFlavourInfosToken_(consumes<reco::JetFlavourInfoMatchingCollection>(iConfig.getParameter<edm::InputTag>("jetFlavourInfos")))
{
    produces<reco::GenJetCollection>();
}

void 
UpdateGenJetFlavourInfo::produce(edm::Event & iEvent, const edm::EventSetup & iSetup) {
    using namespace edm;
    using namespace std;

    // we have 2 handles for the same collection
    // one is for a reco::GenJet view, to get a copy of the reco::GenJet
    // the other is a reco::Jet view, to get the ref used in the JetFlavourInfoMatchingCollection keys
    // (can't use the reco::GenJet ref it seems)
    // this only works so long as the jetSrc used here is the same as the one 
    // in the JetFlavourClustering module that made the JetFlavourInfoMatchingCollection
    Handle<View<reco::GenJet> > genJets;
    iEvent.getByToken(genJetsToken_, genJets);
    
    Handle<View<reco::Jet> > recoJets;
    iEvent.getByToken(recoJetsToken_, recoJets);

    Handle<reco::JetFlavourInfoMatchingCollection> jetFlavourInfos;
    iEvent.getByToken(jetFlavourInfosToken_, jetFlavourInfos);

    std::auto_ptr< reco::GenJetCollection > updatedJets ( new reco::GenJetCollection() );
    updatedJets->reserve(genJets->size());

    for (unsigned int i=0; i<genJets->size(); ++i) {
      reco::GenJet newJet(genJets->at(i));
      newJet.setPdgId(((*jetFlavourInfos)[recoJets->refAt(i)]).getPartonFlavour());
      updatedJets->push_back(newJet);
    }
    iEvent.put(updatedJets);
}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(UpdateGenJetFlavourInfo);
