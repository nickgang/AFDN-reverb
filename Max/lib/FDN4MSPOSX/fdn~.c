/*
 *   fdn.c  - a feedback delay network (reverb tail)
 *   using a housholder reflection feedback matrix (In - 2/n 11T)
 *   Copyright (c) 2000-2003 by Tom Schouten
 *   (translated to msp by raja)
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "ext.h"
#include "ext_obex.h"
#include "z_dsp.h"

#define FDN_MIN_DECAY_TIME .01

static t_class *fdn_class = NULL;

typedef struct fdnctl
{
    t_int c_order;      /* veelvoud van 4 */
    t_int c_maxorder;
    t_double c_leak;
    t_double *c_buf;
    t_double *c_gain_in;
    t_double *c_gain_state;
    t_double c_timehigh;
    t_double c_timelow;
    t_int *c_tap;       /* cirular feed: N+1 pointers: 1 read, (N-1)r/w, 1 write */
    t_double *c_length;  /* delay lengths in ms */
    t_int c_bufsize;
    t_double c_fsample;
    t_double *c_vector[2];
    t_double *c_vectorbuffer;
    t_int c_curvector;
} t_fdnctl;

typedef struct _fdn {
    t_pxobject	x_obj;
    
    t_fdnctl x_ctl;
} t_fdn;

void *fdn_new(t_symbol *s, long argc, t_atom *argv);
void fdn_free(t_fdn *x);
void fdn_order(t_fdn *x, t_int order);
void fdn_print(t_fdn *x);
void fdn_reset(t_fdn *x);
void fdn_setupdeline(t_fdn *x);
void fdn_time(t_fdn *x, t_double lo, t_double hi);
void fdn_timehi(t_fdn *x, t_double hi);
void fdn_timelo(t_fdn *x, t_double lo);
void fdn_lines(t_fdn *x, t_symbol *s, int argc, t_atom *argv);
void fdn_lin(t_fdn *x, t_double forder, t_double min, t_double max);
void fdn_exp(t_fdn *x, t_double forder, t_double min, t_double max);
void fdn_dsp64(t_fdn *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags);
void fdn_perform64(t_fdn *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long veccount, long flags, void *userparam);
void fdn_assist(t_fdn *x, void *b, long m, long a, char *s);


int C74_EXPORT main(void)
{
    t_class *c = class_new("fdn~", (method)fdn_new, (method)fdn_free, sizeof(t_fdn), NULL, A_GIMME, 0);
    
    class_addmethod(c, (method)fdn_dsp64, "dsp64", A_CANT, 0); 	// respond to the dsp message
    class_addmethod(c, (method)fdn_print, "print", A_NOTHING, 0);
    class_addmethod(c, (method)fdn_reset, "reset", A_NOTHING, 0);
    class_addmethod(c, (method)fdn_timehi, "hi", A_FLOAT, 0);
    class_addmethod(c, (method)fdn_timelo, "lo", A_FLOAT, 0);
    class_addmethod(c, (method)fdn_lines, "lines", A_GIMME, 0);
    class_addmethod(c, (method)fdn_lin, "lin", A_FLOAT, A_FLOAT, A_FLOAT, 0);
    class_addmethod(c, (method)fdn_exp, "exp", A_FLOAT, A_FLOAT, A_FLOAT, 0);
    class_addmethod(c, (method)fdn_assist, "assist", A_CANT, 0);
    class_dspinit(c);												// must call this function for MSP object classes

	class_register(CLASS_BOX, c);
    
    fdn_class = c;
    
    post("fdn~ - ported from PD(zwizwa.be/darcs/creb/modules/fdn~.c) by raja");
    
	return 0;
}


void *fdn_new(t_symbol *s, long argc, t_atom *argv)
{
    t_int order;
    t_float maxbufsz;
    t_int bufround;
    t_int bufsize;
    t_double scale;
    
    atom_arg_getlong(&order, 0, argc, argv);
    atom_arg_getfloat(&maxbufsz, 1, argc, argv);
    
    scale = sys_getsr() * .001f;
    bufsize = (t_int)(scale * maxbufsz);
    
    t_fdn *x = object_alloc(fdn_class);
    
    dsp_setup((t_pxobject *)x, 1);					// set up DSP for the instance and create signal inlets
    
    outlet_new((t_object *)x, "signal");
    outlet_new((t_object *)x, "signal");
    //x->x_obj.z_misc |= Z_NO_INPLACE;
    
    /* init data */
    if (order < 4) order = 8;
    post("%d", order);
    post("%d", bufsize);
    if (bufsize < 64) bufsize = 65536;
    
    bufround = 1;
    while (bufround < bufsize) bufround *= 2;
    bufsize = bufround;
    
    post("fdn~ - maximum # of delay lines %d, total buffer"
    "size %d samples (%f secs)", order, bufsize, ((t_double)bufsize)/sys_getsr());
    
    x->x_ctl.c_maxorder = order;
    x->x_ctl.c_buf = (t_double *) sysmem_newptrclear(sizeof(t_double) * bufsize);
    x->x_ctl.c_bufsize = bufsize;
    x->x_ctl.c_fsample = sys_getsr();
    x->x_ctl.c_tap = (t_int *) sysmem_newptrclear((order + 1) * sizeof(t_int));
    x->x_ctl.c_length = (t_double *) sysmem_newptrclear(order * sizeof(t_double));
    x->x_ctl.c_gain_in = (t_double *) sysmem_newptrclear(order * sizeof(t_double));
    x->x_ctl.c_gain_state = (t_double *) sysmem_newptrclear(order * sizeof(t_double));
    x->x_ctl.c_vectorbuffer = (t_double *) sysmem_newptrclear(order * 2 * sizeof(t_double));
    
    x->x_ctl.c_curvector = 0;
    x->x_ctl.c_vector[0] = &x->x_ctl.c_vectorbuffer[0];
    x->x_ctl.c_vector[1] = &x->x_ctl.c_vectorbuffer[order];
    
    /* preset */
    fdn_order(x, 8);
    x->x_ctl.c_length[0] = 29.0f;
    x->x_ctl.c_length[1] = 31.0f;
    x->x_ctl.c_length[2] = 37.0f;
    x->x_ctl.c_length[3] = 67.0f;
    x->x_ctl.c_length[4] = 82.0f;
    x->x_ctl.c_length[5] = 110.0f;
    x->x_ctl.c_length[6] = 172.0f;
    x->x_ctl.c_length[7] = 211.0f;
    
    fdn_setupdeline(x);
    fdn_time(x, 4, 1);
    
    /* reset delay memory to zero*/
    fdn_reset(x);
    
    return x;
}

void fdn_free(t_fdn *x)
{
    dsp_free((t_pxobject *) x);
    if (x->x_ctl.c_tap) sysmem_freeptr(x->x_ctl.c_tap);
    if (x->x_ctl.c_length) sysmem_freeptr(x->x_ctl.c_length);
    if (x->x_ctl.c_gain_in) sysmem_freeptr(x->x_ctl.c_gain_in);
    if (x->x_ctl.c_gain_state) sysmem_freeptr(x->x_ctl.c_gain_state);
    if (x->x_ctl.c_buf) sysmem_freeptr(x->x_ctl.c_buf);
    if (x->x_ctl.c_vectorbuffer) sysmem_freeptr(x->x_ctl.c_vectorbuffer);
}


void fdn_assist(t_fdn *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_INLET) {
		switch (a) {	
			case 0:
				sprintf(s,"Input Signal And Messages");
				break;
		}
    }
        else
        {
            switch (a) {
                case 0:
                    sprintf(s,"Out Left");
                    break;
                case 1:
                    sprintf(s,"Out Right");
                    break;
            }
        }
}

void fdn_order(t_fdn *x, t_int order)
{
    if (order > x->x_ctl.c_maxorder)
    {
        x->x_ctl.c_order = x->x_ctl.c_maxorder;
        x->x_ctl.c_leak = -2. / x->x_ctl.c_maxorder;
        post("fdn~ - order %d > maxorder(%d) ...set to maxorder", order, x->x_ctl.c_maxorder);
    }
    else
    {
    x->x_ctl.c_order = order;
    x->x_ctl.c_leak = -2. / order;
    }
}

void fdn_print(t_fdn *x)
{
    short i;
    post("fdn~ - maxorder: %d - order: %d - lo: %f - hi: %f - delay coefficients:",
         x->x_ctl.c_maxorder, x->x_ctl.c_order, x->x_ctl.c_timelow, x->x_ctl.c_timehigh);
    for (i=0; i < x->x_ctl.c_order; i++)
    {
        post("%f", x->x_ctl.c_length[i]);
    }
}

void fdn_reset(t_fdn *x)
{
    int i;
    if (x->x_ctl.c_buf)
    {
        for (i=0; i<x->x_ctl.c_bufsize; i++)
        {
            x->x_ctl.c_buf[i] = 0.0;
        }
    }
    if (x->x_ctl.c_vectorbuffer)
    {
        for (i=0; i<(x->x_ctl.c_maxorder * 2); i++)
        {
        x->x_ctl.c_vectorbuffer[i] = 0;
        }
    }
}

/* each delay line is filtered with a first order IIR filter:
 (gl: dc gain, gh: ny gain)
 
 H(z) = 2 gl gh / (gl + gh - z^-1 (gl - gh))
 
 this results in the difference equation:
 
 yk = (2 gl gh) / (gl + gh) x + (gl - gh) / (gl + gh) yk-1 
 */

void fdn_time(t_fdn *x, t_double lo, t_double hi)
{
    int i;
    t_double elow, ehigh;
    t_double gainlow, gainhigh, gainscale;
    
    if (lo < FDN_MIN_DECAY_TIME) lo = FDN_MIN_DECAY_TIME;
    if (hi < FDN_MIN_DECAY_TIME) hi = FDN_MIN_DECAY_TIME;
    
    elow = -.003 / (lo);
    ehigh = -.003 / (hi);
    
    /* setup gains */
    for (i=0; i < x->x_ctl.c_order; i++)
    {
        gainlow = pow(10, elow * (x->x_ctl.c_length[i]));
        gainhigh = pow(10, ehigh * (x->x_ctl.c_length[i]));
        gainscale = 1.0f / (gainlow + gainhigh);
        x->x_ctl.c_gain_in[i] = 2.0f * gainlow * gainhigh * gainscale;
        x->x_ctl.c_gain_state[i] = (gainlow - gainhigh) * gainscale;
    }
    x->x_ctl.c_timehigh = hi;
    x->x_ctl.c_timelow = lo;
}

void fdn_timehi(t_fdn *x, t_double hi)
{
    x->x_ctl.c_timehigh = fabs(hi);
    fdn_time(x, x->x_ctl.c_timelow, x->x_ctl.c_timehigh);
}

void fdn_timelo(t_fdn *x, t_double lo)
{
    x->x_ctl.c_timelow = fabs(lo);
    fdn_time(x, x->x_ctl.c_timelow, x->x_ctl.c_timehigh);
}

void fdn_setupdeline(t_fdn *x)
{
    int t, sum;
    int mask = x->x_ctl.c_bufsize - 1;
    int start = x->x_ctl.c_tap[0];
    t_int *tap = x->x_ctl.c_tap;
    t_double *length = x->x_ctl.c_length;
    t_double scale = sys_getsr() * .001f;
    
    sum = 0;
    tap[0] = (start & mask);
    for (t=1; t <= x->x_ctl.c_order; t++)
    {
        sum += CLAMP( (int)(length[t-1] * scale), 0, mask-sum);
        tap[t]=(start+sum)&mask;
    }
    if (sum > mask){
        post("fdn: warning: not enough delay memory, behaviour "
             "is undefined (this could lead to instability...)");
        
    }
}

void fdn_lines(t_fdn *x, t_symbol *s, int argc, t_atom *argv)
{
    int i;
    int order = argc & 0xfffffffc;
    
    if (order < 4) return;
    if (order > x->x_ctl.c_maxorder) return;
    
    fdn_order(x, order);
    for (i=0; i<order; i++)
        if (argv[i].a_type == A_FLOAT) x->x_ctl.c_length[i] = argv[i].a_w.w_float;
    
    fdn_setupdeline(x);
    fdn_time(x, x->x_ctl.c_timelow, x->x_ctl.c_timehigh);
}

void fdn_lin(t_fdn *x, t_double forder, t_double min, t_double max)
{
	t_int order = ((int)forder) & 0xfffffffc;
    t_double length, inc;
    int i;
    
    if (order < 4) return;
    if (order > x->x_ctl.c_maxorder) return;
    if (min <= 0) return;
    if (max <= 0) return;
    
    inc = (max - min) / (t_double)(order-1);
    length = min;
    
    for (i=0; i<order; i++)
    {
        x->x_ctl.c_length[i] = length;
        length += inc;
    }
    
    fdn_order(x, order);
    fdn_setupdeline(x);
    fdn_time(x, x->x_ctl.c_timelow, x->x_ctl.c_timehigh);
}


void fdn_exp(t_fdn *x, t_double forder, t_double min, t_double max)
{
	t_int order = ((int)forder) & 0xfffffffc;
    t_double length, inc;
    int i;
    
    if (order < 4) return;
    if (order > x->x_ctl.c_maxorder) return;
    if (min <= 0) return;
    if (max <= 0) return;
    
    inc = pow( max/min, 1.0f/((t_double)(order-1)));
    length = min;
    
    for (i=0; i<order; i++)
    {
        x->x_ctl.c_length[i] = length;
        length *= inc;
    }
    
    fdn_order(x, order);
    fdn_setupdeline(x);
    fdn_time(x, x->x_ctl.c_timelow, x->x_ctl.c_timehigh);
}


void fdn_perform64(t_fdn *x, t_object *dsp64, double **ins, long numins, double **outs, long numouts, long veccount, long flags, void *userparam)
{
	t_double *in = ins[0];
	t_double *outl = outs[0];
    t_double *outr = outs[1];
    int n = veccount;
    t_fdnctl *ctl = &x->x_ctl;
    t_double *gain_in = ctl->c_gain_in;
    t_double *gain_state = ctl->c_gain_state;
    t_int order = ctl->c_order;
    t_int *tap = ctl->c_tap;
    t_double *buf = ctl->c_buf;
    t_int mask = ctl->c_bufsize - 1;
    t_double leak = ctl->c_leak;
    t_int curvctr = ctl->c_curvector;
    
    t_int i,j;
    t_double insamp, trickle, left, right, readsamp;
    
    t_double *cvec, *lvec;
    t_double save;
    
	for (i=0; i<n; i++)
    {
        insamp = *in++;
        trickle = 0;
        left = 0;
        right = 0;
        
        /* get temporary vector buffers */
        cvec = ctl->c_vector[curvctr];
        lvec = ctl->c_vector[curvctr ^ 1];
        curvctr ^= 1;
        
        /* read input vector + get sum and left/right output */
        for (j=0; j<order;)
        {
            readsamp = buf[tap[j]];
            cvec[j] = readsamp;
            trickle += readsamp;
            left += readsamp;
            right += readsamp;
            j++;
            
            readsamp = buf[tap[j]];
            cvec[j] = readsamp;
            trickle += readsamp;
            left -= readsamp;
            right += readsamp;
            j++;
            
            readsamp = buf[tap[j]];
            cvec[j] = readsamp;
            trickle += readsamp;
            left += readsamp;
            right -= readsamp;
            j++;
            
            readsamp = buf[tap[j]];
            cvec[j] = readsamp;
            trickle += readsamp;
            left -= readsamp;
            right -= readsamp;
            j++;
        }
        
        /* write output */
        *outl++ = left;
        *outr++ = right;
        
        /* trickle == leak to all inputs */
        trickle *= leak;
        
        /* perform feedback (todo: decouple feedback & permutation) */
        save = cvec[0];
        for (j=0; j<order-1; j++)
        {
            cvec[j] = cvec[j+1] + trickle + insamp;
        }
        cvec[order-1] = save + trickle + insamp;
        
        /* apply gain + store result vector in delay lines + increment taps */
        tap[0] = (tap[0]+1)&mask;
        for(j=0; j<order; j++)
        {
            save = gain_in[j] * cvec[j] + gain_state[j] * lvec[j];
            save = IS_DENORM_DOUBLE(save) ? 0 : save;
            cvec[j] = save;
            buf[tap[j+1]] = save;
            tap[j+1] = (tap[j+1] + 1) & mask;
        }
    }
}		


// method called when dsp is turned on
void fdn_dsp64(t_fdn *x, t_object *dsp64, short *count, double samplerate, long maxvectorsize, long flags)
{
    object_method(dsp64, gensym("dsp_add64"), x, fdn_perform64, 0, NULL);
}

