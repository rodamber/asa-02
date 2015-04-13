#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

/*******************************************************************************
 * Structs
 ******************************************************************************/

typedef struct node
{
    int         weight;
    unsigned    key;
    struct node *next;
} node;

typedef struct
{
    int      cost;
    unsigned key;
    unsigned predecessor;
    node     *head;
} sllist;

#define GRAPH sllist **

sllist *
new_sllist(unsigned key)
{
    sllist *ll = malloc(sizeof (sllist));
    ll->key    = key;
    ll->head   = NULL;
    return ll;
}

void
sllist_insert(sllist *ll, unsigned key, int weight)
{
    node *n   = malloc(sizeof (node));
    n->key    = key;
    n->weight = weight;
    n->next   = ll->head;
    ll-> head = n;
}

void
sllist_free(sllist *ll)
{
    node *n = ll->head;
    while (n)
    {
        node *tmp = n;
        n         = n->next;
        free(tmp);
    }
    free(ll);
}

GRAPH
new_graph(size_t v)
{
    int i;
    GRAPH g = malloc( v * sizeof (sllist *) );
    for ( i = 0; i < v; i++ )
    {
        g[i] = new_sllist( i + 1 );
    }
    return g;
}

void
graph_insert(GRAPH g, int i, unsigned key, int weight)
{
    sllist_insert(g[i], key, weight);
}

void
graph_free(GRAPH g, size_t v)
{
    int i;
    for ( i = 0; i < v; i++ )
    {
        sllist_free(g[i]);
    }
    free(g);
}

/*******************************************************************************
 * Bellman-Ford Algorithm
 ******************************************************************************/

#define NIL 0
#define OVERFLOW(A, B) \
    ( ( A->cost == INT_MIN && B < 0 ) || \
      ( A->cost == INT_MAX && B > 0 ) )

void
initialize_single_source(GRAPH g, size_t v, unsigned src)
{
    int i;
    for ( i = 0; i < v; i++ )
    {
        g[i]->cost        = INT_MAX;
        g[i]->predecessor = NIL;
    }
    g[src - 1]->cost = 0;
}


#define RELAX(u, v, w)                              \
    if ( !OVERFLOW(u, w) && v->cost > u->cost + w ) \
    {                                               \
        v->cost        = u->cost + w;               \
        v->predecessor = u->key;                    \
    }

void
bellman_ford(GRAPH g, size_t v, unsigned src)
{
    int i, j;

    initialize_single_source(g, v, src);
    for ( j = 0; j < v; j++ )
    {
        for ( i = 0; i < v; i++ )
        {
            node *n;
            for ( n = g[i]->head; n; n = n->next )
            {
                RELAX( g[i], g[n->key - 1], n->weight );
            }
        }
    }

    /*
     * Apply corrections to the costs.
     * Cost to source is always 0.
     */
    for ( i = 0; i < v; i++ )
    {
        node *n;
        for ( n = g[i]->head; n; n = n->next )
        {
            if ( g[n->key - 1]->cost > g[i]->cost + n->weight )
            {
                int v = n->key;
                do {
                    g[v - 1]->cost = INT_MIN;
                    v              = g[v - 1]->predecessor;
                }
                while (v && v != n->key && g[v - 1]->cost != INT_MIN);
            }
        }
    }
    g[src - 1]->cost = 0;
}

/*******************************************************************************
 * Main
 ******************************************************************************/

#define WEIGHT_MAX_DIGITS 10

int
main(void)
{
    int i;
    unsigned N, /* Number of locations. */
             C, /* Number of known costs. */
             H; /* Company headquarters. */
    GRAPH g;

    /* Initialization. */
    if ( scanf( "%u %u\n%u", &N, &C, &H ) != 3 ) return -1;
    g = new_graph(N);

    /* Insertion. */
    for ( i = 0; i < C; i++ )
    {
        unsigned u, v;
        char *w = malloc( ( WEIGHT_MAX_DIGITS + 1 )* sizeof (char) );

        if ( scanf( "%u %u ", &u, &v ) != 2 ) return -1;
        w = fgets(w, WEIGHT_MAX_DIGITS, stdin);

        graph_insert(g, u - 1, v, strtol( w, NULL, 10 ));
        free( w );
    }

    /* Get shortest paths. */
    bellman_ford(g, N, H);

    /* Print output. */
    for ( i = 0; i < N; i++ )
    {
        char c = g[i]->cost == INT_MIN ? 'I' :
                 g[i]->cost == INT_MAX ? 'U' : '\0';

        if ( c == 'I' || c == 'U' )
        {
            printf( "%c\n", c );
        }
        else
        {
            printf( "%d\n", g[i]->cost );
        }

    }

    graph_free(g, N);
    return 0;
}

