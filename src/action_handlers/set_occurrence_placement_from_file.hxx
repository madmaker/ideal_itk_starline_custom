#ifndef SET_OCCURRENCE_PLACEMENT_FROM_FILE_HXX_
#define SET_OCCURRENCE_PLACEMENT_FROM_FILE_HXX_

#include <epm/epm.h>
#include <tc/tc.h>
#include <string.h>
#include <vector>
using namespace std;

int get_designator_and_placement_indexes(char* line, int* designator, int* placement);
vector<string> split_eda_string(char* line);
int parseLine(char* line, int designator_pos, int placement_pos, char** designator, char** placement);
int find_placement(vector<string> designators, vector<string> placements, char* designator);
int find_and_set_placement(tag_t bom_line, vector<string> designators, vector<string> placements);
int cycle_through_bls_and_set_placement(tag_t bvr);
int find_and_read_placements_dataset(tag_t top_bl, vector<string>* designators, vector<string>* placements);
int read_placements_dataset(tag_t dataset, vector<string>* designators, vector<string>* placements);
int set_occurrence_placement_from_file(EPM_action_message_t);

#endif
