#ifndef ATTCH_AS_REFS_IF_OTHER_ACTIVE_WORKFLOW_HXX_
#define ATTCH_AS_REFS_IF_OTHER_ACTIVE_WORKFLOW_HXX_

#include <epm/epm.h>
#include <tc/tc.h>

/**
 * Handler проверяет, есть ли ещё активные рабочие процессы, связанные с одним из таргетов и если есть, то отрпавляет эти объекты в референсы
 */

int is_in_another_active_workflow(tag_t object, tag_t root_task, logical* result);
int attach_as_refs_if_other_active_workflow(EPM_action_message_t);

#endif
