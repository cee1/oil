/* -*- coding: utf-8; -*- 
 * Distributed under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <oilcore.h>
#include "oilfuncs.h"

static void parse_line (char *line, char **section);

/**
 * oilfuncs_init_from_ini:
 * Initialize oilfuncs, it selects implements according to an internal ini-string.
 */
void oilfuncs_init_from_ini ()
{
    oil_class_init (NULL);

    oilfuncs_register_classes ();
    oilfuncs_register_implements ();

# include "oilfuncs.ini"
    oil_load_ini (oilfuncs_ini);
}

#define MAX_LINE_LEN 1024
/* 
 * an Iterator is used implicitly, ie:
 * 1) get_val (iter)
 * 2) move (iter)
 * 3) cond (iter)
 *  */
#define EAT_BLANK(get_val, move, cond) do { \
    int c;                                  \
    while ((cond)) {                        \
        c = (get_val);                      \
        if (c != ' ' && c != '\t')          \
            break;                          \
        (move);                             \
    }                                       \
} while (0)

/**
 * oil_load_ini:
 * @ini_string: an ini-string, describing each active implement set for each platform.
 *
 * Parses @ini_string, selects implements according to @ini_string, which will like:
 * [platform]
 * function_class = implement
 */
void oil_load_ini (const char *ini_string)
{
    if (ini_string) {
        const char *start, *end;
        char *section = NULL;
        
        char line[MAX_LINE_LEN] = {0,};
        
        start = (char *) ini_string;
        end = strchr (start, '\n');
        
        while (end) {
            strncpy (line, start, MAX_LINE_LEN - 1);
            parse_line (line, &section);
            
            start = end + 1;
            end = strchr (start, '\n');
        }
        
        strncpy (line , start, MAX_LINE_LEN - 1);
        parse_line (line, &section);
        free (section);
    }
}

static void parse_line (char *line, char **section)
{
    char *iter = line;
    
    EAT_BLANK (*iter, iter++, *iter != '\0');
    
    switch (iter[0]) {
        char *right_bracket, *equal_sign, *i, *key, *val;
        
        case '\0':
        case '#':
            /* ignore the blank and comment */
            break;
        case '[':
            /* process section*/
            right_bracket = strchr (iter, ']');
            if (right_bracket == NULL) {
                fprintf (stderr, "Failed to process \"%s\": not closed square bracket\n", line);
                abort ();
            }
            free (*section);
            *section = strndup (iter + 1, right_bracket - iter);
            break;
        default:
            /* process a key assignment line*/
            if (*iter == '=') {
                fprintf (stderr, "Failed to process \"%s\": the key is empty", line);
                abort ();
            }
            
            equal_sign = strchr (iter, '=');
            if (equal_sign == NULL) {
                fprintf (stderr, "Failed to process \"%s\": no \"=\" token find\n", line);
                abort ();
            }
            
            /* we've pHAVE_STATrocessed '=' starting line, so it's safe to ignore quit condition */
            i = equal_sign - 1;
            EAT_BLANK (*i, i--, 1);
            *(i + 1) = '\0';
            key = iter;
            
            i = equal_sign + 1;
            EAT_BLANK (*i, i++, *i != '\0');
            val = i;
            
            if (*val != '\0') {
                i = val + strlen (val);
                EAT_BLANK (*i, i--, 1);
                *(i + 1) = '\0';
            }
            
            /* do action */
            if (strcmp (*section, OIL_PLATFORM_STRING) == 0) {
                OilClass *cls = oil_class_get (key);
                if (cls == NULL)
                    fprintf (stderr, "No function class names \"%s\"", key);
                else {
                    oil_class_activate_implement (cls, val);
                    if (strcmp (oil_class_get_active_implement (cls), val) != 0)
                        fprintf (stderr, "Activate \"%s\".\"%s\" failed", key, val);
                }
            }
            
            break;
    }

}
