#include "gea.h"

void LAYER_DATA::GABA()
{
    GABA(GABA_Stable_Iter);
};

void LAYER_DATA::GABA(int stable_iter_threshold)
{
    
    int gcount;

	Model Temp;
	double temp_fitness;
	int i, j;
    int index;
    int snp_number = 0;
    bool stable = false;
    double MaxFit = -1.0e300;
    int stable_iter_count;
    int father;
    int mother;    
    population = new Model[Np]; 
    int pick;   
    int Top;
    int choice;
    int pop_index;
    float centroid;
    
    FILE *fp_Manhattan;
    
    if (stable_iter_threshold <=0) Abort("Stable Iteration Number must be greater than 0");


    srand(time(0) + rand());//Seed of the random-number generator. This is before the first invoke of rand
    rand();
    
    if (firstmodel != NULL)
    {
        population[0].rule = firstmodel; 
        printf("The starting model\n");
        population[0].ToScreen();
    } else
    {
        printf("Generating models\n");
        do {
           population[0].rule = (ModelUnit *) Random_Rules();
           population[0].ToScreen();
        } while (population[0].rule == NULL);
    };
    population[0].SetLength();    
    
    for(index = 1;index < Np;){
        population[index].rule = Random_Rules();
        if(population[index].rule != NULL){
		   population[index].SetLength();
           index++;
        }
    }
    
    printf("Number of models=%d\n",Np);
    
    Top = (int) (Top_Ratio * Np); 
    printf("Calculate Top=%d\n",Top);
    if (Top <= Preserved_Model) Abort ("Population size is too small. Number of Top models must be equal or greater than Preserved_Model");       
    
    int iter = 0;
    gcount = 0;
    
    Calculate_Number_Case_Controls();
    //Check the number of people with class labels 0 and 1. IF other class label detected, trigger ANOVA    
    
    if (run_anova == 1)
        printf("Starting ANOVA Analysis\n");
		ClassLabel_Counts();
    	//Check the number of people with various class labels 0 for ANOVA    

		
    do {   
       if (run_cox == 1)
       	   	Fitness_Cox_likelihood();
	   else if (run_anova == 0)
	   		Fitness_U_statistics(); // For maximizing AUROC
    	else 
    	    //Abort("end ANOVA Analysis");
			Fitness_ANOVA();
       
       BubbleSort(population, Np); //all the variables needed to be updated if model variable definition changes
       
       if (MaxFit < population[0].fitness) 
       {
              MaxFit = population[0].fitness;
              stable = false;
              stable_iter_count = 0;
       } else
              stable_iter_count++;
              
       if (stable_iter_count > stable_iter_threshold)
              stable = true;
              
       
       //Output the best result for every 500 iterations
       iter++;

       if(iter == ITER_NO)
       {
          population[0].ToScreen(gcount, iter);
          population[1].ToScreen(gcount, iter);
          population[2].ToScreen(gcount, iter);
          population[0].ToFile(midfile, gcount, iter);

          gcount++;
		  iter = 0;
       }

       for(j = Top;j < Np;j++)
       {
          population[j].Release();  
       
          choice = rand() % 3 + 1;
          //printf("choice=%d\n",choice);
          switch(choice) //more aggressively update the models
          { 
             case 1:
                mother = rand() % Top;
                population[j].rule = Crossover(population[mother], population[0]);

                population[j].SetLength(); 
                break;              
             case 2:
                mother = rand() % Top;
                population[j].rule = Crossover(population[0], population[mother]);

                population[j].SetLength(); 
                break; 

             case 3:
                population[j].rule = Random_Rules();
                population[j].SetLength();
                break;
          }
          population[j].SetLength();
          if (population[j].length == 0) j--;
	   }   
	   
       //printf("Generate 1-Top Populations. iter=%d\n",iter);

          
       for(j = Preserved_Model;j < Top;j++) //only the best Preserved_Model (3) populations are preserved, others are locally mutated (search proxmity)
       {
          
          do {
            Temp.rule = Mutate(population[j].rule, 0.5);
          } while (Temp.rule == NULL);
          
          population[j].Release(); 

          population[j].rule = Temp.rule;
          population[j].SetLength();
          
       } 
       
       for(j = 0;j < Preserved_Model;j++) //only the best Preserved_Model (3) populations are preserved, others are locally mutated (search proxmity)
       {
          
		  temp_fitness = population[j].fitness;
          Temp.rule = Rules_Condensing(population[j].rule);
          
          population[j].Release(); 

          population[j].rule = Temp.rule;
          population[j].fitness = temp_fitness;
          population[j].SetLength();
          
       } 
    }while(!stable);
    
     
    population[0].ToScreen(gcount, iter);
    population[0].ToFile(midfile, gcount, iter); //the stable model write to the last line of the midfile

}

double LAYER_DATA::Scoring(int record_index, int population_index) //scoring a subject by a model
{
	ModelUnit *temp;//current model pointer
	CaList *calc = NULL;
	CaList *calc2;
	CaList *now;
	
	double result=0;
	double probability=-1;
	int file_index;
	int snploc;
    double matching=0;	 //match a model unit
    
    temp = population[population_index].rule;

    result = 0; 
    //result_binary = 0; //initial state   
    if(temp == NULL)
    {
       printf("Warning: There is no model\n");
       //result_binary = -1;//null rule don't predict 
    } else
    {

	  //first pass, handling multiplication
      while(temp != NULL)
      { 
       file_index = temp->file_index;
       snploc = temp->snploc;
	   matching = 0;
	   

          matching = temp->al1.value*individual_file[file_index].Case_Control.record[record_index][snploc].value;
         
          if (calc == NULL)
          {
              calc = new CaList;
              calc->value = matching;
              calc->link = NULL;
		  } else if(temp->oper == '*') //multiplying values
          {
              calc->value *= matching;
		  } else if(temp->oper == '+') // do the addition later on
          {
              calc2 = new CaList;
              calc2->value = matching;
              calc2->link=calc;
              calc = calc2;
          }

       temp = temp->link;
      };
      //printf("second pass, handling addition\n");

         while (calc != NULL) 
         {
           now=calc;
           result +=calc->value;
           calc = calc->link;
           delete now;
          }
         //printf("result=%f\n" , result);
         


    };
    return result; 
  
};

