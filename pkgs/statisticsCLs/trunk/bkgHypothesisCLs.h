
#ifndef H_BKG_HYPOTHESIS_CLS
#define H_BKG_HYPOTHESIS_CLS

#include "statisticsCLs.h"

class bkgHypothesisCLs : public statisticsCLs {
 public:
  bkgHypothesisCLs();
  bkgHypothesisCLs(bkgHypothesisCLs const&);
  ~bkgHypothesisCLs();

  double getProb(double);

};

#endif
