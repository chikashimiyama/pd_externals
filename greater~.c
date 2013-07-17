/* code by Chikashi Miyama 2009 */

#include "m_pd.h"
#include <math.h>
#include <stdlib.h>

#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

/* ----------------------------- greater----------------------------- */
static t_class *greater_class;

typedef struct _greater
{
    t_object x_obj;
	t_float x_sr;
	t_float x_f;

} t_greater;


static void *greater_new(void)
{

	t_greater*x = (t_greater*)pd_new(greater_class);
	inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
	outlet_new(&x->x_obj, &s_signal);
	return (x);
    
}

t_int *greater_perform(t_int *w)
{
	t_greater *x = (t_greater*)(w[1]);
    t_sample *in = (t_sample *)(w[2]);
    t_sample *thresh = (t_sample *)(w[3]);
    t_sample *out = (t_sample *)(w[4]);
	
	int n = (int)(w[5]);
    while (n--){
		*out++ = *in++ > *thresh++;
	}
    return (w+6);
}


static void greater_dsp(t_greater*x, t_signal **sp)
{
	x->x_sr = sp[0]->s_sr;
    dsp_add(greater_perform, 5, x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n);//data, in 1, in 2, out. vector

}

void greater_tilde_setup(void)
{
    greater_class = class_new(gensym("greater~"), (t_newmethod)greater_new, 0, sizeof(t_greater), 0, 0);

    class_addmethod(greater_class, (t_method)greater_dsp, gensym("dsp"), 0);
    CLASS_MAINSIGNALIN(greater_class, t_greater, x_f);
    class_sethelpsymbol(greater_class, gensym("sigbinops"));
}
