// C Source File
// Created 2005-08-29; 22:26:56

// Delete or comment out the items you do not need.

#define RETURN_VALUE prop
#include <tigcclib.h>

FILE *f = NULL;

float matval(short i, short j)
{
  static float d;
  static short fpos;
  static char buffer[10];

	fpos = 7834 - (2 + 712 * (i-1) + j * 10);
	fseek (f, fpos, SEEK_SET);
	fread(buffer,1,10,f);

	d = estack_number_to_Float (buffer+9);

	return d;
}

float ps_w_pv(float pv, float p){return 0.622 * (pv)/(p-pv);}
float ps_h_tw(float T, float w){return 1.005 * T+w * (2501.+1.86 * T);}

// Main Function
void _main(void)
{
  ESI argptr;
	const char tabela[]="tabh2oa2";
	short i,a,b,inicio,fim,meio;
	const short i1=2,i2=41,j1=1,j2=2;
	float t,w,valm[2][2],tlista[3]={0,0,0},wlista[3]={0,0,0},val[3]={0,0,0},cte[4]={0,0,0,0};
	const float cpv=1.86,cpa=1.005;
	float h,HR,p;

	//get arguments
	argptr = top_estack;
	h = GetFloatArg (argptr);
	HR = GetFloatArg (argptr);
	p = GetFloatArg (argptr);

  while (GetArgType (top_estack) != END_TAG)  // Clean up arguments
    top_estack = next_expression_index (top_estack);
  top_estack--;

	f = fopen (tabela, "rb");
	if (f!=NULL)
	{
		
		a=0;
		b=0;
		
		inicio=i1;
		fim=i2;
		val[0]=matval(j1,inicio);
		val[2]=matval(j1,fim);
		tlista[0]=matval(j2,inicio);
		tlista[2]=matval(j2,fim);

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
			
			  val[1]=matval(j1,meio);
			  tlista[1]=matval(j2,meio);
			
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
	    goto undefed;
		}
		else if (a==b)
		{
		  w=ps_w_pv(HR * matval(j2,a),p);
		  t=matval(j1,a);
		}
		else
		{
		  valm[0][1]=ps_w_pv(HR * matval(j2,a),p);
		  valm[0][0]=matval(j1,a);
		  
		  valm[1][1]=ps_w_pv(HR * matval(j2,b),p);
		  valm[1][0]=matval(j1,b);
		  
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
		    goto undefed;
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
		undefed:
	  push_END_TAG();
		push_quantum(UNDEF_TAG);
		push_quantum(UNDEF_TAG);
	  push_LIST_TAG();	
		return;
	}
}