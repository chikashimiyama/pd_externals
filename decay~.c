/* code by Chikashi Miyama 2009 */

#include "m_pd.h"
#include <math.h>
#include <stdlib.h>

#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

/* ----------------------------- decay----------------------------- */
static t_class *decay_class;

typedef struct _decay
{
    t_object x_obj;
    t_float x_previous;
    t_float x_factor;
} t_decay;


static void *decay_new(void)
{
	t_decay*x = (t_decay*)pd_new(decay_class);
    x->x_previous = 0.0;
    inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
	outlet_new(&x->x_obj, &s_signal);
    
	return (x);
}

t_int *decay_perform(t_int *w)
{
    
	t_decay *x  = (t_decay *)(w[1]);
    t_sample *in = (t_sample *)(w[2]);
    t_sample *factor = (t_sample *)(w[3]);
    t_sample *out = (t_sample *)(w[4]);
    int n = (int)(w[5]);
    t_sample prev = x->x_previous;
    
    while(n--)
    {
        float f = *(in++);
        if(f > prev ){
            *out = f;
        }else{
            *out = prev - ((prev - f) * *factor);
        }
        prev = *out;
        *out++;
        *factor++;
    }
    x->x_previous = prev;
    return (w+6);
}


static void decay_dsp(t_decay *x, t_signal **sp)
{
    dsp_add(decay_perform, 5, x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n);
}

void decay_tilde_setup(void)
{
    decay_class = class_new(gensym("decay~"), (t_newmethod)decay_new, 0, sizeof(t_decay), 0, 0);

    class_addmethod(decay_class, (t_method)decay_dsp, gensym("dsp"), 0);
    CLASS_MAINSIGNALIN(decay_class, t_decay, x_factor);
    class_sethelpsymbol(decay_class, gensym("sigbinops"));
}
