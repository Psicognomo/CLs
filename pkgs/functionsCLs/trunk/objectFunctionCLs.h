
#ifndef H_OBJECT_FUNCTION_CLS
#define H_OBJECT_FUNCTION_CLS

#include "messanger.h"

#include <map>

#include <TH1.h>
#include <TRandom3.h>

#include <RooRealVar.h>
#include <RooAbsPdf.h>

class objectFunctionCLs {
 public:
  objectFunctionCLs();
  objectFunctionCLs(objectFunctionCLs const&);
  ~objectFunctionCLs();

  bool setObsName(std::string);
  virtual bool addPdf(RooAbsPdf*);

  void setBinning(int);

  double getMin();
  double getMax();

  TH1D* getHisto() const; 

  std::map<std::string,RooRealVar*> getParameters() const;

 public:
  virtual bool isValid() const;
  void Print();

 protected:
  bool extractVariables(RooAbsReal*);
  void createHistogram(RooAbsPdf*);
  int getBinning();

 protected:
  std::string _observableName;
  RooRealVar *_observable;

  std::map<std::string,RooRealVar*> _parameters;
  TH1D *_h_model;

 protected:
  TRandom3 *_rdm;
};

#endif
