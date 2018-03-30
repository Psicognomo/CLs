
#include "managerCLs.h"
#include "minimize.h"

#include <algorithm>
#include <utility>
#include <string>
#include <stdlib.h>

#include <TCanvas.h>
#include <TLegend.h>

#include <RooMsgService.h>
#include <RooArgSet.h>
#include <RooGaussian.h>
#include <RooPlot.h>
#include <RooDataHist.h>
#include <RooDataSet.h>

managerCLs::managerCLs()
  : _observedLimit(0),
    _expectedLimit(0),
    _expectedLimit_err(0),
    _observed_statistics(0),
    _observedProb_sb(1),
    _observedProb__b(1),
    _norm_Bkg(0),
    _norm_Sgn(0),
    _f_b_gen(nullptr),
    _f_s_gen(nullptr),
    _f__b_fit(nullptr),
    _f_sb_fit(nullptr),
    _namePOI(""),
    _POI(nullptr),
    _sim_bkgsgn(nullptr),
    _sim_bkg(nullptr),
    _sim_sgn(nullptr),
    _data(nullptr),
    CL__b(nullptr),
    CL_sb(nullptr),
    _doObserved(false),
    _doExpected(true),
    _rdm( new TRandom3(0) )
{
}

managerCLs::managerCLs(managerCLs const& other)
  : _observedLimit(other._observedLimit),
    _expectedLimit(other._expectedLimit),
    _expectedLimit_err(other._expectedLimit_err),
    _observed_statistics( other._observed_statistics ),
    _observedProb_sb( other._observedProb_sb ),
    _observedProb__b( other._observedProb__b ),
    _norm_Bkg(other._norm_Bkg),
    _norm_Sgn(other._norm_Sgn),
    _f_b_gen( (other._f_b_gen == nullptr) ? nullptr : new genFunctionCLs(*other._f_b_gen) ),
    _f_s_gen( (other._f_s_gen == nullptr) ? nullptr : new genFunctionCLs(*other._f_s_gen) ),
    _f__b_fit( (other._f__b_fit == nullptr) ? nullptr : new fitFunctionCLs(*other._f__b_fit) ),
    _f_sb_fit( (other._f_sb_fit == nullptr) ? nullptr : new fitFunctionCLs(*other._f_sb_fit) ),
    _sim_bkgsgn( (other._sim_bkgsgn == nullptr) ? nullptr : new TH1D(*other._sim_bkgsgn) ),
    _sim_bkg( (other._sim_bkg == nullptr) ? nullptr : new TH1D(*other._sim_bkg) ),
    _sim_sgn( (other._sim_sgn == nullptr) ? nullptr : new TH1D(*other._sim_sgn) ),
    _data( (other._data == nullptr) ? nullptr : new TH1D(*other._data) ),
    _namePOI(other._namePOI),
    _POI( other._POI ),
    CL__b( (other.CL__b == nullptr) ? nullptr : new bkgHypothesisCLs(*other.CL__b) ),
    CL_sb( (other.CL_sb == nullptr) ? nullptr : new bkgSgnHypothesisCLs(*other.CL_sb) ),
    _doObserved( other._doObserved ),
    _doExpected( other._doExpected ),
    _tested_CL__b( other._tested_CL__b ),
    _tested_CL_sb( other._tested_CL_sb ),
    _rdm( new TRandom3(0) )
{
}

managerCLs::~managerCLs()
{
  if (_sim_bkgsgn != nullptr)
    delete _sim_bkgsgn;
  if (_sim_bkg != nullptr)
    delete _sim_bkg;
  if (_sim_sgn != nullptr)
    delete _sim_sgn;
  if (CL__b != nullptr)
    delete CL__b;
  if (CL_sb != nullptr)
    delete CL_sb;
  delete _rdm;
}

double managerCLs::observedLimit()
{
  return _observedLimit;
}

double managerCLs::expectedLimit()
{
  return _expectedLimit;
}

double managerCLs::expectedLimit(double &err)
{
  err = _expectedLimit_err;
  return this->expectedLimit();
}


bool managerCLs::addPOI(RooRealVar *poi)
{
  if (_POI != nullptr)
    Error("POI already specified. It cannot be changed !");

  _namePOI = poi->GetName();
  _POI = poi;
  return true;
}

bool managerCLs::addGeneratorBkg(genFunctionCLs* container,double norm)
{
  if (norm < 0)
    Error("Normalization for Bkg MUST be positive !");
  if (! container->isValid() )
    Error("Model for Bkg generation not valid !");

  _f_b_gen = container;
  _norm_Bkg = norm;
  return true;
}

bool managerCLs::addGeneratorSgn(genFunctionCLs* container,double norm)
{
  if (! container->isValid())
    Error("Model for Sgn generation not valid !");
  if (norm < 0)
    Error("Normalization for Sgn MUST be positive !");

  _f_s_gen = container;
  _norm_Sgn = norm;
  return true;
}

bool managerCLs::addModel_BkgOnly(fitFunctionCLs* container)
{
  if (! container->isValid())
    Error("Model BkgOnly not valid !");

  _f__b_fit = container;
  return true;
}

bool managerCLs::addModel_Bkg_Sgn(fitFunctionCLs* container)
{
  if (! container->isValid())
    Error("Model Bkg_Sgn not valid !");
  
  _f_sb_fit = container;
  return true;
}

bool managerCLs::addData(TH1D *data)
{
  if (_data != nullptr)
    Error("Data histogram already specified ! Cannot be overridden !");

  _data = data;
  _doObserved = true;
  return true;
}

void managerCLs::saveStatistics()
{
  typedef std::map<std::string, bkgHypothesisCLs* >::iterator it_type;
  for(it_type iterator = _tested_CL__b.begin(); iterator != _tested_CL__b.end(); iterator++)
    this->drawStatistics(iterator->first,_tested_CL__b[ iterator->first ] , _tested_CL_sb[ iterator->first ]);
}

void managerCLs::drawStatistics(std::string keyMu,statisticsCLs *bkgOnly,statisticsCLs *bkg_sgn)
{
  std::vector<double> values_bkgOnly = bkgOnly->getValues();
  std::vector<double> values_bkg_sgn = bkg_sgn->getValues();
  sort( values_bkgOnly.begin(),values_bkgOnly.end() );
  sort( values_bkg_sgn.begin(),values_bkg_sgn.end() );

  double minimum = (values_bkgOnly.at(0) < values_bkg_sgn.at(0)) ? values_bkgOnly.at(0) : values_bkg_sgn.at(0) ;
  double maximum = (values_bkgOnly.at(values_bkgOnly.size()-1) > values_bkg_sgn.at(values_bkg_sgn.size()-1)) ? 
    values_bkgOnly.at(values_bkgOnly.size()-1) :
    values_bkg_sgn.at(values_bkg_sgn.size()-1) ;

  minimum -= 1;
  maximum += 1;

  TH1D histo_bkgOnly("histo_bkgOnly","histo_bkgOnly",100,minimum,maximum);
  TH1D histo_bkg_sgn("histo_bkg_sgn","histo_bkg_sgn",100,minimum,maximum);

  for (unsigned int index(0); index < values_bkgOnly.size(); index++)
    histo_bkgOnly.Fill( values_bkgOnly.at(index) );
  for (unsigned int index(0); index < values_bkg_sgn.size(); index++)
    histo_bkg_sgn.Fill( values_bkg_sgn.at(index) );

  RooRealVar _x("_x","-2*ln(Q)",minimum,maximum);
  RooDataHist dh_bkgOnly("dh_bkgOnly","dh_bkgOnly",_x,&histo_bkgOnly);
  RooDataHist dh_bkg_sgn("dh_bkg_sgn","dh_bkg_sgn",_x,&histo_bkg_sgn);

  RooRealVar mean_bkgOnly("mean_bkgOnly","mean_bkgOnly",bkgOnly->getMean());
  RooRealVar sigma_bkgOnly("sigma_bkgOnly","sigma_bkgOnly",bkgOnly->getSigma());
  RooGaussian gauss_bkgOnly("gauss_bkgOnly","gauss_bkgOnly",_x,mean_bkgOnly,sigma_bkgOnly);

  RooRealVar mean_bkg_sgn("mean_bkg_sgn","mean_bkg_sgn",bkg_sgn->getMean());
  RooRealVar sigma_bkg_sgn("sigma_bkg_sgn","sigma_bkg_sgn",bkg_sgn->getSigma());
  RooGaussian gauss_bkg_sgn("gauss_bkg_sgn","gauss_bkg_sgn",_x,mean_bkg_sgn,sigma_bkg_sgn);

  RooPlot *frame = _x.frame();
  dh_bkgOnly.plotOn(frame,RooFit::MarkerColor(2),RooFit::LineColor(2));
  dh_bkg_sgn.plotOn(frame,RooFit::MarkerColor(4),RooFit::LineColor(4));
  gauss_bkgOnly.plotOn(frame,RooFit::LineColor(2));
  gauss_bkg_sgn.plotOn(frame,RooFit::LineColor(4));

  TCanvas *c0 = new TCanvas();
  frame->Draw();
  c0->SaveAs( Form("plot_statistics_mu_%s.pdf",keyMu.c_str()) );
  c0->Close();

  delete frame;
  delete c0;
}

bool managerCLs::isValid(objectFunctionCLs* container)
{
  std::map<std::string,RooRealVar*> _parameters = container->getParameters();
  if ( _parameters.find( _namePOI.c_str() ) == _parameters.end() ) return false;
  return true;
}

void managerCLs::generateSimulation(double mu)
{
  if (_sim_bkgsgn != nullptr) delete _sim_bkgsgn;
  if (_sim_bkg != nullptr) delete _sim_bkg;
  if (_sim_sgn != nullptr) delete _sim_sgn;

  _sim_bkg = (TH1D*) _f_b_gen->generateMCpoisson(_norm_Bkg)->Clone();
  _sim_sgn = (TH1D*) _f_s_gen->generateMCpoisson(mu * _norm_Sgn)->Clone();
  _sim_bkgsgn = (TH1D*) _sim_bkg->Clone("_sim_bkg");
  _sim_bkgsgn->Add( _sim_sgn , 1.);
}

bool managerCLs::execute(int nExperiments)
{
  RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL);
  Info("Staring Execution of CLs Algorithm ...");

  if (! this->isValid(_f_sb_fit)) return false;

  if (CL__b == nullptr) delete CL__b;
  if (CL_sb == nullptr) delete CL_sb;

  CL__b = nullptr;
  CL_sb = nullptr;

  // ====== Calculate Observed
  if (_doObserved && _data != nullptr)
    _observedLimit = this->calculateObservedLimit( _data , nExperiments );

  // ====== Calculate Expected
  if (!_doExpected) return true;

  double expectedLimit = 0;

  std::vector<double> muUp;
  for (int index(0); index < 200; index++) 
    {
      double value = this->calculateExpectedLimit( nExperiments );
      muUp.push_back( value );
    }

  std:: sort(muUp.begin(),muUp.end());

  double tmp_mean_mu_up = ( muUp.at(0) + muUp.at( muUp.size()-1 ) )/2;
  RooRealVar x_mu_up("x_mu_up","x_mu_up",muUp.at(0), muUp.at( muUp.size()-1 ));
  RooRealVar mean_mu_up("mean_mu_up","mean_mu_up",tmp_mean_mu_up,muUp.at(0)  , muUp.at( muUp.size()-1 ));
  RooRealVar sigma_mu_up("sigma_mu_up","sigma_mu_up",0.5*tmp_mean_mu_up,0.01 , muUp.at( muUp.size()-1 ));
  RooGaussian gauss_mu_up("gauss_mu_up","gauss_mu_up",x_mu_up,mean_mu_up,sigma_mu_up);

  RooDataSet _data("_data","_data",RooArgSet(x_mu_up));
  for (int index(0); index < muUp.size(); index++)
    {
      x_mu_up.setVal( muUp.at(index) );
      _data.add( RooArgSet(x_mu_up) );
    }

  RooAbsReal *nll = gauss_mu_up.createNLL( _data );
  int status = minimize(nll);
  delete nll;

  _expectedLimit = mean_mu_up.getValV();
  _expectedLimit_err = sigma_mu_up.getValV();

  return true;
}

double managerCLs::evaluateStatistics(TH1D* sample, bool &status)
{
  int status__b = _f__b_fit->fit( sample );
  int status_sb = _f_sb_fit->fit( sample );

  double L__b = _f__b_fit->getNegLogLikelihood();
  double L_sb = _f_sb_fit->getNegLogLikelihood();

  status = (status__b == 0) && (status_sb == 0);
  return 2*( L_sb - L__b );
}

double managerCLs::calculateObservedLimit(TH1D *sample,int nExperiments)
{
  int nTry = 0;
  double CLs = 1;
  double pre_mu = -1;
  double mu = 0;

  double ceiling = 0;
  double floor   = 0;

  do {
    
    CL__b = new bkgHypothesisCLs();
    CL_sb = new bkgSgnHypothesisCLs();

    _POI->setVal(mu * _norm_Sgn);
    _POI->setConstant(true);

    bool success_observed = false;
    double value_observed__b = evaluateStatistics( sample , success_observed);
    if (! success_observed) continue;

    this->calculateDistributions( mu , nExperiments );
    CLs = this->evaluateCLs( value_observed__b );


    // Saving output variables
    if (sample == _data)
      {
	_observed_statistics = value_observed__b;

	_observedProb_sb = CL_sb->getProb( _observed_statistics );
	_observedProb__b = CL__b->getProb( _observed_statistics );
	
	if (fabs(CLs - _observedProb_sb/(1-_observedProb__b)) > 1e-3)
	  Error("Sbagliato il calcolo!!!!");
      }


    double tmpMu = mu;
    if (CLs > 0.05)
      {
	if (ceiling > mu)
	  mu = (ceiling + mu)/2;
	else
	  mu += 3;
	floor = tmpMu;
      }
    else
      {
	if (floor < mu)
	  mu = (floor + mu)/2;
	else
	  mu -= 3;
	ceiling = tmpMu;
	
      }
    pre_mu = tmpMu;


    delete CL__b;
    delete CL_sb;
    CL__b = nullptr;
    CL_sb = nullptr;

    nTry++;

  } while( fabs(CLs - 0.05) > 0.005 && nTry < 200);

  if (nTry == 25) 
    {
      Warning("Reached maximum number of attempts !");
      return 0;
    }

  return pre_mu;
}

double managerCLs::calculateExpectedLimit(int nExperiments)
{
  this->generateSimulation(0);
  TH1D *_simulationExpected = (TH1D*) _sim_bkg->Clone();
  double value = this->calculateObservedLimit( _simulationExpected , nExperiments ); 
  delete _simulationExpected;
  return value;
}

bool managerCLs::calculateDistributions(double mu, int nExperiments)
{

  if ( this->hasBeenTested(Form("%.4f",mu)) )
    {
      CL__b = new bkgHypothesisCLs( *_tested_CL__b[Form("%.4f",mu)] );
      CL_sb = new bkgSgnHypothesisCLs( *_tested_CL_sb[Form("%.4f",mu)] );
      return true;
    }

  _POI->setVal(mu * _norm_Sgn);
  _POI->setConstant(true);

  for (int index(0); index < nExperiments; index++)
    {
      this->generateSimulation(mu);

      bool valido_bkgOnly = false;
      double value_CL__b = evaluateStatistics( _sim_bkg    , valido_bkgOnly);
      
      bool valido_bkg_sgn = valido_bkgOnly;
      double value_CL_sb = value_CL__b;
      if (mu != 0) value_CL_sb = evaluateStatistics( _sim_bkgsgn , valido_bkg_sgn);

      if (!valido_bkgOnly || !valido_bkg_sgn)
	{
	  index--;
	  continue;
	}

      CL__b->fill( value_CL__b );
      CL_sb->fill( value_CL_sb );
    }

  _tested_CL__b[Form("%.4f",mu)] = new bkgHypothesisCLs( *CL__b ) ;
  _tested_CL_sb[Form("%.4f",mu)] = new bkgSgnHypothesisCLs( *CL_sb );

  return true;
}

double managerCLs::evaluateCLs(double value)
{
  double value_CL_sb = CL_sb->getProb( value );
  double value_CL__b = 1 - CL__b->getProb( value );

  double CL__s = value_CL_sb / value_CL__b;
  return CL__s;
}

bool managerCLs::hasBeenTested(std::string key)
{
  if ( _tested_CL_sb.find( key.c_str() ) != _tested_CL_sb.end() ) return true;
  return false;
}

double managerCLs::getProb_sb() const
{
  return _observedProb_sb;
}

double managerCLs::getProb__b() const
{
  return _observedProb__b;
}

double managerCLs::getStatisticsValue() const
{
  return _observed_statistics;
}

void managerCLs::doObserved(bool doIt)
{
  _doObserved = doIt;
}

void managerCLs::doExpected(bool doIt)
{
  _doExpected = doIt;
}

std::vector< std::pair<double,double> > managerCLs::getCoverage()
{
  std::vector< std::pair<double,double> > output;

  typedef std::map<std::string, bkgSgnHypothesisCLs* >::iterator it_type;
  for(it_type iterator = _tested_CL_sb.begin(); iterator != _tested_CL_sb.end(); iterator++)
    {
      double muValue = atof( iterator->first.c_str() );
      double coverage = evaluateCoverage( _tested_CL__b[iterator->first] , _tested_CL_sb[iterator->first] );
      output.push_back( std::make_pair(muValue , coverage) );
    }
  
  return output;
}

double managerCLs::evaluateCoverage(bkgHypothesisCLs *_bkgHypo,bkgSgnHypothesisCLs *_bkgSgnHypo)
{
  std::vector<double> valuesBkg = _bkgHypo->getValues();
  std::vector<double> valuesBkgSgn = _bkgSgnHypo->getValues();

  std::vector<double> valuesMerged(valuesBkg.begin(),valuesBkg.end());
  valuesMerged.insert(valuesMerged.end(),valuesBkgSgn.begin(),valuesBkgSgn.end()); 
  sort(valuesMerged.begin(),valuesMerged.end());

  double preCutCLs = valuesMerged.at(0);
  double cutCLs = ( valuesMerged.at(0) + valuesMerged.at(valuesMerged.size()-1) )/2;

  double valueCLs = 0;
  double prob_BS = 0;
  double prob__B = 0;

  double ceiling = 0;
  double floor  = 0;

  int nTry = 0;

  do
  {
    prob_BS = _bkgSgnHypo->getProb(cutCLs);
    prob__B = _bkgHypo->getProb(cutCLs);
    
    valueCLs = prob_BS / (1. - prob__B);
   
    double tmpCut = cutCLs;
    if (valueCLs > 0.05)
      {
        if (ceiling > cutCLs)
          cutCLs = (ceiling + cutCLs)/2;
        else
          cutCLs += 3;
        floor = tmpCut;
      }
    else
      {
        if (floor < cutCLs)
          cutCLs = (floor + cutCLs)/2;
        else
          cutCLs -= 3;
        ceiling = tmpCut;
      }
    preCutCLs = tmpCut;

    nTry++;

  } while ( fabs(valueCLs - 0.05) > 0.001 && nTry < 200);

  return 1. - prob_BS;
}
