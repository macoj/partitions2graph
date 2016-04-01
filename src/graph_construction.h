/*
 * export_nodes.h
 *
 *  Created on: Aug 2, 2015
 *      Author: marcos
 */

#ifndef EXPORT_NODES_H_
#define EXPORT_NODES_H_

int read_graph_and_export(const char * graph_file_name,
						  const char * output_metis_filename,
						  const char * attributes_filename);


#endif /* EXPORT_NODES_H_ */
