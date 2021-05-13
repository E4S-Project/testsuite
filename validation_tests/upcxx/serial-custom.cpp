#include <upcxx/upcxx.hpp>
#include <cassert>
#include <iostream>
#include <map>
#include <vector>

int total_num_vertices;
int rank;
int nranks;

#define VERTICES_PER_RANK ((total_num_vertices + nranks - 1) / nranks)
#define OWNER_RANK(vertex_id) ((vertex_id) / VERTICES_PER_RANK)
#define START_RANK_VERTICES(_rank) ((_rank) * VERTICES_PER_RANK)
#define END_RANK_VERTICES(_rank) (((_rank) + 1) * VERTICES_PER_RANK > total_num_vertices ? total_num_vertices : ((_rank) + 1) * VERTICES_PER_RANK)

class vertex;
vertex *get_vertex_from_store(int vertex_id);

//SNIPPET
class vertex {
    private:
        int id;
        std::vector<vertex *> neighbors;

    public:
        vertex(int _id) : id(_id) { }
        int get_id() const { return id; }
        int n_neighbors() const { return neighbors.size(); }
        bool has_edge(int other) const;
        void add_neighbor(int neighbor_id); 

        std::vector<vertex*>::const_iterator neighbors_begin() const {
            return neighbors.begin();
        }
        std::vector<vertex*>::const_iterator neighbors_end() const {
            return neighbors.end();
        }

        /*
         * An example of using a member struct upcxx_serialization to implement
         * custom serialization for the vertex class.
         */
        struct upcxx_serialization {
            template<typename Writer>
            static void serialize (Writer& writer, vertex const & object) {
                writer.write(object.get_id());
                writer.write(object.n_neighbors());
                for (auto i = object.neighbors_begin(), e = object.neighbors_end();
                        i != e; i++) {
                    vertex *neighbor = *i;
                    writer.write(neighbor->get_id());
                }
            }

            template<typename Reader>
            static vertex* deserialize(Reader& reader, void* storage) {
                int id = reader.template read<int>();
                int n_neighbors = reader.template read<int>();

                vertex *v = new(storage) vertex(id);
                for (int n = 0; n < n_neighbors; n++) {
                    v->add_neighbor(reader.template read<int>());
                }
                return v;
            }
        };
};
//SNIPPET

std::map<int, vertex *> local_vertex_store;

bool vertex::has_edge(int other) const {
    for (auto i = neighbors.begin(), e = neighbors.end(); i != e; i++) {
        vertex *v = *i;
        if (v->get_id() == other) return true;
    }
    return false;
}

void vertex::add_neighbor(int neighbor_id) {
    vertex *neighbor = get_vertex_from_store(neighbor_id);
    if (std::find(neighbors.begin(), neighbors.end(), neighbor) ==
            neighbors.end()) {
        neighbors.push_back(neighbor);
    }
}

vertex *get_vertex_from_store(int vertex_id) {
    if (local_vertex_store.find(vertex_id) == local_vertex_store.end()) {
        assert(OWNER_RANK(vertex_id) != rank);
        vertex *new_vert = new vertex(vertex_id);
        local_vertex_store.insert(std::pair<int, vertex*>(vertex_id, new_vert));
    }

    auto iter = local_vertex_store.find(vertex_id);
    assert(iter != local_vertex_store.end());
    return iter->second;
}

void create_edge(vertex* local, int other_id) {
    int local_id = local->get_id();
    vertex *other = get_vertex_from_store(other_id);

    /*
     * Add a vertex* for this ID to our neighbors list. This should
     * either be an existing vertex* pulled from local_vertex_store or a
     * newly created vertex*, which should be inserted in
     * local_vertex_store.
     */
    local->add_neighbor(other_id);

    /*
     * Ship this vertex and its neighbors to the rank that owns
     * other_id. Use an RPC to then add this vertex to its neighbors.
     * Fetch that vertex back.
     */
    upcxx::future<vertex> other_fut = upcxx::rpc(OWNER_RANK(other_id),
            [] (const vertex &tmp, int other_id) {
                vertex *other = get_vertex_from_store(other_id);

                other->add_neighbor(tmp.get_id());

                return *other;
        }, *local, other_id);

    /*
     * Applications may want to use information on the remote vertex, so we
     * demonstrate fetching it locally here.
     */
    vertex transferred_other = other_fut.wait();
}

int main(void) {
    upcxx::init();
    rank = upcxx::rank_me();
    nranks = upcxx::rank_n();

    srand(rank);

    int niters = 10;
    int edges_per_iter = 10;
    total_num_vertices = nranks * niters * edges_per_iter;

    int start_local_vertices = START_RANK_VERTICES(rank);
    int end_local_vertices = END_RANK_VERTICES(rank);

    int *edges_to_insert = new int[niters * edges_per_iter * 2];
    assert(edges_to_insert);
    for (int i = 0; i < niters * edges_per_iter; i++) {
        // Choose a random local vertex
        edges_to_insert[2 * i] = start_local_vertices + (rand() %
                (end_local_vertices - start_local_vertices));
        // Choose a random vertex
        edges_to_insert[2 * i + 1] = (rand() % total_num_vertices);
    }

    upcxx::barrier();

    // Seed our local vertex store
    for (int v = start_local_vertices; v < end_local_vertices; v++) {
        vertex *new_vert = new vertex(v);
        local_vertex_store.insert(std::pair<int, vertex*>(v, new_vert));
    }

    upcxx::barrier();

    for (int iter = 0; iter < niters; iter++) {
        for (int e = iter * edges_per_iter; e < (iter + 1) * edges_per_iter;
                e++) {
            int a = edges_to_insert[2 * e];
            int b = edges_to_insert[2 * e + 1];
            vertex *v_a = get_vertex_from_store(a);
            create_edge(v_a, b);
        }

        upcxx::barrier();
    }

    upcxx::barrier();

    /*
     * Validation. Assert that each of the edges we were supposed to create were
     * created by checking local and remote state.
     */
    upcxx::future<> fut = upcxx::make_future();
    for (int e = 0; e < niters * edges_per_iter; e++) {
        int a = edges_to_insert[2 * e];
        int b = edges_to_insert[2 * e + 1];
        vertex *v_a = get_vertex_from_store(a);
        assert(v_a->has_edge(b));

        fut = upcxx::when_all(fut, upcxx::rpc(OWNER_RANK(b), [a, b] {
                    vertex *vb = get_vertex_from_store(b);
                    assert(vb->has_edge(a));
                }));
    }
    fut.wait();
    delete [] edges_to_insert;

    upcxx::finalize();

    if (rank == 0) {
        printf("SUCCESS\n");
    }

    return 0;
}
