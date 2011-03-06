#define RETURN_VALUE resgener
#include <tigcclib.h>
#define SLEN 50


FILE *f = NULL;


float matval(short i, short j, short jmax, short fsz, const BOOL transpose)
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

float matval2(short i, short j) {return matval(i,j,71,7834,0);}

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

	if (width>0)
	{
		strcat(rftoa,"000000000000");
		rftoa[width]=0x0;
	}
	else if (strlen(rftoa)==1)
	{if (rftoa[0]==46) strcpy(rftoa,"0.");}
	
}


float ps_w_pv(float pv, float p){return 0.622 * (pv)/(p-pv);}
float ps_h_tw(float T, float w){return 1.005 * T+w * (2501.+1.86 * T);}

void cleanarg(void)
{
	while (GetArgType (top_estack) != END_TAG)  // Clean up arguments
    top_estack = next_expression_index (top_estack);
  top_estack--;
}

// Main Function
void _main(void)
{
  ESI argptr;
  short opcao;

	//get arguments
	argptr = top_estack;
	opcao = GetIntArg (argptr);

  if (opcao==1)
  	goto valget;
 	else if (opcao==2)
  	goto ps_d_hhr;
 	else if (opcao==3)
  	goto getfmtd;
 	else if (opcao==4)
  	goto wlimits;
  else
  	goto novefora;
  	
//VALGET -----------------------------------------------------------------
  valget:;
  short i1,i2,j1,*j2,transpose,justget;
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

  cleanarg();

	f = fopen (tabela, "rb");
	if (f!=NULL)
	{

		a=0;
		b=0;
		
		inicio=i1;
		fim=i2;
		val1=matval(j1, i1, jmax, fsz, transpose);
		val3=matval(j1, i2, jmax, fsz, transpose);
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
			  //meio=(short) (ceil( (((float)(inicio+fim))/2.)));
			  meio=(inicio+fim)/2;
			
				val2=matval(j1, meio, jmax, fsz, transpose);
			  
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

		if ((a==0) | (b==0))
		{
		  undefed1:
		  
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
		    prop[i]=matval(j2[i], a, jmax, fsz, transpose);
		}
		else
		{
		  if (!justget)
		  {
				lin1=matval(j1, a, jmax, fsz, transpose);
			  lin2=matval(j1, b, jmax, fsz, transpose);
			  col1=calloc (dj2, sizeof(float));
			  col2=calloc (dj2, sizeof(float));
			  for (i=0;i<dj2;i++)
			  {
			    col1[i]=matval(j2[i], a, jmax, fsz, transpose);
			    col2[i]=matval(j2[i], b, jmax, fsz, transpose);
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
					prop[i]=matval(j2[i], a, jmax, fsz, transpose);
					prop[i+dj2]=matval(j2[i], b, jmax, fsz, transpose);
				}
				dj2*=2;
			}
		}
	
	  push_END_TAG();
	  for (i=dj2-1;i>=0;i--)
			push_Float(prop[i]);
	  push_LIST_TAG();
	
		free(prop);
	  free(j2);
		fclose(f);
		return;
	}
	else
		goto undefed1;

//ps_d_hhr -----------------------------------------------------------------
  ps_d_hhr:;
	//short i,a,b,inicio,fim,meio;
	const short j22=2;
	i1=2;i2=41;j1=1;
	float t,w,valm[2][2],tlista[3]={0,0,0},wlista[3]={0,0,0},val[3]={0,0,0},cte[4]={0,0,0,0};
	const float cpv=1.86,cpa=1.005;
	float h,HR,p;

	//get arguments
	h = GetFloatArg (argptr);
	HR = GetFloatArg (argptr);
	p = GetFloatArg (argptr);

  cleanarg();

	f = fopen ("tabh2oa2", "rb");
	if (f!=NULL)
	{
		
		a=0;
		b=0;
		
		inicio=i1;
		fim=i2;
		val[0]=matval2(j1,inicio);
		val[2]=matval2(j1,fim);
		tlista[0]=matval2(j22,inicio);
		tlista[2]=matval2(j22,fim);

		for (i=0;i<3;i+=2)
		{
		  tlista[i]*=HR;
		  wlista[i]=ps_w_pv(tlista[i],p);
		  tlista[i]=h-ps_h_tw(val[i],wlista[i]);
		}

	  if (!((tlista[0]<0.) | (tlista[2]>0.)))
	  {
			do
			{
			
			  meio=(short) (ceil( (((float)(inicio+fim))/2.)));
			
			  val[1]=matval2(j1,meio);
			  tlista[1]=matval2(j22,meio);
			
			  tlista[1]*=HR;
			  wlista[1]=ps_w_pv(tlista[1],p);
			  tlista[1]=h-ps_h_tw(val[1],wlista[1]);

				if (tlista[0]==0)
				{
			    a=inicio;
			    b=a;
			    break;
				}
			  else if (tlista[1]==0)
			  {
			    a=meio;
			    b=a;
			    break;
			  }
			  else if (tlista[2]==0)
			  {
			    a=fim;
			    b=a;
			    break;
			  }
			  else if (fim-inicio==1)
			  {
			    a=inicio;
			    b=fim;
			    break;
			  }
			  else if ((tlista[0]>0) & (tlista[1]<0))
			  {
			    fim=meio;
			    val[2]=val[1];
			    tlista[2]=tlista[1];
			  }
			  else if ((tlista[1]>0) & (tlista[2]<0))
			  {
			    inicio=meio;
			    val[0]=val[1];
			    tlista[0]=tlista[1];
			  }
			
			}while(inicio>0);
	  }
		
		if ((a==0) | (b==0))
		{
			fclose(f);
	    goto undefed2;
		}
		else if (a==b)
		{
		  w=ps_w_pv(HR * matval2(j22,a),p);
		  t=matval2(j1,a);
		}
		else
		{
		  valm[0][1]=ps_w_pv(HR * matval2(j22,a),p);
		  valm[0][0]=matval2(j1,a);
		  
		  valm[1][1]=ps_w_pv(HR * matval2(j22,b),p);
		  valm[1][0]=matval2(j1,b);
		  
		  cte[0]=(valm[0][1]-valm[1][1]) * cpv;
		  cte[1]=(valm[0][0] * (valm[1][1] * cpv+cpa)-valm[1][0] 
		  			 * (valm[0][1] * cpv+cpa)+(valm[0][1]-valm[1][1]) * 2501.);
		  cte[2]=(valm[0][0] * valm[1][1]-valm[1][0] * valm[0][1])*2501.;
		  cte[3]=valm[0][0]-valm[1][0];

		  cte[0]/=cte[3];
		  cte[1]/=cte[3];
		  cte[2]/=cte[3];
		  cte[2]-=h;
		  cte[3]=sqrt(pow(cte[1],2.) - 4. * cte[0] * cte[2]);
		  tlista[0]=(-cte[1] + cte[3]) / (2. * cte[0]);
		  tlista[1]=(-cte[1] - cte[3]) / (2. * cte[0]);

		  if ((0.<tlista[0]) & (tlista[0]<71.))
		    t=tlista[0];
		  else if ((0.<tlista[1]) & (tlista[1]<71.))
		    t=tlista[1];
		  else
		  {
				fclose(f);
		    goto undefed2;
		  }
		
			w=((valm[1][1]-valm[0][1])/(valm[1][0]-valm[0][0]))*(t-valm[0][0])+valm[0][1];

		}
		
	  push_END_TAG();
		push_Float(w);
		push_Float(t);
	  push_LIST_TAG();
		
		fclose(f);
		return;
	}
	else
	{
		undefed2:
	  push_END_TAG();
		push_quantum(UNDEF_TAG);
		push_quantum(UNDEF_TAG);
	  push_LIST_TAG();	
		return;
	}

//getfmtd -----------------------------------------------------------------
  getfmtd:;
	//const char *tabela;
	char (*fmt)[20],buffer[SLEN],buffer2[SLEN];
  //short i1,i2,transpose,fsz,jmax,i;
  short i3,*js,*width,djs,dwidth,dfmt,jj;
  
	fmt=(char*) NULL;
	dfmt=0;

	//get arguments (i1,i2,i3,*js,*width,*txtappend,tabname,fsz,jmax,transpose)
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
	
  cleanarg();
  
	f = fopen (tabela, "rb");
	if (f!=NULL)
	{
	  push_END_TAG();
	  for (i=i2;i>=i1;i-=i3)
	  {
	   	memset(buffer2, (char) NULL, SLEN*sizeof(char));
	   	for (jj=0;jj<djs;jj++)
	   	{
	   		memset(buffer, (char) NULL, SLEN*sizeof(char));
	   		ftoa(matval(js[jj], i, jmax, fsz, transpose), width[jj], buffer);
	   		if (jj<dfmt)
		   		strcat(buffer,(char*) &(fmt[jj]));
	   		strcat(buffer2,buffer);
	   	}
			push_zstr (buffer2);
		}
		fclose(f);
	  push_LIST_TAG();
	}

  free(fmt);
  free(width);
  free(js);
	return;

//wlimits -----------------------------------------------------------------
	wlimits:;
	//short i1,i2,i;
	float p_ar,pv; //,resultado
	
	i1 = GetIntArg (argptr);
	i2 = GetIntArg (argptr);
	p_ar = GetFloatArg (argptr);
	
  cleanarg();
  
	f = fopen ("tabh2oa2", "rb");
	if (f!=NULL)
	{
		push_END_TAG();
		for (i=i2;i>=i1;i--)
		{
			pv=matval(2, i, 71, 7834, 0);
			resultado=0.622 * pv/(p_ar-pv);
			push_Float(resultado);
		}
		push_LIST_TAG();
		fclose(f);
		return;
	}
	else
	{
	  push_END_TAG();
	  for (i=i1;i<=i2;i++)
			push_quantum(UNDEF_TAG);
	  push_LIST_TAG();	
		return;
	}
	
//NOVEFORA -----------------------------------------------------------------
	novefora:
	push_END_TAG();
	push_quantum(UNDEF_TAG);
	push_LIST_TAG();
	return;
}
