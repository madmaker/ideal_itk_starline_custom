#ifndef NO_OTHER_ACTIVE_WORKFLOWS_HXX_
#define NO_OTHER_ACTIVE_WORKFLOWS_HXX_

#include <epm/epm.h>
#include <tc/tc.h>

/**
 * Handler проверяет, есть ли ещё активные рабочие процессы, связанные с одним из таргетов
 */

int is_in_another_active_workflow(tag_t object, tag_t root_task, logical* result);
EPM_decision_t no_other_active_workflows(EPM_rule_message_t);

#endif
