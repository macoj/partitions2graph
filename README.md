# partitions2graph
From partitions of a graph to a whole graph

Usage:

```bash
$partitions2graph -i graphml_filename -s split_filename -a attributes_split_filename
```

Example:
```bash 
./partitions2graph -i graph.graphml -s split_001000 -a split_attributes.ametis > graph_simplified.edges
```

# TODO
* Support any type of id in the nodes. Now it assumes the id attribute is an integer number. 
* Export graphml.
* Better handling of .ametis files
