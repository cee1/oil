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
static int parse_flags_string (const char *flags_string, unsigned int *flags);
static void do_activate_impl (const char *section, char *key, char *value);

/**
 * oilfuncs_init_from_ini:
 *
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

#define MAX_LINE_LEN 1023
/* 
 * an Iterator is used implicitly, ie:
 * 1) get_val (iter)
 * 2) move (iter)
 * 3) cond (iter)
 *  */
#define EAT_BLANK(get_val, move, cond) do {     \
    int _____c;                                 \
    while ((cond)) {                            \
        _____c = (get_val);                     \
        if (_____c != ' ' && _____c != '\t')    \
            break;                              \
        (move);                                 \
    }                                           \
} while (0)

#define STR_CHUG(str_p) EAT_BLANK (*str_p, str_p++, *str_p != '\0')

#define STR_CHOMP(str_p) do {                                       \
    if (*str_p != '\0') {                                           \
        char *_____tail = str_p + strlen (str_p) - 1;               \
        EAT_BLANK (*_____tail, _____tail--, _____tail >= str_p);    \
        *(_____tail + 1) = '\0';                                    \
    }                                                               \
} while (0)

#define STR_STRIP(str_p) do {               \
    STR_CHUG (str_p);                       \
    STR_CHOMP (str_p);                      \
} while (0)

/**
 * oil_load_ini:
 * @ini_string: an ini-string, describing each active implement set for a specified CPU. 
 *
 * Parse @ini_string, select & activate implements according to @ini_string, which will like:
 * <informalexample><programlisting>
 * [white-separated flags]
 * function_class = implement</programlisting></informalexample>
 */
void oil_load_ini (const char *ini_string)
{
    if (ini_string) {
        const char *start, *end;
        char *section = NULL;
        
        char line[MAX_LINE_LEN + 1] = {0,};
        
        start = (char *) ini_string;
        end = strchr (start, '\n');
        
        while (end) {
            unsigned int len = end - start;
            if (len > MAX_LINE_LEN) {
                fprintf (stderr, 
                        "oil_load_ini: line buffer overflow(>%d Bytes)",
                        MAX_LINE_LEN);
                len = MAX_LINE_LEN;
            }
            strncpy (line, start, len);
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
    
    STR_CHUG (iter);
    
    switch (iter[0]) {
        char *right_bracket, *equal_sign, *key, *val;
        
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
            
            iter++;
            *right_bracket = '\0';
            STR_STRIP (iter);
            
            *section = strdup (iter);
            break;
        default:
            /* process a key assignment line*/
            if (*iter == '=') {
                fprintf (stderr, "Failed to process \"%s\": the key is empty\n", line);
                abort ();
            }
            
            equal_sign = strchr (iter, '=');
            if (equal_sign == NULL) {
                fprintf (stderr, "Failed to process \"%s\": no \"=\" token find\n", line);
                abort ();
            }
            
            /* we've processed '=' starting line, so it's safe to ignore quit condition */
            *equal_sign = '\0';
            key = iter;
            STR_CHOMP (key);
            
            val = equal_sign + 1;
            STR_STRIP (val);
            
            /* do action */
            do_activate_impl (*section, key, val);
            
            break;
    }

}

/* 
 * parse_flags_string:
 * @flags_string: is not starting and tailing with white characters, should not be NULL
 * @flags: should not be NULL
 *
 * Returns: 1 => True(success), 0 => False(fail)
 */
static int parse_flags_string (const char *flags_string, unsigned int *flags)
{
    unsigned int _flags = 0;
    int ret = 1;
    const char *current = flags_string, *next;
    
    while ((next = strpbrk (current, " \t"))) {
        unsigned int flag;
        unsigned int len = next - current;
        char flag_string[len + 1];
        
        memcpy (flag_string, current, len);
        flag_string[len] = '\0';
        
        flag = oil_cpu_string_to_flag (flag_string);
        if (flag == 0) {
            fprintf (stderr, "parse_flags_string: Unknown flag \"%s\".\n", flag_string);
            ret = 0;
        }
        _flags |= flag;
        EAT_BLANK (*next, next++, *next != '\0');
        current = next;
        STR_CHUG (current);
    }
    if (*current != '\0'){
        unsigned int flag;
        unsigned int len = strlen (current);
        char flag_string[len + 1];
        
        memcpy (flag_string, current, len);
        flag_string[len] = '\0';
        
        flag = oil_cpu_string_to_flag (flag_string);
        if (flag == 0) {
            fprintf (stderr, "parse_flags_string: Unknown flag \"%s\".\n", flag_string);
            ret = 0;
        }
        _flags |= flag;
    }
    
    *flags = _flags;
    
    return ret;
}

static void do_activate_impl (const char *section, char *key, char *value)
{
    unsigned int flags;
    OilClass *cls;
    
    if (section == NULL) {
        fprintf (stderr, "No section defined, assume no flags need");
        flags = 0;
     } else if (parse_flags_string (section, &flags) == 0)
        return;
    
    cls = oil_class_get (key);

    if (cls == NULL)
        fprintf (stderr, "No function class names \"%s\"\n", key);
    else if ((oil_cpu_get_flags () & flags) == flags) {
        
        oil_class_implements_get (cls, value, &flags);
        if ((oil_cpu_get_flags () & flags) == flags) {
            char *t;
            oil_class_activate_implement (cls, value);
        
            /* check, whether we'v activated the implement successfully */
            t = oil_class_get_active_implement (cls);
            if (!t || strcmp (t, value) != 0)
                fprintf (stderr, "Activate %s.%s failed\n", key, value);
            
        } else {
            fprintf (stderr,
                    "Implement %s.%s in section [%s], isn't runnable in current CPU\n",
                    key,
                    value,
                    section);
        }  
    }
}

