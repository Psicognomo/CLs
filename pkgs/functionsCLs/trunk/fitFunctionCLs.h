
#ifndef H_FIT_FUNCTION_CLS
#define H_FIT_FUNCTION_CLS

#include <map>

#include "objectFunctionCLs.h"
#include "genFunctionCLs.h"

class fitFunctionCLs : public objectFunctionCLs {
 public:
  fitFunctionCLs();
  fitFunctionCLs(fitFunctionCLs const&);
  ~fitFunctionCLs();

  bool addPdf(RooAbsPdf*);

  int fit(TH1D*);
  int fit(genFunctionCLs*,int);

  double getNegLogLikelihood();

 public:
  bool isValid() const;

 protected:
  void init();

 private:
  RooAbsPdf *_pdf;
  double _negLogLike;

 private:
  std::map<std::string,double> _initial;
};

#endif
