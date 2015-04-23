#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

/*******************************************************************************
 * Graph implementation - Vertex.
 ******************************************************************************/

typedef struct Vertex {
        int             key;
        int             visited;
        struct Edge     *adjacent;

        int             bellman_ford_cost;
        struct Vertex   *bellman_ford_predecessor;

        int             bfs_color;
        int             bfs_distance;
        struct Vertex   *bfs_predecessor;
} Vertex;

/*******************************************************************************
 * Graph implementation - Edge.
 ******************************************************************************/

typedef struct Edge {
        int             visited;
        struct Edge     *adjacent;

        struct Vertex   *in;
        struct Vertex   *out;

        int             bellman_ford_weight;
} Edge;

Vertex *new_vertex( int key ) {
        Vertex *v   = malloc( sizeof ( Vertex ) );
        v->key      = key;
        v->adjacent = NULL;
        return v;
}

Edge *new_edge( Vertex *in, Vertex *out, Edge *adjacent ) {
        Edge *e = malloc( sizeof( Edge ) );

        e->in       = in;
        e->out      = out;
        e->adjacent = adjacent;

        return e;
}

/*******************************************************************************
 * Graph implementation - Graph.
 ******************************************************************************/

typedef struct Graph {
        struct Vertex   **vertices;
        size_t          size;
} Graph;

Graph *new_graph(size_t s) {
        int i;
        Graph *g = malloc( sizeof ( Graph ) );

        g->vertices = malloc( sizeof ( Vertex *) * s );
        g->size     = s;

        for ( i = 0; i < s; i++ ) {
                g->vertices[i] = new_vertex( i );
        }
        return g;
}

void free_graph( Graph *g ) {
        int i;
        for ( i = 0; i < g->size; i++ ) {
                Edge *e = g->vertices[i]->adjacent;
                while (e) {
                        Edge *tmp = e;
                        e         = e->adjacent;
                        free(tmp);
                }
                free( g->vertices[i] );
        }
        free( g->vertices );
        free( g );
}

Edge *add_edge( Graph *g, int in, int out ) {
        Vertex *u = g->vertices[in];
        Vertex *v = g->vertices[out];
        u->adjacent = new_edge( u, v, u->adjacent );
        return u->adjacent;
}

Vertex *get_vertex( Graph *g, int key ) {
        return g->vertices[key];
}

void foreach_vertex( Graph *g, int (*f)( Vertex *, void * ), void *f_data ) {
        int i, ok = 1;
        for ( i = 0; i < g->size && ok; i++ ) {
                ok = f( g->vertices[i], f_data );
        }
}

void foreach_edge( Graph *g, int (*f)( Edge *, void * ), void *f_data ) {
        int i, ok = 1;
        for ( i = 0; i < g->size && ok; i++ ) {
                Edge *e;
                for ( e = g->vertices[i]->adjacent; e && ok; e = e->adjacent ) {
                        ok = f( e, f_data );
                }
        }

}

void foreach_adjacent_edge( Vertex *v, int (*f)( Edge *, void * ), void *f_data ) {
        Edge *e;
        int ok = 1;
        for ( e = v->adjacent; e && ok; e = e->adjacent ) {
                ok = f( e, f_data );
        }
}

/*******************************************************************************
 * Queue implementation.
 ******************************************************************************/

typedef struct Qnode {
        void            *data;
        struct Qnode    *next;
} Qnode;

struct Qnode *new_qnode( void *d ) {
        Qnode *qn = malloc( sizeof ( Qnode ) );
        qn->data  = d;
        qn->next  = NULL;
        return qn;
}

typedef struct Queue {
        struct Qnode    *first;
        struct Qnode    *last;
} Queue;

struct Queue *new_queue() {
        Queue *q = malloc( sizeof( Queue ) );
        q->first = NULL;
        q->last  = NULL;
        return q;
}

void enqueue ( Queue *q, void *d ) {
        Qnode *qn = new_qnode( d );

        if ( q->first ) {
                q->last->next = qn;
                q->last       = q->last->next;
        } else {
                q->first = q->last = qn;
        }
}

void *dequeue( Queue *q ) {
        void  *d   = q->first->data;
        Qnode *tmp = q->first;

        q->first = q->first->next;
        free( tmp );

        if ( q->first == NULL ) {
                q->last = NULL;
        }
        return d;
}

int is_empty( Queue *q ) {
        return q->first == NULL;
}

/*******************************************************************************
 * Breadth-First Search
 *
 * Modified to mark vertices in or that come out of a negative cycle with a
 * shortest-path distance of INT_MIN in the Bellman-Ford algorithm.
 ******************************************************************************/

#define WHITE 0
#define GRAY  1
#define BLACK 2

int bfs_initialize_vertex( Vertex *v, void *null ) {
        v->bfs_color       = WHITE;
        v->bfs_distance    = INT_MAX; /* Hackish, but works. */
        v->bfs_predecessor = NULL;
        return 1;
}

int grayify( Edge *e, void *queue ) {
        Queue  *q = (Queue *) queue;
        Vertex *u = e->in;
        Vertex *v = e->out;

        if ( v->bfs_color == WHITE && v->bellman_ford_cost != INT_MIN ) {
                v->bfs_color         = GRAY;
                v->bfs_distance      = u->bfs_distance + 1;
                v->bfs_predecessor   = u;
                v->bellman_ford_cost = INT_MIN;
                enqueue( q, v );
        }
        return 1;
}

void bfs_bellman_ford( Graph *g, Vertex *src ) {
        Queue *q = new_queue();

        foreach_vertex( g, &bfs_initialize_vertex, NULL );
        src->bfs_color         = GRAY;
        src->bfs_distance      = 0;
        src->bellman_ford_cost = INT_MIN;

        enqueue( q, src );
        while ( !is_empty( q ) ) {
                Vertex *u    = dequeue( q );
                foreach_adjacent_edge( u, &grayify, q);
                u->bfs_color = BLACK;
        }
        free(q);
}

/*******************************************************************************
 * Bellman-Ford single-source shortest path algorithm.
 *
 * Modified so that the shortest-path estimate of every vertex on a negative
 * cycle or that it's predecessor-chain starts on one.
 ******************************************************************************/

#define _BF_IS_INFINITE(A) ( A == INT_MIN || A == INT_MAX )

int sum( int a, int b ) {
        if ( _BF_IS_INFINITE( a ) ) return a;
        if ( _BF_IS_INFINITE( b ) ) return b;
        return a + b;
}

int bellman_ford_initialize_vertex( Vertex *v, void *null ) {
        v->bellman_ford_cost        = INT_MAX; /* Hackish, but works. */
        v->bellman_ford_predecessor = NULL;
        v->visited                  = 0;
        return 1;
}

void initialize_single_source( Graph *g, Vertex *src ) {
        foreach_vertex( g, &bellman_ford_initialize_vertex, NULL );
        src->visited           = 1;
        src->bellman_ford_cost = 0;
}

typedef struct {
        int changes;
} relax_data;

int relax( Edge *e, void *r_data ) {
        Vertex *u = e->in;
        Vertex *v = e->out;

        if ( v->bellman_ford_cost > sum( u->bellman_ford_cost, e->bellman_ford_weight ) ) {
                v->bellman_ford_cost = sum( u->bellman_ford_cost, e->bellman_ford_weight );
                v->bellman_ford_predecessor = u;

                v->visited                     = 1;
                ((relax_data*)r_data)->changes = 1;
        }
        return 1;
}

typedef struct {
        Graph  *graph;
} finalize_neg_cycles_data;

int finalize_neg_cycles( Edge *e, void *fnc_data) {
        Vertex *u = e->in;
        Vertex *v = e->out;
        Graph  *g = ((finalize_neg_cycles_data*)fnc_data)->graph;

        if ( v->bellman_ford_cost > sum( u->bellman_ford_cost, e->bellman_ford_weight) ) {
                /* There is a negative cycle. */
                if ( v->bellman_ford_cost != INT_MIN ) {
                        v->bellman_ford_cost = INT_MIN;
                        bfs_bellman_ford( g, v );
                }
        }
        return 1;
}

void bellman_ford( Graph *g, Vertex *src ) {
        int i, j;
        relax_data               *r_data;
        finalize_neg_cycles_data *fnc_data;

        initialize_single_source( g, src );

        r_data          = malloc( sizeof ( relax_data ) );
        r_data->changes = 1;
        for ( i = 0; i < g->size - 1 && r_data->changes; i++ ) {
                r_data->changes = 0;
                for ( j = 0; j < g->size; j++ ) {
                        Vertex *v = get_vertex( g, j );
                        /*
                         * If v has a distance value that has not changed since
                         * the last time its out-edges were relaxed, there is no
                         * need to relax these edges a second time.
                         */
                        if ( v->visited ) {
                                foreach_adjacent_edge( v, &relax, r_data );
                                v->visited = 0;
                        }
                }
        }

        if ( r_data->changes ) {
                fnc_data        = malloc( sizeof ( finalize_neg_cycles_data ) );
                fnc_data->graph = g;
                foreach_edge( g, &finalize_neg_cycles, fnc_data );
                free( fnc_data );
        }
        free( r_data );
}

/*******************************************************************************
 * Main, where the magic happens.
 ******************************************************************************/

int main(void) {
        int i;
        int nlocations, ncosts, headquarters_key;
        const int weight_max_digits = 10;
        Graph *g;
        Edge  *e;

        /* Initialization. */
        if ( scanf( "%u %u\n%u", &nlocations, &ncosts, &headquarters_key ) != 3 ) return -1;
        g = new_graph( nlocations );

        /* Insertion. */
        for ( i = 0; i < ncosts; i++ ) {
                int  u, v;
                char *w = malloc( ( weight_max_digits + 1 )* sizeof (char) );

                if ( scanf( "%u %u ", &u, &v ) != 2 ) return -1;
                w = fgets(w, weight_max_digits, stdin);

                e = add_edge( g, u - 1, v - 1 );
                e->bellman_ford_weight = strtol( w, NULL, 10 );

                free( w );
        }

        /* Get shortest paths. */
        bellman_ford( g, get_vertex( g, headquarters_key - 1 ) );

        /* Print output. */
        for ( i = 0; i < nlocations; i++ ) {
                Vertex *v = get_vertex( g, i );

                char c = v->bellman_ford_cost == INT_MIN ? 'I' :
                         v->bellman_ford_cost == INT_MAX ? 'U' : '\0';

                if ( c == 'I' || c == 'U' ) {
                        printf( "%c\n", c );
                } else {
                        printf( "%d\n", v->bellman_ford_cost );
                }
        }

        free_graph(g);
        return 0;
}

