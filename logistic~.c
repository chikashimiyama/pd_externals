/* code by Chikashi Miyama 2009 */

#include "m_pd.h"
#include <math.h>
#include <stdlib.h>

#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

/* ----------------------------- logistic----------------------------- */
static t_class *logistic_class;

typedef struct _logistic
{
    t_object x_obj;
	t_float x_next;
	t_float x_previous;
	t_float x_head;
	t_float x_gap;
	t_float x_hold;
	t_float x_sr;
	t_float x_f;

} t_logistic;


static void *logistic_new(void)
{

	t_logistic*x = (t_logistic*)pd_new(logistic_class);
	inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
	inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
	outlet_new(&x->x_obj, &s_signal);
	x->x_next = 0.5;
	x->x_head = 1.1;
	return (x);
    
}

t_int *logistic_perform(t_int *w)
{
	t_logistic *x = (t_logistic*)(w[1]);
    t_sample *freq = (t_sample *)(w[2]);
    t_sample *coef = (t_sample *)(w[3]);

    t_sample *curve = (t_sample *)(w[4]);
    t_sample *out = (t_sample *)(w[5]);

	t_float next = x->x_next;
	t_float previous = x->x_previous;
	t_float head = x->x_head;
	t_float gap = x->x_gap;
	t_float hold = x->x_hold;	
	t_float sr = x->x_sr;
	t_float co;
	int n = (int)(w[6]);
	float step,cfrq, distorted;
    while (n--){
		
		if(head > 1.0)
		{
			head = 0.0;
			previous = next;
			co = *coef;
			if(co >= 4.0)
			{
				co = 3.9999;
			}
			else if(co < 2.0)
			{
				co = 2.0;
			}
			
			next = co* previous * (1.0-previous);
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
		*coef++;
			   
		*out++ = previous + distorted * gap;
	}
	
	x->x_next = next;
	x->x_previous = previous;
	x->x_head = head;
	x->x_gap = gap;
	x->x_hold = hold;	
	
    return (w+7);
}


static void logistic_dsp(t_logistic*x, t_signal **sp)
{
	x->x_sr = sp[0]->s_sr;
    dsp_add(logistic_perform, 6, x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec, sp[0]->s_n);//data, in 1, in 2, out. vector

}

void logistic_tilde_setup(void)
{
    logistic_class = class_new(gensym("logistic~"), (t_newmethod)logistic_new, 0, sizeof(t_logistic), 0, 0);

    class_addmethod(logistic_class, (t_method)logistic_dsp, gensym("dsp"), 0);
    CLASS_MAINSIGNALIN(logistic_class, t_logistic, x_f);
    class_sethelpsymbol(logistic_class, gensym("sigbinops"));
}
