/*
NAMETEMPLATE
*/
#ifndef NAMETEMPLATE_h
#define NAMETEMPLATE_h

#include <vector>
#include <cmath>

#if not defined(__CINT__) || defined(__MAKECINT__)
#include "TMVA/Tools.h"
#include "TMVA/Reader.h"
#endif

#include "Math/GenVector/VectorUtil.h"


#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <cmath>
#include <algorithm>
#include "functional"
#include <utility>

#include "ConfigParser.h"
#include "ConfigFileLine.h"


#include "TCanvas.h"
#include "TH1.h"
#include "THStack.h"



/** get the parameters from a congiguration file */
int parseConfigFile (const TString& config) ;



///==== GetTrendInfo ====
///==== Transform TH1 with "trace" information to TH1 ====
TH1F* GetTrendInfo(TH1F* hTrend, double min = -1.5, double max = 1.5);

///==== Pull Plot: drawing utility ====
void PullPlot(TCanvas* canvas, TH1* hDATA, TH1* hMC);
void PullPlot(TCanvas* canvas, TH1* hDATA, THStack* hsMC);  
TH1F* PullPlot(TH1F* hDATA, TH1F* hMC);

///==== Draw Stack ====
void DrawStack(THStack* hs, int error = 0, double syst = 0);
void DrawStackError(THStack* hs, double syst = 0);

///==== Add systrematic error ====
void AddError(THStack* hs, double syst = 0);



/** compute delta phi */
double deltaPhi (const double& phi1, const double& phi2);

/** compute delta eta */
double deltaEta (const double& eta1, const double& eta2);

 
#endif

