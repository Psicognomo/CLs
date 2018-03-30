
#include "messanger.h"

void Info(std::string message)
{
  printf("##### %15s ::: %30s\n","INFO",message.c_str());
}

void Warning(std::string message)
{
  printf("##### %15s ::: %30s\n","WARNING",message.c_str());
}

void Error(std::string message)
{
  printf("##### %15s ::: %30s\n","ERROR",message.c_str());
  exit(0);
}

