/* code by Chikashi Miyama 2009 */

#include "m_pd.h"
#include <math.h>
#include <stdlib.h>

#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

/* ----------------------------- dyadic----------------------------- */
static t_class *dyadic_class;

typedef struct _dyadic
{
    t_object x_obj;
	t_float x_next;
	t_float x_previous;
	t_float x_head;
	t_float x_gap;
	t_float x_hold;
	t_float x_sr;
	t_float x_f;
	t_float x_precoef;

} t_dyadic;


static void *dyadic_new(void)
{

	t_dyadic*x = (t_dyadic*)pd_new(dyadic_class);
	inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
	inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
	outlet_new(&x->x_obj, &s_signal);
	x->x_next = 0.5;
	x->x_head = 1.1;
	x->x_precoef = 0.33421;
	return (x);
    
}

t_int *dyadic_perform(t_int *w)
{
	t_dyadic *x = (t_dyadic*)(w[1]);
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
	t_float preco = x->x_precoef;
	int n = (int)(w[6]);
	float step,cfrq, distorted;
    while (n--){
		
		if(head > 1.0)
		{
			head = 0.0;
			previous = next;
			co = *coef;
			if(co < 0.0)
			{
				co = 0.0;
			}
			else if(co > 1.0)
			{
				co = 1.0;
			}
			
			if(preco == co)
			{
				if(0.0 <= previous && previous < 0.5)
				{
					next = 2.0 * previous;
				}
				else if(0.5 <= previous && previous < 1.0)
				{
					next = 2.0 * previous - 1.0;
				}
			}
			else{
				preco = co;
				if(0.0 <= preco && preco < 0.5)
				{
					next = 2.0 * preco;
				}
				else if(0.5 <= preco && preco < 1.0)
				{
					next = 2.0 * preco - 1.0;
				}
				
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
	x->x_precoef = preco;
    return (w+7);
}


static void dyadic_dsp(t_dyadic*x, t_signal **sp)
{
	x->x_sr = sp[0]->s_sr;
    dsp_add(dyadic_perform, 6, x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec, sp[0]->s_n);//data, in 1, in 2, out. vector

}

void dyadic_tilde_setup(void)
{
    dyadic_class = class_new(gensym("dyadic~"), (t_newmethod)dyadic_new, 0, sizeof(t_dyadic), 0, 0);

    class_addmethod(dyadic_class, (t_method)dyadic_dsp, gensym("dsp"), 0);
    CLASS_MAINSIGNALIN(dyadic_class, t_dyadic, x_f);
    class_sethelpsymbol(dyadic_class, gensym("sigbinops"));
}
