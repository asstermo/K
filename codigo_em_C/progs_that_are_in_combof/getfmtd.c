// C Source File
// Created 2005-08-30; 01:37:45

#define RETURN_VALUE rsultstr
#define SLEN 50

//#define SAVE_SCREEN

#include <tigcclib.h>

float matval(FILE *f, short i, short j, short jmax, short fsz, const BOOL transpose)
{
  static float d;
  static short fpos, ii, jj;
  static char buffer[10];

	if (transpose)
	{ii=j;jj=i;}
	else
	{ii=i;jj=j;}

	fpos = fsz - (2 + (jmax * 10 + 2) * (ii-1) + jj * 10);
	fseek (f, fpos, SEEK_SET);
	fread(buffer,1,10,f);

	d = estack_number_to_Float (buffer+9);

	return d;
}


short *GetIntListArg (ESI *argptr, short *numlist)
{
	short *int_list, mylist[20], i, szi;
	
	memset (mylist, (short) NULL, sizeof(mylist));
	int_list=NULL;
	*numlist=(short) NULL;

	if (GetArgType(*argptr) == LIST_TAG)	
	{
		(*argptr)--;
		for (i=0;i<20;i++)
		{
			if ( GetArgType (*argptr) != END_TAG)
			{
				if (GetArgType (*argptr) == POSINT_TAG)
				{
					(*argptr)--;
					szi=(BYTE) **argptr;
					(*argptr)-=szi;
					mylist[i]=(BYTE) **argptr;
					(*numlist)++;
				}
			}
			else
				break;
			(*argptr)--;
		}
		(*argptr)--;

		if (*numlist>0)
		{
			int_list=calloc(*numlist, sizeof(short));
			memcpy(int_list,mylist,sizeof(short) * (*numlist));
		}
	}

	return int_list;
}


char *GetStrnListArg (ESI *argptr, short *numlist)
{
	char *str_list, mylist[20][20];
	short i;
	ESI *argptr2;
	
	memset (mylist, (short) NULL, 400);
	str_list=NULL;
	*numlist=(short) NULL;

	if (GetArgType(*argptr) == LIST_TAG)	
	{
		(*argptr)--;
		for (i=0;i<20;i++)
		{
			if ( GetArgType (*argptr) != END_TAG)
			{
				if (GetArgType (*argptr) == STR_TAG)
				{

					(*argptr)--;
					argptr2=(ESI*) *argptr;
					do{(*argptr)--;}while((BYTE) **argptr != 0);
					strncpy(mylist[i],(char*) (*argptr)+1,  ((long) argptr2 - (long) *argptr));
					(*numlist)++;
				}
			}
			else
				break;
			(*argptr)--;
		}
		(*argptr)--;

		if (*numlist>0)
		{
			str_list=calloc(*numlist, 20*sizeof(char));
			memcpy(str_list,mylist,20*sizeof(char) * (*numlist));
		}
	}

	return str_list;
}

void ftoa(float valor, short width, char *rftoa)
{
	char left[20],right[20];
	float leftl,rightl,sign;
	
	memset(left, (char) NULL, 20*sizeof(char));
	memset(right, (char) NULL, 20*sizeof(char));

	sign= (valor>=0.) ? 1. : -1.;
	rightl=abs(valor);
	leftl=(float) ((long) rightl);
	rightl=rightl-leftl;
	
//printf("%f %f %f %f\n",sign,leftl,rightl,valor);
//ngetchx();

	if (sign<0.)
		strcpy(rftoa,"\xAD");
	
	if (leftl>0.)
	{
		sprintf(left,"%f",leftl);
		strcat(rftoa,left);
	}
	
	strcat(rftoa,".");

	sprintf(right,"%.15f",rightl);
	strcat(rftoa,right+2);

//printf("%s\n%s\n%s\n",rftoa,left,right);
//ngetchx();
	if (width>0)
	{
		strcat(rftoa,"000000000000");
		rftoa[width]=0x0;
	}
	else if (strlen(rftoa)==1)
	{if (rftoa[0]==46) strcpy(rftoa,"0.");}
	
}

// Main Function
void _main(void)
{

/*
int (*A)[100] = NULL;   // A pointer to an array
...
A = calloc (200, sizeof (*A));
*/		

  ESI argptr;
  FILE *f;
	const char *tabela;
	char (*fmt)[20],(*devolver)[SLEN],buffer[SLEN];
  short i1,i2,i3,*js,*width,transpose,fsz,jmax,djs,dwidth,dfmt,i,jj,idev,idevsz;
  
  //clrscr();
	fmt=(char*) NULL;
	dfmt=0;

	//get arguments (i1,i2,i3,*js,*width,*txtappend,tabname,fsz,jmax,transpose)
	argptr = top_estack;
	i1 = GetIntArg (argptr);
	i2 = GetIntArg (argptr);
	i3 = GetIntArg (argptr);
	js = GetIntListArg (&argptr,&djs);
	width = GetIntListArg (&argptr,&dwidth);
	fmt = GetStrnListArg (&argptr,&dfmt);
	tabela = GetStrnArg (argptr);
	fsz = GetIntArg (argptr);
	jmax = GetIntArg (argptr);
	transpose = GetIntArg (argptr);

  while (GetArgType (top_estack) != END_TAG)  // Clean up arguments
    top_estack = next_expression_index (top_estack);
  top_estack--;
  
  idevsz=(i2-i1+1)/i3;
  devolver=calloc(idevsz, SLEN*sizeof(char));
  
  memset (devolver, (char) NULL, idevsz*SLEN*sizeof(char));
/*
printf("0x%lp 0x%lp 0x%lp\n",devolver,buffer,fmt);
printf("%d %d %d %d\n",i1,i2,i3,djs);
ngetchx();
*/
	f = fopen (tabela, "rb");
	if (f!=NULL)
	{
	  idev=0;
	  for (i=i1;i<=i2;i+=i3)
	  {
	   	for (jj=0;jj<djs;jj++)
	   	{
	   		memset(buffer, (char) NULL, SLEN*sizeof(char));
	   		ftoa(matval(f, js[jj], i, jmax, fsz, transpose), width[jj], buffer);
	   		if (jj<dfmt)
		   		strcat(buffer,(char*) &(fmt[jj]));
	   		//sprintf(buffer,(char*) &(fmt[jj]),matval(f, js[jj], i, jmax, fsz, transpose));
	   		strcat(devolver[idev],buffer);
/*
printf("%d '%s' 0x%lp\n",js[jj],&(fmt[jj]),&(fmt[jj]));
printf("i=%d jj=%d idev=%d \n%s\n",i,jj,idev,devolver[idev]);
ngetchx();
*/
	   	}
	   	idev++;
	  }
		fclose(f);
	}
  
  push_END_TAG();
  for (i=idevsz-1;i>=0;i--)
		push_zstr (devolver[i]);
  push_LIST_TAG();
  
  free(fmt);
  free(devolver);
	return;
}
