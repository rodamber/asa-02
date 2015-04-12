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
    int      distance;
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

void
initialize_single_source(GRAPH g, size_t v, unsigned src)
{
    int i;
    for ( i = 0; i < v; i++ )
    {
        g[i]->distance    = INT_MAX;
        g[i]->predecessor = NIL;
    }
    g[src - 1]->distance = 0;
}

#define RELAX(u, v, w)                    \
    if (v->distance > u->distance + w)    \
    {                                     \
        v->distance    = u->distance + w; \
        v->predecessor = u->key;          \
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
            for ( node *n = g[i]->head; n; n = n->next )
            {
                RELAX( g[i], g[n->key - 1], n->weight );
            }
        }
    }
    for ( i = 0; i < v; i++ )
    {
        for ( node *n = g[i]->head; n; n = n->next )
        {
            if ( g[n->key - 1]->distance > g[i]->distance + n->weight )
            {
                g[n->key - 1]->distance = INT_MIN;
            }
        }
    }
}

/*******************************************************************************
 * Main
 ******************************************************************************/

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
        int      w;
        char     sign;

        if ( scanf( "%u %u", &u, &v )      != 2 ) return -1;
        if ( scanf( " %c%d\n", &sign, &w ) == 2 )
        {
            w = -w;
        } else if ( scanf( " %d\n", &w )   != 1 ) return -1;

        graph_insert(g, u - 1, v, w);
    }

    bellman_ford(g, N, H);

    /* Print output. */
    for ( i = 0; i < N; i++ )
    {
        char c;

        switch ( g[i]->distance )
        {
            case INT_MIN :
                c = 'I';
                break;

            case INT_MAX :
                c = 'U';
                break;
        }

        if ( c == 'I' || c == 'U' )
        {
            printf( "%c\n", c );
        }
        else
        {
            printf( "%d\n", g[i]->distance );
        }

    }

    graph_free(g, N);
    return 0;
}
































