/* -*- coding: utf-8; -*- 
 * Distributed under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */

#ifndef __OIL_CHECKER_H__
#define __OIL_CHECKER_H__

#include <glib.h>
#include <oilparameter.h>

G_BEGIN_DECLS

typedef struct _OilChecker OilChecker;
struct _OilChecker {
    gpointer data;
    
    void (*generate_sample) (
            OilChecker *checker,
            OilGenericType *src);

    gboolean (*check_result) (
            OilChecker *checker,
            const OilGenericType *ref,
            OilGenericType *tdata);
    
    void (*init) (
            OilChecker *checker, 
            guint n_params, 
            OilParameter *params);
    
    void (*uninit) (OilChecker *checker);
};

extern OilChecker oil_checker_default;

void oil_checker_init (
        OilChecker *checker, 
        guint n_params, 
        OilParameter *params);

void oil_checker_uninit (OilChecker *checker);

void oil_checker_sample_generator (
        OilChecker *checker,
        OilGenericType *src);

gboolean oil_checker_check_result (
        OilChecker *checker,
        const OilGenericType *ref,
        OilGenericType *tdata);

G_END_DECLS

#endif /* __OIL_CHECKER_H__ */
