#ifndef __COMMON_H__
#define __COMMON_H__

#include <glib.h>

#define ARRAY_SIZE(x)       sizeof(x)/sizeof(x[0])
#define IS_64BIT_MACHINE    (8 == sizeof(gpointer))

typedef union _Color
{
    struct
    {
        guint32 blue:8;
        guint32 green:8;
        guint32 red:8;
        guint32 alpha:8;
    };
    guint32 value;
} Color;

typedef struct _Point
{
    guint x;
    guint y;
} Point;

typedef struct _Rect
{
    guint x;
    guint y;
    guint width;
    guint height;
} Rect;

enum _Weekday
{
    ENUM_MON = 0,
    ENUM_TUE = 1,
    ENUM_WED = 2,
    ENUM_THU = 3,
    ENUM_FRI = 4,
    ENUM_SAT = 5,
    ENUM_SUN = 6,
    ENUM_WEEKDAY_LAST
} Weekday;

extern const gchar *weekday_name[];

typedef struct _Schedules
{
    gchar *schedule[ENUM_WEEKDAY_LAST];
} Schedules;

#endif /* __COMMON_H__ */
