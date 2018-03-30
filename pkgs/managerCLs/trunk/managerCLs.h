
#ifndef H_MANAGER_CLS
#define H_MANAGER_CLS

#include <string>

#include <TRandom3.h>

#include "genFunctionCLs.h"
#include "fitFunctionCLs.h"
#include "bkgHypothesisCLs.h"
#include "bkgSgnHypothesisCLs.h"

class managerCLs {
 public:
  managerCLs();
  managerCLs(managerCLs const&);
  ~managerCLs();

  double observedLimit();
  double expectedLimit();
  double expectedLimit(double&);

  bool addPOI(RooRealVar*);

  bool addGeneratorBkg(genFunctionCLs*,double);
  bool addGeneratorSgn(genFunctionCLs*,double);

  bool addModel_BkgOnly(fitFunctionCLs*);
  bool addModel_Bkg_Sgn(fitFunctionCLs*);

  bool addData(TH1D*);

  bool execute(int nExperiments = 500);

  double getStatisticsValue() const;
  double getProb_sb() const;
  double getProb__b() const;
  double getProbSigma_sb() const;
  double getProbSigma__b() const;

  std::vector< std::pair<double,double> > getCoverage();

  void doObserved(bool doIt = true);
  void doExpected(bool doIt = true);

  void saveStatistics();

 protected:
  void generateSimulation(double);
  void drawStatistics(std::string,statisticsCLs*,statisticsCLs*);

 private:
  bool isValid(objectFunctionCLs*);

  bool calculateDistributions(double,int);
  double calculateObservedLimit(TH1D*,int);
  double calculateExpectedLimit(int);

  double evaluateStatistics(TH1D*,bool&);
  double evaluateCLs(double);

  bool hasBeenTested(std::string);

  double evaluateCoverage(bkgHypothesisCLs*,bkgSgnHypothesisCLs*);

  // ================ //
  // == Attributes == //
  // ================ //

 private:
  double _observedLimit;
  double _expectedLimit;
  double _expectedLimit_err;

  double _observed_statistics;

  double _observedProb_sb;
  double _observedProb__b;

 private:
  double _norm_Bkg;
  double _norm_Sgn;

 private:
  genFunctionCLs* _f_b_gen;
  genFunctionCLs* _f_s_gen;

  fitFunctionCLs* _f__b_fit;
  fitFunctionCLs* _f_sb_fit;

 private:
  std::string _namePOI;
  RooRealVar* _POI;

 private:
  TH1D* _sim_bkgsgn;
  TH1D* _sim_bkg;
  TH1D* _sim_sgn;

 private:
  TH1D* _data;

 private:
  bkgHypothesisCLs* CL__b;
  bkgSgnHypothesisCLs* CL_sb;

 private:
  bool _doObserved;
  bool _doExpected;

 private:
  TRandom3 *_rdm;

 private:
  std::map<std::string,bkgSgnHypothesisCLs*> _tested_CL_sb;
  std::map<std::string,bkgHypothesisCLs*> _tested_CL__b;
};

#endif
