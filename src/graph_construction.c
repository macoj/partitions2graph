/*
 * graph_construction.c
 *
 *  Created on: Apr 1, 2016
 *      Author: marcos
 */
#include <igraph/igraph.h>
#include <stdio.h>
#include <unistd.h>
#include <glib.h>

#include "graph_construction.h"

gint ** create_table_node_clusters(const char * attributes_file_name, const char * split_file_name) {
	int id_column = -1;
	int total_number_of_items = 0;
	int item = 0;

	size_t line_length = 0;
	ssize_t byte_read;

	char * line = NULL;
	char * partition = NULL;
	gchar ** columns = NULL;

	FILE * split_file = NULL;
	FILE * attributes_file = NULL;

	gint ** table = NULL;
	GRegex *regex = g_regex_new ("([\n\"])+", G_REGEX_DOTALL | G_REGEX_OPTIMIZE, 0, NULL);


	if (attributes_file_name != NULL) {
		attributes_file = fopen(attributes_file_name, "r");
	}
	if (split_file_name != NULL) {
		split_file = fopen(split_file_name, "r");
	}

	if (attributes_file != NULL && split_file != NULL) {
		// we need to know which column is the 'id' column
		if (getline(&line, &line_length, attributes_file) != -1) {
			columns = g_strsplit(line, ",", -1);
			for (int i = 0; i < g_strv_length(columns); ++i) {
				if (g_strcmp0(columns[i], "id") == 0) {
					id_column = i;
				}
			}
			g_strfreev(columns);
			columns = NULL;
		}

		while ((byte_read = getline(&line, &line_length, split_file)) != -1) {
			total_number_of_items += 1;
		}
		fseek(split_file, 0L, SEEK_SET);

		table = (gint **) g_malloc0(sizeof(gint *) * total_number_of_items);

		while ((byte_read = getline(&line, &line_length, attributes_file)) != -1) {
			columns = g_strsplit(line, ",", -1);
			if (getline(&partition, &line_length, split_file) != -1) {
				gchar * key = g_regex_replace (regex, columns[id_column], -1, 0, "", 0, NULL);
				gchar * value = g_regex_replace (regex, partition, -1, 0, "", 0, NULL);
				table[atol(key)] = atol(value);
				g_free(key);
				g_free(value);
			}
			item += 1;
			g_strfreev(columns);
		}
		// you might need to create a FREE helper TODO

		if (attributes_file != NULL) {
			fclose(attributes_file);
		}

		if (split_file != NULL) {
			fclose(split_file);
		}
	}

	return table;
}

void construct_graph_from_partitions(const igraph_t* graph, gint ** table, const char * graphml_output_file_name) {
	igraph_vs_t vertex_selector;
	igraph_vs_t adjacent_vertex_selector;
	igraph_vit_t vertex_iterator;
	igraph_vit_t adjacent_vertex_iterator;
	igraph_integer_t nodes_number;
	igraph_vector_t vertices_attributes_types;
	igraph_strvector_t vertices_attributes_names;

	igraph_t output_graph;

	int id_attribute = -1;

	// NEED TO GET THE ID PROPERTY AND USE THE TABLE, THEN CREATE THE GRAPH
	igraph_vector_init(&vertices_attributes_types, 0);
	igraph_strvector_init(&vertices_attributes_names, 0);
	igraph_cattribute_list(graph, NULL, NULL, &vertices_attributes_names, &vertices_attributes_types, NULL, NULL);
	for (int attribute = 0; attribute < vertices_attributes_names.len; attribute++) {
		if (g_strcmp0(vertices_attributes_names.data[attribute], "id") == 0) {
			id_attribute = attribute;
			break;
		}
	}

	if (id_attribute != -1) {
		// outputgraph
//		igraph_empty(&output_graph, 0, IGRAPH_UNDIRECTED);

		// vertices selector
		igraph_vs_all(&vertex_selector);
		igraph_vs_size(graph, &vertex_selector, &nodes_number);

		// vertices iterator
		igraph_vit_create(graph, vertex_selector, &vertex_iterator);
		int vertice;
		long int id_vertice = 0;
		while (!IGRAPH_VIT_END(vertex_iterator)) {
			vertice = (long int) IGRAPH_VIT_GET(vertex_iterator);
			if (VECTOR(vertices_attributes_types)[id_attribute] == IGRAPH_ATTRIBUTE_NUMERIC) {
				id_vertice = VAN(graph, STR(vertices_attributes_names, id_attribute), vertice);
			} else {
				id_vertice = atol(VAS(graph, STR(vertices_attributes_names, id_attribute), vertice));
			}
			// exports adjacency
			igraph_vs_adj(&adjacent_vertex_selector, vertice, IGRAPH_ALL);
			igraph_vit_create(graph, adjacent_vertex_selector, &adjacent_vertex_iterator);
			while (!IGRAPH_VIT_END(adjacent_vertex_iterator)) {
				long int id_neighbor = 0;
				long int id_neighbor_igraph = (long int) IGRAPH_VIT_GET(adjacent_vertex_iterator);
				if (VECTOR(vertices_attributes_types)[id_attribute] == IGRAPH_ATTRIBUTE_NUMERIC) {
					id_neighbor = VAN(graph, STR(vertices_attributes_names, id_attribute), id_neighbor_igraph);
				} else {
					id_neighbor = atol(VAS(graph, STR(vertices_attributes_names, id_attribute), id_neighbor_igraph));
				}
				if (table[id_vertice] != table[id_neighbor]) {
					printf("%ld %ld\n", table[id_vertice], table[id_neighbor]);
				}
				IGRAPH_VIT_NEXT(adjacent_vertex_iterator);
			}
			igraph_vit_destroy(&adjacent_vertex_iterator);
			igraph_vs_destroy(&adjacent_vertex_selector);
			IGRAPH_VIT_NEXT(vertex_iterator);
		}
		igraph_vit_destroy(&vertex_iterator);
		igraph_vs_destroy(&vertex_selector);
	}
}

int read_graph_partitions_and_export(const char * graphml_file_name,
									 const char * split_file_name,
									 const char * attributes_file_name,
									 const char * graphml_output_file_name) {
	int ret = -1;
	FILE * input_file = NULL;

	gint ** table = create_table_node_clusters(attributes_file_name, split_file_name);

	if (graphml_file_name != NULL) {
		input_file = fopen(graphml_file_name, "r");
	}

	if (input_file != NULL && table != NULL) {
		printf("asd");
		igraph_i_set_attribute_table(&igraph_cattribute_table);  // this must be before the graph creation.
		igraph_t graph;
		ret = igraph_read_graph_graphml(&graph, input_file, 0);
		if (ret == IGRAPH_SUCCESS) {
			construct_graph_from_partitions(&graph, table, graphml_output_file_name);
			igraph_destroy(&graph);
		}
	}
	if (input_file != NULL) {
		fclose(input_file);
	}

	return ret;
}
