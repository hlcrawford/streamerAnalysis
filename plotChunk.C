void plotChunk(Int_t option=0) {
  TTree *data = new TTree();
  data->ReadFile("tr.out");
  
  if (option == 0) {
    TCanvas *m = new TCanvas();
    m->Divide(1,5,0,0);
    m->cd(1);
    data->Draw("wf:index>>wfplot", "", "l");
    m->cd(2);
    data->Draw("led:index>>ledplot", "", "l");
    m->cd(3);
    data->Draw("trapE:index>>trapEplot", "", "l");
    m->cd(4);
    data->Draw("pz:index>>pzEplot", "", "l");
    m->cd(5);
    data->Draw("pzBL:index>>pzBLEplot", "", "l");
  } else if (option == 1) {
    TCanvas *m = new TCanvas();
    data->Draw("trapE>>plot(40000,-10000,30000)", "", "");
  }

}
