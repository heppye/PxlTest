void final_disk(){
  //  SetStyle();
  gROOT->Reset();
  gROOT->SetStyle("Plain");
  gStyle->SetOptFit(1111);
  gStyle->SetOptStat("");
  gStyle->SetLegendBorderSize(0);

  gStyle->SetErrorX(0);
  gSystem->Load("libRooFit");
  using namespace RooFit;

  TCanvas *c1h = new TCanvas("c1h","kskpi",10,10,800,600);

  c1h->SetTicks(2,2);
  c1h->SetTopMargin(0.12);
  c1h->SetBottomMargin(0.2);
  c1h->SetLeftMargin(0.18);
  c1h->SetRightMargin(0.1);

  TChain * chain = new TChain("newtree"," ");
  chain->Add("./job.root/Histos/newtree");
  TH1D *hdh0 = new TH1D("hdh0","hdh0", 20, 0,20);
  chain->Draw("xblade>>hdh0");


//  hph1->Sumw2();
//  hph2->Sumw2();
//
//  hih1->Sumw2();
//  hih2->Sumw2();
//
//  hdh1->Sumw2();
//  hdh2->Sumw2();
/*
  hph1->GetYaxis()->SetRangeUser(0.0,30);

  hph1->SetTitle("");
  hph1->GetXaxis()->SetTitle("Integrated Luminosity (fb^{-1})");
  hph1->GetYaxis()->SetTitle("FPix Residue");
  hph1->GetXaxis()->SetTitleOffset(1.4);
  hph1->GetXaxis()->SetTitleSize(0.06);
  hph1->GetXaxis()->SetLabelOffset(0.01);
  hph1->GetXaxis()->SetLabelSize(0.04);
  hph1->GetYaxis()->SetTitleOffset(1.2);
  hph1->GetYaxis()->SetTitleSize(0.06);
  hph1->GetYaxis()->SetLabelOffset(0.01);

  hph1->GetYaxis()->SetLabelSize(0.04);
  hph1->GetXaxis()->CenterTitle();
  hph1->SetMarkerStyle(25);
  hph1->SetMarkerColor(1);
  hph1->SetMarkerSize(1);
  hph1->Draw();
*/


//
//  TLatex *text = new TLatex(1.0,12,"Minumum Bias 2012C with |p_{T}|>4 GeV"); 
//  text->SetTextSize(0.04);
//  text->Draw();
//
//
//
//
//  //TLine *line = new TLine(1.400,1.00000,0.0,1.000);
//  // line->Draw();
//
//
//
//
//  leg = new TLegend(1.0,10,3.2,12);

 // leg = new TLegend(0.5,0.4,0.85,0.5);

 // leg->SetFillColor(0);
 // leg->SetTextSize(0.04);
 // leg->SetHeader("");
 // leg->AddEntry(hph1,"2012C with |p_{T}|>4 GeV","p");
////
 // leg->Draw();
//

  c1h->SaveAs("iblade.eps");
  c1h->SaveAs("iblade.png");
  c1h->SaveAs("iblade.jpg");


  TH1D *hdh1 = new TH1D("hdh1","hdh1", 5, -1,4);
  chain->Draw("xmodule>>hdh1");

  c1h->SaveAs("imodule.eps");
  c1h->SaveAs("imodule.png");
  c1h->SaveAs("imodule.jpg");


  TH1D *hdh2 = new TH1D("hdh2","hdh2", 5, -1,4);
  chain->Draw("xpanel>>hdh2");

  c1h->SaveAs("ipanel.eps");
  c1h->SaveAs("ipanel.png");
  c1h->SaveAs("ipanel.jpg");
}



