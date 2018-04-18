#include "gea.h"

extern const char *para_header[Max_Number_Header_Parameters];
extern int UseHeader;

AlleleUnit::AlleleUnit()
{
    value = 0;
}   

CaList::CaList()
{
 value = 0;
}   

ClassLabel::ClassLabel()
{
	class_symbol = -100000;
	class_count = 0;
	next_label =  NULL;
}   

SAMPLE_FILE::SAMPLE_FILE(int s, int r)
{
   no_record = r; 
   no_snp = s;
   
   record = new AlleleUnit*[r];
   for(int i = 0;i < r;i++){
      record[i] = new AlleleUnit[s + 1];
   }
}


ModelUnit::ModelUnit()
{
   file_index = 0;;
   snploc = 0;
   link = NULL;
   oper = '-';
}

void Model::ToScreen()
{
    ToScreen(0,0);
};

void Model::ToScreen(int gcount, int iter)
{
   printf("-----Model-----\n");
   ModelUnit *p = rule;
      
   if(p != NULL){
      for(;p != NULL;p = p->link){
         printf("%c %d (%3.3f)\n",p->oper,  p->snploc, p->al1.value);
      }
   }

   printf("Fitness=%3.2f\n",fitness);
   printf("Score=%3.2f\n",accuracy);
   
   
   printf("Generation = %d.\n",(ITER_NO * gcount + iter));
}
void Model::ToFile(char file[])
{
    ToFile(file,0,0);
}  
void Model::ToFile(char file[],int gcount, int iter)
{
   FILE *fpt; 
   fpt = fopen(file, "a");

   ModelUnit *p = rule;
   
   fprintf(fpt, "Fit=%4.5f\t",fitness);
   fprintf(fpt, "Score=%4.5f\t",accuracy);
   //fprintf(fpt, "MST=%4.5f\t",tp);    
   //fprintf(fpt, "MSE=%4.5f\t",tn); 
   //fprintf(fpt, "SST=%4.5f\t",fp);   
   //fprintf(fpt, "SSE=%4.5f\t",fn);
          
   fprintf(fpt, "%d\t",(ITER_NO * gcount + iter));
   
   fprintf(fpt, "%d =",(length));

   if(p != NULL){
      for(;p != NULL;p = p->link){
         if (UseHeader && (p->snploc < Max_Number_Header_Parameters))
             fprintf(fpt, " %c %s * (%7.4f)",p->oper, para_header[p->snploc], p->al1.value);
         else
             fprintf(fpt, " %c %d * (%7.4f)",p->oper, p->snploc, p->al1.value);         
      }
   }
   fprintf(fpt, "\n");
   
   fclose(fpt);
}  

Model::Model()
{
   rule = NULL;
   fitness = 0.0;
   accuracy = 0.0;
   length = 0;

   //tp = 0;
   //tn = 0;
   //fp = 0;
   //fn = 0;

}

void Model::Release()
{
   ModelUnit *now, *next;
   now = rule;
   while(now != NULL){
      next = now->link;
      delete now;
      now = next;
   }
   rule = NULL;
   fitness = 0.0;
   //tp = 0;
   //tn = 0;
   //fp = 0;
   //fn = 0;
   length = 0;
}  

void Model::SetLength()
{
	ModelUnit *temp = rule;
	int count = 0;
	while(temp != NULL){
		count++;
		temp = temp->link;
	}
	length = count;
}
		
