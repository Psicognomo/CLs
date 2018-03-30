
#include "statisticsCLs.h"
#include "minimize.h"

#include <algorithm>

#include <RooRealVar.h>
#include <RooGaussian.h>
#include <RooDataHist.h>
#include <RooDataSet.h>
#include <RooArgSet.h>

#include "TMath.h"
#include "Math/ProbFuncMathCore.h"
#include "Math/QuantFuncMathCore.h"

statisticsCLs::statisticsCLs()
  : _distribution(nullptr),
    _rdm( new TRandom3(0)),
    _mean(0),
    _sigma(-1)
{
}

statisticsCLs::statisticsCLs(statisticsCLs const& other)
  : _distribution( (other._distribution == nullptr) ? nullptr : new TH1D(*other._distribution) ),
    _values(other._values.begin(),other._values.end()),
    _rdm( new TRandom3(0) ),
    _mean( other._mean ),
    _sigma( other._sigma )
{
}

statisticsCLs::~statisticsCLs()
{
  if (_distribution != nullptr)
    delete _distribution;
  delete _rdm;
}

void statisticsCLs::fill(double value)
{
  _values.push_back(value);
}

void statisticsCLs::clear()
{
  _values.clear();
  delete _distribution;
  _distribution = nullptr;
}

TH1D* statisticsCLs::getHisto()
{
  if (_distribution != nullptr) delete _distribution;
  if (_values.size() == 0) return nullptr;

  std::sort( _values.begin(), _values.end());
  _distribution = new TH1D(Form("_distribution_%.0f_%.0f",1e6*_rdm->Rndm(),1e6*_rdm->Rndm()),
			   Form("_distribution_%.0f_%.0f",1e6*_rdm->Rndm(),1e6*_rdm->Rndm()),
			   50, _values.at(0), _values.at( _values.size()-1 ));

  for (unsigned int index(0);index < _values.size();index++)
      _distribution->Fill( _values.at(index) );

  return _distribution;
}

std::vector<double> statisticsCLs::getValues()
{
  std::sort( _values.begin(), _values.end());
  return _values;
}

double statisticsCLs::getProb(double value)
{
  if (this->getSigma() < 0)
    {
      std::sort( _values.begin(),_values.end() );
      
      double _minimum = _values.at(0) - 2;
      double _maximum = _values.at( _values.size() - 1 ) + 2;
      
      RooRealVar _x("_x","_x", _minimum , _maximum);
      
      double half = (_maximum - _minimum)/2 ;
      RooRealVar _mean("_mean","_mean", (_maximum + _minimum)/2 , _minimum , _maximum);
      RooRealVar _sigma("_sigma","_sigma", fabs(_maximum - _minimum)/6 ,0.01 , fabs(_maximum - _minimum)/3 );
      RooGaussian _gauss("_gauss","_gauss",_x,_mean,_sigma);
      
      RooDataSet _data("_data","_data",RooArgSet(_x));
      for (unsigned int index(0); index < _values.size(); index++)
	{
	  _x.setVal( _values.at(index) );
	  _data.add( RooArgSet(_x) );
	}
      
      RooAbsReal *nll = _gauss.createNLL( _data );
      int status = minimize(nll);
      
      this->setMean( _mean.getValV() );
      this->setSigma( _sigma.getValV() );
      
      delete nll;
    }
  
  double output = ROOT::Math::gaussian_cdf(value,this->getSigma(),this->getMean());
  return 1. - output;
}

void statisticsCLs::setMean(double value)
{
  _mean = value;
}

void statisticsCLs::setSigma(double value)
{
  _sigma = value;
}

double statisticsCLs::getMean() const
{
  return _mean;
}

double statisticsCLs::getSigma() const
{
  return _sigma;
}
