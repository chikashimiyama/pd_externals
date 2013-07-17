/* code by Chikashi Miyama 2009 */

#include "m_pd.h"
#include <math.h>
#include <stdlib.h>

#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif


/* ----------------------------- tinkerbell----------------------------- */
static t_class *tinkerbell_class;

typedef struct _tinkerbell
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
} t_tinkerbell;


static void *tinkerbell_new(void)
{

	t_tinkerbell*x = (t_tinkerbell*)pd_new(tinkerbell_class);
	inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
	inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
	inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
	inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
	inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);

	outlet_new(&x->x_obj, &s_signal);
	outlet_new(&x->x_obj, &s_signal);

	x->x_previousx = 0.6;
	x->x_previousy = 0.6;

	x->x_nextx = 0.1;
	x->x_nexty = 0.1;

	x->x_head = 1.1;

	return (x);
    
}

t_int *tinkerbell_perform(t_int *w)
{
	t_tinkerbell *x = (t_tinkerbell*)(w[1]);
    t_sample *freq = (t_sample *)(w[2]);
    t_sample *coef1 = (t_sample *)(w[3]);
    t_sample *coef2 = (t_sample *)(w[4]);
    t_sample *coef3 = (t_sample *)(w[5]);
    t_sample *coef4 = (t_sample *)(w[6]);
	
    t_sample *curve = (t_sample *)(w[7]);
    t_sample *out1 = (t_sample *)(w[8]);
    t_sample *out2 = (t_sample *)(w[9]);

	t_float nextx = x->x_nextx;
	t_float nexty = x->x_nexty;
	t_float previousx = x->x_previousx;
	t_float previousy = x->x_previousy;
	t_float gapx = x->x_gapx;
	t_float gapy = x->x_gapy;
	t_float head = x->x_head;
	t_float hold = x->x_hold;
	
	t_float sr = x->x_sr;
	t_float co1,co2,co3,co4;
	
	int n = (int)(w[10]);
	float step,cfrq, distorted;
    while (n--){
		
		if(head > 1.0)
		{
			head = 0.0;
			previousx = nextx;
			previousy = nexty;

			co1 = *coef1;
			co2 = *coef2;
			co3 = *coef3;
			co4 = *coef4;
			
			nextx = previousx * previousx - previousy * previousy + co1 * previousx + co2 * previousy;
			nexty = 2.0 * previousx * previousy +  co3 * previousx + co4 * previousy;
			
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
		*coef3++;
		*coef4++;

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

	
    return (w+11);
}


static void tinkerbell_dsp(t_tinkerbell*x, t_signal **sp)
{
	x->x_sr = sp[0]->s_sr;
    dsp_add(tinkerbell_perform, 10, x, 
			sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[5]->s_vec, //in
			sp[6]->s_vec, sp[7]->s_vec, sp[0]->s_n); // out
}

void tinkerbell_tilde_setup(void)
{
    tinkerbell_class = class_new(gensym("tinkerbell~"), (t_newmethod)tinkerbell_new, 0, sizeof(t_tinkerbell), 0, 0);

    class_addmethod(tinkerbell_class, (t_method)tinkerbell_dsp, gensym("dsp"), 0);
    CLASS_MAINSIGNALIN(tinkerbell_class, t_tinkerbell, x_f);
    class_sethelpsymbol(tinkerbell_class, gensym("sigbinops"));
}
