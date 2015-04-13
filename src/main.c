#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

/*******************************************************************************
 * Structs
 ******************************************************************************/

typedef struct node {
int flag;
        int         weight;
        unsigned    key;
        struct node *next;
} node;

typedef struct {
        int      loss;
        unsigned key;
        unsigned predecessor;
        node     *head;
} sllist;

#define GRAPH sllist **

sllist *new_sllist(unsigned key) {
        sllist *ll = malloc(sizeof (sllist));
        ll->key    = key;
        ll->head   = NULL;
        return ll;
}

void sllist_insert(sllist *ll, unsigned key, int weight) {
        node *n   = malloc(sizeof (node));
        n->key    = key;
        n->weight = weight;
        n->next   = ll->head;
        ll-> head = n;
}

void sllist_free(sllist *ll) {
        node *n = ll->head;
        while (n) {
                node *tmp = n;
                n         = n->next;
                free(tmp);
        }
        free(ll);
}

GRAPH new_graph(size_t v) {
        int i;
        GRAPH g = malloc( v * sizeof (sllist *) );
        for ( i = 0; i < v; i++ ) {
                g[i] = new_sllist( i );
        }
        return g;
}

void graph_insert(GRAPH g, int i, unsigned key, int weight) {
        sllist_insert(g[i], key, weight);
}

void graph_free(GRAPH g, size_t v) {
        int i;
        for ( i = 0; i < v; i++ ) {
                sllist_free(g[i]);
        }
        free(g);
}

/*******************************************************************************
 * Bellman-Ford Algorithm
 ******************************************************************************/

#define NIL -1
#define IS_INFINITE(A) ( A == INT_MIN || A == INT_MAX )

int sum( int a, int b ) {
        if ( IS_INFINITE( a ) ) return a;
        if ( IS_INFINITE( b ) ) return b;
        return a + b;
}

void initialize_single_source(GRAPH g, size_t v, unsigned src) {
        int i;
        for ( i = 0; i < v; i++ ) {
                g[i]->loss        = INT_MAX;
                g[i]->predecessor = NIL;
        }
        g[src]->loss = 0;
}

void relax(sllist *u, sllist *v, int w) {
        if ( v->loss > sum( u->loss, w ) ) {
                v->loss        = sum( u->loss, w );
                v->predecessor = u->key;
        }
}

void bellman_ford(GRAPH g, size_t v, unsigned src) {
        int i, j;
        int *predecessor_chain = calloc( v, sizeof ( int ) );

        initialize_single_source(g, v, src);

        for ( j = 0; j < v; j++ ) {
                for ( i = 0; i < v; i++ ) {
                        node *n;
                        for ( n = g[i]->head; n; n = n->next ) {
                                relax( g[i], g[n->key], n->weight );
                        }
                }
        }

        for ( i = 0; i < v; i++ ) {
                node *n;
                for ( n = g[i]->head; n; n = n->next ) {
                        if ( g[n->key]->loss > sum( g[i]->loss, n->weight ) ) {
                                int x = n->key;
                                while ( predecessor_chain[x] != 1 ) {
                                        g[x]->loss = INT_MIN;
                                        predecessor_chain[x] = 1;
                                        x = g[x]->predecessor;
                                }
                                for ( j = 0; j < v; j++ ) {
                                        predecessor_chain[j] = 0;
                                }
                        }
                }
        }
        free( predecessor_chain );
}

/*******************************************************************************
 * Main
 ******************************************************************************/

#define WEIGHT_MAX_DIGITS 10

int main(void) {
        int i;
        unsigned N, /* Number of locations. */
                 C, /* Number of known costs. */
                 H; /* Company headquarters. */
        GRAPH g;

        /* Initialization. */
        if ( scanf( "%u %u\n%u", &N, &C, &H ) != 3 ) return -1;
        g = new_graph(N);

        /* Insertion. */
        for ( i = 0; i < C; i++ ) {
                unsigned u, v;
                char *w = malloc( ( WEIGHT_MAX_DIGITS + 1 )* sizeof (char) );

                if ( scanf( "%u %u ", &u, &v ) != 2 ) return -1;
                w = fgets(w, WEIGHT_MAX_DIGITS, stdin);

                graph_insert(g, u - 1, v - 1, strtol( w, NULL, 10 ));
                free( w );
        }

        /* Get shortest paths. */
        bellman_ford(g, N, H - 1);

        /* Print output. */
        for ( i = 0; i < N; i++ ) {
                char c = g[i]->loss == INT_MIN ? 'I' :
                         g[i]->loss == INT_MAX ? 'U' : '\0';

                if ( c == 'I' || c == 'U' ) {
                        printf( "%c\n", c );
                } else {
                        printf( "%d\n", g[i]->loss );
                }

        }

        graph_free(g, N);
        return 0;
}

