
#include "minimize.h"

#include <iostream>
#include <string>

#include <RooMinimizer.h>
#include <RooNLLVar.h>
#include <RooFitResult.h>

int minimize(RooAbsReal* fcn, bool callHesse, bool callMinos)
{

  int printLevel = ROOT::Math::MinimizerOptions::DefaultPrintLevel();
  RooFit::MsgLevel msglevel = RooMsgService::instance().globalKillBelow();
  if (printLevel < 0) RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL);


  int strat = ROOT::Math::MinimizerOptions::DefaultStrategy(); 
  int save_strat = strat;
  RooMinimizer minim(*fcn);
  minim.setStrategy(strat);
  minim.setPrintLevel(printLevel);

  int status = minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str());

//up the strategy
  if (status != 0 && status != 1 && strat < 2)
  {
    strat++;
    std::cout << "Fit failed with status " << status << ". Retrying with strategy " << strat << std::endl;
    minim.setStrategy(strat);
    status = minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str());
  }

  if (status != 0 && status != 1 && strat < 2)
  {
    strat++;
    std::cout << "Fit failed with status " << status << ". Retrying with strategy " << strat << std::endl;
    minim.setStrategy(strat);
    status = minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str());
  }


/* //switch minuit version and try again */
/*   if (status != 0 && status != 1) */
/*   { */

/*     std::string minType = ROOT::Math::MinimizerOptions::DefaultMinimizerType(); */
/*     std::string newMinType; */
/*     if (minType == "Minuit2") newMinType = "Minuit"; */
/*     else newMinType = "Minuit2"; */
  
/*     std::cout << "Switching minuit type from " << minType << " to " << newMinType << std::endl; */
  
/*     ROOT::Math::MinimizerOptions::SetDefaultMinimizer(newMinType.c_str()); */
/*     strat = ROOT::Math::MinimizerOptions::DefaultStrategy(); */
/*     minim.setStrategy(strat); */

/*     status = minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str()); */


/*     if (status != 0 && status != 1 && strat < 2) */
/*     { */
/*       strat++; */
/*       std::cout << "Fit failed with status " << status << ". Retrying with strategy " << strat << std::endl; */
/*       minim.setStrategy(strat); */
/*       status = minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str()); */
/*     } */

/*     if (status != 0 && status != 1 && strat < 2) */
/*     { */
/*       strat++; */
/*       std::cout << "Fit failed with status " << status << ". Retrying with strategy " << strat << std::endl; */
/*       minim.setStrategy(strat); */
/*       status = minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str()); */
/*     } */

/*     ROOT::Math::MinimizerOptions::SetDefaultMinimizer(minType.c_str()); */


/*   } */


  if (status != 0 && status != 1)
  {
    std::cout << "Fit failed with status " << status << std::endl;
  }

  if (printLevel < 0) RooMsgService::instance().setGlobalKillBelow(msglevel);
  ROOT::Math::MinimizerOptions::SetDefaultStrategy(save_strat);

  std::cout << "CHECK: Fit " << status << ". strategy " << strat << std::endl;

  if (callHesse) {
    std::cout << "Calling Hesse ..."  << std::endl;
    int hessStatus = minim.hesse();
    std::cout << "Hesse Status : " << hessStatus << std::endl;

    if(callMinos){
      std::cout<< "Calling Minos ..." <<std::endl;
      int minosStatus = minim.minos();
      std::cout<< "Minos Status : " << minosStatus <<std::endl;
    }
  }

  return status;
}


