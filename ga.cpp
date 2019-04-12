#include"gea.h"
#include <math.h>
#include <algorithm>

SAMPLE_FILE::SAMPLE_FILE()
{
    no_snp = 0;
    no_record = 0;
};




float FindCentroid(ModelUnit *head) 
{
	float centroid = 0;
	int count=0;
	
	for(ModelUnit *t1 = head;t1 != NULL;t1 = t1->link)
    {
       centroid += t1->snploc;
       count++;
    };
    
    centroid /= count;
   
   return (centroid);
}

bool Check2(ModelUnit *rule, int newfile, int newloc) 
//chek the newloc not exist in the rule
{
   bool answer = true;
   for(ModelUnit *t = rule;t != NULL;t = t->link){
      if(t->file_index == newfile && t->snploc == newloc){
         answer = false;  //redundant detected
         break;
      }
   }
   
   return answer;
} 

bool Check_and_Update(ModelUnit *rule, int newfile, int newloc, double original_value) 
//if exist same item, update and condense. Otherwise, report true
{
   bool updated = false;
   for(ModelUnit *t = rule; t != NULL; t = t->link)
   {
      if(t->file_index == newfile && t->snploc == newloc && t->oper == '+' && (t->link == NULL || t->link->oper == '+'))
	  {
      	t->al1.value += original_value;
        updated = true;  
        break;
      }
   }
   
   return updated;
} 
      
                  
ModelUnit * LAYER_DATA::Obtain_Coef()
{
	ModelUnit *element;
	
	element = new ModelUnit;
    
    double current;   
   	//srand(time(0) + rand());//Seed of the random-number generator
    current = (double) (rand() - (RAND_MAX/2))/(RAND_MAX/6);    //rand() = 0-32767, so the range is about (-3 ~ 3)
    
    //printf("%f\n",current);
    
    element->al1.value=current;
    return (element);
};
                     


   
ModelUnit * LAYER_DATA::Random_Rules()
{
	ModelUnit *head = NULL;
	ModelUnit *element;
	int file_index;
	int loc;
	int number; 
    int i;

    number = (rand() % Maxmodel) +1;
    
    for(i = 0;i < number;i++){
       file_index = Multifile_Random();
       loc = rand() % individual_file[file_index].no_snps;
       element = Obtain_Coef();
       //printf("%f\n", element->al1.value);
       element->file_index = file_index;
       element->snploc = loc;
       if (rand() % 2 )
          element->oper = '+';
       else element->oper = '*';
       element->link = head;
       head = element;
    } 
    
    return (head);
}

// condense the rules and also solve the parmeter bounding problem
// It create a new model while the original model retained. The original model is deteted in memory by a higher level function
ModelUnit * LAYER_DATA::Rules_Condensing (ModelUnit *original)
{
	ModelUnit *current;// current end	
	ModelUnit *current2;
	ModelUnit *original_next;
	ModelUnit *head = NULL;
	ModelUnit *element;//new element
	bool IsHead;

   	IsHead = true;
	head = NULL;
    current = NULL;
    
	while(original != NULL)
    {	
    	original_next = original->link;
    	
  	   if (IsHead) 
  	   {
	     element = new ModelUnit;
	     element->file_index = original->file_index;
	     element->snploc = original->snploc;
	     element->oper = original->oper;
         element->al1.value = original->al1.value;		 
         element->link = NULL;	 
         head = element;         
         current = element;
         IsHead = false; 
	   }
		else if (original->oper == '*' || (original_next != NULL && original_next->oper == '*')) //model element unaltered, just copying
	   {
	     element = new ModelUnit;
	     element->file_index = original->file_index;
	     element->snploc = original->snploc;
	     element->oper = original->oper;
         element->al1.value = original->al1.value;
         element->link = NULL;
         current->link = element;        
         current = current->link;	
	   }
		else if (Check_and_Update(head, original->file_index, original->snploc, original->al1.value)) 
		//only "+ element +" or "+ element Null" needed to be checked 
		{}
	    else {
	     element = new ModelUnit;
	     element->file_index = original->file_index;
	     element->snploc = original->snploc;
	     element->oper = original->oper;
         element->al1.value = original->al1.value;
         element->link = NULL;
         current->link = element;        
         current = current->link;
         }
         original = original->link;

     }
    return (head);
}

// It create a new model while the original model retained. The original model is deteted in memory by a higher level function    
ModelUnit * LAYER_DATA::Mutate(ModelUnit *original, double mutation_rate)
{
	double probability;
	ModelUnit *current;// current end	
	ModelUnit *head = NULL;
	ModelUnit *element;//new element
	int file_index;
	int loc;
	char oper;
	bool IsHead;
	int choice;
    
	IsHead = true;
	head = NULL;
    current = NULL;
   
	while(original != NULL)
    {			
	   //RAND_MAX is max of rand. RAND_MAX = 0x7fff (32767)
	   probability = rand() * 1.0 / RAND_MAX;
	   if (probability > mutation_rate) //model element unaltered, just copying
	   {
	     element = new ModelUnit;
	     element->file_index = original->file_index;
	     element->snploc = original->snploc;
	     element->oper = original->oper;
         element->al1.value = original->al1.value;

         element->link = NULL;

         original = original->link;
         if(IsHead)
         {
            head = element;
            current = element;
         } else
         { 
           current->link = element;
           current = current->link;
         };
         IsHead = false; 
       } else //mutate
       {  
		   choice = rand() % 8 + 1;
		   switch(choice){
		      case 1://delete the current node  
                 original = original->link;
                 break;
              case 2://insert after current, before original
                 file_index = Multifile_Random();
                 loc = rand() % individual_file[file_index].no_snps;
                 element = Obtain_Coef(); 
                 element->file_index = file_index;
				 element->snploc = loc;

                 if (rand() % 2 )
                    element->oper = '+';
                 else element->oper = '*';
                 element->link = NULL;
                 if(IsHead)
                 {
                    head = element;
                    current = element;
                 } else 
                 { 
                    current->link = element;
                    current = current->link;
                  };
                  IsHead = false; 
                 break;
              case 3://Change variable and coefficient but not sign
			     file_index = original->file_index;
			     loc = rand() % individual_file[file_index].no_snps;
			     element = Obtain_Coef();
			     element->file_index=file_index;
				 element->snploc = loc;
                 element->oper = original->oper;
                 element->link = NULL;
                 if(IsHead)
                 {
                    head = element;
                    current = element;
                 } else 
                 { 
                    current->link = element;
                    current = current->link;
                 };
                 original = original->link; 
                 IsHead = false;
	             break;              
              case 4://
              case 5:// coefficient multiply a random value if it is not higher than SquareOfDouble, for preventing overflow
			     file_index = original->file_index;
			     loc = original->snploc;
			     element = Obtain_Coef();
				 if (abs(original->al1.value) < SquareOfDouble)		     
			     	element->al1.value = (original->al1.value) * (element->al1.value);
			     else element->al1.value = (original->al1.value) + (element->al1.value); //minor modification
			     	
			     element->file_index=file_index;
				 element->snploc = loc;
                 element->oper = original->oper;
                 element->link = NULL;
                 if(IsHead)
                 {
                    head = element;
                    current = element;
                 } else 
                 { 
                    current->link = element;
                    current = current->link;
                 };
                 original = original->link; 
                 IsHead = false;
	             break;              
              case 6:// coefficient change
              case 7:// coefficient change
			     file_index = original->file_index;
			     loc = original->snploc;
			     element = Obtain_Coef();
			     element->file_index=file_index;
				 element->snploc = loc;
                 element->oper = original->oper;
                 element->link = NULL;
                 if(IsHead)
                 {
                    head = element;
                    current = element;
                 } else 
                 { 
                    current->link = element;
                    current = current->link;
                 };
                 original = original->link; 
                 IsHead = false;
	             break;
             case 8: //change operator
                 oper = original->oper;
                 element = new ModelUnit;
                 element->file_index = original->file_index;
                 element->snploc = original->snploc;
                 element->al1.value = original->al1.value;

                 if (oper == '+') element->oper = '*';
                 else element->oper = '+';
                 element->link = NULL;
                 if(IsHead)
                 {
                    head = element;
                    current = element;
                 } else
                 { 
                    current->link = element;
                    current = current->link;
                  };
                  original = original->link;
                 IsHead = false; 
                 break; 
           }  //switch   
	   } //else
   } //while  

   return (head);
}



ModelUnit *Crossover(Model rule1, Model rule2)
{
	//double probability;
	int choice;
	int cut1, cut2;
	ModelUnit *temp1, *temp2;
	ModelUnit *head = NULL;
	ModelUnit *current = NULL;
	ModelUnit *element;
	//bool start, end;
	int counter = 0;
	bool IsHead = true;
	

    //srand(time(0) + rand());
    
	choice = rand() % 2; // determine which model goes first
    if(choice){
	   temp1 = rule1.rule;
	   cut1 = rand() % rule1.length;
	   temp2 = rule2.rule;
	   cut2 = rand() % rule2.length;	   
    } 
    else{
	   temp1 = rule2.rule;
	   cut1 = rand() % rule2.length;
	   temp2 = rule1.rule;
	   cut2 = rand() % rule1.length;	   
    }

    counter = 0;
   	while(temp1 != NULL && counter <= cut1)
    {
			//copy an element without change
	        element = new ModelUnit;
	        element->file_index = temp1->file_index;
			element->snploc = temp1->snploc;
			//element->weight = temp1->weight;
			element->oper=temp1->oper;
            element->al1.value = temp1->al1.value;

            element->link = NULL;
            if(IsHead)
            {
                head = element;
                current = element;
             } else
             { 
                current->link = element;
                current = current->link;
              };
              IsHead = false; 
            temp1 = temp1->link;
			counter++;
	}
	
	counter = 0;
   	while(temp2 != NULL)
    {

 	    if(counter >=cut2)
        {
			//copy an element without change
	        element = new ModelUnit;
	        element->file_index= temp2->file_index;
			element->snploc = temp2->snploc;
			element->oper=temp2->oper;
            element->al1.value = temp2->al1.value;
            element->link = NULL;
            if(IsHead)
            {
                head = element;
                current = element;
             } else
             { 
                current->link = element;
                current = current->link;
              };
              IsHead = false; 
			counter++;
        }
        temp2 = temp2->link;
	}
    
    return (head);
}


void LAYER_DATA::Fitness_ANOVA()

{

    int index = 0;
    int subject_id = 0;
    int no_class;
    double SSE = 0;
    double SST = 0;
    double MSE = 0;
    double MST = 0;
    double *subject_score;
    double total_mean_score = 0;
    ClassLabel *classlabel_ptr = classlabel_link;
    
	subject_score = new double [no_subjects];
    
    //index indicate models
    for(index = 0; index < Np; index++)
	{

      
       SSE = 0;
       SST = 0;
       MSE = 0;
       MST = 0;
       total_mean_score = 0;
       no_class = 0;
    	
		for (subject_id = 0;subject_id <no_subjects; subject_id++)
       	  		subject_score[subject_id] = Scoring(subject_id, index);

		//calculate means
		classlabel_ptr = classlabel_link;       	  		
       while (classlabel_ptr != NULL)
       {
       		classlabel_ptr->class_mean_score = 0;
       	
       		for (subject_id = 0;subject_id <no_subjects; subject_id++)	   
       			if (individual_file[0].Case_Control.label[subject_id]==classlabel_ptr->class_symbol)
       			{
       	  			classlabel_ptr->class_mean_score += subject_score[subject_id];
					total_mean_score += subject_score[subject_id];	  
				};
 	   		classlabel_ptr->class_mean_score /= classlabel_ptr->class_count; 
			no_class++; 
 	   		classlabel_ptr = classlabel_ptr->next_label;
 	   	};
 	   	
 	   	total_mean_score /= no_subjects;
 	   	
 	   	
		//calculate variances       	  		
		classlabel_ptr = classlabel_link;   
		while (classlabel_ptr != NULL)
       {
       	
       		for (subject_id = 0;subject_id <no_subjects; subject_id++)	   
       			if (individual_file[0].Case_Control.label[subject_id]==classlabel_ptr->class_symbol)
       	  			SSE += (subject_score[subject_id]-classlabel_ptr->class_mean_score)*(subject_score[subject_id]-classlabel_ptr->class_mean_score);
	   		  
 	   		SST += (classlabel_ptr->class_mean_score-total_mean_score)*(classlabel_ptr->class_mean_score-total_mean_score)*classlabel_ptr->class_count;
 	   		classlabel_ptr = classlabel_ptr->next_label;
 	   	};
 	   	
 	   	MSE = (double) SSE/(no_subjects-no_class);
 	   	MST = (double) SST/(no_class-1);
               
       population[index].accuracy = (double) MST/MSE; 
       population[index].fitness = population[index].accuracy * (1- population[index].length * penalty_per_snp);  

   } //for the entire cohort
}


void LAYER_DATA::Calculate_Number_Case_Controls()

{
    int i = 0;
    int j = 0;

    int subject_id;

       i=0; //for case
       j=0; //for control
       run_anova = 0;
       
       subject_id = 0;
       do
       {
       		if (individual_file[0].Case_Control.label[subject_id]==0)
       	  		j++;
	   		else if (individual_file[0].Case_Control.label[subject_id]==1)
          		i++;
          	else run_anova = 1; //When class label is other than 1/0, run ANOVA
			subject_id++;
       } while (subject_id <no_subjects && run_anova == 0);
    no_case = i;
    no_control = j; 
	
	if (run_anova == 0)
		printf ("Number of Cases = %d\nNumber of Controls = %d\n", no_case, no_control);
    if (no_case <= 0 || no_control <= 0)
    	Abort ("Data contained only one class. Cannot perform Mann-Whitney test or Cox regression.");
};

void LAYER_DATA::ClassLabel_Counts()
{

    int subject_id;
    ClassLabel *current_ptr; 
    bool empty_link = true;
    int class_count = 0;
	

       subject_id = 0;
       do
       {
       		current_ptr = classlabel_link; // point to the start of the link list
			while (current_ptr != NULL && individual_file[0].Case_Control.label[subject_id] != current_ptr->class_symbol)
       		{
       			current_ptr = current_ptr->next_label;
			}
			if (current_ptr == NULL)
			{
				current_ptr = new ClassLabel();
				current_ptr->class_symbol = individual_file[0].Case_Control.label[subject_id];
				current_ptr->class_count = 1; // starting from 1
				current_ptr->next_label = classlabel_link;
				classlabel_link = current_ptr;
			} else current_ptr->class_count++;
			subject_id++;
       } while (subject_id <no_subjects);

       	current_ptr = classlabel_link; // point to the start of the link list
       	while (current_ptr != NULL)
       	{
       		printf ("Class Label %d\tNumber = %d\n", current_ptr->class_symbol, current_ptr->class_count);
       		current_ptr = current_ptr->next_label;
       		class_count++;
		}
		if (class_count<=1)
			Abort ("Class label less than 2. Cannot perform ANOVA analysis.");

};

void LAYER_DATA::Fitness_U_statistics(bool two_way)

{
    int i = 0;
    int j = 0;
    int index=0;
    int subject_id;
    double *case_score;
    double *control_score;
    double U_statistics;
    
    case_score = new double [no_subjects];
    control_score = new double [no_subjects];
    
    //printf("Starting receiver-operating characteristics Analysis\n");      

    //index indicate models
    for(index = 0; index < Np; index++){

      
       i=0; //for case
       j=0; //for control
       for (subject_id = 0;subject_id <no_subjects; subject_id++)
       {
	   
       		if (individual_file[0].Case_Control.label[subject_id]==0)
       		{
       	  		control_score[j] = Scoring(subject_id, index);
       	  		j++;
	   		} else if (individual_file[0].Case_Control.label[subject_id]==1)
       		{
          		case_score[i] = Scoring(subject_id, index); 
          		i++;
          	};

       };  
          
       U_statistics = 0;
       
       for(i = 0;i < no_case;i++)
           for(j = 0;j < (no_control); j++)
           {
               if (case_score[i] > control_score[j]) U_statistics++;
               else if (case_score[i] == control_score[j]) U_statistics += 0.5;
               }
       population[index].accuracy = (double) U_statistics/(no_case*(no_subjects-no_case)); 
       if (two_way && population[index].accuracy < 0.5)
            population[index].fitness = 1- population[index].accuracy;
       else population[index].fitness = population[index].accuracy;
       
       population[index].fitness -= (double) population[index].length * penalty_per_snp;

   } //for the entire cohort
}

void LAYER_DATA::Fitness_Cox_likelihood()

{
    int i = 0;
    int j = 0;
    int index=0;
    int subject_id;
    double *case_score;
    double *control_score;
    double *case_time;
    double *control_time;
    double HR_post_Event;
    double Likelihood_per_Event;
    //double Cox_likelihood;
    double Log_Cox_likelihood;
    double Log_Cox_likelihood_temp;
	     
    case_score = new double [no_subjects];
    control_score = new double [no_subjects];
    case_time = new double [no_subjects];
    control_time = new double [no_subjects];
    
    //printf("Starting Cox regression Analysis\n");  
    //index indicate models
    for(index = 0; index < Np; index++){

      
       i=0; //for case
       j=0; //for control
       for (subject_id = 0;subject_id <no_subjects; subject_id++)
       {
	   
	   		// Survival Events defined as "case" and class label = 1
       		if (individual_file[0].Case_Control.label[subject_id]==0)
       		{
       	  		control_score[j] = std::min(exp(Scoring(subject_id, index)),UpperBound);
       	  		control_time[j] = individual_file[0].Case_Control.time[subject_id];
       	  		j++;
	   		} else if (individual_file[0].Case_Control.label[subject_id]==1)
       		{
          		case_score[i] = std::min(exp(Scoring(subject_id, index)),UpperBound); 
          		case_time[i] = individual_file[0].Case_Control.time[subject_id];
          		i++;
          	};

       };  
          
       Log_Cox_likelihood = 0;      
       Log_Cox_likelihood_temp = 0;    
	   	       
       for(i=0; i<no_case; i++)
       {
			HR_post_Event = 0;
			for (j=0; j<no_control; j++)
               if (control_time[j] >= case_time[i]) HR_post_Event += control_score[j];
               
			for (j=0; j<no_case; j++)
               if (case_time[j] >= case_time[i]) HR_post_Event += case_score[j];      
			
			if (HR_post_Event == 0) 
			{
				Likelihood_per_Event = PositiveZero; // neutralize this event
				//Abort("Divide by 0");      
			} else 
				Likelihood_per_Event = std::max(case_score[i]/HR_post_Event,PositiveZero); 
			
            Log_Cox_likelihood_temp = std::max(log(Likelihood_per_Event),LowerBound);
            Log_Cox_likelihood = std::max(Log_Cox_likelihood+Log_Cox_likelihood_temp,LowerBound);
        }
        
		population[index].accuracy = (double) 2 * Log_Cox_likelihood; 
		
        population[index].fitness = population[index].accuracy - population[index].length * COXPenaltyFactor * penalty_per_snp;

   } //for all models
}

void BubbleSort(Model array[], int Np)
{
    bool change;
    ModelUnit *t;
    double a;
    double b;

    int e;
    
	for(int i = 0;i < Np - 1;i++){
      change = false;
      for(int j = Np - 2;j >=i;j--){
         if((array[j].fitness < array[j + 1].fitness) 
         || (array[j].fitness == array[j + 1].fitness && array[j].length > array[j + 1].length)){
            t = array[j].rule;
            array[j].rule = array[j + 1].rule;
            array[j + 1].rule = t;
            
            a = array[j].fitness;
            array[j].fitness = array[j + 1].fitness;
            array[j + 1].fitness = a;
            
            b = array[j].accuracy;
            array[j].accuracy = array[j + 1].accuracy;
            array[j + 1].accuracy = b;
            
            e = array[j].length;
            array[j].length = array[j + 1].length;
            array[j + 1].length = e;
            
            change = true;
         }
      }
      if(!change){
         break;
      }
   }
}


