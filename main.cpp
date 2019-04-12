#include "gea.h"

// GIMOV

using namespace std;
int UseHeader;
char *para_header[Max_Number_Header_Parameters];
    
int main(int argc, char *argv[])
{
    
    PARA_FILE Para;
    LAYER_DATA top_layer;
    int i;
    char matrix_filename[FILENAME_LENGTH];
    char header_filename[FILENAME_LENGTH];
    FILE *header_fp;    
    int parameter_index;
    
    char *ver = "1.4.5";


    int no_para;
    
    printf("Generalized Iterative Modelling %s\n\n", ver);
    top_layer.run_cox = 0;
     
    if (argc == 2) 
    {
    	printf("Loading GIM Parameter file: %s\n", argv[1]);
    	Para.Load(argv[1]);
	} else if (argc == 3 && (strcmp(argv[1], "-cox") == 0)) 
    {
    	printf("Loading GIM Parameter file: %s\n", argv[2]);
    	Para.Load(argv[2]);
    	top_layer.run_cox = 1;
	} else
	{
		printf("Usage: \n");
		printf("GIM [parameter_filename] (when the data does not have the time-to-event information.)\n");		
		printf("or\n");
		printf("GIM -cox [parameter_filename] (when the data contains a column of the time-to-event information.)\n\n");
						
		printf("Definition of items in the parameter file:\n");
		printf("- number of data files (a people-by-biomarker matrix)\n");
		printf("- Filename (without the extension .txt)\n");
		printf("- number of biomarker variables\n");
		printf("- number of people\n");
		printf("- iteration number\n");
		printf("- penalty of the fitness score for adding one additional variable to the model\n\n");

		printf("An Example parameter file is as follows:\n");
		printf("1\n");
		printf("GIM\n");
		printf("34\n");
		printf("253\n");
		printf("500\n");
		printf("0.5\n\n");
		Abort ("Program stopped.");
	}

    printf("Setting parameters for the top layer.\n");
    top_layer.GetPara(Para.no_file, Para.no_subjects, Para.Np, Para.penalty_per_snp);
          
    sprintf(header_filename, "%s%s", Para.filename_prefix[0],"-header.txt"); 
    //only the first data file can have header. 
    // header is only used for presentation, not affecting computation. If no header, numerical values will be used
    
    UseHeader = 0;         
    header_fp = fopen(header_filename, "r");     
	if(header_fp == NULL )	
	{
	    printf ("No header file detected, header not used.\n\n");
    } else
    {               
         UseHeader = 1;  
         no_para = Para.no_snps[0];  //number of header assumed to be the same of the first file              
         for (parameter_index = 0; (parameter_index< no_para && parameter_index< Max_Number_Header_Parameters); parameter_index++)
         {

               para_header[parameter_index] = new char [105];
               fscanf(header_fp, "%100s", para_header[parameter_index]); //Note: header limited to only 100 characters
               printf ("Header %d %s\n", parameter_index, para_header[parameter_index]);
          };
      fclose (header_fp);
    };

	//loading the subject by (classlabel + feature) matrix
	// Warning: when multple files are loaded, the class label in the first data file dominates
    for (i=0; i<Para.no_file; i++)
    {
   	    sprintf(top_layer.individual_file[i].matrix_filename, "%s%s", Para.filename_prefix[i],".txt");  
        printf("Loading Data file: %s\n", top_layer.individual_file[i].matrix_filename);
        //printf("Parameters number = %d\n", Para.no_snps[i]);
        top_layer.individual_file[i].no_snps = Para.no_snps[i];
        top_layer.individual_file[i].LoadMatrix(top_layer.no_subjects, top_layer.run_cox); 

    };
    
   	sprintf(top_layer.midfile, "%s%s%s%s", Para.filename_prefix[0],  "-GIMv",ver,".txt");    //version coded in the filename

    top_layer.GABA ();

    
    return 0;
};

// terminate the programme
void Abort(char *message)
{
    if (*message)
	  fprintf (stderr, "ERROR: %s\n", message);
    exit (3);
}
