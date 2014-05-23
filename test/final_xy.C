void final_xy(){

	SetStyle();
	SetStyle();
	SetPrelimStyle();

	gROOT->SetStyle("Plain");
	gStyle->SetOptFit(1111);
	gStyle->SetOptStat("e");
	gStyle->SetTitleFont(42,"XY");
	gStyle->SetLabelFont(42,"XY");
	gStyle->SetTextFont(42);




	//gROOT->Reset();
	gROOT->SetStyle("Plain");
	gStyle->SetOptFit(1111);
	gStyle->SetOptStat("");
	gStyle->SetLegendBorderSize(0);



	SetStyle();
	gStyle->SetErrorX(0);
	gSystem->Load("libRooFit");
	using namespace RooFit;

	TCanvas *c1h = new TCanvas("c1h","Jpsikkk",10,10,800,600);

	c1h->SetTicks(2,2);
	c1h->SetTopMargin(0.12);
	c1h->SetBottomMargin(0.2);
	c1h->SetLeftMargin(0.18);
	c1h->SetRightMargin(0.1);

//	TFile  f1("../root/job_all.root"); 


	TChain * chain = new TChain("newtree","");
        chain->Add("./job.root/Histos/newtree");  
        TH2D *hdh0 = new TH2D("hdh0", "hdh0", 400,-15,15,400,-15,15);

        chain->Draw("xpx2_l:xpy2_l>>hdh0");
	//  TDirectory* dir  = (TDirectory*)file->Get(dirname.c_str());
//	TDirectory *dir =(TDirectory*) chain.Get("Histos");
//	TH1D * residue =(TH1D*) dir->Get("h410");
	//  TTree*      tree = (TTree*)dir->Get("t");
	//
	//	RooRealVar x("x","x",-150,150) ;
	//
	//
	//
	//	RooDataHist dh("dh","dh",x,Import(*residue)); 
	//
	//	RooRealVar mean_1("Mean","mean of gaussians",0.0,0.0,0.0) ;
	//
	//	RooRealVar sigma_1("#sigma_1","sigma of gaussians",12.1,10.0,30.0);
	//
	//	RooRealVar sigma_2("#sigma_2","sigma of gaussians",12.1,10.0,50.0);
	//
	//
	//	RooGaussian  gauss1("sig","Signal ",x,mean_1,sigma_1) ;
	//
	//	RooGaussian  gauss2("sig","Signal ",x,mean_1,sigma_2) ;
	//
	//
	//
	//	RooRealVar gm1frac("gm1frac","fraction of gm1",0.5,1.0) ;
	//	RooAddModel gauss("gmsum","sum of gm1 and gm2",RooArgList(gauss1,gauss2),gm1frac);
	//
	//
	//	RooPlot* frame = x.frame(Title("Imported TH1 with Poisson error bars")) ;
	//	// Make plot of binned dataset showing Poisson error bars (RooFit default)
	//	dh.plotOn(frame) ; 
	//
	//	gauss.fitTo(dh);
	//
	//	gauss.fitTo(dh) ;
	//	gauss.plotOn(frame); 
	//
	//	frame->Draw();
	//

//	residue->Draw(); 
//	residue->SetTitle("");
//	residue->GetXaxis()->SetTitle("Residue on Endcap Disk 1");
//	residue->GetYaxis()->SetTitle("Tracks");
//	residue->GetXaxis()->SetTitleOffset(1.4);
//	residue->GetXaxis()->SetTitleSize(0.06);
//	residue->GetXaxis()->SetLabelOffset(0.01);
//	residue->GetXaxis()->SetLabelSize(0.04);
//	residue->GetYaxis()->SetTitleOffset(1.2);
//	residue->GetYaxis()->SetTitleSize(0.06);
//	residue->GetYaxis()->SetLabelOffset(0.01);
//
//	residue->GetYaxis()->SetLabelSize(0.04);
//	residue->GetXaxis()->CenterTitle();
//	residue->SetMarkerStyle(20);
//	residue->SetMarkerColor(1);
//	residue->SetMarkerSize(1);

	//c1h->Modified();
	c1h->SaveAs("x_y.eps");
	c1h->SaveAs("x_y.png");


}



