/* code by Chikashi Miyama 2009 */

#include "m_pd.h"
#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

/* ----------------------------- equal ----------------------------- */
static t_class *equal_class, *scalarequal_class;

typedef struct _equal
{
    t_object x_obj;
    t_float x_f;
} t_equal;

typedef struct _scalarequal
{
    t_object x_obj;
    t_float x_f;
    t_float x_g;            /* inlet value */
} t_scalarequal;

static void *equal_new(t_symbol *s, int argc, t_atom *argv)
{
    if (argc > 1) post("equal~: extra arguments ignored");
    if (argc) // compared with arg 1
    {
        t_scalarequal *x = (t_scalarequal *)pd_new(scalarequal_class);
        floatinlet_new(&x->x_obj, &x->x_g);
        x->x_g = atom_getfloatarg(0, argc, argv);
        outlet_new(&x->x_obj, &s_signal);
        x->x_f = 0;
        return (x);
    }
    else
    {
        t_equal *x = (t_equal *)pd_new(equal_class);
        inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
        outlet_new(&x->x_obj, &s_signal);
        x->x_f = 0;
        return (x);
    }
}

t_int *equal_perform(t_int *w)
{
    t_sample *in1 = (t_sample *)(w[1]);
    t_sample *in2 = (t_sample *)(w[2]);
    t_sample *out = (t_sample *)(w[3]);
    int n = (int)(w[4]);
    while (n--) *out++ = *in1++ == *in2++; 
    return (w+5);
}

t_int *equal_perf8(t_int *w)
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

        out[0] = f0 == g0; out[1] = f1 == g1; out[2] = f2 == g2; out[3] = f3 == g3;
        out[4] = f4 == g4; out[5] = f5 == g5; out[6] = f6 == g6; out[7] = f7 == g7;
    }
    return (w+5);
}

t_int *scalarequal_perform(t_int *w)
{
    t_sample *in = (t_sample *)(w[1]);
    t_float f = *(t_float *)(w[2]);
    t_sample *out = (t_sample *)(w[3]);
    int n = (int)(w[4]);
    while (n--) *out++ = *in++ == f; 
    return (w+5);
}

t_int *scalarequal_perf8(t_int *w)
{
    t_sample *in = (t_sample *)(w[1]);
    t_float g = *(t_float *)(w[2]);
    t_sample *out = (t_sample *)(w[3]);
    int n = (int)(w[4]);
    for (; n; n -= 8, in += 8, out += 8)
    {
        t_sample f0 = in[0], f1 = in[1], f2 = in[2], f3 = in[3];
        t_sample f4 = in[4], f5 = in[5], f6 = in[6], f7 = in[7];

        out[0] = f0 == g; out[1] = f1 == g; out[2] = f2 == g; out[3] = f3 == g;
        out[4] = f4 == g; out[5] = f5 == g; out[6] = f6 == g; out[7] = f7 == g;
    }
    return (w+5);
}

void dsp_add_equal(t_sample *in1, t_sample *in2, t_sample *out, int n)
{
    if (n&7)
        dsp_add(equal_perform, 4, in1, in2, out, n);
    else        
        dsp_add(equal_perf8, 4, in1, in2, out, n);
}

static void equal_dsp(t_equal *x, t_signal **sp)
{
    dsp_add_equal(sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n);
}

static void scalarequal_dsp(t_scalarequal *x, t_signal **sp)
{
    if (sp[0]->s_n&7)
        dsp_add(scalarequal_perform, 4, sp[0]->s_vec, &x->x_g,
            sp[1]->s_vec, sp[0]->s_n);
    else        
        dsp_add(scalarequal_perf8, 4, sp[0]->s_vec, &x->x_g,
            sp[1]->s_vec, sp[0]->s_n);
}

void equal_tilde_setup(void)
{
    equal_class = class_new(gensym("equal~"), (t_newmethod)equal_new, 0, sizeof(t_equal), 0, A_GIMME, 0);

    class_addmethod(equal_class, (t_method)equal_dsp, gensym("dsp"), 0);
    CLASS_MAINSIGNALIN(equal_class, t_equal, x_f);
    class_sethelpsymbol(equal_class, gensym("sigbinops"));
    scalarequal_class = class_new(gensym("equal~"), 0, 0, sizeof(t_scalarequal), 0, 0);
    CLASS_MAINSIGNALIN(scalarequal_class, t_scalarequal, x_f);
    class_addmethod(scalarequal_class, (t_method)scalarequal_dsp, gensym("dsp"),
        0);
    class_sethelpsymbol(scalarequal_class, gensym("sigbinops"));
}
