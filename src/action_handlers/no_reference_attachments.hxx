#ifndef NO_REFERENCE_ATTACHMENTS_HXX_
#define NO_REFERENCE_ATTACHMENTS_HXX_

#include <epm/epm.h>
#include <tc/tc.h>

/**
 * Handler проверяет, есть ли объекты, связанные с задачей как ссылки
 */

EPM_decision_t no_reference_attachments(EPM_rule_message_t);

#endif
