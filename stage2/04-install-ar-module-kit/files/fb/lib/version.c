#include "version.h"
#include "log.h"

char *version_git_get(void) {

    return GITSHA;
}

char *version_date_get(void) {

    return DATE;
}
