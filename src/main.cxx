 
#include "iostream"
#include "genFunctionCLs.h"
#include "fitFunctionCLs.h"
#include "managerCLs.h"

#include "RooBernstein.h"
#include "RooGaussian.h"
#include "RooExponential.h"
#include "RooAddPdf.h"
#include "RooFormulaVar.h"

#include "TApplication.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TPad.h"
#include "TLatex.h"

#include "TStyle.h"
#include "TGraphErrors.h"

#include "TLegend.h"
#include "TLine.h"

#include <algorithm>


#include "TMath.h"
#include "Math/ProbFuncMathCore.h"
#include "Math/QuantFuncMathCore.h"



using namespace std;

int nBkg = 10000;
int nSgn = 150;
 
TApplication Runner("gui",0,NULL);

int main()
{

  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);

  RooRealVar x("x","x",80,300);
  x.setBins( (300-80)/2 );

  // ================ Bkg

  RooRealVar e0_gen("e0_gen","e0_gen",-0.01);
  RooRealVar e0_fit("e0_fit","e0_fit",-0.02,-0.05,0.05);

  RooExponential expo_gen("expo_gen","expo_gen",x,e0_gen);
  RooExponential expo_fit("expo_fit","expo_fit",x,e0_fit);

  // ================ Sgn

  RooRealVar mean_gen("mean_gen","mean_gen",150);
  RooRealVar sigma_gen("sigma_gen","sigma_gen",5);
  mean_gen.setConstant(true);
  sigma_gen.setConstant(true);

  RooRealVar mean_fit("mean_fit","mean_fit",mean_gen.getValV());
  RooRealVar sigma_fit("sigma_fit","sigma_fit",sigma_gen.getValV());
  mean_fit.setConstant(true);
  sigma_fit.setConstant(true);

  RooGaussian signal_gen("signal_gen","signal_gen",x,mean_gen,sigma_gen);
  RooGaussian signal_fit("signal_gfit","signal_fit",x,mean_fit,sigma_fit);

  // ================ Normalizations

  // w/  signal
  RooRealVar n_bkg_fit_floating("n_bkg_fit_floating","n_bkg_fit_floating",nBkg,0.5*nBkg,1.5*nBkg);
  RooRealVar n_sgn_fit_floating("n_sgn_fit_floating","n_sgn_fit_floating",nSgn,-100*nSgn,100*nSgn);
  RooAddPdf model_fit_floating("model_fit_floating","model_fit_floating",RooArgList(expo_fit,signal_fit),RooArgList(n_bkg_fit_floating,n_sgn_fit_floating));
  n_sgn_fit_floating.setConstant(true);

  // w/o signal
  RooRealVar n_bkg_fit_fix("n_bkg_fit_fix","n_bkg_fit_fix",nBkg,0.5*nBkg,1.5*nBkg);
  RooRealVar n_sgn_fit_fix("n_sgn_fit_fix","n_sgn_fit_fix",0);
  RooAddPdf model_fit_fix("model_fit_fix","model_fit_fix",RooArgList(expo_fit,signal_fit),RooArgList(n_bkg_fit_fix,n_sgn_fit_fix));
  n_sgn_fit_fix.setConstant(true);



  // for MC simulation of data

  genFunctionCLs generator_b_data;
  generator_b_data.setObsName("x");
  generator_b_data.addPdf(&expo_gen);

  genFunctionCLs generator_s_data;
  generator_s_data.setObsName("x");
  generator_s_data.addPdf(&signal_gen);


  // ================ Calculation

  std::vector<double> mass;
  std::vector<double> err_mass;

  std::vector<double> observed;
  std::vector<double> err_observed;

  std::vector<double> expected_68;
  std::vector<double> expected_95;
  std::vector<double> err_expected_68;
  std::vector<double> err_expected_95;

  std::vector<double> prob_b_observed;
  std::vector<double> prob_b_expected;

  TH1D *_data_simul = (TH1D*) generator_b_data.generateMCpoisson( nBkg )->Clone();
  TH1D *_histo_sgn = (TH1D*) generator_s_data.generateMCpoisson( nSgn )->Clone();
  _data_simul->Add( (TH1D*) _histo_sgn->Clone() , 2);


  double hypo = 120;

  while( hypo < 185 )
     {
       mean_gen.setVal( hypo );
       mean_fit.setVal( hypo );
       mean_gen.setConstant(true);
       mean_fit.setConstant(true);

       // ================ Models

       genFunctionCLs generator_b;
       generator_b.setObsName("x");
       generator_b.addPdf(&expo_gen);
       
       genFunctionCLs generator_s;
       generator_s.setObsName("x");
       generator_s.addPdf(&signal_gen);
       
       
       fitFunctionCLs fitter_b;
       fitter_b.setObsName("x");
       fitter_b.addPdf(&model_fit_fix);
       
       fitFunctionCLs fitter_sb;
       fitter_sb.setObsName("x");
       fitter_sb.addPdf(&model_fit_floating);
       

       mass.push_back( hypo );
       err_mass.push_back( 0 );
       
       managerCLs manager;
       manager.addGeneratorBkg(&generator_b,nBkg);
       manager.addGeneratorSgn(&generator_s,nSgn);
       
       manager.addModel_BkgOnly(&fitter_b);
       manager.addModel_Bkg_Sgn(&fitter_sb);
       
       manager.addPOI( &n_sgn_fit_floating );
       manager.addData( _data_simul );

       manager.execute(  );

       double _observed = manager.observedLimit();
       double _errore = 0;
       double _expected = manager.expectedLimit(_errore);
       
       observed.push_back( _observed );
       err_observed.push_back( 0 );
       
       expected_68.push_back( _expected );
       expected_95.push_back( _expected );
       
       err_expected_68.push_back( _errore );
       err_expected_95.push_back( 2*_errore );
       
       prob_b_observed.push_back( manager.getProb__b() );
       prob_b_expected.push_back( 0.5 );

       printf("Mass Hipothesis: %.0f GeV\n",mean_fit.getValV());
       printf("Observed limit : %.2f\n",_observed);
       printf("Expected limit : %.2f +/- %.2f\n\n",_expected,_errore);
       


       std::vector< std::pair<double,double> > coverage = manager.getCoverage();
       std::vector<double> _muValue;
       std::vector<double> _coverageValue;
       for (unsigned int index(0); index < coverage.size(); index++)
	 {
	   _muValue.push_back( coverage.at(index).first );
	   _coverageValue.push_back( coverage.at(index).second );
	 }

       //       manager.saveStatistics();

       TCanvas *c2 = new TCanvas();
       TGraph *grCoverage = new TGraph(_muValue.size(), &_muValue[0],&_coverageValue[0]);
       grCoverage->Draw("APL");
       grCoverage->SetMarkerStyle(20);
       grCoverage->SetMarkerSize(0.6);

       grCoverage->GetXaxis()->SetTitle("mu");
       grCoverage->GetYaxis()->SetTitle( Form("Coverage [mass : %.0f]",hypo) );

       grCoverage->SetMaximum(1.01);
       grCoverage->SetMinimum(0.90);

       c2->Draw();
       c2->Update();
       c2->SaveAs( Form("plot_coverage_mass_%.0f.pdf",hypo) );

       hypo += 2.;
     }

  std::vector<double> range_y;
  for(int i = 0; i < observed.size(); i++)
    range_y.push_back( observed.at(i) );

  for(int i = 0; i < expected_95.size(); i++)
    {
      range_y.push_back( expected_95.at(i) + err_expected_95.at(i) );
      range_y.push_back( expected_95.at(i) - err_expected_95.at(i) );
    }

  sort(range_y.begin(),range_y.end());


  std::cout<<"### RESULTS"<<std::endl;
  for (int i = 0 ;i < mass.size(); i++)
    printf("%.0f %.4f %.4f %.4f :: %.4f [%.4f expected]\n", 
	   mass.at(i),
	   observed.at(i),
	   expected_68.at(i),
	   err_expected_68.at(i),
	   prob_b_observed.at(i),
	   prob_b_expected.at(i));
  std::cout<<std::endl;


  TGraphErrors *gr_expected_95 = new TGraphErrors(mass.size(),&mass[0],&expected_95[0],&err_mass[0],&err_expected_95[0]); 
  TGraphErrors *gr_expected_68 = new TGraphErrors(mass.size(),&mass[0],&expected_68[0],&err_mass[0],&err_expected_68[0]); 
  TGraphErrors *gr_observed = new TGraphErrors(mass.size(),&mass[0],&observed[0],&err_mass[0],&err_observed[0]);

  TGraphErrors *gr_expected = new TGraphErrors(mass.size(),&mass[0],&expected_68[0],&err_mass[0],&err_mass[0]); 

  TCanvas *c0 = new TCanvas();
  gr_expected_95->Draw("APL3");
  gr_expected_68->Draw("PLSAME3");
  gr_observed->Draw("PLSAME");
  gr_expected->Draw("PLSAME");

  gr_expected_95->GetXaxis()->SetTitle("Mass");
  gr_expected_95->GetYaxis()->SetTitle("CLs limit on mu");

  gr_expected_68->SetLineColor(1);
  gr_expected_95->SetLineColor(1);

  gr_expected->SetLineColor(1);
  gr_expected->SetMarkerStyle(20);
  gr_expected->SetMarkerSize(0.8);

  gr_observed->SetLineColor(2);

  gr_expected_95->SetFillColor(kYellow);
  gr_expected_68->SetFillColor(kGreen);

  gr_expected_95->SetMarkerStyle(20);
  gr_expected_68->SetMarkerStyle(20);
  gr_observed->SetMarkerStyle(20);

  gr_expected_95->SetMarkerSize(0.8);
  gr_expected_68->SetMarkerSize(0.8);
  gr_observed->SetMarkerSize(0.8);

  gr_observed->SetMarkerColor(2);

  gr_expected_95->SetMinimum( range_y.at(0) - 1);
  gr_expected_95->SetMaximum( range_y.at( range_y.size() - 1) + 1);

  TLegend *legenda = new TLegend(0.6,0.7,0.85,0.85);
  legenda->SetLineColor(0);
  legenda->SetFillColor(0);

  legenda->AddEntry(gr_observed,"Observed","lp");
  legenda->AddEntry(gr_expected,"Expected","lp");
  legenda->AddEntry(gr_expected_68,"Expected 68%","f");
  legenda->AddEntry(gr_expected_95,"Expected 95%","f");

  legenda->Draw("SAME");

  c0->Draw();
  c0->Update();
  c0->SaveAs("plot_limitCLs.pdf");
 
  std:vector<double> tmp(prob_b_observed.begin(),prob_b_observed.end());
  sort(tmp.begin(),tmp.end());

  TGraph *gr_prob_b_observed = new TGraph(mass.size(),&mass[0],&prob_b_observed[0]);
  TGraph *gr_prob_b_expected = new TGraph(mass.size(),&mass[0],&prob_b_expected[0]);

  TCanvas *c1 = new TCanvas();
  gr_prob_b_observed->Draw("APL");
  gr_prob_b_expected->Draw("LSAME");

  gr_prob_b_observed->SetMaximum(2);
  gr_prob_b_observed->SetMinimum(1e-5);
  if (tmp.at(0) / 10. < 1e-5)
    gr_prob_b_observed->SetMinimum(tmp.at(0) / 10.);
  gPad->SetGridx();  
  gPad->SetGridy();
  gPad->SetLogy();

  gr_prob_b_observed->GetXaxis()->SetTitle("Mass");
  gr_prob_b_observed->GetYaxis()->SetTitle("1 - CL_{b}");

  gr_prob_b_observed->SetMarkerStyle(20);
  gr_prob_b_observed->SetMarkerSize(0.6);

  gr_prob_b_expected->SetLineColor(4);
  gr_prob_b_expected->SetLineStyle(2);

  gr_prob_b_observed->GetXaxis()->SetRangeUser(mass.at(0),mass.at(mass.size()-1));

  int n = 2;
  double probSigma = 1;
  while ( probSigma > gr_prob_b_observed->GetMinimum() )
    {
      double sigma = 1.;
      probSigma = 1. - ( ROOT::Math::gaussian_cdf( n*sigma ,sigma,0 ) - ROOT::Math::gaussian_cdf( -n*sigma ,sigma,0 ) );
      n++;

      if (probSigma < gr_prob_b_observed->GetMinimum())
	continue;

      TLine *line = new TLine();
      line->SetLineWidth(2);
      line->SetLineColor(2);
      line->SetLineStyle(2);
      line->DrawLine(mass.at(0),probSigma,mass.at(mass.size()-1),probSigma);

      TLatex *latexSigma = new TLatex();
      latexSigma->DrawLatex(mass.at(mass.size()-1) + 0.5,probSigma,Form("#scale[0.8]{%d #sigma}",n-1));
    }


  TLegend *legenda_prob = new TLegend(0.6,0.25,0.85,0.32);
  legenda_prob->SetFillColor(0);

  legenda_prob->AddEntry(gr_prob_b_expected,"Expected for Bkg","l");
  legenda_prob->AddEntry(gr_prob_b_observed,"Observed","lp");

  legenda_prob->Draw("SAME");

  c1->Draw();
  c1->Update();
  c1->SaveAs("plot_probCLs.pdf");

  //  Runner.Run(true);
}

