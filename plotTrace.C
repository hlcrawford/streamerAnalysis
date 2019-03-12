void plotEvent(TTree *tree, Int_t i, Float_t eLow, Float_t eHigh){

  g3OUT *g3 = new g3OUT();
  tree->SetBranchAddress("g3", &g3);

  TGraph *gr;
  Double_t x[10000] = {0}; Double_t y[10000] = {0};

  Int_t getOut = 0;
  Int_t num = 0;
  Int_t wf;

  while (getOut == 0) {
    
    tree->GetEntry(i);
    // if (g3->xtals.[0].chn[0].eRaw > eLow && g3->xtals[0].chn[0].eRaw < eHigh) {
    Float_t Energy = g3->xtals[0].chn[0].eRaw;
    if (Energy > eLow && Energy < eHigh) {
      wf = g3->xtals[0].chn[0].wf.raw.size()-2;
      for (Int_t j=0; j<g3->xtals[0].chn[0].wf.raw.size()-2; j++) {
	x[num] = num;
	y[num] = g3->xtals[0].chn[0].wf.raw[j];
	num++;
      }
    }
    if (num>0) { 
      gr = new TGraph(num, x, y); 
      gr->SetLineColor(kRed+2);
      gr->Draw("AL");
      getOut = 1;
    } else { 
      i++;
    }
  }


  printf("Event # displayed = %d\n", i);


}
