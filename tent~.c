/* code by Chikashi Miyama 2009 */

#include "m_pd.h"
#include <math.h>
#include <stdlib.h>

#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

/* ----------------------------- tent----------------------------- */
static t_class *tent_class;

typedef struct _tent
{
    t_object x_obj;
	t_float x_next;
	t_float x_previous;
	t_float x_head;
	t_float x_gap;
	t_float x_hold;
	t_float x_sr;
	t_float x_f;

} t_tent;


static void *tent_new(t_symbol *s, int argc, t_atom *argv)
{

	t_tent*x = (t_tent*)pd_new(tent_class);
	inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
	inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
	outlet_new(&x->x_obj, &s_signal);
	x->x_previous = 0.8;
	x->x_next = 0.8;
	x->x_head = 1.1;
	return (x);
    
}

t_int *tent_perform(t_int *w)
{
	t_tent *x = (t_tent*)(w[1]);
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
			previous = next ;
			co = *coef;	
			if(co >= 2.0)
			{
				co = 1.9999;
			}
			else if(co < 1.0){
				co = 1.0;	
			}
			if (previous < 0.5)
			{
				next = co * previous;
			}else{
				next = co * (1-previous);
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


static void tent_dsp(t_tent*x, t_signal **sp)
{
	x->x_sr = sp[0]->s_sr;
    dsp_add(tent_perform, 6, x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec, sp[0]->s_n);//data, in 1, in 2, out. vector

}

void tent_tilde_setup(void)
{
    tent_class = class_new(gensym("tent~"), (t_newmethod)tent_new, 0, sizeof(t_tent), 0, A_GIMME, 0);

    class_addmethod(tent_class, (t_method)tent_dsp, gensym("dsp"), 0);
    CLASS_MAINSIGNALIN(tent_class, t_tent, x_f);
    class_sethelpsymbol(tent_class, gensym("sigbinops"));
}
