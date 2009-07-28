/* -*- coding: utf-8; -*- 
 * Distributed under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */

#include <glib.h>
#include <oiltest.h>

void test_param ()
{
    static gchar *prototypes[] = {
        "uint8_t *d_8x8, int ds, uint8_t *s_8x8, int ss",
        "int16_t *d_8x8, uint8_t *s1_8x8, int ss1, uint8_t *s2_8x8, int ss2, uint8_t *s3_8x8, int ss3",
        "uint8_t *d_4xn, uint8_t* s_3xn, int n",
        "uint32_t *i_n, uint32_t *s1_n, int n",
        "int16_t *i1_6xn, int is1, int16_t *s1_6xn, int ss1, uint8_t *s2_6xn, int ss2, int n",
        "double *d, double *s, int n",
        "int16_t *d, int16_t *s1, int16_t *s2, int16_t *s3, int16_t *s4_1, int n",
        "uint32_t *d_n, const uint8_t *s1_12x12, int ss1, uint8_t *s2_12xnp11, int ss2, int n",
        "uint8_t *d, uint8_t const *s1_np7, int16_t *s2_8, int16_t *s3_2, int n",
    };
    
    /* uint8_t *d_8x8, int ds, uint8_t *s_8x8, int ss */
    static OilParameter p1[] = {
        {
            .parameter = "uint8_t *d_8x8",
            .c_type = OIL_TYPE_u8p,
            .arg_class = OIL_ARG_T_DEST,
            .arg_type = OIL_ARG_DEST1,
            
            .is_pointer = TRUE,
            .is_stride = FALSE,
            
            .index = 1,
            .prestride_var = 0,
            .poststride_var = 0,
            .prestride_length = 8,
            .poststride_length = 8,
        },
        {
            .parameter = "int ds",
            .c_type = OIL_TYPE_INT,
            .arg_class = OIL_ARG_T_DEST,
            .arg_type = OIL_ARG_DSTR1,
            
            .is_pointer = FALSE,
            .is_stride = TRUE,
            
            .index = 1,
        },
        {
            .parameter = "uint8_t *s_8x8",
            .c_type = OIL_TYPE_u8p,
            .arg_class = OIL_ARG_T_SRC,
            .arg_type = OIL_ARG_SRC1,
            
            .is_pointer = TRUE,
            .is_stride = FALSE,
            
            .index = 1,
            .prestride_var = 0,
            .poststride_var = 0,
            .prestride_length = 8,
            .poststride_length = 8,
        },
        {
            .parameter = "int ss",
            .c_type = OIL_TYPE_INT,
            .arg_class = OIL_ARG_T_SRC,
            .arg_type = OIL_ARG_SSTR1,
            
            .is_pointer = FALSE,
            .is_stride = TRUE,
            
            .index = 1,
        },
    };

    /* int16_t *d_8x8, uint8_t *s1_8x8, int ss1, uint8_t *s2_8x8, int ss2, uint8_t *s3_8x8, int ss3 */
    static OilParameter p2[] = {
        {
            .parameter = "int16_t *d_8x8",
            .c_type = OIL_TYPE_s16p,
            .arg_class = OIL_ARG_T_DEST,
            .arg_type = OIL_ARG_DEST1,
            
            .is_pointer = TRUE,
            .is_stride = FALSE,
            
            .index = 1,
            .prestride_var = 0,
            .poststride_var = 0,
            .prestride_length = 8,
            .poststride_length = 8,
        },
        {
            .parameter = "uint8_t *s1_8x8",
            .c_type = OIL_TYPE_u8p,
            .arg_class = OIL_ARG_T_SRC,
            .arg_type = OIL_ARG_SRC1,
            
            .is_pointer = TRUE,
            .is_stride = FALSE,
            
            .index = 1,
            .prestride_var = 0,
            .poststride_var = 0,
            .prestride_length = 8,
            .poststride_length = 8,
        },
        {
            .parameter = "int ss1",
            .c_type = OIL_TYPE_INT,
            .arg_class = OIL_ARG_T_SRC,
            .arg_type = OIL_ARG_SSTR1,
            
            .is_pointer = FALSE,
            .is_stride = TRUE,
            
            .index = 1,
        },
        {
            .parameter = "uint8_t *s2_8x8",
            .c_type = OIL_TYPE_u8p,
            .arg_class = OIL_ARG_T_SRC,
            .arg_type = OIL_ARG_SRC2,
            
            .is_pointer = TRUE,
            .is_stride = FALSE,
            
            .index = 2,
            .prestride_var = 0,
            .poststride_var = 0,
            .prestride_length = 8,
            .poststride_length = 8,
        },
        {
            .parameter = "int ss2",
            .c_type = OIL_TYPE_INT,
            .arg_class = OIL_ARG_T_SRC,
            .arg_type = OIL_ARG_SSTR2,
            
            .is_pointer = FALSE,
            .is_stride = TRUE,
            
            .index = 2,
        },
        {
            .parameter = "uint8_t *s3_8x8",
            .c_type = OIL_TYPE_u8p,
            .arg_class = OIL_ARG_T_SRC,
            .arg_type = OIL_ARG_SRC3,
            
            .is_pointer = TRUE,
            .is_stride = FALSE,
            
            .index = 3,
            .prestride_var = 0,
            .poststride_var = 0,
            .prestride_length = 8,
            .poststride_length = 8,
        },
        {
            .parameter = "int ss3",
            .c_type = OIL_TYPE_INT,
            .arg_class = OIL_ARG_T_SRC,
            .arg_type = OIL_ARG_SSTR3,
            
            .is_pointer = FALSE,
            .is_stride = TRUE,
            
            .index = 3,
        },
    };
    
    /* uint8_t *d_4xn, uint8_t* s_3xn, int n */
    static OilParameter p3[] = {
        {
            .parameter = "uint8_t *d_4xn",
            .c_type = OIL_TYPE_u8p,
            .arg_class = OIL_ARG_T_DEST,
            .arg_type = OIL_ARG_DEST1,
            
            .is_pointer = TRUE,
            .is_stride = FALSE,
            
            .index = 1,
            .prestride_var = 0,
            .poststride_var = 1,
            .prestride_length = 4,
            .poststride_length = 0,
        },
        {
            .parameter = "uint8_t* s_3xn",
            .c_type = OIL_TYPE_u8p,
            .arg_class = OIL_ARG_T_SRC,
            .arg_type = OIL_ARG_SRC1,
            
            .is_pointer = TRUE,
            .is_stride = FALSE,
            
            .index = 1,
            .prestride_var = 0,
            .poststride_var = 1,
            .prestride_length = 3,
            .poststride_length = 0,
        },
        {
            .parameter = "int n",
            .c_type = OIL_TYPE_INT,
            .arg_class = OIL_ARG_T_SCALE,
            .arg_type = OIL_ARG_N,
            
            .is_pointer = FALSE,
            .is_stride = FALSE,
        },
    };
    
    /* uint32_t *i_n, uint32_t *s1_n, int n */
    static OilParameter p4[] = {
        {
            .parameter = "uint32_t *i_n",
            .c_type = OIL_TYPE_u32p,
            .arg_class = OIL_ARG_T_INPLACE,
            .arg_type = OIL_ARG_INPLACE1,
            
            .is_pointer = TRUE,
            .is_stride = FALSE,
            
            .index = 1,
            .prestride_var = 0,
            .poststride_var = 1,
            .prestride_length = 1,
            .poststride_length = 0,
        },
        {
            .parameter = "uint32_t *s1_n",
            .c_type = OIL_TYPE_u32p,
            .arg_class = OIL_ARG_T_SRC,
            .arg_type = OIL_ARG_SRC1,
            
            .is_pointer = TRUE,
            .is_stride = FALSE,
            
            .index = 1,
            .prestride_var = 0,
            .poststride_var = 1,
            .prestride_length = 1,
            .poststride_length = 0,
        },
        {
            .parameter = "int n",
            .c_type = OIL_TYPE_INT,
            .arg_class = OIL_ARG_T_SCALE,
            .arg_type = OIL_ARG_N,
            
            .is_pointer = FALSE,
            .is_stride = FALSE,
        },
    };
    
    /* int16_t *i1_6xn, int is1, int16_t *s1_6xn, int ss1, uint8_t *s2_6xn, int ss2, int n */
    static OilParameter p5[] = {
        {
            .parameter = "int16_t *i1_6xn",
            .c_type = OIL_TYPE_s16p,
            .arg_class = OIL_ARG_T_INPLACE,
            .arg_type = OIL_ARG_INPLACE1,
            
            .is_pointer = TRUE,
            .is_stride = FALSE,
            
            .index = 1,
            .prestride_var = 0,
            .poststride_var = 1,
            .prestride_length = 6,
            .poststride_length = 0,
        },
        {
            .parameter = "int is1",
            .c_type = OIL_TYPE_INT,
            .arg_class = OIL_ARG_T_INPLACE,
            .arg_type = OIL_ARG_ISTR1,
            
            .is_pointer = FALSE,
            .is_stride = TRUE,
            
            .index = 1,
        },
        {
            .parameter = "int16_t *s1_6xn",
            .c_type = OIL_TYPE_s16p,
            .arg_class = OIL_ARG_T_SRC,
            .arg_type = OIL_ARG_SRC1,
            
            .is_pointer = TRUE,
            .is_stride = FALSE,
            
            .index = 1,
            .prestride_var = 0,
            .poststride_var = 1,
            .prestride_length = 6,
            .poststride_length = 0,
        },
        {
            .parameter = "int ss1",
            .c_type = OIL_TYPE_INT,
            .arg_class = OIL_ARG_T_SRC,
            .arg_type = OIL_ARG_SSTR1,
            
            .is_pointer = FALSE,
            .is_stride = TRUE,
            
            .index = 1,
        },
        {
            .parameter = "uint8_t *s2_6xn",
            .c_type = OIL_TYPE_u8p,
            .arg_class = OIL_ARG_T_SRC,
            .arg_type = OIL_ARG_SRC2,
            
            .is_pointer = TRUE,
            .is_stride = FALSE,
            
            .index = 2,
            .prestride_var = 0,
            .poststride_var = 1,
            .prestride_length = 6,
            .poststride_length = 0,
        },
        {
            .parameter = "int ss2",
            .c_type = OIL_TYPE_INT,
            .arg_class = OIL_ARG_T_SRC,
            .arg_type = OIL_ARG_SSTR2,
            
            .is_pointer = FALSE,
            .is_stride = TRUE,
            
            .index = 2,
        },
        {
            .parameter = "int n",
            .c_type = OIL_TYPE_INT,
            .arg_class = OIL_ARG_T_SCALE,
            .arg_type = OIL_ARG_N,
            
            .is_pointer = FALSE,
            .is_stride = FALSE,
        },
    };
    
    /* double *d, double *s, int n */
    static OilParameter p6[] = {
        {
            .parameter = "double *d",
            .c_type = OIL_TYPE_f64p,
            .arg_class = OIL_ARG_T_DEST,
            .arg_type = OIL_ARG_DEST1,
            
            .is_pointer = TRUE,
            .is_stride = FALSE,
            
            .index = 1,
            .prestride_var = 0,
            .poststride_var = 1,
            .prestride_length = 1,
            .poststride_length = 0,
        },
        {
            .parameter = "double *s",
            .c_type = OIL_TYPE_f64p,
            .arg_class = OIL_ARG_T_SRC,
            .arg_type = OIL_ARG_SRC1,
            
            .is_pointer = TRUE,
            .is_stride = FALSE,
            
            .index = 1,
            .prestride_var = 0,
            .poststride_var = 1,
            .prestride_length = 1,
            .poststride_length = 0,
        },
        {
            .parameter = "int n",
            .c_type = OIL_TYPE_INT,
            .arg_class = OIL_ARG_T_SCALE,
            .arg_type = OIL_ARG_N,
            
            .is_pointer = FALSE,
            .is_stride = FALSE,
        },
    };
    
    /* int16_t *d, int16_t *s1, int16_t *s2, int16_t *s3, int16_t *s4_1, int n */
    static OilParameter p7[] = {
        {
            .parameter = "int16_t *d",
            .c_type = OIL_TYPE_s16p,
            .arg_class = OIL_ARG_T_DEST,
            .arg_type = OIL_ARG_DEST1,
            
            .is_pointer = TRUE,
            .is_stride = FALSE,
            
            .index = 1,
            .prestride_var = 0,
            .poststride_var = 1,
            .prestride_length = 1,
            .poststride_length = 0,
        },
        {
            .parameter = "int16_t *s1",
            .c_type = OIL_TYPE_s16p,
            .arg_class = OIL_ARG_T_SRC,
            .arg_type = OIL_ARG_SRC1,
            
            .is_pointer = TRUE,
            .is_stride = FALSE,
            
            .index = 1,
            .prestride_var = 0,
            .poststride_var = 1,
            .prestride_length = 1,
            .poststride_length = 0,
        },
        {
            .parameter = "int16_t *s2",
            .c_type = OIL_TYPE_s16p,
            .arg_class = OIL_ARG_T_SRC,
            .arg_type = OIL_ARG_SRC2,
            
            .is_pointer = TRUE,
            .is_stride = FALSE,
            
            .index = 2,
            .prestride_var = 0,
            .poststride_var = 1,
            .prestride_length = 1,
            .poststride_length = 0,
        },
        {
            .parameter = "int16_t *s3",
            .c_type = OIL_TYPE_s16p,
            .arg_class = OIL_ARG_T_SRC,
            .arg_type = OIL_ARG_SRC3,
            
            .is_pointer = TRUE,
            .is_stride = FALSE,
            
            .index = 3,
            .prestride_var = 0,
            .poststride_var = 1,
            .prestride_length = 1,
            .poststride_length = 0,
        },
        {
            .parameter = "int16_t *s4_1",
            .c_type = OIL_TYPE_s16p,
            .arg_class = OIL_ARG_T_SRC,
            .arg_type = OIL_ARG_SRC4,
            
            .is_pointer = TRUE,
            .is_stride = FALSE,
            
            .index = 4,
            .prestride_var = 0,
            .poststride_var = 0,
            .prestride_length = 1,
            .poststride_length = 1,
        },
        {
            .parameter = "int n",
            .c_type = OIL_TYPE_INT,
            .arg_class = OIL_ARG_T_SCALE,
            .arg_type = OIL_ARG_N,
            
            .is_pointer = FALSE,
            .is_stride = FALSE,
        },
    };
    
    /* uint32_t *d_n, const uint8_t *s1_12x12, int ss1, uint8_t *s2_12xnp11, int ss2, int n */
    static OilParameter p8[] = {
        {
            .parameter = "uint32_t *d_n",
            .c_type = OIL_TYPE_u32p,
            .arg_class = OIL_ARG_T_DEST,
            .arg_type = OIL_ARG_DEST1,
            
            .is_pointer = TRUE,
            .is_stride = FALSE,
            
            .index = 1,
            .prestride_var = 0,
            .poststride_var = 1,
            .prestride_length = 1,
            .poststride_length = 0,
        },
        {
            .parameter = "const uint8_t *s1_12x12",
            .c_type = OIL_TYPE_u8p,
            .arg_class = OIL_ARG_T_SRC,
            .arg_type = OIL_ARG_SRC1,
            
            .is_pointer = TRUE,
            .is_stride = FALSE,
            
            .index = 1,
            .prestride_var = 0,
            .poststride_var = 0,
            .prestride_length = 12,
            .poststride_length = 12,
        },
        {
            .parameter = "int ss1",
            .c_type = OIL_TYPE_INT,
            .arg_class = OIL_ARG_T_SRC,
            .arg_type = OIL_ARG_SSTR1,
            
            .is_pointer = FALSE,
            .is_stride = TRUE,
            
            .index = 1,
        },
        {
            .parameter = "uint8_t *s2_12xnp11",
            .c_type = OIL_TYPE_u8p,
            .arg_class = OIL_ARG_T_SRC,
            .arg_type = OIL_ARG_SRC2,
            
            .is_pointer = TRUE,
            .is_stride = FALSE,
            
            .index = 2,
            .prestride_var = 0,
            .poststride_var = 1,
            .prestride_length = 12,
            .poststride_length = 11,
        },
        {
            .parameter = "int ss2",
            .c_type = OIL_TYPE_INT,
            .arg_class = OIL_ARG_T_SRC,
            .arg_type = OIL_ARG_SSTR2,
            
            .is_pointer = FALSE,
            .is_stride = TRUE,
            
            .index = 2,
        },
        {
            .parameter = "int n",
            .c_type = OIL_TYPE_INT,
            .arg_class = OIL_ARG_T_SCALE,
            .arg_type = OIL_ARG_N,
            
            .is_pointer = FALSE,
            .is_stride = FALSE,
        },
    };
    
    /* uint8_t *d, uint8_t const *s1_np7, int16_t *s2_8, int16_t *s3_2, int n */
    static OilParameter p9[] = {
        {
            .parameter = "uint8_t *d",
            .c_type = OIL_TYPE_u8p,
            .arg_class = OIL_ARG_T_DEST,
            .arg_type = OIL_ARG_DEST1,
            
            .is_pointer = TRUE,
            .is_stride = FALSE,
            
            .index = 1,
            .prestride_var = 0,
            .poststride_var = 1,
            .prestride_length = 1,
            .poststride_length = 0,
        },
        {
            .parameter = "uint8_t const *s1_np7",
            .c_type = OIL_TYPE_u8p,
            .arg_class = OIL_ARG_T_SRC,
            .arg_type = OIL_ARG_SRC1,
            
            .is_pointer = TRUE,
            .is_stride = FALSE,
            
            .index = 1,
            .prestride_var = 0,
            .poststride_var = 1,
            .prestride_length = 1,
            .poststride_length = 7,
        },
        {
            .parameter = "int16_t *s2_8",
            .c_type = OIL_TYPE_s16p,
            .arg_class = OIL_ARG_T_SRC,
            .arg_type = OIL_ARG_SRC2,
            
            .is_pointer = TRUE,
            .is_stride = FALSE,
            
            .index = 2,
            .prestride_var = 0,
            .poststride_var = 0,
            .prestride_length = 1,
            .poststride_length = 8,
        },
        {
            .parameter = "int16_t *s3_2",
            .c_type = OIL_TYPE_s16p,
            .arg_class = OIL_ARG_T_SRC,
            .arg_type = OIL_ARG_SRC3,
            
            .is_pointer = TRUE,
            .is_stride = FALSE,
            
            .index = 3,
            .prestride_var = 0,
            .poststride_var = 0,
            .prestride_length = 1,
            .poststride_length = 2,
        },
        {
            .parameter = "int n",
            .c_type = OIL_TYPE_INT,
            .arg_class = OIL_ARG_T_SCALE,
            .arg_type = OIL_ARG_N,
            
            .is_pointer = FALSE,
            .is_stride = FALSE,
        },
    };
    
    static guint p_n_params[] = {
        sizeof (p1) / sizeof (OilParameter),
        sizeof (p2) / sizeof (OilParameter),
        sizeof (p3) / sizeof (OilParameter),
        sizeof (p4) / sizeof (OilParameter),
        sizeof (p5) / sizeof (OilParameter),
        sizeof (p6) / sizeof (OilParameter),
        sizeof (p7) / sizeof (OilParameter),
        sizeof (p8) / sizeof (OilParameter),
        sizeof (p9) / sizeof (OilParameter),
    };
    
    static OilParameter *expected[] = { p1, p2, p3, p4, p5, p6, p7, p8, p9 };
    
    guint i, j;
    for (i = 0; i < sizeof (prototypes) / sizeof (gchar *); i++) {
        gboolean ret;
        guint n_params;
        OilParameter *params;
        gchar *p = prototypes[i];
        
        g_test_message ("Process prototype \"%s\"", p);
        
        ret = oil_prototype_parse (p, &n_params, &params);
        g_assert (ret == TRUE);
        g_assert (n_params == p_n_params[i]);
        
        for (j = 0; j < n_params; j++) {
            OilParameter *a = params + j, *b = expected[i] + j;
            
            g_assert_cmpstr (a->parameter, ==, b->parameter);
            g_assert_cmpint (a->c_type, ==, b->c_type);
            g_assert_cmpint (a->arg_class, ==, b->arg_class);
            g_assert_cmpint (a->arg_type, ==, b->arg_type);
            
            g_assert (a->is_pointer == b->is_pointer);
            g_assert (a->is_stride == b->is_stride);
            g_assert (!a->is_pointer || !a->is_stride);
            
            g_assert_cmpint (a->index, ==, b->index);
            g_assert_cmpint (a->prestride_var, ==, b->prestride_var);
            g_assert_cmpint (a->poststride_var, ==, b->poststride_var);
            g_assert_cmpint (a->prestride_length, ==, b->prestride_length);
            g_assert_cmpint (a->poststride_length, ==, b->poststride_length);
            
            /* should be zero block */
            g_assert (a->value.i64 == 0);
            
            g_assert (a->header == 0);
            g_assert (a->footer == 0);
            g_assert (a->pre_n == 0);
            g_assert (a->post_n == 0);
            g_assert (a->stride == 0);
            g_assert (a->size == 0);
            g_assert (a->guard == 0);
        }
        
        for (j = 0; j < n_params; j++)
            g_free ((params + j)->parameter);
        g_free (params);
    }
}
