/* code by Chikashi Miyama 2009 */

#include "m_pd.h"
#include <math.h>
#include <stdlib.h>

#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

#define TPI 6.28318531

/* ----------------------------- chirikov----------------------------- */
static t_class *chirikov_class;

typedef struct _chirikov
{
    t_object x_obj;
	t_float x_nextx;
	t_float x_nexty;
	t_float x_previousx;
	t_float x_previousy;
	t_float x_gapx;
	t_float x_gapy;
	t_float x_hold;
	t_float x_head;
	t_float x_sr;
	t_float x_f;
} t_chirikov;


static void *chirikov_new(void)
{

	t_chirikov*x = (t_chirikov*)pd_new(chirikov_class);
	inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
	inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);

	outlet_new(&x->x_obj, &s_signal);
	outlet_new(&x->x_obj, &s_signal);

	x->x_previousx = 0.6;
	x->x_previousy = 0.6;

	x->x_nextx = 0.1;
	x->x_nexty = 0.213;

	x->x_head = 1.1;

	return (x);
    
}

t_int *chirikov_perform(t_int *w)
{
	t_chirikov *x = (t_chirikov*)(w[1]);
    t_sample *freq = (t_sample *)(w[2]);
    t_sample *coef1 = (t_sample *)(w[3]);

    t_sample *curve = (t_sample *)(w[4]);
    t_sample *out1 = (t_sample *)(w[5]);
    t_sample *out2 = (t_sample *)(w[6]);

	t_float nextx = x->x_nextx;
	t_float nexty = x->x_nexty;
	t_float previousx = x->x_previousx;
	t_float previousy = x->x_previousy;
	t_float gapx = x->x_gapx;
	t_float gapy = x->x_gapy;
	t_float head = x->x_head;
	t_float hold = x->x_hold;
	
	t_float sr = x->x_sr;
	t_float co1,co2,mag;
	
	int n = (int)(w[7]);
	float step,cfrq, distorted;
    while (n--){
		
		if(head > 1.0)
		{
			head = 0.0;
			previousx = nextx;
			previousy = nexty;

			co1 = *coef1;
			
			nextx = previousx + co1*sin(previousy);
			nextx = fmodf(nextx, TPI);
			if(nextx < 0.0)
			{
				nextx += TPI;
			}
			nexty = previousy + nextx;
			nexty = fmodf(nexty,TPI);
			if(nexty < 0.0)
			{
				nexty += TPI;
			}
			gapx = nextx - previousx;
			gapy = nexty - previousy;
			hold = *curve;

		}
		cfrq = *freq++;
		if(cfrq == 0)
		{	
			step = 0;
		}
		else
		{
			step = 1.0/(sr / cfrq);
		}
		head += step;

		//power
		
		if(hold == 0.0)
		{
			distorted = 0.0;
		}
		else if(hold == 1.0)
		{
			distorted = head;
		}
		else
		{
			distorted = pow(head, hold);
		}
		

		*curve++;
		*coef1++;

		*out1++ = distorted * gapx + previousx;
		*out2++ = distorted * gapy + previousy;





	}
	
	x->x_nextx = nextx;
	x->x_nexty = nexty;
	x->x_previousx = previousx;
	x->x_previousy = previousy;
	x->x_head = head;
	x->x_gapx = gapx;
	x->x_gapy = gapy;
	x->x_hold = hold;

	
    return (w+8);
}


static void chirikov_dsp(t_chirikov*x, t_signal **sp)
{
	x->x_sr = sp[0]->s_sr;
    dsp_add(chirikov_perform, 7, x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[0]->s_n);
}

void chirikov_tilde_setup(void)
{
    chirikov_class = class_new(gensym("chirikov~"), (t_newmethod)chirikov_new, 0, sizeof(t_chirikov), 0, 0);

    class_addmethod(chirikov_class, (t_method)chirikov_dsp, gensym("dsp"), 0);
    CLASS_MAINSIGNALIN(chirikov_class, t_chirikov, x_f);
    class_sethelpsymbol(chirikov_class, gensym("sigbinops"));
}
