/*
 * graph_construction.h
 *
 *  Created on: Apr 1, 2016
 *      Author: marcos
 */

#ifndef GRAPH_CONSTRUCTION_H_
#define GRAPH_CONSTRUCTION_H_

int read_graph_partitions_and_export(const char * graphml_file_name,
								     const char * split_file_name,
									 const char * attributes_file_name,
									 const char * graphml_output_file_name);

#endif /* GRAPH_CONSTRUCTION_H_ */
