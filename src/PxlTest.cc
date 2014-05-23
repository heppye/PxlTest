// -*- C++ -*-
//
// Package:  PxlTest
// Class:    PxlTest
//
// my/PxlTest/src/PxlTest.cc
//
// Pixel (and strip) triplet residuals
//
// Original Author:  Daniel Pitzl, DESY
//         Created:  Sat Feb 12 12:12:42 CET 2011
// $Id$
//
// system include files:
#include <memory>
#include <iostream>
#include <iomanip>
#include <cmath>
// ROOT:
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
// CMS and user include files:
#include "../interface/PxlTest.h"
#include "CommonTools/UtilAlgos/interface/TFileService.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
//#include <FWCore/Framework/interface/EventSetup.h>
#include "FWCore/Framework/interface/MakerMacros.h"
#include <DataFormats/BeamSpot/interface/BeamSpot.h>
#include "DataFormats/VertexReco/interface/Vertex.h"
#include "DataFormats/VertexReco/interface/VertexFwd.h"
#include "DataFormats/METReco/interface/PFMET.h"
#include "DataFormats/METReco/interface/PFMETFwd.h"
#include "DataFormats/TrackReco/interface/Track.h"
//#include "DataFormats/TrackReco/interface/TrackExtra.h"
#include "DataFormats/TrackReco/interface/TrackFwd.h"
#include <DataFormats/TrackReco/interface/HitPattern.h>
// To convert detId to subdet/layer number:
#include "DataFormats/SiStripDetId/interface/StripSubdetector.h"
#include "DataFormats/SiPixelDetId/interface/PixelSubdetector.h"
#include "DataFormats/SiStripDetId/interface/TIBDetId.h"
#include "DataFormats/SiStripDetId/interface/TOBDetId.h"
#include "DataFormats/SiStripDetId/interface/TECDetId.h"
#include "DataFormats/SiStripDetId/interface/TIDDetId.h"
#include "DataFormats/SiPixelDetId/interface/PXBDetId.h"
#include "DataFormats/SiPixelDetId/interface/PXFDetId.h"
#include "DataFormats/TrackingRecHit/interface/TrackingRecHit.h"
//#include "DataFormats/GeometryVector/interface/GlobalPoint.h"
#include "DataFormats/TrackerRecHit2D/interface/SiPixelRecHit.h"
#include "DataFormats/TrackerRecHit2D/interface/SiStripRecHit2D.h"
#include "DataFormats/SiPixelCluster/interface/SiPixelCluster.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "Geometry/CommonDetUnit/interface/GeomDetUnit.h"
#include "Geometry/TrackerGeometryBuilder/interface/TrackerGeometry.h"
#include "Geometry/Records/interface/TrackerDigiGeometryRecord.h"
#include "Geometry/CommonTopologies/interface/Topology.h"
#include "Geometry/CommonTopologies/interface/PixelTopology.h"
#include "Geometry/CommonTopologies/interface/StripTopology.h"
#include "TrackingTools/TransientTrack/interface/TransientTrackBuilder.h"
#include "TrackingTools/Records/interface/TransientTrackRecord.h"
//#include "TrackingTools/TransientTrack/interface/TransientTrack.h"
#include <TrackingTools/TrajectoryState/interface/FreeTrajectoryState.h>
#include <MagneticField/Engine/interface/MagneticField.h>
#include "TrackingTools/TransientTrackingRecHit/interface/TransientTrackingRecHitBuilder.h"
#include "TrackingTools/TransientTrackingRecHit/interface/TransientTrackingRecHit.h"
#include "TrackingTools/Records/interface/TransientRecHitRecord.h"
#include "TrackingTools/PatternTools/interface/Trajectory.h"
#include "TrackingTools/PatternTools/interface/TrajectoryBuilder.h"
#include "TrackingTools/TrackFitters/interface/TrajectoryFitter.h"
#include "TrackingTools/Records/interface/TransientRecHitRecord.h"
#include "TrackingTools/Records/interface/TrackingComponentsRecord.h"
#include "TrackingTools/TrackFitters/interface/TrajectoryStateCombiner.h"
#include "TrackingTools/TrajectoryState/interface/TrajectoryStateTransform.h"
#include "TrackingTools/DetLayers/interface/DetLayer.h"
//#include "TrackingTools/GeomPropagators/interface/AnalyticalPropagator.h"
#include "FWCore/Common/interface/TriggerNames.h"
#include "DataFormats/Common/interface/TriggerResults.h"
#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"
#include "FWCore/Utilities/interface/RandomNumberGenerator.h"
#include "CLHEP/Random/RandFlat.h"
#include "CondFormats/SiPixelObjects/interface/SiPixelLorentzAngle.h"
#include "CondFormats/DataRecord/interface/SiPixelLorentzAngleRcd.h"

class myCounters{
   public:
      static int neve;
      static unsigned int prevrun;
};

int myCounters::neve = 0;
unsigned int myCounters::prevrun = 0;
PxlTest::PxlTest(const edm::ParameterSet& iConfig)
{
   std::cout << "PxlTest constructed\n";
   _triggerSrc = iConfig.getParameter<edm::InputTag>("triggerSource");
   _ttrhBuilder = iConfig.getParameter<std::string>("ttrhBuilder");
}
PxlTest::~PxlTest()
{
   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)
}
//
// member functions:
// method called once each job just before starting event loop
//
void PxlTest::beginJob()
{
   edm::Service<TFileService> fs;
  // h410 = fs->make<TH1D>( "h410", "PXB2 residuals #Deltax;PXB2 #Deltax [#mum];hits", 100, -150, 150 );
   newtree =fs->make<TTree>("newtree","test");
   newtree->Branch("residue",&residue,"residue/D");
   newtree->Branch("resid_disk1",&resid_disk1,"resid_disk1/D");
   newtree->Branch("resid_disk2",&resid_disk2,"resid_disk2/D");
   newtree->Branch("residue_refit",&residue_refit,"residue_refit/D");
   newtree->Branch("final_pt",&final_pt,"final_pt/D");
   newtree->Branch("final_phi",&final_phi,"final_phi/D");
   newtree->Branch("run_num",&run_num,"run_num/I");
   newtree->Branch("lumi_block",&lumi_block,"lumi_block/I");
   newtree->Branch("xpx2_g",&xpx2_g,"xpx2_g/D");
   newtree->Branch("xpy2_g",&xpy2_g,"xpy2_g/D");
   newtree->Branch("xpz2_g",&xpz2_g,"xpz2_g/D");

   newtree->Branch("xpx2_l",&xpx2_l,"xpx2_l/D");
   newtree->Branch("xpy2_l",&xpy2_l,"xpy2_l/D");
   newtree->Branch("xpz2_l",&xpz2_l,"xpz2_l/D");
   //
}

//----------------------------------------------------------------------
// method called for each event:

void PxlTest::beginRun(const edm::Run& iRun, const edm::EventSetup& iSetup)
{
   bool hltSetupChanged = false;
   if(!HLTConfig.init(iRun, iSetup, _triggerSrc.process(), hltSetupChanged))
      throw cms::Exception("Failed to init HLT config");
}

void PxlTest::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup){

   using namespace std;
   using namespace edm;
   using namespace reco;
   using namespace math;

   const double pi = 4*atan(1);
   const double wt = 180/pi;
   const double twopi = 2*pi;
   const double pihalf = 2*atan(1);
   const double sqrtpihalf = sqrt(pihalf);

   myCounters::neve++;

   if( myCounters::prevrun != iEvent.run() ){
      time_t unixZeit = iEvent.time().unixTime();
      cout << "new run " << iEvent.run();
      cout << ", LumiBlock " << iEvent.luminosityBlock();
      cout << " taken " << ctime(&unixZeit); // ctime has endline
      myCounters::prevrun = iEvent.run();
   }// new run

   int idbg = 0;
   if( myCounters::neve < 2 ) idbg = 1;

   int jdbg = 0;
   if( idbg ) {
      cout << endl;
      cout << "run " << iEvent.run();
      cout << ", LumiBlock " << iEvent.luminosityBlock();
      cout << ", event " << iEvent.eventAuxiliary().event();
      time_t unixZeit = iEvent.time().unixTime();
      cout << ", taken " << ctime(&unixZeit); // ctime has endline
   }


   run_num=iEvent.run();
   lumi_block=iEvent.luminosityBlock();   
   //--------------------------------------------------------------------
   // beam spot:

   edm::Handle<reco::BeamSpot> rbs;
   iEvent.getByLabel( "offlineBeamSpot", rbs );

   XYZPoint bsP = XYZPoint(0,0,0);
   int ibs = 0;

   if( rbs.failedToGet() ) return;
   if( ! rbs.isValid() ) return;
   ibs = 1;
   bsP = XYZPoint( rbs->x0(), rbs->y0(), rbs->z0() );
   double bx = rbs->BeamWidthX();
   double by = rbs->BeamWidthY();
   if( idbg ){
      cout << "beam spot x " << rbs->x0();
      cout << ", y " << rbs->y0();
      cout << ", z " << rbs->z0();
      cout << endl;
   }
   //--------------------------------------------------------------------
   // primary vertices:
   Handle<VertexCollection> vertices;
   iEvent.getByLabel( "offlinePrimaryVertices", vertices );
   if( vertices.failedToGet() ) return;
   if( !vertices.isValid() ) return;
   int nvertex = vertices->size();
   //   h010->Fill( nvertex );
   // need vertex global point for tracks
   // from #include "DataFormats/GeometryVector/interface/GlobalPoint.h"
   // Global points are three-dimensional by default
   // typedef Global3DPoint  GlobalPoint;
   // typedef Point3DBase< float, GlobalTag> Global3DPoint;

   XYZPoint vtxN = XYZPoint(0,0,0);
   XYZPoint vtxP = XYZPoint(0,0,0);

   double bestNdof = 0;
   double maxSumPt = 0;
   Vertex bestPvx;

   double xBS = 0;
   double yBS = 0;
   if( ibs ) {
      xBS = bsP.x();
      yBS = bsP.y();
   }
   else {
      xBS = vtxP.x();
      yBS = vtxP.y();
   }

   //--------------------------------------------------------------------
   // MET:

   edm::Handle< edm::View<reco::PFMET> > pfMEThandle;
   iEvent.getByLabel( "pfMet", pfMEThandle );

   if( !pfMEThandle.failedToGet() && pfMEThandle.isValid()){

      //      h026->Fill( pfMEThandle->front().sumEt() );
      //      h027->Fill( pfMEThandle->front().et() );

   }

   //--------------------------------------------------------------------
   // get a fitter to refit TrackCandidates, the same fitter as used in standard reconstruction:
   // Fitter = cms.string('KFFittingSmootherWithOutliersRejectionAndRK'),
   // KalmanFilter
   // RungeKutta

   ESHandle<TrajectoryFitter> TF;
   iSetup.get<TrajectoryFitter::Record>().get( "KFFittingSmootherWithOutliersRejectionAndRK", TF );
   const TrajectoryFitter* theFitter = TF.product();
   // TrackPropagator:
   edm::ESHandle<Propagator> prop;
   iSetup.get<TrackingComponentsRecord>().get( "PropagatorWithMaterial", prop );
   const Propagator* thePropagator = prop.product();

   //--------------------------------------------------------------------
   // tracks:
   Handle<TrackCollection> tracks;
   iEvent.getByLabel( "generalTracks", tracks );
   if( tracks.failedToGet() ) return;
   if( !tracks.isValid() ) return;
   if( idbg ){
      cout << "  tracks " << tracks->size();
      cout << endl;
   }
   //   h030->Fill( tracks->size() );
   //----------------------------------------------------------------------------
   // get tracker geometry:
   edm::ESHandle<TrackerGeometry> pTG;
   iSetup.get<TrackerDigiGeometryRecord>().get( pTG );
   if( ! pTG.isValid() ) {
      cout << "Unable to find TrackerDigiGeometry. Return\n";
      return;
   }

   // loop over tracker detectors:

   for( TrackerGeometry::DetContainer::const_iterator idet = pTG->dets().begin();
	 idet != pTG->dets().end(); ++idet ) {

      DetId mydetId = (*idet)->geographicalId();
      uint32_t mysubDet = mydetId.subdetId();
      //    if( myCounters::neve == 1 ){
      if( myCounters::neve == 0 ){
	 //cout << "Det " << mydetId.det();
	 //cout << ", subDet " << mydetId.subdetId();
	 if( mysubDet == PixelSubdetector::PixelBarrel ) {
	    cout << ": PXB layer " << PXBDetId(mydetId).layer();
	    cout << ", ladder " << PXBDetId(mydetId).ladder();
	    cout << ", module " << PXBDetId(mydetId).module();
	    cout << ", at R " << (*idet)->position().perp();
	    cout << ", F " << (*idet)->position().barePhi()*wt;
	    cout << ", z " << (*idet)->position().z();
	    cout << endl;
	    cout << "rot x";
	    cout << "\t" << (*idet)->rotation().xx();
	    cout << "\t" << (*idet)->rotation().xy();
	    cout << "\t" << (*idet)->rotation().xz();
	    cout << endl;
	    cout << "rot y";
	    cout << "\t" << (*idet)->rotation().yx();
	    cout << "\t" << (*idet)->rotation().yy();
	    cout << "\t" << (*idet)->rotation().yz();
	    cout << endl;
	    cout << "rot z";
	    cout << "\t" << (*idet)->rotation().zx();
	    cout << "\t" << (*idet)->rotation().zy();
	    cout << "\t" << (*idet)->rotation().zz();
	    cout << endl;

	    // normal vector: includes alignment (varies from module to module along z on one ladder)
	    // neighbouring ladders alternate with inward/outward orientation

	    cout << "normal";
	    cout << ": x " << (*idet)->surface().normalVector().x();
	    cout << ", y " << (*idet)->surface().normalVector().y();
	    cout << ", z " << (*idet)->surface().normalVector().z();
	    cout << ", f " << (*idet)->surface().normalVector().barePhi()*wt;
	    cout << endl;

	 }//PXB

	 if( mysubDet == PixelSubdetector::PixelEndcap ) {

	    cout << ": PXD side " << PXFDetId(mydetId).side();
	    cout << ", disk " << PXFDetId(mydetId).disk();
	    cout << ", blade " << PXFDetId(mydetId).blade();
	    cout << ", panel " << PXFDetId(mydetId).panel();
	    cout << ", module " << PXFDetId(mydetId).module();
	    cout << ", at R " << (*idet)->position().perp();
	    cout << ", F " << (*idet)->position().barePhi()*wt;
	    cout << ", z " << (*idet)->position().z();
	    cout << endl;
	    cout << "rot x";
	    cout << "\t" << (*idet)->rotation().xx();
	    cout << "\t" << (*idet)->rotation().xy();
	    cout << "\t" << (*idet)->rotation().xz();
	    cout << endl;
	    cout << "rot y";
	    cout << "\t" << (*idet)->rotation().yx();
	    cout << "\t" << (*idet)->rotation().yy();
	    cout << "\t" << (*idet)->rotation().yz();
	    cout << endl;
	    cout << "rot z";
	    cout << "\t" << (*idet)->rotation().zx();
	    cout << "\t" << (*idet)->rotation().zy();
	    cout << "\t" << (*idet)->rotation().zz();
	    cout << endl;
	    cout << "normal";
	    cout << ": x " << (*idet)->surface().normalVector().x();
	    cout << ", y " << (*idet)->surface().normalVector().y();
	    cout << ", z " << (*idet)->surface().normalVector().z();
	    cout << ", f " << (*idet)->surface().normalVector().barePhi()*wt;
	    cout << endl;
	 }//PXD
      }//idbg
   }//idet

   //----------------------------------------------------------------------------
   // transient track builder, needs B-field from data base (global tag in .py)

   edm::ESHandle<TransientTrackBuilder> theB;
   iSetup.get<TransientTrackRecord>().get( "TransientTrackBuilder", theB );
   // transient rec hits:
   ESHandle<TransientTrackingRecHitBuilder> hitBuilder;
   iSetup.get<TransientRecHitRecord>().get( _ttrhBuilder, hitBuilder );

#if 0
   edm::ESHandle<SiPixelLorentzAngle> SiPixelLorentzAngle_; 
   iSetup.get<SiPixelLorentzAngleRcd>().get(SiPixelLorentzAngle_);
   std::map<unsigned int,float> detid_la= SiPixelLorentzAngle_->getLorentzAngles();
   std::map<unsigned int,float>::const_iterator it;
   for (it=detid_la.begin();it!=detid_la.end();it++)
   {
      unsigned int subdet   = DetId(it->first).subdetId();
      if(subdet == static_cast<int>(PixelSubdetector::PixelBarrel))
	 std::cout << it->first << ": " << it->second << std::endl;
   }
#endif

   //----------------------------------------------------------------------------
   // Tracks:

   double sumpt = 0;
   double sumq = 0;
   Surface::GlobalPoint origin = Surface::GlobalPoint(0,0,0);
   for( TrackCollection::const_iterator iTrack = tracks->begin();
	 iTrack != tracks->end(); ++iTrack ) {
      // cpt = cqRB = 0.3*R[m]*B[T] = 1.14*R[m] for B=3.8T
      // D = 2R = 2*pt/1.14
      // calo: D = 1.3 m => pt = 0.74 GeV/c
      double pt = iTrack->pt();
      double pp = iTrack->p();
      //if( pt < 0.75 ) continue;// curls up
      //if( pt < 1.75 ) continue;// want sharper image
      if( abs( iTrack->dxy(vtxP) ) > 5*iTrack->dxyError() ) continue; // not prompt
      sumpt += pt;
      sumq += iTrack->charge();

      double logpt = log(pt) / log(10);

      //      h031->Fill( iTrack->charge() );
      //      h032->Fill( pt );
      //      h033->Fill( pt );

      const reco::HitPattern& hp = iTrack->hitPattern();

      //      h035->Fill( hp.numberOfValidTrackerHits() );
      //      h036->Fill( hp.numberOfValidPixelBarrelHits() );
      //      h037->Fill( hp.trackerLayersWithMeasurement() );
      //      h038->Fill( hp.pixelBarrelLayersWithMeasurement() );

      if( idbg ) {
	 cout << endl;
	 cout << "Track " << distance( tracks->begin(), iTrack );
	 cout << ": pt " << iTrack->pt();
	 cout << ", eta " << iTrack->eta();
	 cout << ", phi " << iTrack->phi()*wt;
	 cout << setprecision(1);
	 cout << ", dxyv " << iTrack->dxy(vtxP)*1E4 << " um";
	 cout << ", dzv " << iTrack->dz(vtxP)*1E1 << " mm";
	 cout << setprecision(4);
	 cout << ", hits " << hp.numberOfHits();
	 cout << ", valid " << hp.numberOfValidTrackerHits();
	 cout << endl;
      }

      double phi = iTrack->phi();
      double dca = iTrack->d0(); // w.r.t. origin                               
      //double dca = -iTrack->dxy(); // dxy = -d0                               
      double dip = iTrack->lambda();
      double z0  = iTrack->dz();
      double tet = pihalf - dip;
      //double eta = iTrack->eta();
      // beam line at z of track, taking beam tilt into account
      double zBeam = iTrack->dz(bsP);//z0p of track along beam line w.r.t. beam z center
      double xBeam = rbs->x0() + rbs->dxdz() * zBeam;//beam at z of track
      double yBeam = rbs->y0() + rbs->dydz() * zBeam;
      double z0p =  zBeam + bsP.z(); // z0p of track along beam line w.r.t. CMS z = 0
      XYZPoint blP = XYZPoint( xBeam, yBeam, z0p );//point on beam line at z of track

      xBS = xBeam;//improve
      yBS = yBeam;//beam tilt taken into account

      double bcap = iTrack->dxy(blP);//impact parameter to beam
      double edca = iTrack->dxyError();
      double ebca = sqrt( edca*edca + bx*by );//round beam
      double sbca = bcap / ebca;//impact parameter significance


      // transient track:

      TransientTrack tTrack = theB->build(*iTrack);

      TrajectoryStateOnSurface initialTSOS = tTrack.innermostMeasurementState();

      double kap = tTrack.initialFreeState().transverseCurvature();

      TrajectoryStateClosestToPoint tscp = tTrack.trajectoryStateClosestToPoint( origin );
      //
      //    if( tscp.isValid() ) {
      //
      //      h057->Fill( tscp.referencePoint().x() ); // 0.0
      //      h058->Fill( tscp.referencePoint().y() ); // 0.0
      //      h059->Fill( tscp.referencePoint().z() ); // 0.0
      //      kap = tscp.perigeeParameters().transverseCurvature();
      //      phi = tscp.perigeeParameters().phi();
      //      dca = tscp.perigeeParameters().transverseImpactParameter();
      //      tet = tscp.perigeeParameters().theta();
      //      z0  = tscp.perigeeParameters().longitudinalImpactParameter();
      //      dip = pihalf - tet;
      //
      //      h051->Fill( kap - tTrack.initialFreeState().transverseCurvature() ); // identical
      //      h052->Fill( phi - iTrack->phi() ); // identical
      //      h053->Fill( dca - iTrack->d0() ); // identical
      //      h054->Fill( dip - iTrack->lambda() ); // identical
      //      h055->Fill( z0  - iTrack->dz() ); // identical
      //
      //    }//tscp valid

      double cf = cos(phi);
      double sf = sin(phi);
      //double xdca =  dca * sf;
      //double ydca = -dca * cf;

      //double tt = tan(tet);

      double rinv = -kap; // Karimaki                                           
      double rho = 1/kap;
      double erd = 1.0 - kap*dca;
      double drd = dca * ( 0.5*kap*dca - 1.0 ); // 0.5 * kap * dca**2 - dca;
      double hkk = 0.5*kap*kap;

      // track w.r.t. beam (cirmov):

      double dp = -xBS*sf + yBS*cf + dca;
      double dl = -xBS*cf - yBS*sf;
      double sa = 2*dp + rinv * ( dp*dp + dl*dl );
      double dcap = sa / ( 1 + sqrt(1 + rinv*sa) );// distance to beam
      double ud = 1 + rinv*dca;
      double phip = atan2( -rinv*xBS + ud*sf, rinv*yBS + ud*cf );//direction

      // track at R(PXB1), from FUNPHI, FUNLEN:

      double R1 = 4.4; // PXB1

      double s1 = 0;
      double fpos1 = phi - pihalf;

      if( R1 >= abs(dca) ) {

	 // sin(delta phi):

	 double sindp = ( 0.5*kap * (R1*R1 + dca*dca) - dca ) / (R1*erd);
	 fpos1 = phi + asin(sindp); // phi position at R1

	 // sin(alpha):

	 double sina = R1*kap * sqrt( 1.0 - sindp*sindp );

	 // s = alpha / kappa:

	 if( sina >= 1.0 )
	    s1 = pi / kap;
	 else{
	    if( sina <= -1.0 )
	       s1 = -pi / kap;
	    else
	       s1 = asin(sina) / kap;//always positive
	 }

	 // Check direction: limit to half-turn

	 if( hkk * ( R1*R1 - dca*dca ) > erd ) s1 = pi/abs(kap) - s1; // always positive

      }// R1 > dca

      if( fpos1 > pi ) fpos1 -= twopi;
      else if( fpos1 < -pi ) fpos1 += twopi;

      double zR1 = z0 + s1*tan(dip); // z at R1
      //--------------------------------------------------------------------------
      // loop over tracker detectors:
      double xcrss[99];
      double ycrss[99];
      double zcrss[99];
      int ncrss = 0;
      for( TrackerGeometry::DetContainer::const_iterator idet = pTG->dets().begin();
	    idet != pTG->dets().end(); ++idet ) {

	 DetId mydetId = (*idet)->geographicalId();
	 uint32_t mysubDet = mydetId.subdetId();

	 if( mysubDet != PixelSubdetector::PixelBarrel ) continue;

	 /*
	    cout << ": PXB layer " << PXBDetId(mydetId).layer();
	    cout << ", ladder " << PXBDetId(mydetId).ladder();
	    cout << ", module " << PXBDetId(mydetId).module();
	    cout << ", at R1 " << (*idet)->position().perp();
	    cout << ", F " << (*idet)->position().barePhi()*wt;
	    cout << ", z " << (*idet)->position().z();
	    cout << endl;
	  */

	 if( PXBDetId(mydetId).layer() == 1 ) {

	    double dz = zR1 - (*idet)->position().z();

	    if( abs(dz) > 4.0 ) continue;

	    double df = fpos1 - (*idet)->position().barePhi();//track phi at R1 vs ladder phi

	    if( df > pi ) df -= twopi;
	    else if( df < -pi ) df += twopi;

	    if( abs(df)*wt > 36.0 ) continue;//coarse matching track to ladder

	    // normal vector: includes alignment (varies from module to module along z on one ladder)
	    // neighbouring ladders alternate with inward/outward orientation

	    /*
	       cout << "normal";
	       cout << ": x " << (*idet)->surface().normalVector().x();
	       cout << ", y " << (*idet)->surface().normalVector().y();
	       cout << ", z " << (*idet)->surface().normalVector().z();
	       cout << ", f " << (*idet)->surface().normalVector().barePhi()*wt;
	       cout << endl;
	     */

	    double phiN = (*idet)->surface().normalVector().barePhi();//normal vector
	    double phidet = phiN - pihalf;// orientation of sensor plane in x-y
	    double ux = cos(phidet);// vector in sensor plane
	    double uy = sin(phidet);
	    double x = (*idet)->position().x();
	    double y = (*idet)->position().y();

	    // intersect helix with line: FUNRXY (in FUNPHI) from V. Blobel
	    // factor f for intersect point (x + f*ux, y + f*uy)

	    double a =                                 kap * ( ux*ux + uy*uy ) * 0.5;
	    double b =       erd * ( ux*sf - uy*cf ) + kap * ( ux*x + uy*y );
	    double c = drd + erd * (  x*sf -  y*cf ) + kap * (  x*x +  y*y ) * 0.5;
	    double dis = b*b - 4.0*a*c;
	    double f = 0;

	    if( dis > 0 ) {

	       dis = sqrt(dis);
	       double f1 = 0;
	       double f2 = 0;

	       if( b < 0 ) {
		  f1 = 0.5 * ( dis - b ) / a;
		  f2 = 2.0 * c / ( dis - b );
	       }
	       else{
		  f1 = -0.5 * ( dis + b ) / a;
		  f2 = -2.0 * c / ( dis + b );
	       }

	       f = f1;
	       if( abs(f2) < abs(f1) ) f = f2;

	    }//dis

	    xcrss[ncrss] = x + f*ux;
	    ycrss[ncrss] = y + f*uy;
	    double r = sqrt( xcrss[ncrss]*xcrss[ncrss] + ycrss[ncrss]*ycrss[ncrss] );

	    double s = 0;

	    if( r >= abs(dca) ) {
	       double sindp = ( 0.5*kap * ( r*r + dca*dca ) - dca ) / (r*erd);
	       double sina = r*kap * sqrt( 1.0 - sindp*sindp );
	       if( sina >= 1.0 )
		  s = pi / kap;
	       else{
		  if( sina <= -1.0 )
		     s = -pi / kap;
		  else
		     s = asin(sina) / kap;
	       }
	       if( hkk * ( r*r - dca*dca ) > erd ) s = pi/abs(kap) - s;
	    }

	    zcrss[ncrss] = z0 + s*tan(dip); // z at r

	    ncrss++;

	 }//PXB1

      }//idet

      //--------------------------------------------------------------------------
      // rec hits from track extra:

      if( iTrack->extra().isNull() ) continue;//next track
      if( ! iTrack->extra().isAvailable() ) continue;//next track

      //    h034->Fill( tTrack.recHitsSize() ); // tTrack

      double rmin = 99.9;
      uint32_t innerDetId = 0;
      double xPXB1 = 0;
      double yPXB1 = 0;
      double zPXB1 = 0;
      double uPXB1 = 0;
      double vPXB1 = 0;
      double ePXB1 = 0;
      double fPXB1 = 0;

      double xPXB2 = 0;
      double yPXB2 = 0;
      double zPXB2 = 0;
      double uPXB2 = 0;
      double vPXB2 = 0;
      double ePXB2 = 0;
      double fPXB2 = 0;

      double xPXB3 = 0;
      double yPXB3 = 0;
      double zPXB3 = 0;
      double uPXB3 = 0;
      double vPXB3 = 0;
      double ePXB3 = 0;
      double fPXB3 = 0;

      int n1 = 0;
      int n2 = 0;
      int n3 = 0;
      double phiN1 = 0;
      double phiN2 = 0;
      double phiN3 = 0;
      double clch1 = 0;
      double clch2 = 0;
      double clch3 = 0;
      int ncol1 = 0;
      int ncol2 = 0;
      int ncol3 = 0;
      int nrow1 = 0;
      int nrow2 = 0;
      int nrow3 = 0;
      double etaX1 = 0;
      double etaX2 = 0;
      double etaX3 = 0;
      double cogp1 = 0;
      double cogp2 = 0;
      double cogp3 = 0;
      double xmid2 = 0;
      double ymid2 = 0;
      const GeomDet * det2;
      int ilad2 = 0;
      int xmin2 = 0;
      int xmax2 = 0;
      int zmin2 = 0;
      int zmax2 = 0;


      edm::OwnVector<TrackingRecHit> recHitVector; // for seed
      std::vector<TransientTrackingRecHit::RecHitPointer> myTTRHvec;
      Trajectory::RecHitContainer coTTRHvec; // for fit, constant

      // loop over recHits on this track:

      for( trackingRecHit_iterator irecHit = iTrack->recHitsBegin();
	    irecHit != iTrack->recHitsEnd(); ++irecHit ) {
	 DetId detId = (*irecHit)->geographicalId();
	 uint32_t subDet = detId.subdetId();
	 // enum Detector { Tracker=1, Muon=2, Ecal=3, Hcal=4, Calo=5 };
	 if( detId.det() != 1 ){
	    cout << "rec hit ID = " << detId.det() << " not in tracker!?!?\n";
	    continue;
	 }

	 recHitVector.push_back( (*irecHit)->clone() );

	 int xmin = 0;
	 int xmax = 0;
	 int ymin = 0;
	 int ymax = 0;

	 double cogp = 0;

	 int icol = 0;
	 int irow = 0;
	 int ncol = 0;
	 int nrow = 0;
	 double clch = 0;

	 bool halfmod = 0;

	 double Q_f_X = 0.0;//first
	 double Q_l_X = 0.0;//last
	 double Q_m_X = 0.0;//middle
	 double etaX = 0;

	 double Q_f_Y = 0.0;//first
	 double Q_l_Y = 0.0;//last
	 double Q_m_Y = 0.0;//middle

	 if( (*irecHit)->isValid() ) {

	    // enum SubDetector{ PixelBarrel=1, PixelEndcap=2 };
	    // enum SubDetector{ TIB=3, TID=4, TOB=5, TEC=6 };

	    //if( idbg ) cout << "  hit in " << subDet << endl;

	    //	h060->Fill( subDet );

	    // cast to SiPixelRecHit:
	    // TrackingRecHit -> RecHit2DLocalPos -> BaseSiTrackerRecHit2DLocalPos -> SiPixelRecHit

	    if( subDet == 1 ){ // PXB

	       int ilay = PXBDetId(detId).layer();
	       int ilad = PXBDetId(detId).ladder();

	       if( ilay == 1 ){

		  if(      ilad ==  5 ) halfmod = 1;
		  else if( ilad ==  6 ) halfmod = 1;
		  else if( ilad == 15 ) halfmod = 1;
		  else if( ilad == 16 ) halfmod = 1;

	       }

	       if( ilay == 2 ){

		  if(      ilad ==  8 ) halfmod = 1;
		  else if( ilad ==  9 ) halfmod = 1;
		  else if( ilad == 24 ) halfmod = 1;
		  else if( ilad == 25 ) halfmod = 1;

	       }

	       if( ilay == 3 ){

		  if( ilad == 11 ) halfmod = 1;
		  if( ilad == 12 ) halfmod = 1;
		  if( ilad == 33 ) halfmod = 1;
		  if( ilad == 34 ) halfmod = 1;

	       }

	       if( idbg ) {
		  cout << "  layer  " << PXBDetId(detId).layer();
		  cout << ", ladder " << PXBDetId(detId).ladder();
		  cout << ", module " << PXBDetId(detId).module();
		  cout << endl;
	       }

	       const SiPixelRecHit *pixhit = dynamic_cast<const SiPixelRecHit*>( &*(*irecHit) );

	       if( pixhit->hasFilledProb() ){
		  float clusProb = pixhit->clusterProbability(0);

		  if( idbg ) cout << "  cluster prob " << clusProb << endl;
	       }

	       // pixel cluster:
	       // TrackingRecHit -> RecHit2DLocalPos -> BaseSiTrackerRecHit2DLocalPos -> SiPixelRecHit -> SiPixelCluster

	       edm::Ref<edmNew::DetSetVector<SiPixelCluster>, SiPixelCluster> const & clust = pixhit->cluster();

	       if( clust.isNonnull() ) {

		  if( idbg ) {
		     cout << setprecision(0);
		     cout << "  charge " << clust->charge();
		     cout << setprecision(4);
		     cout << ", cols " << clust->minPixelCol() << " - " << clust->maxPixelCol(); //0..416 = 8*52
		     cout << " = " << clust->sizeY();
		     cout << ", rows " << clust->minPixelRow() << " - " << clust->maxPixelRow();//0..159 left and right
		     cout << " = " << clust->sizeX();
		     cout << endl;
		  }

		  // Fetch the pixels vector from the cluster:
		  const vector<SiPixelCluster::Pixel> & pixelsVec = clust->pixels();

		  // Obtain boundaries in index units:
		  xmin = clust->minPixelRow();
		  xmax = clust->maxPixelRow();
		  ymin = clust->minPixelCol();
		  ymax = clust->maxPixelCol();

		  // cluster matrix:

		  int QQ[9][99];
		  for( int ix = 0; ix < 9; ++ix ){
		     for( int jz = 0; jz < 99; ++jz ){
			QQ[ix][jz] = 0;
		     }
		  }
		  double xsum = 0;
		  double qsum = 0;

		  // loop over the pixels:

		  int isize = pixelsVec.size();

		  for( int i = 0;  i < isize; ++i ) {

		     int ix = pixelsVec[i].x - xmin;
		     int jz = pixelsVec[i].y - ymin;
		     if( ix > 8 ) ix = 8;
		     if( jz > 98 ) jz = 98;
		     QQ[ix][jz] = pixelsVec[i].adc;

		     double pix_adc = pixelsVec[i].adc;
		     qsum += pix_adc;
		     xsum += pix_adc * pixelsVec[i].x;

		     // X projection:
		     if( pixelsVec[i].x == xmin )
			Q_f_X += pix_adc;
		     else{
			if( pixelsVec[i].x == xmax ) 
			   Q_l_X += pix_adc;
			else
			   Q_m_X += pix_adc;
		     }

		     //	      if( ! halfmod ) {
		     //		h049->Fill( pixelsVec[i].y, pixelsVec[i].x );
		     //		h067->Fill( pixelsVec[i].x );//0..159
		     //		h068->Fill( pixelsVec[i].x, pix_adc*1E-3 );//weighted
		     //	      }

		     // Y projection:
		     if( pixelsVec[i].y == ymin ) 
			Q_f_Y += pix_adc;
		     else{
			if( pixelsVec[i].y == ymax ) 
			   Q_l_Y += pix_adc;
			else 
			   Q_m_Y += pix_adc;
		     }

		  }//loop over pixels

		  etaX = ( Q_f_X - Q_l_X ) / ( Q_f_X + Q_l_X + Q_m_X );

		  cogp = xsum / qsum;

		  clch = clust->charge();//electrons
		  icol = clust->minPixelCol();
		  irow = clust->minPixelRow();
		  ncol = clust->sizeY();
		  nrow = clust->sizeX();

		  if( ncol > 5 && idbg ){
		     cout << setprecision(1);
		     cout.setf(ios::showpoint);
		     cout.setf(ios::uppercase);
		     //cout.setf(ios::scientific);
		     cout.setf(ios::fixed);
		     cout << "  dip " << setw(5) << dip*wt;
		     cout << setprecision(4);
		     cout << ", layer  " << PXBDetId(detId).layer();
		     cout << ", ladder " << PXBDetId(detId).ladder();
		     cout << ", module " << PXBDetId(detId).module();
		     cout << ", x " << xmin << " - " << xmax;
		     cout << ", z " << ymin << " - " << ymax;
		     cout << endl;
		     for( int ix = 0; ix < min( 9, nrow ); ++ix ){
			cout << "    row " << setw(3) << ix + xmin;
			for( int jz = 0; jz < min( 99, ncol ); ++jz ){
			   cout << setw(6) << QQ[ix][jz] / 100;
			}
			cout << endl;
		     }
		  }//long ncol
		  //
		  //dip -7.E+01, layer  1, ladder 18, module 2, x 148 - 149, z 141 - 148
		  //row 148     0     0     0     0     0     0    75    78
		  //row 149   191   164   111   166    98    96   150     0
		  //
		  //dip 8.E+01, layer  1, ladder 9, module 8, x 21 - 22, z 368 - 375
		  //row  21     0     0     0     0     0     0     0    84
		  //row  22   259   171   113   115   144   106   161     0
		  //
		  //dip -8.E+01, layer  1, ladder 8, module 1, x 8 - 10, z 279 - 290
		  //row   8     0     0     0     0     0     0     0     0     0     0    70    99
		  //row   9     0     0    91   129   131   112    97    97   146   134   107     0
		  //row  10    55   107    56     0     0     0     0     0     0     0     0     0
		  //
		  //dip 8.E+01, layer  1, ladder 2, module 7, x 139 - 140, z 135 - 144
		  //row 139    94   107   173    52     0     0     0     0     0     0
		  //row 140     0     0     0    70   121   128   116   132    99    91
		  //
		  //dip -7.E+01, layer  2, ladder 10, module 2, x 139 - 140, z 408 - 415
		  //row 139    50   397   107   100    71     0     0     0
		  //row 140     0     0     0     0    48   130   246   239

		  //	    h070->Fill( clch/1E3 );
		  //	    h071->Fill( clch/1E3*cos(dip) );
		  //	    h072->Fill( ncol );
		  //	    h073->Fill( nrow );
		  //	    h074->Fill( dip*wt, ncol );
		  //	    h075->Fill( dip*wt, nrow );
		  //	    if( nrow == 2 ) h076->Fill( etaX );
		  //
	       }//clust nonNull

	    }//PXB

	 }//valid

	 // build transient hit: (from what?)

	 TransientTrackingRecHit::RecHitPointer transRecHit = hitBuilder->build( &*(*irecHit), initialTSOS);

	 myTTRHvec.push_back( transRecHit );
	 coTTRHvec.push_back( transRecHit );

	 if( ! (*irecHit)->isValid() ) continue;

	 double xloc = transRecHit->localPosition().x();// 1st meas coord
	 double yloc = transRecHit->localPosition().y();// 2nd meas coord or zero
	 //double zloc = transRecHit->localPosition().z();// up, always zero

	 double vxloc = transRecHit->localPositionError().xx();//covariance
	 double vyloc = transRecHit->localPositionError().yy();//covariance

	 double gX = transRecHit->globalPosition().x();
	 double gY = transRecHit->globalPosition().y();
	 double gZ = transRecHit->globalPosition().z();

	 if( transRecHit->canImproveWithTrack() ) {//use z from track to apply alignment

	    //if( idbg ) cout << "  try to improve\n";

	    TrajectoryStateOnSurface propTSOS = thePropagator->propagate( initialTSOS, transRecHit->det()->surface() );

	    if( propTSOS.isValid() ){

	       //if( idbg ) cout << "  have propTSOS\n";

	       TransientTrackingRecHit::RecHitPointer preciseHit = transRecHit->clone(propTSOS);

	       //if( idbg ) cout << "  have preciseHit\n";

	       xloc = preciseHit->localPosition().x();// 1st meas coord
	       yloc = preciseHit->localPosition().y();// 2nd meas coord or zero
	       // zloc = preciseHit->localPosition().z();// up, always zero

	       vxloc = preciseHit->localPositionError().xx();//covariance
	       vyloc = preciseHit->localPositionError().yy();//covariance

	       if( idbg ) {
		  cout << "  improved hit in " << subDet;
		  cout << setprecision(4);
		  cout << ", xloc from " << transRecHit->localPosition().x();
		  cout << " to " << preciseHit->localPosition().x();
		  cout << ", yloc from " << transRecHit->localPosition().y();
		  cout << " to " << preciseHit->localPosition().y();
		  cout << endl;
	       }

	       gX = preciseHit->globalPosition().x();
	       gY = preciseHit->globalPosition().y();
	       gZ = preciseHit->globalPosition().z();

	    }//valid propTSOS
	    else{
	       if( idbg ) cout << "  propTSOS not valid\n";
	    }
	 }//canImprove

	 double gF = atan2( gY, gX );
	 double gR = sqrt( gX*gX + gY*gY );

	 //  h061->Fill( xloc );
	 //  h062->Fill( yloc );
	 //  h063->Fill( gR );
	 //  h064->Fill( gF*wt );
	 //  h065->Fill( gZ );

	 if( subDet == PixelSubdetector::PixelBarrel ||
	       subDet == StripSubdetector::TIB ||
	       subDet == StripSubdetector::TOB ) { // barrel

	    //	h066->Fill( gX, gY );
	 }

	 if( gR < rmin ) {
	    rmin = gR;
	    innerDetId = detId.rawId();
	 }

	 double phiN = transRecHit->det()->surface().normalVector().barePhi();//normal vector

	 double xmid = transRecHit->det()->position().x();
	 double ymid = transRecHit->det()->position().y();

	 // PXB:

	 if( subDet == PixelSubdetector::PixelBarrel ) {

	    double xpix = fmod( xloc+0.82, 0.01 );// xpix = 0..0.01

	    //	h080->Fill( xpix*1E4 );
	    //	if( nrow == 2 )
	    //	  h081->Fill( xpix*1E4 );
	    //	else
	    //	  h082->Fill( xpix*1E4 );
	    //
	    //	if( nrow == 2 ) h083->Fill( xpix*1E4, etaX );
	    //
	    double df = phiN - gF;//normal vector vs position vector: inwards or outwards

	    // take care of cut at +180/-180:

	    if( df > pi ) df -= twopi;
	    else if( df < -pi ) df += twopi;

	    // outward/inward have different Lorentz drift:

	    //	if( nrow == 2 ) {
	    //	  if( abs(df) < pihalf ) // outward
	    //	    h084->Fill( xpix*1E4, etaX );
	    //	  else // inward, identical
	    //	    h085->Fill( xpix*1E4, etaX ); // identical
	    //	}
	    //
	    //	if( nrow == 1 ) h086->Fill( xloc );
	    //	if( nrow == 2 ) h087->Fill( xloc );

	    int ilay = PXBDetId(detId).layer();
	    int ilad = PXBDetId(detId).ladder();
	    int imod = PXBDetId(detId).module();

	    if( idbg ) {
	       cout << "  xloc " << xloc;
	       cout << ", cogp " << cogp;
	       double cogx = (cogp + 0.5 - 80) * 0.01 - 0.0054;
	       if( cogp < 79 ) cogx -= 0.01; // big pix
	       if( cogp > 80 ) cogx += 0.01; // big pix
	       cout << ", cogx " << cogx;
	       cout << ", dx = " << cogx - xloc;
	       cout << endl;
	    }

	    //	h100->Fill( ilay ); // 1,2,3

	    if( ilay == 2 ) {

	       n1++;
	       xPXB1 = gX;
	       yPXB1 = gY;
	       zPXB1 = gZ;
	       uPXB1 = xloc;
	       vPXB1 = yloc;
	       ePXB1 = sqrt( vxloc );
	       fPXB1 = sqrt( vyloc );
	       phiN1 = phiN;
	       clch1 = clch;
	       ncol1 = ncol;
	       nrow1 = nrow;
	       etaX1 = etaX;
	       cogp1 = cogp;


	       //	  h101->Fill( ilad );// 1..20
	       //	  h102->Fill( imod );// 1..8
	       //
	       //	  h103->Fill( gR ); // <R1> = 4.36 cm
	       //	  h104->Fill( gF*wt );
	       //	  h105->Fill( gZ );
	       //
	       //	  h106->Fill( gF*wt, gZ ); // phi-z of hit

	       if(      ilad ==  5 ) halfmod = 1;
	       else if( ilad ==  6 ) halfmod = 1;
	       else if( ilad == 15 ) halfmod = 1;
	       else if( ilad == 16 ) halfmod = 1;

	       //	  if( !halfmod ) h107->Fill( xloc, yloc ); // hit within one module

	       // track impact parameter to beam:


	       // my crossings:

	       for( int icrss = 0; icrss < ncrss; ++icrss ){

		  double fcrss = atan2( ycrss[icrss], xcrss[icrss] );
		  double df = gF - fcrss;
		  if( df > pi ) df -= twopi;
		  else if( df < -pi ) df += twopi;
		  double du = gR*df;
		  double dz = gZ - zcrss[icrss];

		  //	    if( abs(du) < 0.01 && abs(dz) < 0.05 ) h111->Fill( gX, gY );
		  //	    h112->Fill( du*1E4 );
		  //	    h113->Fill( dz*1E4 );
		  //
		  //	    if( abs(dz) < 0.02 ) h114->Fill( du*1E4 );
		  //	    if( abs(du) < 0.01 ) h115->Fill( dz*1E4 );

	       }//crss

	    }//PXB1
	 }


	 if( subDet == PixelSubdetector::PixelEndcap) {

	    int idisk = PXFDetId(detId).disk();

	    int iblade = PXFDetId(detId).blade();
	    int ipanel =  PXFDetId(detId).panel();
	    int imodule =   PXFDetId(detId).module();


	    if( idisk == 1 ){

	       n2++;
	       xPXB2 = gX; // precise hit in CMS global coordinates
	       yPXB2 = gY;
	       zPXB2 = gZ;
	       uPXB2 = xloc; // precise hit in local coordinates (w.r.t. sensor center)
	       vPXB2 = yloc;
	       phiN2 = phiN;
	       ePXB2 = sqrt( vxloc );
	       fPXB2 = sqrt( vyloc );
	       clch2 = clch; // cluster charge [e]
	       ncol2 = ncol;
	       nrow2 = nrow;
	       etaX2 = etaX;
	       cogp2 = cogp;
	       xmid2 = xmid; // middle of sensor in global CMS coordinates
	       ymid2 = ymid;
	       xmin2 = xmin;
	       xmax2 = xmax;
	       zmin2 = ymin;
	       zmax2 = ymax;

	       xpx2_g=xPXB2;
	       xpy2_g=yPXB2;
	       xpz2_g=zPXB2;

	       det2 = transRecHit->det();


	    }//PXB2

	    if( idisk == 2 ){

	       n3++;
	       xPXB3 = gX;
	       yPXB3 = gY;
	       zPXB3 = gZ;
	       uPXB3 = xloc;
	       vPXB3 = yloc;
	       ePXB3 = sqrt( vxloc );
	       fPXB3 = sqrt( vyloc );
	       phiN3 = phiN;
	       clch3 = clch;
	       ncol3 = ncol;
	       nrow3 = nrow;
	       etaX3 = etaX;
	       cogp3 = cogp;

	    }//PXB3

	 }//PXB

      }//loop rechits

      //if( pt < 0.75 ) continue;// curls up

      //------------------------------------------------------------------------
      // refit the track:

      //edm::RefToBase<TrajectorySeed> seed = iTrack->seedRef(); // not present in RECO

      //if( idbg ) cout << "  prepare refit\n";

      PTrajectoryStateOnDet PTraj = trajectoryStateTransform::persistentState( initialTSOS, innerDetId );
      const TrajectorySeed seed( PTraj, recHitVector, alongMomentum );

      //if( idbg ) cout << "  have seed\n";

      std::vector<Trajectory> refitTrajectoryCollection = theFitter->fit( seed, coTTRHvec, initialTSOS );

      if( refitTrajectoryCollection.size() > 0 ) { // should be either 0 or 1            

	 const Trajectory& refitTrajectory = refitTrajectoryCollection.front();

	 // Trajectory.measurements:

	 Trajectory::DataContainer refitTMs = refitTrajectory.measurements();

	 if( idbg ) {
	    cout << "  refitTrajectory has " << refitTMs.size() <<" hits in subdet";
	 }

	 // hits in subDet:

	 if( idbg ) {

	    for( Trajectory::DataContainer::iterator iTM = refitTMs.begin();
		  iTM != refitTMs.end(); iTM++ ) {

	       TransientTrackingRecHit::ConstRecHitPointer iTTRH = iTM->recHit();
	       if( iTTRH->hit()->isValid() ){
		  cout << "  " << iTTRH->geographicalId().subdetId();
	       }
	    }
	    cout << endl;

	    cout << "         pt " << refitTrajectory.geometricalInnermostState().globalMomentum().perp();
	    cout << ", eta " << refitTrajectory.geometricalInnermostState().globalMomentum().eta();
	    cout << ", phi " << refitTrajectory.geometricalInnermostState().globalMomentum().barePhi()*wt;
	    cout << ", at R " << refitTrajectory.geometricalInnermostState().globalPosition().perp();
	    cout << ", z " << refitTrajectory.geometricalInnermostState().globalPosition().z();
	    cout << ", phi " << refitTrajectory.geometricalInnermostState().globalPosition().barePhi()*wt;
	    cout << endl;

	 }//dbg

	 // trajectory residuals:

	 for( Trajectory::DataContainer::iterator iTM = refitTMs.begin();
	       iTM != refitTMs.end(); iTM++ ) {

	    if( ! iTM->recHit()->isValid() ) continue;

	    DetId detId = iTM->recHit()->geographicalId();

	    uint32_t subDet = detId.subdetId();

	    // enum SubDetector{ PixelBarrel=1, PixelEndcap=2 };
	    // enum SubDetector{ TIB=3, TID=4, TOB=5, TEC=6 };

	    double xHit = iTM->recHit()->localPosition().x(); // primary measurement direction
	    double yHit = iTM->recHit()->localPosition().y(); // always 0 in strips

	    int ilay = 0;
	    if( detId.subdetId() == 1 ){
	       ilay = PXBDetId( detId ).layer();
	    }
	    /*
	       if( subDet == 1 && idbg ){//1=PXB
	       cout << "  PXB layer " << ilay << endl;
	       }
	     */

	    double dx = xHit - iTM->predictedState().localPosition().x();
	    double dy = yHit - iTM->predictedState().localPosition().y();
	    double vxh = iTM->recHit()->localPositionError().xx();//covariance
	    double vxt = iTM->predictedState().localError().positionError().xx();//

	    //if( subDet == 1 && idbg ){//1=PXB
	    if( subDet == 4 && idbg ){//4=TID
	       cout << "  predictdStateResid = " << dx*1E4 << " um";
	       cout << ", eh = " << sqrt(vxh)*1E4 << " um";
	       cout << ", et = " << sqrt(vxt)*1E4 << " um";
	       cout << endl;
	    }

	    TrajectoryStateOnSurface combinedPredictedState =
	       TrajectoryStateCombiner().combine( iTM->forwardPredictedState(), iTM->backwardPredictedState() );

	    if( ! combinedPredictedState.isValid() ) continue;//skip hit

	    if( jdbg ) cout << "  have combinedPredictedState\n";

	    double R = combinedPredictedState.globalPosition().perp();
	    double F = combinedPredictedState.globalPosition().barePhi();
	    double Z = combinedPredictedState.globalPosition().z();

	    double xptch;
	    double yptch;	

	    if( subDet <  3 ){//1,2=pixel
	       PixelTopology & pixelTopol = (PixelTopology&) iTM->recHit()->detUnit()->topology();
	       xptch = pixelTopol.pitch().first;
	       yptch = pixelTopol.pitch().second;
	    }
	    else {//strip
	       StripTopology & stripTopol = (StripTopology&) iTM->recHit()->detUnit()->topology();
	       xptch = stripTopol.localPitch( combinedPredictedState.localPosition() );
	       yptch = stripTopol.localStripLength( combinedPredictedState.localPosition() );
	    }

	    //	if( subDet == 4 && idbg ){//4=TID
	    //	  cout << "  TID side " << TIDDetId(detId).side();
	    //	  cout << ", disk " << TIDDetId(detId).diskNumber();
	    //	  cout << ", ring " << TIDDetId(detId).ringNumber();
	    //	  cout << ", stereo " << TIDDetId(detId).isStereo();
	    //	  cout << endl;
	    //	  cout << setprecision(4);
	    //	  cout << "  xHit " << xHit;
	    //	  cout << ", yHit " << yHit;
	    //	  cout << setprecision(1);
	    //	  cout << ", pitch " << xptch*1E4 << " um";
	    //	  cout << setprecision(4);
	    //	  cout << endl;
	    //	}

	    dx = xHit - combinedPredictedState.localPosition().x(); //x = primary measurement
	    dy = yHit - combinedPredictedState.localPosition().y(); //
	    vxh = iTM->recHit()->localPositionError().xx();//covariance
	    vxt = combinedPredictedState.localError().positionError().xx();//

	    // angles of incidence:
	    // local z = upwards = normal vector
	    // local x = primary measurement direction
	    // local y = secondary measurement direction

	    double alf_inc = atan2( combinedPredictedState.localDirection().x(), combinedPredictedState.localDirection().z() );
	    double bet_inc = atan2( combinedPredictedState.localDirection().y(), combinedPredictedState.localDirection().z() );

	    double phiinc = alf_inc;
	    if( phiinc > pihalf ) phiinc -= pi;
	    else if( phiinc < -pihalf ) phiinc += pi;

	    if( bet_inc > pihalf ) bet_inc -= pi;
	    else if( bet_inc < -pihalf ) bet_inc += pi;

	    //if( subDet == 1 && idbg ){//1=PXB
	    if( subDet == 4 && idbg ){//4=TID

	       cout << setprecision(1);
	       cout << "  combinedStateResid = " << dx*1E4 << " um";
	       cout << ", eh = " << sqrt(vxh)*1E4 << " um";
	       cout << ", et = " << sqrt(vxt)*1E4 << " um";
	       cout << ", dy = " << dy*1E4 << " um";
	       cout << setprecision(4);
	       cout << ", track at x " << combinedPredictedState.localPosition().x();
	       cout << ", y " << combinedPredictedState.localPosition().y();
	       cout << endl;
	    }

	    // use Topology. no effect in PXB, essential in TID, TEC

	    const Topology* theTopology = &(iTM->recHit()->detUnit()->topology() );

	    // MeasurementPoint [pitch] (like channel number)

	    // TODO: Use the measurementPosition(point, trackdir) version of this function in order to take bows into account!
	    MeasurementPoint hitMeasurement = theTopology->measurementPosition( iTM->recHit()->localPosition() );

	    // TID and TEC have trapezoidal detectors:
	    // translation from channel number into local x depends on local y
	    // track prediction has local x,y => can convert into proper channel number MeasurementPoint:

	    // TODO: Use the measurementPosition(point, trackdir) version of this function in order to take bows into account!
	    MeasurementPoint combinedPredictedMeasurement = theTopology->measurementPosition( combinedPredictedState.localPosition() );

	    dx = hitMeasurement.x() - combinedPredictedMeasurement.x(); //in units of pitch
	    dy = hitMeasurement.y() - combinedPredictedMeasurement.y(); //in units of pitch
	    dx = dx * xptch;//convert back into [cm] using local pitch
	    dy = dy * yptch;//[cm]

	    if( jdbg ) cout << "  have combinedPredictedMeasurement\n";

	    //if( subDet == 1 && idbg ){//1=PXB
	    //	if( subDet == 4 && idbg ){//4=TID
	    //
	    //	  cout << setprecision(1);
	    //	  cout << "  topologyStateResid = " << dx*1E4 << " um";
	    //	  cout << setprecision(4);
	    //	  cout << ", hit at x " << hitMeasurement.x();
	    //	  cout << ", y " << hitMeasurement.y();
	    //	  cout << ", track at x " << combinedPredictedMeasurement.x();
	    //	  cout << ", y " << combinedPredictedMeasurement.y();
	    //	  cout << endl;
	    //	}


	    // use topology: needed for TEC

	    double xx = hitMeasurement.x();
	    double yy;
	    if( subDet < 3 ) // pixel is 2D
	       yy = hitMeasurement.y();
	    else // strips are 1D
	       yy = combinedPredictedMeasurement.y();

	    MeasurementPoint mp( xx, yy );

	    //2012 StripTopology & stripTopol = (StripTopology&) iTM->recHit()->detUnit()->topology();

	    Surface::LocalPoint lp = theTopology->localPosition( mp );
	    //2012 Surface::LocalPoint lp = stripTopol.localPosition( mp ); // makes no difference in TEC

	    //const GeomDet * myGeomDet = pTG->idToDet( detId );
	    const GeomDet * myGeomDet = iTM->recHit()->det(); // makes no difference in TEC
	    Surface::GlobalPoint gp = myGeomDet->toGlobal( lp );

	    double gX = gp.x();
	    double gY = gp.y();
	    double gZ = gp.z();



	    double lX = lp.x();
	    double lY = lp.y();
	    double lZ = lp.z();

	    //double phiN = myGeomDet->surface().normalVector().barePhi();//normal vector
	    double phiN = iTM->recHit()->det()->surface().normalVector().barePhi();//normal vector


	    //2012: overwrite PXB global coordinates once more, using topology:

	    if( subDet == PixelSubdetector::PixelBarrel ) {

	       int ilay = PXBDetId(detId).layer();

	       if( ilay == 2 ) {
		  xPXB1 = gX;
		  yPXB1 = gY;
		  zPXB1 = gZ;
	       }
	    }

	    else  if( subDet == PixelSubdetector::PixelEndcap) {

	       int idisk = PXFDetId(detId).disk();

	       int iblade = PXFDetId(detId).blade();
	       int ipanel =  PXFDetId(detId).panel();
	       int imodule =   PXFDetId(detId).module();


	       if( idisk == 1 ) {
		  xPXB2 = gX;
		  yPXB2 = gY;
		  zPXB2 = gZ;

		  xpx2_l=lX;
		  xpy2_l=lY;
		  xpz2_l=lZ;
		  //		  cout<<"gx="<<gX<<"  gy="<<gY<<"   gz="<<gZ<<endl;


	       }

	       else if( idisk == 2 ) {
		  xPXB3 = gX;
		  yPXB3 = gY;
		  zPXB3 = gZ;
	       }

	    }


	 }//loop iTM

	 }//refitted trajectory

	 //if( pt < 0.75 ) continue;// curls up

	 //------------------------------------------------------------------------
	 // refit once more, leaving out hit in 2nd PXB:

	 double kap2 = 0;
	 bool refit2valid = 0;

	 if( n2 > 0 ){

	    Trajectory::RecHitContainer nyTTRHvec; // for fit

	    for( vector<TransientTrackingRecHit::RecHitPointer>::iterator iTTRH = myTTRHvec.begin();
		  iTTRH != myTTRHvec.end(); ++iTTRH ) {

	       if( idbg == 9 ) {
		  cout << "  hit " << distance( (vector<TransientTrackingRecHit::RecHitPointer>::iterator)myTTRHvec.begin(), iTTRH );
		  if( (*iTTRH)->hit()->isValid() ){
		     cout << ": subdet " << (*iTTRH)->hit()->geographicalId().subdetId();
		     cout << ", weight " << (*iTTRH)->weight(); // default weight is 1
		     cout << endl;
		  }
		  else cout << " not valid\n";
	       }

	       int iuse = 1;
	       if( (*iTTRH)->hit()->isValid() ){
		  if( (*iTTRH)->hit()->geographicalId().subdetId() == 1 ){ //PXB
		     if( PXBDetId( (*iTTRH)->hit()->geographicalId() ).layer() == 2 ) iuse = 0; // skip PXB2: unbiased track
		  }
	       }
	       if( iuse ) nyTTRHvec.push_back( *iTTRH );
	    }//copy

	    if( idbg ) {
	       cout << "  all hits " << myTTRHvec.size();
	       cout << ", without PXB2 " << nyTTRHvec.size();
	       cout << endl;
	    }

	    // re-fit without PXB2:

	    std::vector<Trajectory> refitTrajectoryVec2 = theFitter->fit( seed, nyTTRHvec, initialTSOS );

	    if( refitTrajectoryVec2.size() > 0 ) { // should be either 0 or 1            

	       const Trajectory& refitTrajectory2 = refitTrajectoryVec2.front();

	       // Trajectory.measurements:

	       Trajectory::DataContainer refitTMvec2 = refitTrajectory2.measurements();

	       if( idbg ) {
		  cout << "  refitTrajectory2 has " << refitTMvec2.size() <<" hits in subdet";
	       }

	       for( Trajectory::DataContainer::iterator iTM = refitTMvec2.begin();
		     iTM != refitTMvec2.end(); iTM++ ) {

		  TransientTrackingRecHit::ConstRecHitPointer iTTRH = iTM->recHit();
		  if( iTTRH->hit()->isValid() ){
		     if( idbg ) cout << "  " << iTTRH->geographicalId().subdetId();
		  }
	       }
	       if( idbg ) cout << endl;

	       if( idbg ) {
		  cout << "  ndof " << refitTrajectory2.ndof();
		  cout << ", found " << refitTrajectory2.foundHits();
		  cout << ", missing " << refitTrajectory2.lostHits();
		  cout << ", chi2 " << refitTrajectory2.chiSquared();
		  cout << ", /ndof " << refitTrajectory2.chiSquared() / refitTrajectory2.ndof();
		  cout << endl;
	       }

	       if( idbg ) {
		  cout << "         pt " << refitTrajectory2.geometricalInnermostState().globalMomentum().perp();
		  cout << ", eta " << refitTrajectory2.geometricalInnermostState().globalMomentum().eta();
		  cout << ", phi " << refitTrajectory2.geometricalInnermostState().globalMomentum().barePhi()*wt;
		  cout << ", at R " << refitTrajectory2.geometricalInnermostState().globalPosition().perp();
		  cout << ", z " << refitTrajectory2.geometricalInnermostState().globalPosition().z();
		  cout << ", phi " << refitTrajectory2.geometricalInnermostState().globalPosition().barePhi()*wt;
		  cout << endl;
	       }
	       refit2valid = 1;
	       kap2 = refitTrajectory2.geometricalInnermostState().transverseCurvature();

	       // now use it!

	    }//refit2
	    else 
	       if( idbg ) cout << "  no refit\n";

	 }// n2 > 0

	 //------------------------------------------------------------------------
	 // 1-2-3 pixel triplet:

	 if( n1*n2*n3 > 0 ) {

	    { // let's open a scope, so we can redefine the variables further down

	       if( jdbg ) cout << "  triplet 1+3 -> 2\n";

	       double f2 = atan2( yPXB2, xPXB2 );//position angle in layer 2

	       double ax = xPXB3 - xPXB1;
	       double ay = yPXB3 - yPXB1;
	       double aa = sqrt( ax*ax + ay*ay ); // from 1 to 3

	       double xmid = 0.5 * ( xPXB1 + xPXB3 );
	       double ymid = 0.5 * ( yPXB1 + yPXB3 );
	       double bx = xPXB2 - xmid;
	       double by = yPXB2 - ymid;
	       double bb = sqrt( bx*bx + by*by ); // from mid point to point 2

	       //	h406->Fill( hp.numberOfValidTrackerHits() );
	       //	h407->Fill( hp.numberOfValidPixelBarrelHits() );
	       //	h408->Fill( hp.trackerLayersWithMeasurement() );

	       // Author: Johannes Gassner (15.11.1996)
	       // Make track from 2 space points and kappa (cmz98/ftn/csmktr.f)
	       // Definition of the Helix :

	       // x( t ) = X0 + KAPPA^-1 * SIN( PHI0 + t )
	       // y( t ) = Y0 - KAPPA^-1 * COS( PHI0 + t )          t > 0
	       // z( t ) = Z0 + KAPPA^-1 * TAN( DIP ) * t

	       // Center of the helix in the xy-projection:

	       // X0 = + ( DCA - KAPPA^-1 ) * SIN( PHI0 )
	       // Y0 = - ( DCA - KAPPA^-1 ) * COS( PHI0 )

	       // Point 1 must be in the inner layer, 3 in the outer:

	       double r1 = sqrt( xPXB1*xPXB1 + yPXB1*yPXB1 );
	       double r3 = sqrt( xPXB3*xPXB3 + yPXB3*yPXB3 );

	       if( r3-r1 < 2.0 ) cout << "warn r1 = " << r1 << ", r3 = " << r3 << endl;

	       // Calculate the centre of the helix in xy-projection that
	       // transverses the two spacepoints. The points with the same
	       // distance from the two points are lying on a line.
	       // LAMBDA is the distance between the point in the middle of
	       // the two spacepoints and the centre of the helix.

	       // we already have kap and rho = 1/kap

	       double lam = sqrt( -0.25 + 
		     rho*rho / ( ( xPXB1 - xPXB3 )*( xPXB1 - xPXB3 ) + ( yPXB1 - yPXB3 )*( yPXB1 - yPXB3 ) ) );

	       // There are two solutions, the sign of kap gives the information
	       // which of them is correct:

	       if( kap > 0 ) lam = -lam;

	       // ( X0, Y0 ) is the centre of the circle
	       // that describes the helix in xy-projection:

	       double x0 =  0.5*( xPXB1 + xPXB3 ) + lam * ( -yPXB1 + yPXB3 );
	       double y0 =  0.5*( yPXB1 + yPXB3 ) + lam * (  xPXB1 - xPXB3 );

	       // Calculate theta:

	       double num = ( yPXB3 - y0 ) * ( xPXB1 - x0 ) - ( xPXB3 - x0 ) * ( yPXB1 - y0 );
	       double den = ( xPXB1 - x0 ) * ( xPXB3 - x0 ) + ( yPXB1 - y0 ) * ( yPXB3 - y0 );
	       double tandip = kap * ( zPXB3 - zPXB1 ) / atan( num / den );
	       double udip = atan(tandip);
	       //double utet = pihalf - udip;

	       // To get phi0 in the right interval one must distinguish
	       // two cases with positve and negative kap:

	       double uphi;
	       if( kap > 0 ) uphi = atan2( -x0,  y0 );
	       else          uphi = atan2(  x0, -y0 );

	       // The distance of the closest approach DCA depends on the sign
	       // of kappa:

	       double udca;
	       if( kap > 0 ) udca = rho - sqrt( x0*x0 + y0*y0 );
	       else          udca = rho + sqrt( x0*x0 + y0*y0 );

	       // angle from first hit to dca point:

	       double dphi = atan( ( ( xPXB1 - x0 ) * y0 - ( yPXB1 - y0 ) * x0 )
		     / ( ( xPXB1 - x0 ) * x0 + ( yPXB1 - y0 ) * y0 ) );

	       double uz0 = zPXB1 + tandip * dphi * rho;
	       //
	       //	h401->Fill( zPXB2 );
	       //	h402->Fill( uphi - iTrack->phi() );
	       //	h403->Fill( udca - iTrack->d0() );
	       //	h404->Fill( udip - iTrack->lambda() );
	       //	h405->Fill( uz0  - iTrack->dz() );

	       // interpolate to middle hit:
	       // cirmov
	       // we already have rinv = -kap

	       double cosphi = cos(uphi);
	       double sinphi = sin(uphi);
	       double dp = -xPXB2*sinphi + yPXB2*cosphi + udca;
	       double dl = -xPXB2*cosphi - yPXB2*sinphi;
	       double sa = 2*dp + rinv * ( dp*dp + dl*dl );
	       double dca2 = sa / ( 1 + sqrt(1 + rinv*sa) );// distance to hit 2
	       double ud = 1 + rinv*udca;
	       double phi2 = atan2( -rinv*xPXB2 + ud*sinphi, rinv*yPXB2 + ud*cosphi );//direction

	       double phiinc = phi2 - phiN2;//angle of incidence in rphi w.r.t. normal vector

	       // phiN alternates inward/outward
	       // reduce phiinc:

	       if( phiinc > pihalf ) phiinc -= pi;
	       else if( phiinc < -pihalf ) phiinc += pi;

	       // arc length:

	       double xx = xPXB2 + dca2 * sin(phi2); // point on track
	       double yy = yPXB2 - dca2 * cos(phi2);

	       double vx = xx - xmid2;//from module center
	       double vy = yy - ymid2;
	       double vv = sqrt( vx*vx + vy*vy );

	       double f0 = uphi;//
	       double kx = kap*xx;
	       double ky = kap*yy;
	       double kd = kap*udca;

	       // Solve track equation for s:

	       double dx = kx - (kd-1)*sin(f0);
	       double dy = ky + (kd-1)*cos(f0);
	       double ks = atan2( dx, -dy ) - f0;// turn angle

	       // Limit to half-turn:

	       if(      ks >  pi ) ks = ks - twopi;
	       else if( ks < -pi ) ks = ks + twopi;

	       double s = ks*rho; // signed
	       double uz2 = uz0 + s*tandip; // track z at R2
	       double dz2 = zPXB2 - uz2;

	       Surface::GlobalPoint gp2( xx, yy, uz2 );
	       Surface::LocalPoint lp2 = det2->toLocal( gp2 );

	       // local x = phi
	       // local y = z in barrel
	       // local z = radial in barrel (thickness)

	       double xpix = fmod( uPXB2 + 0.82, 0.01 ); // xpix = 0..0.01 reconstructed
	       double xpx2 = fmod( uPXB2 + 0.82, 0.02 ); // xpix = 0..0.02 reconstructed
	       double xpx1 = fmod( uPXB1 + 0.82, 0.01 ); // xpix = 0..0.01 reconstructed
	       double xpx3 = fmod( uPXB3 + 0.82, 0.01 ); // xpix = 0..0.01 reconstructed

	       //double dpix = fmod( uPXB2 + dca2 + 0.82, 0.01 ); // dpix = 0..0.01 predicted

	       double vpix = fmod( vv, 0.01 ); // vpix = 0..0.01 predicted
	       if( uPXB2 < 0 ) vpix = -vpix; // vv is unsigned distance from module center

	       double lpix = fmod( lp2.x() + 0.82, 0.01 ); // lpix = 0..0.01 predicted
	       double tpix = fmod( lp2.x() + 0.82, 0.02 ); // tpix = 0..0.02 predicted

	       double zpix = fmod( lp2.y() + 3.24, 0.015 ); // zpix = 0..0.015 predicted
	       double spix = fmod( lp2.y() + 3.24, 0.03  ); // spix = 0..0.03  predicted

	       int smin = zmin2%52; // 0..51 column along z
	       int smax = zmax2%52; // 0..51 column along z

	       double cogx = (cogp2 + 0.5 - 80) * 0.01 - 0.0054; // Lorentz shift
	       if( cogp2 < 79 ) cogx -= 0.01; // big pix
	       if( cogp2 > 80 ) cogx += 0.01; // big pix

	       double mpix = fmod( cogx + 0.82, 0.01 ); // mpix = 0..0.01 from cluster COG
	       double cogdx = cogx - lp2.x(); // residual

	       // hybrid method:

	       double hybx = uPXB2; // template
	       if( mpix*1E4 < 20 ) hybx = cogx; // COG
	       if( mpix*1E4 > 75 ) hybx = cogx;
	       //double hpix = fmod( hybx + 0.82, 0.01 ); // hpix = 0..0.01 from cluster hybrid method
	       double hybdx = hybx - lp2.x(); // residual

	       bool halfmod = 0;
	       if(      ilad2 ==  8 ) halfmod = 1;
	       else if( ilad2 ==  9 ) halfmod = 1;
	       else if( ilad2 == 24 ) halfmod = 1;
	       else if( ilad2 == 25 ) halfmod = 1;


	       residue=dca2*1E4 ;
	       final_pt=pt;
	       final_phi=phi;

//	       if( pt > 4 ) {
//		  //
//		  //	  h308->Fill( bb/aa ); // lever arm
//		  //	  h409->Fill( f2*wt, phiinc*wt );
//		  h410->Fill( dca2*1E4 );
//		  //
//	       } // pt > 4

	       newtree->Fill();

	    }//triplet 1+3 -> 2




//	    if( pt > 4 ) {

	       // loop over hits on this track:

	       for( vector<TransientTrackingRecHit::RecHitPointer>::iterator jTTRH = myTTRHvec.begin();
		     jTTRH != myTTRHvec.end(); ++jTTRH ) {

		  if( ! (*jTTRH)->hit()->isValid() ) continue;

		  Surface::GlobalPoint gp = (*jTTRH)->globalPosition();

		  Trajectory::RecHitContainer nyTTRHvec; // for fit

		  for( vector<TransientTrackingRecHit::RecHitPointer>::iterator iTTRH = myTTRHvec.begin();
			iTTRH != myTTRHvec.end(); ++iTTRH ) {

		     if( iTTRH == jTTRH ) continue;//skip hit i

		     nyTTRHvec.push_back( *iTTRH );//copy all others

		  }//copy

		  // re-fit without hit i:

		  std::vector<Trajectory> refitTrajectoryVec2 = theFitter->fit( seed, nyTTRHvec, initialTSOS );

		  if( refitTrajectoryVec2.size() > 0 ) { // should be either 0 or 1

		     const Trajectory& refitTrajectory2 = refitTrajectoryVec2.front();

		     // Trajectory.measurements:

		     const TrajectoryMeasurement closestTM = refitTrajectory2.closestMeasurement( gp );

		     TrajectoryStateOnSurface closestTSOS = closestTM.updatedState();


		     TrajectoryStateOnSurface propTSOS = thePropagator->propagate( closestTSOS, (*jTTRH)->det()->surface() );

		     if( propTSOS.isValid() ){

			const Topology* theTopology = &( (*jTTRH)->detUnit()->topology() );

			// MeasurementPoint [pitch] (like channel number)

			MeasurementPoint hitMeasurement = theTopology->measurementPosition( (*jTTRH)->localPosition() );

			// TID and TEC have trapezoidal detectors:
			// translation from channel number into local x depends on local y
			// track prediction has local x,y => can convert into proper channel number MeasurementPoint:

			MeasurementPoint predictedPosition = theTopology->measurementPosition( propTSOS.localPosition() );

			double resid = hitMeasurement.x() - predictedPosition.x();//[pitch]

			DetId detId = (*jTTRH)->hit()->geographicalId();
			uint32_t subDet = detId.subdetId();

			double xptch;
			//double yptch;

			if( subDet <  3 ){//1,2=pixel
			   PixelTopology & pixelTopol = (PixelTopology&) (*jTTRH)->detUnit()->topology();
			   xptch = pixelTopol.pitch().first;
			   //yptch = pixelTopol.pitch().second;
			}
			else {//strip
			   StripTopology & stripTopol = (StripTopology&) (*jTTRH)->detUnit()->topology();
			   xptch = stripTopol.localPitch( propTSOS.localPosition() );
			   //yptch = stripTopol.localStripLength( propTSOS.localPosition() );
			}

			resid = resid * xptch;//[cm]

			if( subDet == PixelSubdetector::PixelEndcap ) {

			   if( PXFDetId(detId).disk() == 1 ) {
			      //                u110->Fill( resid*1E4 );
			      resid_disk1=resid*1E4;
			   }//layer

			   if( PXFDetId(detId).disk() == 2 ) {
			      resid_disk2=resid*1E4;
			      //              u120->Fill( resid*1E4 );
			   }//layer


			}//PXB


		     }
		  }

	       }

	    //}

	    //------------------------------------------------------------------------
	    // Triplet 1+3 -> 2 with refitted track:

	    if( refit2valid ){

	       if( jdbg ) cout << "  triplet 1+3 -> 2 refitted\n";

	       double f2 = atan2( yPXB2, xPXB2 );//position angle in layer 2

	       double rho2 = 1/kap2;

	       double lam = sqrt( -0.25 + 
		     rho2*rho2 / ( ( xPXB1 - xPXB3 )*( xPXB1 - xPXB3 ) + ( yPXB1 - yPXB3 )*( yPXB1 - yPXB3 ) ) );

	       // There are two solutions, the sign of kap gives the information
	       // which of them is correct:

	       if( kap2 > 0 ) lam = -lam;

	       // ( X0, Y0 ) is the centre of the circle
	       // that describes the helix in xy-projection:

	       double x0 =  0.5*( xPXB1 + xPXB3 ) + lam * ( -yPXB1 + yPXB3 );
	       double y0 =  0.5*( yPXB1 + yPXB3 ) + lam * (  xPXB1 - xPXB3 );

	       // Calculate theta:

	       double num = ( yPXB3 - y0 ) * ( xPXB1 - x0 ) - ( xPXB3 - x0 ) * ( yPXB1 - y0 );
	       double den = ( xPXB1 - x0 ) * ( xPXB3 - x0 ) + ( yPXB1 - y0 ) * ( yPXB3 - y0 );
	       double tandip = kap2 * ( zPXB3 - zPXB1 ) / atan( num / den );
	       double udip = atan(tandip);
	       //double utet = pihalf - udip;

	       // To get phi0 in the right interval one must distinguish
	       // two cases with positve and negative kap:

	       double uphi;
	       if( kap2 > 0 ) uphi = atan2( -x0,  y0 );
	       else          uphi = atan2(  x0, -y0 );

	       // The distance of the closest approach DCA depends on the sign
	       // of kappa:

	       double udca;
	       if( kap2 > 0 ) udca = rho2 - sqrt( x0*x0 + y0*y0 );
	       else          udca = rho2 + sqrt( x0*x0 + y0*y0 );

	       // angle from first hit to dca point:

	       double dphi = atan( ( ( xPXB1 - x0 ) * y0 - ( yPXB1 - y0 ) * x0 )
		     / ( ( xPXB1 - x0 ) * x0 + ( yPXB1 - y0 ) * y0 ) );

	       double uz0 = zPXB1 + tandip * dphi * rho2;


	       // interpolate to middle hit:
	       // cirmov

	       double rinv2 = -kap2;
	       double cosphi = cos(uphi);
	       double sinphi = sin(uphi);
	       double dp = -xPXB2*sinphi + yPXB2*cosphi + udca;
	       double dl = -xPXB2*cosphi - yPXB2*sinphi;
	       double sa = 2*dp + rinv2 * ( dp*dp + dl*dl );
	       double dca2 = sa / ( 1 + sqrt(1 + rinv2*sa) );// distance to hit 2
	       double ud = 1 + rinv2*udca;
	       double phi2 = atan2( -rinv2*xPXB2 + ud*sinphi, rinv2*yPXB2 + ud*cosphi );//direction

	       double phiinc = phi2 - phiN2;//angle of incidence in rphi w.r.t. normal vector

	       // phiN alternates inward/outward
	       // reduce phiinc:

	       if( phiinc > pihalf ) phiinc -= pi;
	       else if( phiinc < -pihalf ) phiinc += pi;

	       // arc length:

	       double xx = xPXB2 + dca2 * sin(phi2); // point on track
	       double yy = yPXB2 - dca2 * cos(phi2);

	       double f0 = uphi;//
	       double kx = kap2*xx;
	       double ky = kap2*yy;
	       double kd = kap2*udca;

	       // Solve track equation for s:

	       double dx = kx - (kd-1)*sin(f0);
	       double dy = ky + (kd-1)*cos(f0);
	       double ks = atan2( dx, -dy ) - f0;// turn angle

	       //---  Limit to half-turn:

	       if(      ks >  pi ) ks = ks - twopi;
	       else if( ks < -pi ) ks = ks + twopi;

	       double s = ks*rho2;// signed
	       double uz2 = uz0 + s*tandip; //track z at R2
	       double dz2 = zPXB2 - uz2;


	    }//triplet 1+3 -> 2 refitted

	    //
	 }//pt cut



	 }// loop over tracks

	 //  h028->Fill( sumpt );
	 //  h029->Fill( sumq );
	 //
	 }//event
	 //----------------------------------------------------------------------
	 // method called just after ending the event loop:
	 //
	 void PxlTest::endJob() {

	    std::cout << "end of job after " << myCounters::neve << " events.\n";

	 }

	 //define this as a plug-in
	 DEFINE_FWK_MODULE(PxlTest);
