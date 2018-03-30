
#include "bkgSgnHypothesisCLs.h"

bkgSgnHypothesisCLs::bkgSgnHypothesisCLs()
{}

bkgSgnHypothesisCLs::bkgSgnHypothesisCLs(bkgSgnHypothesisCLs const& other)
  : statisticsCLs(other)
{}

bkgSgnHypothesisCLs::~bkgSgnHypothesisCLs()
{}

double bkgSgnHypothesisCLs::getProb(double value)
{
  double output = statisticsCLs::getProb( value );
  return output;
}
