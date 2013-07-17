/* code by Chikashi Miyama 2009 */

#include "m_pd.h"
#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

/* ----------------------------- smaller ----------------------------- */
static t_class *smaller_class, *scalarsmaller_class;

typedef struct _smaller
{
    t_object x_obj;
    t_float x_f;
} t_smaller;

typedef struct _scalarsmaller
{
    t_object x_obj;
    t_float x_f;
    t_float x_g;            /* inlet value */
} t_scalarsmaller;

static void *smaller_new(t_symbol *s, int argc, t_atom *argv)
{
    if (argc > 1) post("smaller~: extra arguments ignored");
    if (argc) // compared with arg 1
    {
        t_scalarsmaller *x = (t_scalarsmaller *)pd_new(scalarsmaller_class);
        floatinlet_new(&x->x_obj, &x->x_g);
        x->x_g = atom_getfloatarg(0, argc, argv);
        outlet_new(&x->x_obj, &s_signal);
        x->x_f = 0;
        return (x);
    }
    else
    {
        t_smaller *x = (t_smaller *)pd_new(smaller_class);
        inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
        outlet_new(&x->x_obj, &s_signal);
        x->x_f = 0;
        return (x);
    }
}

t_int *smaller_perform(t_int *w)
{
    t_sample *in1 = (t_sample *)(w[1]);
    t_sample *in2 = (t_sample *)(w[2]);
    t_sample *out = (t_sample *)(w[3]);
    int n = (int)(w[4]);
    while (n--) *out++ = *in1++ < *in2++; 
    return (w+5);
}

t_int *smaller_perf8(t_int *w)
{
    t_sample *in1 = (t_sample *)(w[1]);
    t_sample *in2 = (t_sample *)(w[2]);
    t_sample *out = (t_sample *)(w[3]);
    int n = (int)(w[4]);
    for (; n; n -= 8, in1 += 8, in2 += 8, out += 8)
    {
        t_sample f0 = in1[0], f1 = in1[1], f2 = in1[2], f3 = in1[3];
        t_sample f4 = in1[4], f5 = in1[5], f6 = in1[6], f7 = in1[7];

        t_sample g0 = in2[0], g1 = in2[1], g2 = in2[2], g3 = in2[3];
        t_sample g4 = in2[4], g5 = in2[5], g6 = in2[6], g7 = in2[7];

        out[0] = f0 < g0; out[1] = f1 < g1; out[2] = f2 < g2; out[3] = f3 < g3;
        out[4] = f4 < g4; out[5] = f5 < g5; out[6] = f6 < g6; out[7] = f7 < g7;
    }
    return (w+5);
}

t_int *scalarsmaller_perform(t_int *w)
{
    t_sample *in = (t_sample *)(w[1]);
    t_float f = *(t_float *)(w[2]);
    t_sample *out = (t_sample *)(w[3]);
    int n = (int)(w[4]);
    while (n--) *out++ = *in++ < f; 
    return (w+5);
}

t_int *scalarsmaller_perf8(t_int *w)
{
    t_sample *in = (t_sample *)(w[1]);
    t_float g = *(t_float *)(w[2]);
    t_sample *out = (t_sample *)(w[3]);
    int n = (int)(w[4]);
    for (; n; n -= 8, in += 8, out += 8)
    {
        t_sample f0 = in[0], f1 = in[1], f2 = in[2], f3 = in[3];
        t_sample f4 = in[4], f5 = in[5], f6 = in[6], f7 = in[7];

        out[0] = f0 < g; out[1] = f1 < g; out[2] = f2 < g; out[3] = f3 < g;
        out[4] = f4 < g; out[5] = f5 < g; out[6] = f6 < g; out[7] = f7 < g;
    }
    return (w+5);
}

void dsp_add_smaller(t_sample *in1, t_sample *in2, t_sample *out, int n)
{
    if (n&7)
        dsp_add(smaller_perform, 4, in1, in2, out, n);
    else        
        dsp_add(smaller_perf8, 4, in1, in2, out, n);
}

static void smaller_dsp(t_smaller *x, t_signal **sp)
{
    dsp_add_smaller(sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n);
}

static void scalarsmaller_dsp(t_scalarsmaller *x, t_signal **sp)
{
    if (sp[0]->s_n&7)
        dsp_add(scalarsmaller_perform, 4, sp[0]->s_vec, &x->x_g,
            sp[1]->s_vec, sp[0]->s_n);
    else        
        dsp_add(scalarsmaller_perf8, 4, sp[0]->s_vec, &x->x_g,
            sp[1]->s_vec, sp[0]->s_n);
}

void smaller_tilde_setup(void)
{
    smaller_class = class_new(gensym("smaller~"), (t_newmethod)smaller_new, 0, sizeof(t_smaller), 0, A_GIMME, 0);

    class_addmethod(smaller_class, (t_method)smaller_dsp, gensym("dsp"), 0);
    CLASS_MAINSIGNALIN(smaller_class, t_smaller, x_f);
    class_sethelpsymbol(smaller_class, gensym("sigbinops"));
    scalarsmaller_class = class_new(gensym("smaller~"), 0, 0, sizeof(t_scalarsmaller), 0, 0);
    CLASS_MAINSIGNALIN(scalarsmaller_class, t_scalarsmaller, x_f);
    class_addmethod(scalarsmaller_class, (t_method)scalarsmaller_dsp, gensym("dsp"),
        0);
    class_sethelpsymbol(scalarsmaller_class, gensym("sigbinops"));
}
