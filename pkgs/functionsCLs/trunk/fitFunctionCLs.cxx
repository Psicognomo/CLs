
#include "fitFunctionCLs.h"

#include "RooDataHist.h"
#include "RooMinimizer.h"

#include "TCanvas.h"
#include "RooPlot.h"

#include "minimize.h"

fitFunctionCLs::fitFunctionCLs()
  : _pdf(nullptr),
    _negLogLike(0)
{
}

fitFunctionCLs::fitFunctionCLs(fitFunctionCLs const& other)
  : objectFunctionCLs(other),
    _negLogLike(0),
    _pdf( (other._pdf == nullptr) ? nullptr : (RooAbsPdf*) other._pdf->Clone() ),
    _initial( other._initial )
{
}

fitFunctionCLs::~fitFunctionCLs()
{
  _pdf = nullptr;
}

bool fitFunctionCLs::addPdf(RooAbsPdf* pdf)
{
  if (! objectFunctionCLs::addPdf(pdf) ) return false;
  _pdf = pdf;

  typedef std::map<std::string, RooRealVar* >::iterator it_type;
  for(it_type iterator = _parameters.begin(); iterator != _parameters.end(); iterator++) 
    {
      _initial[iterator->first] = iterator->second->getValV();
    }

  return true;
}

int fitFunctionCLs::fit(TH1D* sim)
{
  this->init();
  RooDataHist H_sim("H_sim","H_sim",*_observable,sim);

  RooAbsReal *nll = _pdf->createNLL( H_sim,RooFit::Extended(true) );
  int output = minimize(nll);
  _negLogLike = nll->getVal();

  this->createHistogram(_pdf);
  delete nll;
  return output;
}

int fitFunctionCLs::fit(genFunctionCLs* generator,int nEvents)
{
  TH1D *simulation = generator->generateMCpoisson(nEvents);
  return this->fit(simulation);
}

double fitFunctionCLs::getNegLogLikelihood()
{
  return _negLogLike;
}

bool fitFunctionCLs::isValid() const
{
  if ( _pdf == nullptr ) return false;
  return objectFunctionCLs::isValid();
}

void fitFunctionCLs::init()
{
  typedef std::map<std::string, RooRealVar* >::iterator it_type;
  for(it_type iterator = _parameters.begin(); iterator != _parameters.end(); iterator++)
    {
      double value = _initial[iterator->first];
      _parameters[iterator->first]->setVal(value);
    }
}
