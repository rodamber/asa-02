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

Edge *new_edge( Vertex *in, Vertex *out, Edge *adjacent ) {
        Edge *e = malloc( sizeof( Edge ) );

        e->in       = in;
        e->out      = out;
        e->adjacent = e;

        return e;
}

typedef struct Graph {
        struct Vertex   **vertices;
        size_t          size;
} Graph;

Graph *new_graph(size_t s) {
        int i;
        Graph *g = malloc( sizeof ( Graph ) );

        g->vertices = malloc( sizeof ( Vertex ) * s );
        g->size     = s;

        for ( i = 0; i < s; i++ ) {
                g->vertices[i]->key      = i;
                g->vertices[i]->adjacent = NULL;
        }
        return g;
}

void free_graph( Graph *g ) {
        int i;
        for ( i = 0; i < g->size; i++ ) {
                Edge *e = g->vertices[i]->adjacent;
                while (e) {
                        Edge *tmp = e;
                        e = e->adjacent;
                        free(tmp);
                }
                free( g->vertices[i] );
        }
}

void add_edge( Graph *g, int in, int out ) {
        Vertex *u = g->vertices[in];
        Vertex *v = g->vertices[out];
        u->adjacent = new_edge( u, v, u->adjacent );
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

void bfs( Graph *g, Vertex *src ) {
        int i;
        Queue *q = new_queue();

        for ( i = 0; i < g->size; i++ ) {
                g->vertices[i]->bfs_color       = WHITE;
                g->vertices[i]->bfs_distance    = INT_MAX; /* Hackish, but it's good enough. */
                g->vertices[i]->bfs_predecessor = NULL;
        }
        src->bfs_color = GRAY;
        src->bfs_distance = 0;

        enqueue( q, src );
        while ( !is_empty( q ) ) {
                Vertex *u = dequeue( q );
                Edge   *e;
                for ( e = u->adjacent; e; e = e->adjacent ) {
                        Vertex *v = e->out;
                        if ( v->bfs_color == WHITE ) {
                                v->bfs_color       = GRAY;
                                v->bfs_distance    = u->bfs_distance + 1;
                                v->bfs_predecessor = u;
                        }
                }
                u->bfs_color = BLACK;
        }
        free(q);
}

/*******************************************************************************
 * Main, where the action happens.
 ******************************************************************************/

int main(void) {
        int i, max_erdos_n;
        int nvertices, nedges, erdos;

        Graph *g;
        int   *erdos_ncount;

        if ( scanf("%d %d\n%d", &nvertices, &nedges, &erdos) != 3 ) return -1;
        g = new_graph( nvertices );

        for ( i = 0; i < nedges; i++ ) {
                int u, v;
                if ( scanf("%d %d", &u, &v) != 2 ) return -1;
                add_edge( g, u, v );
                add_edge( g, v, u );
        }

        bfs( g, g->vertices[erdos] );

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
        for (i = 1; i < max_erdos_n + 1; i++) {
                printf("%d\n", erdos_ncount[i]);
        }

        free( erdos_ncount );
        free_graph( g );
        return 0;
}































#define IS_INFINITE(A) ( A == INT_MIN || A == INT_MAX )

int sum( int a, int b ) {
        if ( IS_INFINITE( a ) ) return a;
        if ( IS_INFINITE( b ) ) return b;
        return a + b;
}
