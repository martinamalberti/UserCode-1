  #g_hardTAGPtMin  20-40 a passi di 5 (5)
  #g_softTAGPtMin  0
  #g_TAGDetaMin    2-4 a passi 0.5 (5)
  #g_TAGDProdEtaMax  0
  #g_TAGMinv 250-700 a passi 100 (6)
  #g_ojetPtMin 15
  #g_ojetsMaxNum 1
  #g_hardELEPtMin 15-30 a passi di 5 (4)
  #g_softELEPtMin 0
  #g_ELEDPhiMin 0.3,0.2,0 (3)
  #g_ELEDPhiMax 1.5,2.5,0.25 (5)
  #g_ELEMinvMin 12
  #g_ELEMinvMax 80
  #g_METMin 0   ; 20 -> 50   20 (5 = 4+1)
  #g_prefix prova
  
  ################################################################### #LOOP PER OTTIMIZZARE TAGLI SUI TAG
  
  #g_hardTAGPtMin  20-40 a passi di 5 (5)
  $g_softTAGPtMin  = 0;
  #$g_TAGDetaMin =  2-4 a passi 0.5 (5)
  $g_TAGDProdEtaMax = 0;
  #$g_TAGMinv = 250-700 a passi 100 (6)
  
  $g_ojetPtMin = 0;
  #$g_ojetEtaFromMean = 2 -> 3     0.25   (5);
  $g_ojetsMaxNum =  0;
  
  $g_hardELEPtMin = 15;
  $g_softELEPtMin =  0;
  $g_ELEDPhiMin =  0;
  $g_ELEDPhiMax =  2.5;
  $g_ELEMinvMin =  12;
  $g_ELEMinvMax =  80;
  $g_METMin = 0 ;
  
  $i = 0;
  for ($g_hardTAGPtMin = 20; $g_hardTAGPtMin <= 40; $g_hardTAGPtMin = $g_hardTAGPtMin + 5) #5
  {
	  for ($g_TAGDetaMin = 2; $g_TAGDetaMin <= 4; $g_TAGDetaMin = $g_TAGDetaMin + 0.5) #5
	  {
		  for ($g_TAGMinv = 250; $g_TAGMinv <= 750; $g_TAGMinv = $g_TAGMinv + 100) #6
		  {
			  for ($g_ojetEtaFromMean = 2; $g_ojetEtaFromMean <= 3; $g_ojetEtaFromMean = $g_ojetEtaFromMean + 0.25) #5
			  {
			  $g_prefix = "set".$i;
                          $EXE = "/gwtera2/users/tancini/WWF/CMSSW_1_6_9/src/HiggsAnalysis/VBFHiggsToWWto2l2nu/doIt";
			  $COMMAND = "$EXE $g_hardTAGPtMin $g_softTAGPtMin $g_TAGDetaMin $g_TAGDProdEtaMax $g_TAGMinv $g_ojetPtMin $g_ojetEtaFromMean $g_ojetsMaxNum $g_hardELEPtMin $g_softELEPtMin $g_ELEDPhiMin $g_ELEDPhiMax $g_ELEMinvMin $g_ELEMinvMax $g_METMin $g_prefix";
			  
			  $LANCIAFILE = "submit_".$g_prefix.".csh";
			  open (LANCIA, ">".$LANCIAFILE) or die "Can't open data list file ".$LANCIAFILE." for input: $!" ;
			  print LANCIA $COMMAND."\n";
			  system ("qsub -V -q shortcms $LANCIAFILE") ; 
			  $i++;
			  }
		  }
	  }
  }