/*
 * graphml2metis.c
 *
 *  Created on: Jul 30, 2015
 *      Author: marcos
 */
#include <igraph/igraph.h>
#include <stdio.h>
#include <unistd.h>

#include "export_nodes.h"

void export_nodes(const igraph_t* graph, FILE * output, FILE * attributes_output) {
	igraph_es_t edge_selector;
	igraph_vs_t vertex_selector;
	igraph_vs_t adjacent_vertex_selector;
	igraph_vit_t vertex_iterator;
	igraph_vit_t adjacent_vertex_iterator;
	igraph_integer_t nodes_number;
	igraph_integer_t edges_number;
	igraph_vector_t vertices_attributes_types;
	igraph_strvector_t vertices_attributes_names;

	int attribute = 0;
	// are we exporting also the attributes?
	if (attributes_output != NULL) {
		igraph_vector_init(&vertices_attributes_types, 0);
		igraph_strvector_init(&vertices_attributes_names, 0);
		igraph_cattribute_list(graph, NULL, NULL, &vertices_attributes_names, &vertices_attributes_types, NULL, NULL);
		for (attribute = 0; attribute < vertices_attributes_names.len - 1; attribute++) {
			fprintf(attributes_output, "%s,", vertices_attributes_names.data[attribute]);
		}
		fprintf(attributes_output, "%s\n", vertices_attributes_names.data[attribute]);
	}

	// vertices selector
	igraph_vs_all(&vertex_selector);
	igraph_vs_size(graph, &vertex_selector, &nodes_number);

	// if we are exporting a METIS...
	if (output != NULL) {
		// edges selector
		igraph_es_all(&edge_selector, IGRAPH_EDGEORDER_ID);
		igraph_es_size(graph, &edge_selector, &edges_number);
		igraph_es_destroy(&edge_selector);
		fprintf(output, "%li %li\n", (long int) nodes_number, (long int) edges_number);
	}

	// vertices iterator
	igraph_vit_create(graph, vertex_selector, &vertex_iterator);
	int vertice;
	while (!IGRAPH_VIT_END(vertex_iterator)) {
		vertice = (long int) IGRAPH_VIT_GET(vertex_iterator);
		// exports attributes
		if (attributes_output != NULL) {
			int attribute;
			for (attribute = 0; attribute < vertices_attributes_names.len - 1; attribute++) {
				if (VECTOR(vertices_attributes_types)[attribute] == IGRAPH_ATTRIBUTE_NUMERIC) {
					fprintf(attributes_output, "");
					igraph_real_fprintf(attributes_output, VAN(graph, STR(vertices_attributes_names, attribute), vertice));
					fprintf(attributes_output, ",");
				} else {
					fprintf(attributes_output, "\"%s\",", VAS(graph, STR(vertices_attributes_names, attribute), vertice));
				}
			}
			if (VECTOR(vertices_attributes_types)[attribute] == IGRAPH_ATTRIBUTE_NUMERIC) {
				igraph_real_fprintf(attributes_output, VAN(graph, STR(vertices_attributes_names, attribute), vertice));
			} else {
				fprintf(attributes_output, "\"%s\"", VAS(graph, STR(vertices_attributes_names, attribute), vertice));
			}
			fprintf(attributes_output, "\n");
		}
		// exports adjacency
		if (output != NULL) {
			igraph_vs_adj(&adjacent_vertex_selector, vertice, IGRAPH_ALL);
			igraph_vit_create(graph, adjacent_vertex_selector, &adjacent_vertex_iterator);
			while (!IGRAPH_VIT_END(adjacent_vertex_iterator)) {
				// igraph indexes starting from 0, we need it starting from 1
				fprintf(output, "%li ", ((long int) IGRAPH_VIT_GET(adjacent_vertex_iterator) + 1));
				IGRAPH_VIT_NEXT(adjacent_vertex_iterator);
			}
			igraph_vit_destroy(&adjacent_vertex_iterator);
			igraph_vs_destroy(&adjacent_vertex_selector);
			fprintf(output, "\n");
		}
		IGRAPH_VIT_NEXT(vertex_iterator);
	}
	igraph_vit_destroy(&vertex_iterator);
	igraph_vs_destroy(&vertex_selector);
}

int read_graph_paritions_and_export(const char * graph_file_name,
									const char * _metis_filename,
									const char * attributes_filename) {
	int ret = -1;
	FILE * input = NULL;
	FILE * output = NULL;
	FILE * attributes_output = NULL;

	if (graph_file_name != NULL) {
		input = fopen(graph_file_name, "r");
	}

	if (output_metis_filename != NULL) {
		output = fopen(output_metis_filename, "w");
	}

	if (attributes_filename != NULL) {
		attributes_output = fopen(attributes_filename, "w");
	}

	if (input != NULL) {
		if (output == NULL && attributes_output == NULL) {
			// if no output given, let's just spit it out in METIS format!
			output = stdout;
		}
		igraph_i_set_attribute_table(&igraph_cattribute_table);  // this must be before the graph creation.
		igraph_t graph;
		ret = igraph_read_graph_graphml(&graph, input, 0);
		if (ret == IGRAPH_SUCCESS) {
			export_nodes(&graph, output, attributes_output);
			igraph_destroy(&graph);
		}
	}
	if (input != NULL) {
		fclose(input);
	}

	if (output != NULL) {
		fclose(output);
	}

	if (attributes_output != NULL) {
		fclose(attributes_output);
	}
	return ret;
}
