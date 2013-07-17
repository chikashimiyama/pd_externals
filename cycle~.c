/* code by Chikashi Miyama 2009 */

#include "m_pd.h"
#include <math.h>
#include <stdlib.h>

#define PI 3.14159265
#define TPI PI * 2

#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

/* ----------------------------- cycle----------------------------- */
static t_class *cycle_class;

typedef struct _cycle
{
    t_object x_obj;
	t_float x_next;
	t_float x_previous;
	t_float x_head;
	t_float x_gap;
	t_float x_sr;
	t_float x_f;
	t_float x_hold;

} t_cycle;


static void *cycle_new(void)
{

	t_cycle* x = (t_cycle*)pd_new(cycle_class);
	inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
	inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
	inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
	outlet_new(&x->x_obj, &s_signal);
	x->x_next = 0.3;
	x->x_head = 1.1;
	return (x);
    
}

t_int *cycle_perform(t_int *w)
{
	t_cycle *x = (t_cycle*)(w[1]);
    t_sample *freq = (t_sample *)(w[2]);
    t_sample *coef1 = (t_sample *)(w[3]);
    t_sample *coef2 = (t_sample *)(w[4]);

    t_sample *curve = (t_sample *)(w[5]);
    t_sample *out = (t_sample *)(w[6]);

	t_float next = x->x_next;
	t_float previous = x->x_previous;
	t_float head = x->x_head;
	t_float gap = x->x_gap;
	t_float hold = x->x_hold;

	t_float sr = x->x_sr;
	t_float co1,co2;
	
	int n = (int)(w[7]);
	float step,cfrq, distorted;
    while (n--){
		
		if(head > 1.0)
		{
			head = 0.0;
			previous = next;

			co1 = *coef1;
			co2 = *coef2;

			next = previous + co2 - (co1 / TPI * sin(TPI*previous));
			next = fmodf( next , TPI);
			if(next < 0.0)
			{
				next += TPI;
			}
			gap = next - previous;
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
		
		if(hold == 0)
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

		*out++ = previous + distorted * gap;
	}
	
	x->x_next = next;
	x->x_previous = previous;
	x->x_head = head;
	x->x_gap = gap;
	x->x_hold = hold;
    return (w+8);
}


static void cycle_dsp(t_cycle*x, t_signal **sp)
{
	x->x_sr = sp[0]->s_sr;
    dsp_add(cycle_perform, 7, x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[0]->s_n);//data, in 1, in 2, out. vector

}

void cycle_tilde_setup(void)
{
    cycle_class = class_new(gensym("cycle~"), (t_newmethod)cycle_new, 0, sizeof(t_cycle), 0, 0);

    class_addmethod(cycle_class, (t_method)cycle_dsp, gensym("dsp"), 0);
    CLASS_MAINSIGNALIN(cycle_class, t_cycle, x_f);
    class_sethelpsymbol(cycle_class, gensym("sigbinops"));
}
