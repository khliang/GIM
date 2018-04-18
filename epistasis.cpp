#include"gea.h"

PARA_FILE::PARA_FILE()
{
    no_subjects =0;
    no_cases = 0;
    no_file = 0;
    penalty_per_snp=0.0;
};

void PARA_FILE::Load(char *para_filename)
{

    FILE *fp;
    int i;
    
    fp=fopen(para_filename, "r");
    if (fp==NULL) Abort ("Cannot open parameter file.");
    
    fscanf(fp, "%d",&no_file);

    if (no_file < 1 || no_file > Max_File) 
    {
        printf("Number of files to be loaded = %d\n",no_file);
        Abort (" 0< Number of files < Max_File (default = 25) ");
    }
    
    for (i=0; i< no_file; i++)
    {
            fscanf(fp, "%s %d",filename_prefix[i],  &no_snps[i]);
            if (no_snps[i] < 1) 
                {
                        printf("Number of parameters at file %d = %d\n",i, no_snps);
                        Abort ("Number of parameters must be equal or larger than 1");
                 }; 
    };
    fscanf(fp, "%d %d %f",&no_subjects, &Np, &penalty_per_snp);

    
    fclose (fp);
    

   
    if (no_subjects < 1) 
    {
        printf("Number of Subjects = %d\n",no_subjects);
        Abort ("Number of Subjects must be equal or larger than 1");
    }
    if (no_subjects < no_cases) 
    {
        printf("Number of case = %d\n",no_cases);
        Abort ("Number of subjects must be equal or larger than the number of case");
    }

    if (Np < 1) 
    {
        printf("Number of populations (epistasis results) = %d\n",Np);
        Abort ("Number of populations (epistasis results) must be equal or larger than 1");
    }
    
    //printf("Penalty=%2.5f\n", penalty_per_snp); //parsimony parameters?

};

LAYER_DATA::LAYER_DATA()
{
    no_subjects =0;
    no_case =0;
    no_control =0;
    run_anova = 0;
    
    classlabel_link = NULL;

    firstmodel = NULL;

};

int LAYER_DATA::Multifile_Random()
{
    int file_i=0;
    int i;
    int total_no_snps=0;
    double Distrib[Max_File];
    double mfrand;
    
    //printf("Run Multifile_Random\n");
    
    //srand ( time(NULL) );
    
    for (i=0; i<no_file; i++)
        total_no_snps += individual_file[i].no_snps;
        
    if (total_no_snps <=0) Abort("No SNP loaded for testing");
    else
    {
        
      //printf("total_no_snps = %d\n", total_no_snps );
        
      for (i=0; i<no_file; i++)
        Distrib[i] = (double) individual_file[i].no_snps/total_no_snps;
        
      file_i = 0;
      do
      {
         mfrand = (double) rand()/RAND_MAX; //RAND_MAX = 32767
      } while (mfrand>=1 || mfrand <=0);
      //printf("mfrand = %.3f\n", mfrand);
      while (mfrand >0)
      {
        if (file_i >= no_file) Abort("Calculate SNP distribution");
        mfrand -= Distrib[file_i];
        if (mfrand >0) file_i++;
      };
     };   
    
    if (individual_file[file_i].no_snps <1) Abort ("Pick File");
    return (file_i);

};
LAYER_FILE_DATA::LAYER_FILE_DATA()
{
   no_snps = 0;

};

void LAYER_DATA::GetPara(int number_file, int no_sub,  int Npp, float penalty)
{
    no_file = number_file;
    no_subjects = no_sub;
    Np = Npp;
    penalty_per_snp = penalty;
    printf("Number of Subjects = %d\n",no_subjects);
   
};
    

void LAYER_FILE_DATA::LoadMatrix(int no_subjects, int flag_cox)
{
	
    FILE *fp;
    float value;
    int classlabel;
    float survival_time;
    char symbol;
    int snp_index = 0;
    int case_control_index = 0;
 
    fp = fopen(matrix_filename, "r" );     
	if(fp == NULL )	
	 Abort ("Cannot open matrix file.");
	 

    //printf("Number of Subjects = %d\n", no_subjects);
    printf("Number of parameters = %d\n", no_snps);
    
    // load all the parameters and the class label as the first column
    
    Case_Control.record = new AlleleUnit *[no_subjects];
    Case_Control.label = new int [no_subjects];
    Case_Control.time = new double [no_subjects];   
    
    for (case_control_index=0; case_control_index<no_subjects; case_control_index++)
       Case_Control.record[case_control_index] = new AlleleUnit [no_snps];
           
    for (case_control_index = 0; case_control_index < no_subjects; case_control_index++)
    {       
		//load class label
        fscanf(fp, "%d", &classlabel);

        //printf("%3.3f\t", value);
        Case_Control.label[case_control_index]= classlabel;  
		
		if (flag_cox == 1) 
		{ 
        	fscanf(fp, "%f", &survival_time);		    
        	Case_Control.time[case_control_index]= (double) survival_time;  
        };
		        
         for (snp_index = 0; snp_index< (no_snps-1); snp_index++) //load values except the last one
         {
               fscanf(fp, "%f", &value);
               //printf("%3.3f\t", value);
               Case_Control.record[case_control_index][snp_index].value= (double) value;    
   
           } 
           fscanf(fp, "%f", &value); // the last value kept here
           //printf("%3.3f\n", value);
           Case_Control.record[case_control_index][snp_index].value= (double) value;    
    };
    	 
    fclose (fp);

    Case_Control.no_snp = no_snps;

};   


