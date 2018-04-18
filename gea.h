#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include<ctime>
#include<math.h>

#ifndef EPISTASIS_H
#define EPISTASIS_H

#define Maxmodel 10 //determine the maximum number of a randomly generated model, can be interpreted as the initial condition, may need to be enlarged for tracing
#define Preserved_Model 1
#define Top_Ratio 0.4
#define ITER_NO 500 //30 number of iterations before reaching an output
#define No_PERMU 1000000 //(1 million)
#define Max_File 25
#define Max_Number_Header_Parameters 100 // limitations of the header 
#define FILENAME_LENGTH 1024
#define LogisticRange 20000
#define SHOW_SENSITIVITY 0

#define Trace_Stable_Iter 2000 //(for tracing)
#define GABA_Stable_Iter 100000 //(for full gaba: 10000 iter for 60 subjects stable about 30 minutes, but for 5000 subjects it takes much much longer)


//a base pair
struct AlleleUnit{
    //char base1;
    //char base2;
    double value;
    AlleleUnit();//constructor
}; // can be used for ATGC and 01

//A unit of one model, a node of a linked list 
struct ModelUnit{
    int file_index;
    int snploc;//snp location
    char oper; // +, *
    AlleleUnit al1;
    //AlleleUnit al2;
    ModelUnit *link;
	
    ModelUnit();
};

//CaList is a simplified ModelUnit just for calculation purpose
struct CaList{
    double value;
    CaList *link;
	
    CaList();
};

//Label list: a link list for labels in the data file
struct LabelList{
    int label;
    LabelList *link;
	
    LabelList();
};
//A model, its data structure is linked list, an individual in my population
struct Model{
   double fitness;//deducted with penalty
   double accuracy;//
   //double tp;
   //double tn;
   //double fp;
   //double fn;
   int length;
   ModelUnit *rule;
   
   void LoadRule(char[]);//input a model from a file
   void ToScreen();//output a model to screen
   void ToScreen(int, int);//output a model to screen
   void ToFile(char[]);//output a model to file
   void ToFile(char[], int, int);//output a model to file
   void SetLength();
   Model();
   void Release();
};

class SAMPLE_FILE
{
  public:
   int no_record;
   int no_snp;
   AlleleUnit **record;
   int *label;
   double *time;
   
   SAMPLE_FILE();
   SAMPLE_FILE(int,int);
   void Load();
};//a file, it can be case or control

class PARA_FILE
{
  public:
   int no_file;
   char filename_prefix[Max_File][FILENAME_LENGTH];
   int no_snps[Max_File];
   int no_subjects;
   int no_cases;
   int Np; //number of models left for each iteration
   float penalty_per_snp;
   
   PARA_FILE();

   void Load(char *);
};
class LAYER_FILE_DATA //an individual genotype files within a layer
{
  public:

   int no_snps;
   
   char matrix_filename[FILENAME_LENGTH];
   char submapfile[FILENAME_LENGTH];
   //AlleleUnit **record;
   SAMPLE_FILE Case_Control;
   //int *label;
   //double *time;

   CaList *taglochead;      //contains the SNPs in the submatrix
   LAYER_FILE_DATA();

   void LoadMatrix(int, int);
   void LoadSubMatrix(Model, int,  char *, char *, char *, int);
   void SubMap(int);
};

class ClassLabel
{
	public:
		int class_symbol;
		int class_count;
		double class_mean_score;
		ClassLabel *next_label;
		
	ClassLabel();
};

class LAYER_DATA //a layer of the pyramid, ontaining multiple genotype files
{
  public:
   int no_file;
   int no_subjects;
   int no_case;
   int no_control;
   int run_anova;
   int run_cox;
   ClassLabel *classlabel_link;
   
   float penalty_per_snp;

   int Np;
   LAYER_FILE_DATA individual_file[Max_File];
   char midfile[FILENAME_LENGTH];
   char topmodelfile[FILENAME_LENGTH];
   AlleleUnit *candidate[Max_File][3];

   Model *population;
   CaList *taglochead;
   ModelUnit *firstmodel;
   
       
   LAYER_DATA();

   void GetPara(int, int, int, float);
   void SubMap();
   void GABA();
   void GABA(int);
   void Calculate_Number_Case_Controls();
   void ClassLabel_Counts();
   void Fitness_U_statistics();
   void Fitness_ANOVA();
   void Fitness_Cox_likelihood();
   double Scoring(int, int);
   int Diagnostics(int, int, double);
   ModelUnit * Random_Rules();
   ModelUnit * Mutate(ModelUnit*, double);
   ModelUnit * Obtain_Allele(int,int);
   ModelUnit * Obtain_Coef(); //coef bounding problem, but can be mitigated using the Rules_condensing function 
   ModelUnit * Rules_Condensing(ModelUnit *);
   int Multifile_Random();
   void TranslateTopModel();
   void Permu(Model);
};



ModelUnit *Crossover(Model, Model);

void BubbleSort(Model[], int);
float FindCentroid(ModelUnit *);
void Abort(char *);

#endif
