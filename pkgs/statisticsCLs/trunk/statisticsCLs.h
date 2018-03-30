
#ifndef H_STATISTICS_CLS
#define H_STATISTICS_CLS

#include "messanger.h"

#include <iostream>
#include <vector>

#include <TH1.h>
#include <TRandom3.h>

class statisticsCLs {
 public:
  statisticsCLs();
  statisticsCLs(statisticsCLs const&);
  ~statisticsCLs();

  void fill(double);
  void clear();

  virtual double getProb(double);

  TH1D* getHisto();
  std::vector<double> getValues();

  double getMean() const;
  double getSigma() const;

 protected:
  void setMean(double);
  void setSigma(double);



 protected:
  std::vector<double> _values;

  TH1D* _distribution;
  TRandom3* _rdm;

 private:
  double _mean;
  double _sigma;
};

#endif
