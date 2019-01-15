{
  
  TString gretinaLib = "libGRETINA.so";
  const char *searchP = "./";
  const char* foundlib;
  foundlib = gSystem->Which(searchP, gretinaLib, EAccessMode::kFileExists);
  if (foundlib) {
    gSystem->Load("libGRETINA.so");
    cout << "Loaded: libGRETINA.so, ";
  } else {
    gretinaLib = "libGRETINA.dll";
    foundlib = gSystem->Which(searchP, gretinaLib, EAccessMode::kFileExists);
    if (foundlib) {
      gSystem->Load("libGRETINA.dll");
      cout << "Loaded: libGRETINA.dll, ";
    } else {
      gretinaLib = "libGRETINA.dylib";
      foundlib = gSystem->Which(searchP, gretinaLib, EAccessMode::kFileExists);
      if (foundlib) {
	gSystem->Load("libGRETINA.dylib");
	cout << "Loaded: libGRETINA.dylib, ";
      } else { 
	cout << "Could not load shared libraries!" << endl;
      }
    }
  }

  gStyle->SetPalette(1);
  gStyle->SetOptStat(0000);
  gStyle->SetCanvasBorderMode(0);
  gStyle->SetPadBorderMode(0);
  gStyle->SetCanvasColor(0);
  gStyle->SetFrameFillColor(0);
  gStyle->SetFrameLineColor(0);
  gStyle->SetFrameBorderMode(0);
  gStyle->SetTitleFillColor(0);
  gStyle->SetPadColor(0);
  gStyle->SetTitleColor(0);
  gStyle->SetStatColor(0);
  gStyle->SetNumberContours(99);
}
