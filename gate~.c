/* code by Chikashi Miyama 2009 */

#include "m_pd.h"
#ifdef NT
#pragma warning( disable : 4244 )
#pragma warning( disable : 4305 )
#endif

/* ----------------------------- gate ----------------------------- */
static t_class *gate_class;

typedef struct _gate
{
    t_object x_obj;
    t_int x_i;
} t_gate;

static void *gate_new(t_symbol *s, int argc, t_atom *argv)
{
	int i;
	t_gate *x = (t_gate *)pd_new(gate_class);

    if (argc > 1) post("gate~: extra arguments ignored");
	else if(argc == 0){
		x->x_i = 1;
	}
	else{
		x->x_i = atom_getintarg(0, argc, argv);
	}
	
	inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
	for(i = 0;i < x->x_i; i++){
		outlet_new(&x->x_obj, &s_signal);
	}
	return (x);
}

t_int *gate_perform(t_int *w)
{
	int numout, index, i,j,  n ;
	t_sample *out;
	t_gate *x = (t_gate *)(w[1]);
    n = (int)(w[2]);
    t_sample *in1 = (t_sample *)(w[3]);
    t_sample *in2 = (t_sample *)(w[4]);
	numout = x->x_i;
	
	for(i = 0; i< n; i++) // vector one by one
	{
		index = (t_int)(*(in1+i));
		if(index > numout) index = numout;
		else if(index < 0 ) index = 0;	
		index--;
		for(j = numout-1; j >= 0 ; j--) // outlet one by one
		{
			out = (t_sample*)(w[5+j]);
			if(j == index)
				*(out+i) = *(in2+i);	
			else
				*(out+i) = 0.0f;
		}
	}
	return (w+5+numout);
}


static void gate_dsp(t_gate *x, t_signal **sp)
{
	// number of outlets plus 4 (t_gate, vector size, leftinlet, rightinlet)
	int numinout = x->x_i + 4;
	switch(x->x_i)
	{
		case 1:
			dsp_add(gate_perform, numinout, x, sp[0]->s_n, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec);
			break;
		case 2:
			dsp_add(gate_perform, numinout, x, sp[0]->s_n, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec);
			break;		
		case 3:
			dsp_add(gate_perform, numinout, x, sp[0]->s_n, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec);
			break;				
		case 4:
			dsp_add(gate_perform, numinout, x, sp[0]->s_n, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[5]->s_vec);
			break;			
		case 5:
			dsp_add(gate_perform, numinout, x, sp[0]->s_n, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[5]->s_vec,sp[6]->s_vec);
			break;			
		case 6:
			dsp_add(gate_perform, numinout, x, sp[0]->s_n, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[5]->s_vec,sp[6]->s_vec,sp[7]->s_vec);
			break;			
		case 7:
			dsp_add(gate_perform, numinout, x, sp[0]->s_n, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[5]->s_vec, sp[6]->s_vec,sp[7]->s_vec,sp[8]->s_vec);
			break;
		case 8:
			dsp_add(gate_perform, numinout, x, sp[0]->s_n, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[5]->s_vec, sp[6]->s_vec, sp[7]->s_vec,sp[8]->s_vec,sp[9]->s_vec);
			break;			
		default:
			dsp_add(gate_perform, numinout, x, sp[0]->s_n, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[5]->s_vec, sp[6]->s_vec, sp[7]->s_vec,sp[8]->s_vec,sp[9]->s_vec);
			break;				

	}
}


void gate_tilde_setup(void)
{
    gate_class = class_new(gensym("gate~"), (t_newmethod)gate_new, 0, sizeof(t_gate), 0, A_GIMME, 0);
    class_addmethod(gate_class, (t_method)gate_dsp, gensym("dsp"), 0);
    CLASS_MAINSIGNALIN(gate_class, t_gate, x_i);
    class_sethelpsymbol(gate_class, gensym("sigbinops"));

}
