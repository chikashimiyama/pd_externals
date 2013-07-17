/* code by Chikashi Miyama 2009 */

#include "m_pd.h"
#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

/* ----------------------------- accum ----------------------------- */
static t_class *accum_class;

typedef struct _accum
{
    t_object x_obj;
    t_sample x_f;
} t_accum;

void accum_set(t_accum *x, t_floatarg g)
{
    x->x_f = g;
}

static void *accum_new(void)
{

    t_accum *x = (t_accum *)pd_new(accum_class);
    outlet_new(&x->x_obj, &s_signal);
    x->x_f = 1;
    return (x);
}

t_int *accum_perform(t_int *w)
{
    t_sample* acp = (t_sample *)(w[1]);
    t_sample *in = (t_sample *)(w[2]);
    t_sample *out = (t_sample *)(w[3]);
    int i,n = (int)(w[4]);
    t_sample ac = *acp;

    for(i = 0;i< n;i++)
	{
		out[i] = in[i] + ac;
		ac =  out[i];
	}
    *acp = ac;
    return (w+5);
}



static void accum_dsp(t_accum *x, t_signal **sp)
{
    dsp_add(accum_perform, 4, &x->x_f, sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n);
}


void accum_tilde_setup(void)
{
    accum_class = class_new(gensym("accum~"), (t_newmethod)accum_new, 0, sizeof(t_accum), 0, A_GIMME, 0);
    CLASS_MAINSIGNALIN(accum_class, t_accum, x_f);
    class_addmethod(accum_class, (t_method)accum_dsp, gensym("dsp"), 0);
    class_addmethod(accum_class, (t_method)accum_set, gensym("set"), A_FLOAT, 0); 


}
