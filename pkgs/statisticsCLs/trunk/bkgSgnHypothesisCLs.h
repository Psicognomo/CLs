
#ifndef H_BKG_SGN_HYPOTHESIS_CLS
#define H_BKG_SGN_HYPOTHESIS_CLS

#include "statisticsCLs.h"

class bkgSgnHypothesisCLs : public statisticsCLs {
 public:
  bkgSgnHypothesisCLs();
  bkgSgnHypothesisCLs(bkgSgnHypothesisCLs const&);
  ~bkgSgnHypothesisCLs();

  double getProb(double);
  double getProbSigma(double);

};

#endif
