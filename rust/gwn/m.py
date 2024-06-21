import rustworkx
import logging


logger = logging.getLogger()
logger.setLevel(logging.DEBUG)

logger.info("#### aaaaaaaa")
# Rustworkx's undirected graph type.
graph = rustworkx.PyGraph()

# Each time add node is called, it returns a new node index
aa = graph.add_node("A")
bb = graph.add_node("B")
cc = graph.add_node("C")

# add_edges_from takes tuples of node indices and weights,
# and returns edge indices
graph.add_edges_from([(aa, bb, 1.5), (aa, cc, 5.0), (bb, cc, 2.5)])

# Returns the path A -> B -> C
dijsp = rustworkx.dijkstra_shortest_paths(graph, aa, cc, weight_fn=float)

logger.info("dijsp.shape : {}".format(dijsp))

