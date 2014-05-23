
import FWCore.ParameterSet.Config as cms

process = cms.Process("DQ")

process.load("TrackingTools/TransientTrack/TransientTrackBuilder_cfi")
process.load("Configuration.StandardSequences.Geometry_cff")
process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
#
# data base for 2011 prompt reco:
#
#process.GlobalTag.globaltag = 'GR_P_V35::All'

# new tracker alignment summer 2011:
#
process.GlobalTag.globaltag = 'FT_R_53_V21::All'
if not "@GLOBALTAG@".startswith('@'):
	process.GlobalTag.globaltag = '@GLOBALTAG@'

process.load("Configuration.StandardSequences.MagneticField_AutoFromDBCurrent_cff")
process.load('RecoTracker.TransientTrackingRecHit.TransientTrackingRecHitBuilder_cfi')
process.load('RecoTracker.TransientTrackingRecHit.TTRHBuilderWithTemplate_cfi')
process.load("RecoTracker.TrackProducer.TrackRefitters_cff")
# Get beamspot from DB
process.load("RecoVertex.BeamSpotProducer.BeamSpot_cfi")
process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(10) )
#from CalibTracker.Configuration.Common.PoolDBESSource_cfi import poolDBESSource
import CalibTracker.Configuration.Common.PoolDBESSource_cfi

process.source = cms.Source("PoolSource",
	fileNames = cms.untracked.vstring(
	#	'/store/data/Run2012D/JetHT/RECO/PromptReco-v1/000/208/686/FE77D054-6241-E211-9BF9-003048D37580.root'
                 #'file:/afs/cern.ch/user/y/yechen/workspace/temp/2012D_JetHT.root'

                 'file:/afs/cern.ch/user/y/yechen/workspace/temp/2012A_AOD.root'

	)
)

ttrhBuilder = 'WithAngleAndTemplate'
process.Histos = cms.EDAnalyzer('PxlTest',
	triggerSource = cms.InputTag('TriggerResults::HLT'),
	ttrhBuilder = cms.string(ttrhBuilder),
)

process.TFileService = cms.Service('TFileService',
	fileName = cms.string('job.root'),
)

process.p = cms.Path(process.offlineBeamSpot*process.Histos)
