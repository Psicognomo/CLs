
#include "objectFunctionCLs.h"

#include <TIterator.h>
#include <RooArgSet.h>

// ================ Public

objectFunctionCLs::objectFunctionCLs()
  : _observableName("UnKnown"),
    _observable(nullptr),
    _h_model(nullptr),
    _rdm( new TRandom3(0) )
{
}
objectFunctionCLs::objectFunctionCLs(objectFunctionCLs const& other)
  : _observableName(other._observableName),
    _parameters( other._parameters.begin(),other._parameters.end() ),
    _observable( (other._observable == nullptr) ? nullptr : new RooRealVar( *other._observable ) ),
    _h_model( (other._h_model == nullptr) ? nullptr : new TH1D( *other._h_model ) ),
    _rdm( new TRandom3(0) )
{
}
objectFunctionCLs::~objectFunctionCLs()
{
  delete _rdm;
  if (_h_model != nullptr)
    delete _h_model;
}

bool objectFunctionCLs::setObsName(std::string obsName)
{
  if ( _observableName != "UnKnown" ) 
      Error("Observable name already specified. It cannot be overridden !");
  _observableName = obsName;
  return true;
}

bool objectFunctionCLs::addPdf(RooAbsPdf *pdf)
{
  if ( _observableName == "UnKnown" )
    Error("Observable has not been specified !");
  if ( _observable != nullptr) 
    Error("Pdf already specified. It cannot be overridden !");
  if (! this->extractVariables(pdf)  ) return false;
  this->createHistogram(pdf);
  return true;
}

void objectFunctionCLs::setBinning(int binning)
{
  if ( binning < 1 ) 
    Error("Binning MUST be > 0 !");
  _observable->setBins(binning);
}

bool objectFunctionCLs::isValid() const
{
  if ( _observable == nullptr) return false;
  if ( _h_model == nullptr ) return false;
  return true;
}

void objectFunctionCLs::Print() 
{
  Info("Printing of function model ...");
  Info(Form("    -- Observable ::: %10s [binning : %d]",_observableName.c_str(),this->getBinning()));

  typedef std::map<std::string, RooRealVar* >::iterator it_type;
  for(it_type iterator = _parameters.begin(); iterator != _parameters.end(); iterator++)
    {
      Info(Form("    -- Variable \"%10s\" ::: %.4f [%.4f,%.4f]",iterator->first.c_str(),
		iterator->second->getValV(),
		iterator->second->getMin(),
		iterator->second->getMax()));
    }
}

TH1D* objectFunctionCLs::getHisto() const
{
  return (TH1D*) _h_model->Clone( Form("_histogram_%.0f_%.0f",1e6*_rdm->Rndm(),1e6*_rdm->Rndm()) );
}

double objectFunctionCLs::getMin()
{
  if (_observable == nullptr) return 0;
  return _observable->getMin();
}

double objectFunctionCLs::getMax()
{
  if (_observable == nullptr) return 0;
  return _observable->getMax();
}

// ================ Protected

bool objectFunctionCLs::extractVariables(RooAbsReal *pdf)
{
  RooArgSet *variables = pdf->getVariables();
  TIterator *iter = variables->createIterator();
  RooRealVar *var = (RooRealVar*) iter->Next();

  while (var)
    {
      std::string nominalName = var->GetName();
      if (nominalName == _observableName) _observable = var;
      else _parameters[nominalName] = var;
      var = (RooRealVar*) iter->Next();
    }

  delete variables;
  delete iter;

  if ( _observable == nullptr )
    Error("Observable was not present in the Pdf !");

  return true;
}

void objectFunctionCLs::createHistogram(RooAbsPdf *pdf)
{
  if ( _h_model != nullptr ) delete _h_model;
  _h_model = new TH1D(Form("_h_model_%.0f_%.0f",1e6*_rdm->Rndm(),1e6*_rdm->Rndm()),
		      Form("_h_model_%.0f_%.0f",1e6*_rdm->Rndm(),1e6*_rdm->Rndm()),
		      this->getBinning(),_observable->getMin(),_observable->getMax());
  _h_model->Sumw2();

  double deltaX = (_observable->getMax() - _observable->getMin()) / this->getBinning();
  RooRealVar *x = _observable;

  for (int i(0); i < this->getBinning(); i++)
    {
      double minimum = _observable->getMin() + i*deltaX;
      x->setRange("Range",minimum,minimum + deltaX);
      RooAbsReal* integral_pdf = pdf->createIntegral(*x,*x,"Range");

      x->setVal(minimum + deltaX / 2);
      _h_model->SetBinContent(i+1,integral_pdf->getVal());
      _h_model->SetBinError(i+1,0);

      delete integral_pdf;
    }

  _h_model->GetXaxis()->SetTitle( x->GetName() );

}

int objectFunctionCLs::getBinning()
{
  return _observable->numBins();
}

std::map<std::string,RooRealVar*>  objectFunctionCLs::getParameters() const
{
  return _parameters;
}
