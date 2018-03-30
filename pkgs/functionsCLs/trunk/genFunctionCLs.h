
#ifndef H_GEN_FUNCTION_CLS
#define H_GEN_FUNCTION_CLS

#include "objectFunctionCLs.h"

class genFunctionCLs : public objectFunctionCLs {
 public:
  genFunctionCLs();
  genFunctionCLs(genFunctionCLs const&);
  ~genFunctionCLs();

  TH1D* generateMCgauss(int);
  TH1D* generateMCpoisson(int);

  TH1D* getSimulation() const;

 private:
  TH1D *_simulation;
};

#endif
