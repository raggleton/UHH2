import FWCore.ParameterSet.Config as cms
from UHH2.core.ntuple_generator import generate_process  # use CMSSW type path for CRAB
from UHH2.core.optionsParse import setup_opts, parse_apply_opts


"""NTuple config for 2016 v2 miniaod MC datasets.

You should try and put any centralised changes in generate_process(), not here.
"""


process = generate_process(year="2016v2", useData=False)

# Please do not commit changes to source filenames - used for consistency testing
process.source.fileNames = cms.untracked.vstring([
    '/store/mc/RunIISummer16MiniAODv2/TT_TuneCUETP8M2T4_13TeV-powheg-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/80000/E022CF80-0ABF-E611-B5CD-00259048A87C.root'
    # '/store/mc/RunIISummer16MiniAODv2/QCD_Pt-15to7000_TuneCUETHS1_Flat_13TeV_herwigpp/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6_ext1-v2/20000/9A862150-7F9F-E811-A0FD-44A842482557.root'
    # '/store/mc/RunIISummer16MiniAODv2/DYJetsToLL_M-50_TuneCUETHS1_13TeV-madgraphMLM-herwigpp/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v2/50000/C6C0A700-DCDE-E611-A924-14187741121F.root'
    # '/store/mc/RunIISummer16MiniAODv2/DYJetsToLL_M-50_HT-100to200_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/80000/362BD21E-47CE-E611-9D61-0025905A612E.root'
])

# Turn on jet constituent storing for all AK4/8 jets (since we have all GPs anyway)
process.MyNtuple.doPFJetConstituentsNjets = 40
process.MyNtuple.doGenJetConstituentsNjets = 40

# Disable (Gen)TopJets
process.MyNtuple.doTopJets = False
process.MyNtuple.doGenTopJets = False

# Disable HOTVR, XCone
process.MyNtuple.doXCone = False
process.MyNtuple.doHOTVR = False
process.MyNtuple.doGenXCone = False
process.MyNtuple.doGenHOTVR = False

# Only keep PUPPI jets
# process.MyNtuple.jet_sources = ["jetsAk4Puppi", "jetsAk8Puppi"]

# Lower AK8 GenJet pT cut, change AK8 GenJet collection
process.ak8GenJetsFat.jetPtMin = cms.double(10.0)
process.ak8GenJetsFatFlavourInfos = process.slimmedGenJetsFlavourInfos.clone(jets="ak8GenJetsFat")
process.ak8GenJetsFatFlavourInfos.rParam = cms.double(0.8)
for task in process.p._tasks:
    # hack to add to first task
    task.add(process.ak8GenJetsFatFlavourInfos)
    break

process.MyNtuple.genjet_sources = ['slimmedGenJets', 'ak8GenJetsFat']

# Save all GenParticles
process.MyNtuple.doStableGenParticles = True

# Only standard METs
process.MyNtuple.met_sources = [x for x in process.MyNtuple.met_sources if x != "slMETsCHS"]

# GenParticles for H++
process.prunedPrunedGenParticles.select = cms.vstring(
    # 'keep *'
   'drop *',
   'keep status == 3',
   'keep 20 <= status <= 30 && pt > 1', # keep Pythia ME particles
   'keep ((1 <= abs(pdgId) <= 6) || abs(pdgId) == 21) && status == 11 && pt > 1', # keep ME partons for H++
   "keep (abs(pdgId) == 11 || abs(pdgId) == 13 || abs(pdgId) == 15) && status > 2", # keep leptons, but not status 1 (those are in packedGenParticles already) or 2 (which are just annoying)
)

# Do this after setting process.source.fileNames, since we want the ability to override it on the commandline
options = setup_opts()
parse_apply_opts(process, options)

with open('pydump_mc_2016v2.py', 'w') as f:
    f.write(process.dumpPython())
