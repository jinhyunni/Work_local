/****************************************************************************
* Calculate yield by centrality  *

* BBCS(BBC in Au going direction) acceptance: -3.9< eta <-3.1

* centrality class

	0~10%: class1	
	10~20%: class2
	20~40%: class3
	40~60%: class4
	60~80%: class5

	
* store pT spectrum of pion0 and centrality in TH2F
* store Ncoll and centrarlity in TH2F -> Integrate to get the 
  number of events in each centrality

* First, try with 10,000 events

****************************************************************************/

//Set files and TTrees
TFile* input0 = new TFile("outfile_pAu200GeV_set00_grp000_00000.root", "read");
TFile* refFile = new TFile("pAu200GeV_option3_pTcent_refhis_v1.root", "read");
	
TTree* ttree0 = (TTree*)input0 -> Get("T");
TH1F* refhis = (TH1F*)refFile -> Get("centRef_pTe5");


//centrality calculatoin fucntion
float centcal(int xbin, int nbin)
{
	float centrality = 0.;
	float numerator = 0.;
	float denominator = 0.;
		
	numerator = refhis -> Integral(xbin, nbin);
	denominator = refhis -> Integral(1, nbin);
	centrality = (numerator/denominator)*100;

	return centrality;	
}

//Analysis
void pion0Analysis()
{	
	//Set files and TTrees
	TFile* input0 = new TFile("outfile_pAu200GeV_set00_grp000_00000.root", "read");
	TFile* refFile = new TFile("pAu200GeV_option3_pTcent_refhis_v1.root", "read");
	
	TTree* ttree0 = (TTree*)input0 -> Get("T");
	TH1F* refhis = (TH1F*)refFile -> Get("centRef_pTe5");

	//Set variables to link branch, ttree0
	int np0 = 5000;
	int p_id0[np0];
	float p_eta0[np0];
	float p_pt0[np0];
	int i_nAbsProj0;
	int i_nAbsTarg0;

	ttree0 -> SetBranchAddress("np", &np0);
	ttree0 -> SetBranchAddress("p_id", &p_id0);
	ttree0 -> SetBranchAddress("p_eta", &p_eta0);
	ttree0 -> SetBranchAddress("p_pt", &p_pt0);
	ttree0 -> SetBranchAddress("i_nAbsProj", &i_nAbsProj0);
	ttree0 -> SetBranchAddress("i_nAbsTarg", &i_nAbsTarg0);

	//pion0 pT distribution vs centrality
	TH2D* pTpion_cent = new TH2D("pTpion0_cent", "", 11, 0, 110, 100, 0, 14 );
	
	//Ncoll vs centrality
	TH2D* ncoll_cent = new TH2D("ncoll_cent", "", 11, 0, 110, 50, 0, 50);
	
	float pTsum = 0.;
	float centrality = 0.;
	float ncoll = 0.;
	
	for(int i = 0; i < ttree0 -> GetEntries(); i++){
		ttree0 -> GetEntry(i);
		ncoll = i_nAbsProj0 + i_nAbsTarg0 - 1;
			for(int j = 0; j < np0; j++){
				if(-3.9 < p_eta0[j] and p_eta0[j] < -3.0 and p_id0[j] != 111){
					pTsum = pTsum + p_pt0[j];
				}				
			}//j
			
			//centrality calculation
			centrality = centcal(refhis -> FindBin(pTsum), 1e5);
			
			//store ncoll_cent
			ncoll_cent -> Fill(centrality, ncoll);

			for(int j = 0; j < np0; j++){
				if(p_id0[j] == 111){
					//store pTpion0_cent
					pTpion_cent -> Fill(centrality, p_pt0[j]);
				}
			}//j

			pTsum = 0;
	}//i, event loop
	
	//new TFile to store centrality-event histogram
	TFile* output = new TFile("pAu200GeV_option3_yield_by_centrality_v3.root", "recreate");

	pTpion_cent -> Write();
	ncoll_cent -> Write();

	output -> Close();

}



