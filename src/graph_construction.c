/*
 * graph_construction.c
 *
 *  Created on: Apr 1, 2016
 *      Author: marcos
 */
#include <igraph/igraph.h>
#include <stdio.h>
#include <unistd.h>

#include "graph_construction.h"

void construct_graph_from_partitions(const igraph_t* graph, FILE * split_file, FILE * attributes_file, const char * graphml_output_file_name) {
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
	if (attributes_file != NULL) {
		igraph_vector_init(&vertices_attributes_types, 0);
		igraph_strvector_init(&vertices_attributes_names, 0);
		igraph_cattribute_list(graph, NULL, NULL, &vertices_attributes_names, &vertices_attributes_types, NULL, NULL);
		for (attribute = 0; attribute < vertices_attributes_names.len - 1; attribute++) {
			fprintf(attributes_file, "%s,", vertices_attributes_names.data[attribute]);
		}
		fprintf(attributes_file, "%s\n", vertices_attributes_names.data[attribute]);
	}

	// vertices selector
	igraph_vs_all(&vertex_selector);
	igraph_vs_size(graph, &vertex_selector, &nodes_number);

	// if we are exporting a METIS...
	if (split_file != NULL) {
		// edges selector
		igraph_es_all(&edge_selector, IGRAPH_EDGEORDER_ID);
		igraph_es_size(graph, &edge_selector, &edges_number);
		igraph_es_destroy(&edge_selector);
		fprintf(split_file, "%li %li\n", (long int) nodes_number, (long int) edges_number);
	}

	// vertices iterator
	igraph_vit_create(graph, vertex_selector, &vertex_iterator);
	int vertice;
	while (!IGRAPH_VIT_END(vertex_iterator)) {
		vertice = (long int) IGRAPH_VIT_GET(vertex_iterator);
		// exports attributes
		if (attributes_file != NULL) {
			int attribute;
			for (attribute = 0; attribute < vertices_attributes_names.len - 1; attribute++) {
				if (VECTOR(vertices_attributes_types)[attribute] == IGRAPH_ATTRIBUTE_NUMERIC) {
					fprintf(attributes_file, "");
					igraph_real_fprintf(attributes_file, VAN(graph, STR(vertices_attributes_names, attribute), vertice));
					fprintf(attributes_file, ",");
				} else {
					fprintf(attributes_file, "\"%s\",", VAS(graph, STR(vertices_attributes_names, attribute), vertice));
				}
			}
			if (VECTOR(vertices_attributes_types)[attribute] == IGRAPH_ATTRIBUTE_NUMERIC) {
				igraph_real_fprintf(attributes_file, VAN(graph, STR(vertices_attributes_names, attribute), vertice));
			} else {
				fprintf(attributes_file, "\"%s\"", VAS(graph, STR(vertices_attributes_names, attribute), vertice));
			}
			fprintf(attributes_file, "\n");
		}
		// exports adjacency
		if (split_file != NULL) {
			igraph_vs_adj(&adjacent_vertex_selector, vertice, IGRAPH_ALL);
			igraph_vit_create(graph, adjacent_vertex_selector, &adjacent_vertex_iterator);
			while (!IGRAPH_VIT_END(adjacent_vertex_iterator)) {
				// igraph indexes starting from 0, we need it starting from 1
				fprintf(split_file, "%li ", ((long int) IGRAPH_VIT_GET(adjacent_vertex_iterator) + 1));
				IGRAPH_VIT_NEXT(adjacent_vertex_iterator);
			}
			igraph_vit_destroy(&adjacent_vertex_iterator);
			igraph_vs_destroy(&adjacent_vertex_selector);
			fprintf(split_file, "\n");
		}
		IGRAPH_VIT_NEXT(vertex_iterator);
	}
	igraph_vit_destroy(&vertex_iterator);
	igraph_vs_destroy(&vertex_selector);
}

int read_graph_partitions_and_export(const char * graphml_file_name,
									 const char * split_file_name,
									 const char * attributes_file_name,
									 const char * graphml_output_file_name) {
	int ret = -1;
	FILE * input_file = NULL;
	FILE * split_file = NULL;
	FILE * attributes_file = NULL;

	if (graphml_file_name != NULL) {
		input_file = fopen(graphml_file_name, "r");
	}

	if (split_file_name != NULL) {
		split_file = fopen(split_file_name, "r");
	}

	if (attributes_file_name != NULL) {
		attributes_file = fopen(attributes_file_name, "r");
	}

	if (input_file != NULL && split_file != NULL && attributes_file != NULL && graphml_output_file_name != NULL) {
		igraph_i_set_attribute_table(&igraph_cattribute_table);  // this must be before the graph creation.
		igraph_t graph;
		ret = igraph_read_graph_graphml(&graph, input_file, 0);
		if (ret == IGRAPH_SUCCESS) {
			construct_graph_from_partitions(&graph, split_file, attributes_file, graphml_output_file_name);
			igraph_destroy(&graph);
		}
	}
	if (input_file != NULL) {
		fclose(input_file);
	}

	if (split_file != NULL) {
		fclose(split_file);
	}

	if (attributes_file != NULL) {
		fclose(attributes_file);
	}
	return ret;
}
