#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

/*******************************************************************************
 * Graph implementation.
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

void add_edge( Graph *g, int in, int out ) {
        Vertex *u = g->vertices[in];
        Vertex *v = g->vertices[out];
        u->adjacent = new_edge( u, v, u->adjacent );
}

Vertex *get_vertex( Graph *g, int key ) {
        return g->vertices[key];
}

void foreach_vertex( Graph *g, void (*f)( Vertex *, void * ), void *f_data ) {
        int i;
        for ( i = 0; i < g->size; i++ ) {
                f( g->vertices[i], f_data );
        }
}

void foreach_edge( Graph *g, void (*f)( Edge *, void * ), void *f_data ) {
        int i;
        for ( i = 0; i < g->size; i++ ) {
                Edge *e;
                for ( e = g->vertices[i]->adjacent; e; e = e->adjacent ) {
                        f( e, f_data );
                }
        }

}

void foreach_adjacent_edge( Vertex *v, void (*f)( Edge *, void * ), void *f_data ) {
        Edge *e;
        for ( e = v->adjacent; e; e = e->adjacent ) {
                f( e, f_data );
        }
}

/*******************************************************************************
 * Queue implementation.
 ******************************************************************************/

typedef Vertex Data;

typedef struct Qnode {
        Data            *data;
        struct Qnode    *next;
} Qnode;

struct Qnode *new_qnode( Data *d ) {
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

void enqueue ( Queue *q, Data *d ) {
        Qnode *qn = new_qnode( d );

        if ( q->first ) {
                q->last->next = qn;
                q->last       = q->last->next;
        } else {
                q->first = q->last = qn;
        }
}

Data *dequeue( Queue *q ) {
        Data  *d   = q->first->data;
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
 * Breadth-First Search (Cormen, Leiserson, Rivest, and Stein, 3rd ed., p. 595).
 ******************************************************************************/

#define WHITE 0
#define GRAY  1
#define BLACK 2

void bfs_initialize_vertex( Vertex *v, void *null ) {
        v->bfs_color       = WHITE;
        v->bfs_distance    = INT_MAX; /* Hackish, but works. */
        v->bfs_predecessor = NULL;
}

void grayify( Edge *e, void *queue ) {
        Queue  *q = (Queue *) queue;
        Vertex *u = e->in;
        Vertex *v = e->out;

        if ( v->bfs_color == WHITE ) {
                v->bfs_color       = GRAY;
                v->bfs_distance    = u->bfs_distance + 1;
                v->bfs_predecessor = u;
                enqueue( q, v );
        }
}

void bfs( Graph *g, Vertex *src ) {
        Queue *q = new_queue();

        foreach_vertex( g, &bfs_initialize_vertex, (void *) NULL );
        src->bfs_color    = GRAY;
        src->bfs_distance = 0;

        enqueue( q, src );
        while ( !is_empty( q ) ) {
                Vertex *u    = dequeue( q );
                foreach_adjacent_edge( u, &grayify, (void *) q);
                u->bfs_color = BLACK;
        }
        free(q);
}

/*******************************************************************************
 * Bellman-Ford single-source shortest path algorithm
 * (modified from Cormen, Leiserson, Rivest, and Stein, 3rd ed., p. 595).
 ******************************************************************************/

#define _BF_IS_INFINITE(A) ( A == INT_MIN || A == INT_MAX )

int sum( int a, int b ) {
        if ( _BF_IS_INFINITE( a ) ) return a;
        if ( _BF_IS_INFINITE( b ) ) return b;
        return a + b;
}

void bellman_ford_initialize_vertex( Vertex *v, void *null ) {
        v->bellman_ford_cost        = INT_MAX; /* Hackish, but works. */
        v->bellman_ford_predecessor = NULL;
}

void initialize_single_source( Graph *g, Vertex *src ) {
        foreach_vertex( g, &bellman_ford_initialize_vertex, (void *) NULL );
        src->bellman_ford_cost = 0;
}

void relax( Edge *e, void *null) {
        Vertex *u = e->in;
        Vertex *v = e->out;

        if ( v->bellman_ford_cost > u->bellman_ford_cost + e->bellman_ford_weight ) {
                v->bellman_ford_cost = u->bellman_ford_cost + e->bellman_ford_weight;
                v->bellman_ford_predecessor = u;
        }
}

void finalize_neg_cycles( Edge *e, void *null ) {
        Vertex *u = e->in;
        Vertex *v = e->out;

        if ( v->bellman_ford_cost > u->bellman_ford_cost + e->bellman_ford_weight ) {
                /* do something */
                printf("There's a silly negative cycle.\n");
        }
}

void bellman_ford( Graph *g, Vertex *src ) {
        int i;
        initialize_single_source( g, src );
        for ( i = 0; i < g->size - 1; i++ ) {
                foreach_edge( g, &relax, (void *) NULL );
        }
        foreach_edge( g, &finalize_neg_cycles, (void *) NULL );
}



/*******************************************************************************
 * Main, where the action happens.
 ******************************************************************************/

int fst_project(void) {
        int i, max_erdos_n;
        int nvertices, nedges, erdos;

        Graph *g;
        int   *erdos_ncount;

        if ( scanf("%d %d\n%d", &nvertices, &nedges, &erdos) != 3 ) return -1;
        g = new_graph( nvertices );

        for ( i = 0; i < nedges; i++ ) {
                int u, v;
                if ( scanf("%d %d", &u, &v) != 2 ) return -1;
                add_edge( g, u - 1, v - 1 );
                add_edge( g, v - 1, u - 1 );
        }

        bfs( g, g->vertices[erdos - 1] );

        for ( i = 0, max_erdos_n = 0; i < nvertices; i++ ) {
                if ( g->vertices[i]->bfs_distance > max_erdos_n ) {
                        max_erdos_n = g->vertices[i]->bfs_distance;
                }
        }

        erdos_ncount = calloc( max_erdos_n + 1, sizeof ( int ) );
        for ( i = 0; i < nvertices; i++ ) {
                erdos_ncount[ g->vertices[i]->bfs_distance ]++;
        }

        printf("%d\n", max_erdos_n);
        for ( i = 1; i < max_erdos_n + 1; i++ ) {
                printf("%d\n", erdos_ncount[i]);
        }

        free( erdos_ncount );
        free_graph( g );
        return 0;
}

int main(void) {
        snd_project();
        return 0;
}

