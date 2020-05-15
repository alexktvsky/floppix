#include "hcnse_portable.h"
#include "hcnse_core.h"


const char *
hcnse_get_metric_prefix(size_t number)
{
    size_t i;

    if (number < HCNSE_METRIC_MULTIPLIER_KILO) {
        return HCNSE_METRIC_PREFIX_EMPTY;
    }
    for (i = 0; number > 0; i++) {
        if (number % 10 != 0) {
            break;
        }
        number /= 10;
    }

    if (i >= 12) {
        return HCNSE_METRIC_PREFIX_TERA;
    }
    else if (i >= 9) {
        return HCNSE_METRIC_PREFIX_GIGA;
    }
    else if (i >= 6) {
        return HCNSE_METRIC_PREFIX_MEGA;
    }
    else if (i >= 3) {
        return HCNSE_METRIC_PREFIX_KILO;
    }
    else {
        return HCNSE_METRIC_PREFIX_EMPTY;
    }
}

size_t
hcnse_convert_to_prefix(size_t number)
{
    size_t i;
    size_t temp;

    temp = number;

    if (number < HCNSE_METRIC_MULTIPLIER_KILO) {
        return number;
    }
    for (i = 0; number > 0; i++) {
        if (number % 10 != 0) {
            break;
        }
        number /= 10;
    }

    if (i >= 12) {
        return temp / HCNSE_METRIC_MULTIPLIER_TERA;
    }
    else if (i >= 9) {
        return temp / HCNSE_METRIC_MULTIPLIER_GIGA;
    }
    else if (i >= 6) {
        return temp / HCNSE_METRIC_MULTIPLIER_MEGA;
    }
    else if (i >= 3) {
        return temp / HCNSE_METRIC_MULTIPLIER_KILO;
    }
    else {
        return temp;
    }
}
