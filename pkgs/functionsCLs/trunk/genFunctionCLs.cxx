
#include "genFunctionCLs.h"

genFunctionCLs::genFunctionCLs() 
  : _simulation(nullptr)
{
}

genFunctionCLs::genFunctionCLs(genFunctionCLs const& other)
  : objectFunctionCLs(other),
    _simulation( (other._simulation == nullptr) ? nullptr : new TH1D(*other._simulation) )
{
}

genFunctionCLs::~genFunctionCLs()
{
  if (_simulation != nullptr)
    delete _simulation;
}

TH1D* genFunctionCLs::generateMCpoisson(int nEvents) 
{
  if (_simulation != nullptr) delete _simulation;
  if (_observable == nullptr)
    Error("MC (poisson) generation requested BUT environment not set !");

  _simulation = new TH1D(Form("simulationPoisson_%.0f_%.0f",1e6*_rdm->Rndm(),1e6*_rdm->Rndm()),
			 Form("simulationPoisson_%.0f_%.0f",1e6*_rdm->Rndm(),1e6*_rdm->Rndm()),
			 this->getBinning(),_observable->getMin(),_observable->getMax());
  _simulation->Sumw2();

  RooRealVar *x = _observable;

  for (int i(1); i <= _h_model->GetNbinsX(); i++) 
    {
      double value = _rdm->Poisson(nEvents * _h_model->GetBinContent(i));
      _simulation->SetBinContent(i,value);
      _simulation->SetBinError(i,sqrt( _simulation->GetBinContent(i) )); 
    }

  _simulation->GetXaxis()->SetTitle( x->GetName() );

  return _simulation;
}

TH1D* genFunctionCLs::generateMCgauss(int nEvents)
{
  if (_simulation != nullptr) delete _simulation;
  if (_observable == nullptr)
    Error("MC (gauss) generation requested BUT environment not set !");

  _simulation = new TH1D(Form("simulationGauss_%.0f_%.0f",1e6*_rdm->Rndm(),1e6*_rdm->Rndm()),
                         Form("simulationGauss_%.0f_%.0f",1e6*_rdm->Rndm(),1e6*_rdm->Rndm()),
			 this->getBinning(),_observable->getMin(),_observable->getMax());
  _simulation->Sumw2();

  RooRealVar *x = _observable;

  for (int i(1); i <= _h_model->GetNbinsX(); i++)
    {
      double value = _rdm->Gaus(nEvents * _h_model->GetBinContent(i) , sqrt( nEvents * _h_model->GetBinContent(i) ));
      _simulation->SetBinContent(i,value);
      _simulation->SetBinError(i,sqrt( _simulation->GetBinContent(i) ));
    }

  _simulation->GetXaxis()->SetTitle( x->GetName() );

  return _simulation;
}

TH1D* genFunctionCLs::getSimulation() const
{
  return _simulation;
}
