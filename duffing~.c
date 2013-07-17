/* code by Chikashi Miyama 2009 */

#include "m_pd.h"
#include <math.h>
#include <stdlib.h>

#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif


/* ----------------------------- duffing----------------------------- */
static t_class *duffing_class;

typedef struct _duffing
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
} t_duffing;


static void *duffing_new(t_symbol *s, int argc, t_atom *argv)
{

	t_duffing*x = (t_duffing*)pd_new(duffing_class);
	inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
	inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
	inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);

	outlet_new(&x->x_obj, &s_signal);
	outlet_new(&x->x_obj, &s_signal);

	x->x_previousx = 0.6;
	x->x_previousy = 0.6;

	x->x_nextx = 0.5;
	x->x_nexty = 0.5;

	x->x_head = 1.1;

	return (x);
    
}

t_int *duffing_perform(t_int *w)
{
	t_duffing *x = (t_duffing*)(w[1]);
    t_sample *freq = (t_sample *)(w[2]);
    t_sample *coef1 = (t_sample *)(w[3]);
    t_sample *coef2 = (t_sample *)(w[4]);

    t_sample *curve = (t_sample *)(w[5]);
    t_sample *out1 = (t_sample *)(w[6]);
    t_sample *out2 = (t_sample *)(w[7]);

	t_float nextx = x->x_nextx;
	t_float nexty = x->x_nexty;
	t_float previousx = x->x_previousx;
	t_float previousy = x->x_previousy;
	t_float gapx = x->x_gapx;
	t_float gapy = x->x_gapy;
	t_float head = x->x_head;
	t_float hold = x->x_hold;
	
	t_float sr = x->x_sr;
	t_float co1,co2;
	
	int n = (int)(w[8]);
	float step,cfrq, distorted;
    while (n--){
		
		if(head > 1.0)
		{
			head = 0.0;
			previousx = nextx;
			previousy = nexty;

			co1 = *coef1;
			co2 = *coef2;
			
			nextx = previousy;
			nexty = -1.0*co2*previousx + co1*previousy - previousy * previousy * previousy;
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
		*coef2++;

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

	
    return (w+9);
}


static void duffing_dsp(t_duffing*x, t_signal **sp)
{
	x->x_sr = sp[0]->s_sr;
    dsp_add(duffing_perform, 8, x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[5]->s_vec, sp[0]->s_n);
}

void duffing_tilde_setup(void)
{
    duffing_class = class_new(gensym("duffing~"), (t_newmethod)duffing_new, 0, sizeof(t_duffing), 0, A_GIMME, 0);

    class_addmethod(duffing_class, (t_method)duffing_dsp, gensym("dsp"), 0);
    CLASS_MAINSIGNALIN(duffing_class, t_duffing, x_f);
    class_sethelpsymbol(duffing_class, gensym("sigbinops"));
}
