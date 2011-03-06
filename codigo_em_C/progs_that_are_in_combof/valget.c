// C Source File
// Created 2005-08-18; 22:46:27

#define RETURN_VALUE rsultado
//#define OPTIMIZE_ROM_CALLS

//#define USE_TI89
#define ENABLE_ERROR_RETURN   // Enable Returning Errors to TIOS

//#define USE_TI92PLUS
//#define USE_V200
//#define SAVE_SCREEN

//#define MIN_AMS 101 //200

#include <tigcclib.h>

/*
float matval(const char * tabela, short i, short j)
{
	static char tabela2[30];
  static float d;
	sprintf(tabela2, "%s[%d,%d]", tabela, i, j);

  CESI tab2cesid;
  push_END_TAG();
	push_zstr (tabela2);
	tab2cesid = top_estack;

	TRY
	push_str_to_expr (tab2cesid);
  tab2cesid = top_estack;
  d = estack_number_to_Float (tab2cesid);
  while (GetArgType (top_estack) != END_TAG)  // Clean up arguments
    top_estack = next_expression_index (top_estack);
  top_estack--;
  FINALLY
  ENDFINAL

	return d;
}
*/

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


float *interpol(float x_1, float *y_1, float x_2, float *y_2, float x, short szy)
{
float *y, rx;
short i;
y = calloc (szy, sizeof(float));

rx=(1/(x_2-x_1))*(x-x_1);
for (i=0;i<szy;i++)
	y[i]=(y_2[i]-y_1[i])*rx+y_1[i];

return (y);
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

// Main Function
void _main(void)
{
  ESI argptr;
  FILE *f;
  short i1,i2,j1,*j2,transpose,justget; //,way
  float baseprop,*rbaseprop,resultado,*rresultado;
	const char *tabela;
	short i,a,b,inicio,fim,meio,dj2,jmax,fsz;
	float *prop,*col1,*col2,lin1,lin2,val1,val2,val3,*rval1,*rval3;
	short bv2s, bv2b, *rbv2s, *rbv2b,*rinicio,*rfim;
	rval1=&val1;
	rval3=&val3;
	rbv2s=&bv2s;
	rbv2b=&bv2b;
	rinicio=&inicio;
	rfim=&fim;
	rbaseprop=&baseprop;
	rresultado=&resultado;

	//clrscr();
  //ngetchx();

	//get arguments
	argptr = top_estack;
	i1 = GetIntArg (argptr);
	i2 = GetIntArg (argptr);
	j1 = GetIntArg (argptr);
	j2 = GetIntListArg (&argptr,&dj2);
	baseprop = GetFloatArg (argptr);
	tabela = GetStrnArg (argptr);
	jmax = GetIntArg (argptr);
	fsz = GetIntArg (argptr);
	transpose = GetIntArg (argptr);
	if (GetArgType (argptr) == POSINT_TAG)
		justget = GetIntArg (argptr);
	else
		justget = 0;

  while (GetArgType (top_estack) != END_TAG)  // Clean up arguments
    top_estack = next_expression_index (top_estack);
  top_estack--;

//  printf("%d %d\n %d %d\n %d",j2[0],j2[1],j2[2],j2[3],dj2);
//  printf("%s %f\n",tabela2,baseprop);
//  ngetchx();

	f = fopen (tabela, "rb");
	if (f!=NULL)
	{

		a=0;
		b=0;
		
		inicio=i1;
		fim=i2;
		val1=matval(f, j1, i1, jmax, fsz, transpose);
		val3=matval(f, j1, i2, jmax, fsz, transpose);
		if (val1>val3)
		{
			rval1=&val3;
			rval3=&val1;
			rbv2b=&bv2s;
			rbv2s=&bv2b;
			rinicio=&fim;
			rfim=&inicio;
			rbaseprop=&resultado;
			rresultado=&baseprop;
		}

	  if ((*rval1<=baseprop) & (baseprop<=*rval3))
	  {
			do
			{
			  meio=(inicio+fim)/2;
			
				val2=matval(f, j1, meio, jmax, fsz, transpose);
			  
  //printf("%f %f %f %f\n",val1,val2,val3,baseprop);
  //printf("%d %d %d\n",inicio,fim,meio);
  //ngetchx();
	
			  bv2b = baseprop>=val2;
			  bv2s = baseprop<=val2;

			  if (baseprop==val1)
			  {
			    b=(a=inicio);
			    break;
			  }
			  else if (baseprop==val2)
			  {
			    b=(a=meio);
			    break;
			  }
			  else if (baseprop==val3)
			  {
			  	b=(a=fim);
			    break;
			  }    
			  else if (fim - inicio ==1)
			  {
			    a=inicio;
			    b=fim;
			    break;
			  }
			  else if ((*rval1<=baseprop) & *rbv2s)
			  {
			    fim=meio;
			    val3=val2;
			  }
			  else if (*rbv2b & (baseprop <= *rval3))
			  {
			    inicio=meio;
			    val1=val2;
			  }
			
			} while(inicio>0);
	  }

/*		
		if (a!=0)
		{
		 
		 for (i=a;i<=i2;i++)
		 {
		  resultado=matval(f, j1, i, jmax, fsz, transpose);
		  if (*rbaseprop<=*rresultado)
		  {
		    b=i;
		    break;
		  }
		 }
		}
*/

//   printf("a e b =>%d %d. justget=%d\n",a,b,justget);
//   ngetchx();
		
		if ((a==0) | (b==0))
		{
		  undefed:
		  
		  push_END_TAG();
		  for (i=0;i<dj2;i++)
				push_quantum(UNDEF_TAG);
		  push_LIST_TAG();	
		  free(j2);
		  fclose(f);
			return;
		}
		else if (a==b)
		{
			prop = calloc (dj2, sizeof(float));
		  for (i=0;i<dj2;i++)
		    prop[i]=matval(f, j2[i], a, jmax, fsz, transpose);
		}
		else
		{
		  if (!justget)
		  {
				lin1=matval(f, j1, a, jmax, fsz, transpose);
			  lin2=matval(f, j1, b, jmax, fsz, transpose);
			  col1=calloc (dj2, sizeof(float));
			  col2=calloc (dj2, sizeof(float));
			  for (i=0;i<dj2;i++)
			  {
			    col1[i]=matval(f, j2[i], a, jmax, fsz, transpose);
			    col2[i]=matval(f, j2[i], b, jmax, fsz, transpose);
			  }
			  prop=interpol(lin1,col1,lin2,col2,baseprop,dj2);
			  free(col1);
			  free(col2);
		  }
			else
			{
				prop=calloc (dj2 * 2, sizeof(float));
				for (i=0;i<dj2;i++)
				{
					prop[i]=matval(f, j2[i], a, jmax, fsz, transpose);
					prop[i+dj2]=matval(f, j2[i], b, jmax, fsz, transpose);
				}
				dj2*=2;
			}
		}
	
	  push_END_TAG();
	  for (i=dj2-1;i>=0;i--)
			push_Float(prop[i]);
	//	push_zstr (tabela2);
	  push_LIST_TAG();
	
		free(prop);
	  free(j2);
		fclose(f);
		return;
	}
	else
		goto undefed;
}

