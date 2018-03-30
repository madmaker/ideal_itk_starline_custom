#ifndef SET_OCCURRENCE_PLACEMENT_FROM_FILE_HXX_
#define SET_OCCURRENCE_PLACEMENT_FROM_FILE_HXX_

#include <epm/epm.h>
#include <tc/tc.h>

int find_and_set_placement(tag_t bom_line);
int cycle_through_bls_and_set_placement(tag_t bvr);
int find_and_read_placements_dataset(tag_t top_bl);
int read_placements_dataset(tag_t dataset);
int set_occurrence_placement_from_file(EPM_action_message_t);

#endif
