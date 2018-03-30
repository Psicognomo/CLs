
#include "bkgHypothesisCLs.h"

bkgHypothesisCLs::bkgHypothesisCLs()
{}

bkgHypothesisCLs::bkgHypothesisCLs(bkgHypothesisCLs const& other)
  : statisticsCLs(other)
{}

bkgHypothesisCLs::~bkgHypothesisCLs()
{}

double bkgHypothesisCLs::getProb(double value)
{
  double output =statisticsCLs::getProb( value );
  return 1. - output;
}
