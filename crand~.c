/* code by Chikashi Miyama 2009 */

#include "m_pd.h"
#include <math.h>
#include <stdlib.h>

#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

/* ----------------------------- crand----------------------------- */
static t_class *crand_class;

typedef struct _crand
{
    t_object x_obj;
	t_float x_next;
	t_float x_previous;
	t_float x_head;
	t_float x_gap;
	t_float x_hold;
	t_float x_sr;
	t_float x_f;

} t_crand;


static void *crand_new(void)
{

	t_crand*x = (t_crand*)pd_new(crand_class);
	inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
	outlet_new(&x->x_obj, &s_signal);
	x->x_previous = 0.0;
	x->x_next = 1.0;
	return (x);
    
}

t_int *crand_perform(t_int *w)
{
	t_crand *x = (t_crand*)(w[1]);
    t_sample *freq = (t_sample *)(w[2]);
    t_sample *curve = (t_sample *)(w[3]);
    t_sample *out = (t_sample *)(w[4]);

	t_float next = x->x_next;
	t_float previous = x->x_previous;
	t_float head = x->x_head;
	t_float gap = x->x_gap;
	t_float hold = x->x_hold;	
	t_float sr = x->x_sr;
	
	int n = (int)(w[5]);
	float step,cfrq, distorted;
    while (n--){
		
		if(head > 1.0)
		{
			head = 0.0;
			previous = next;
			next = (float)(rand() % 10000) / 10000.0;
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
			distorted = previous;
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

		*out++ = previous + distorted * gap;

	}
	
	x->x_next = next;
	x->x_previous = previous;
	x->x_head = head;
	x->x_gap = gap;
	x->x_hold = hold;	
	
    return (w+6);
}


static void crand_dsp(t_crand*x, t_signal **sp)
{
	x->x_sr = sp[0]->s_sr;
    dsp_add(crand_perform, 5, x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n);//data, in 1, in 2, out. vector

}

void crand_tilde_setup(void)
{
    crand_class = class_new(gensym("crand~"), (t_newmethod)crand_new, 0, sizeof(t_crand), 0, 0);

    class_addmethod(crand_class, (t_method)crand_dsp, gensym("dsp"), 0);
    CLASS_MAINSIGNALIN(crand_class, t_crand, x_f);
    class_sethelpsymbol(crand_class, gensym("sigbinops"));
}
