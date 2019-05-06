#include <stdio.h>
#include <stdlib.h>
#include <assert.h>  
#include <mpi.h>
#include <string.h>

#define NN 384000000
#define MAX_INT ((int)((unsigned int)(-1)>>1))

int valors[ NN+1 ];
int valors2[ NN+1 ];

void qs(int *val, int ne)
{
    int i,f;
    int pivot,vtmp,vfi;

    pivot = val[0];
    i = 1;
    f = ne-1; 
    vtmp = val[i];

    while (i <= f)
    {
        if (vtmp < pivot) {
            val[i-1] = vtmp;
            i ++;
            vtmp = val[i];
        }
        else {
            vfi = val[f];
            val[f] = vtmp;
            f --;
            vtmp = vfi;
        }
    }
    val[i-1] = pivot;

    if (f>1) qs(val,f);
    if (i < ne-1) qs(&val[i],ne-f-1);
}

void merge2(int* val, int n, int *vo)
{
    //int vtmp;
    int i, posi, posj; 
    //printf ("Merging : %d    %d\n", val, vo);
    posi = 0;
    posj = (n/2);

    for (i=0;i<n;i++)
        if (((posi < n/2) && (val[posi] <= val[posj])) || (posj >= n))
            vo[i] = val[posi++];
        else if (posj < n)
            vo[i] = val[posj++];
}

int main(int nargs,char* args[])
{
    /* ID of process */
    int id;
    /* Number of processos */
    int n_procc;
    /* Source ID */
    //int sid;
    /* Destination ID */
    //int did;
    /* Message TAG */
    //int tag = 0;

    int ndades, i, j, parts, porcio;
    int *vin, *vout;
    long long sum=0;

    /* Status */
    //MPI_Status status;
    assert(nargs == 3);

    ndades = atoi(args[1]);
    assert(ndades <= NN);

    parts = atoi(args[2]);
    if (parts < 2) assert("Han d'haver dues parts com a minim" == 0);
    if (ndades % parts) assert("N ha de ser divisible per parts" == 0);

    porcio = ndades/parts;

    /* Init MPI */
    //MPI_Init(&nargs, args);
    MPI_Init(NULL, NULL);
    /* Get the id or rank */
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    /* Get number of processes*/
    MPI_Comm_size(MPI_COMM_WORLD, &n_procc);


    if( id == 0 ){
        for( i=0; i<ndades; i++) valors[i]=rand()%MAX_INT;
    }
    
    int *sub_valors = (int *) malloc( sizeof(int) * ndades );
    assert( sub_valors != NULL );

    MPI_Scatter( &valors, porcio, MPI_INT, sub_valors, porcio, MPI_INT, 0, MPI_COMM_WORLD );
    // Quicksort a parts
    qs( sub_valors, porcio);
    MPI_Gather( sub_valors, porcio, MPI_INT, &valors, porcio, MPI_INT, 0, MPI_COMM_WORLD);


    int * scounts = (int *) malloc( sizeof( int ) * n_procc );
    int * displs = (int *) malloc( sizeof( int ) * n_procc );
    int offset;
    
    porcio = porcio * 2;
    while( ndades/porcio > 1 ){
        if ( id == 0 ) {
            offset = 0;
            j = 0;
            for ( i = ndades; i > 0; i = i-porcio ) {
                scounts[j] = porcio;
                displs[j] = offset;
                offset += porcio;
                j++;
            }
            for (; j < n_procc; j++ ){
                scounts[j] = 1;
                displs[j] = offset;
            }
        }

        MPI_Bcast( scounts, n_procc, MPI_INT, 0, MPI_COMM_WORLD );

        //printf("%d tracta abans el scatter\n", id );
        MPI_Scatterv( &valors, scounts, displs, MPI_INT, sub_valors, porcio, MPI_INT, 0, MPI_COMM_WORLD );

        //MPI_Scatter( &valors, porcio, MPI_INT, sub_valors, porcio, MPI_INT, 0, MPI_COMM_WORLD );
        vin = sub_valors;
        vout = valors2;
        //printf("%d tracta %d dades\n", id, scounts[id]);
        merge2( vin, scounts[ id ], vout );
        
        MPI_Gatherv( vout, scounts[ id ], MPI_INT, &valors, scounts, displs, MPI_INT, 0, MPI_COMM_WORLD);
        //printf("%d tracta després el gather\n", id );

        porcio = porcio * 2;
    }

    if( id == 0 ){
        vin = valors;
        vout = valors2;
        merge2( vin, porcio, vout );
        vin = vout;
   
        // Validacio
        for ( i=1; i<ndades; i++ ) assert(vin[i-1]<=vin[i]);
        for ( i=0; i<ndades; i+=100 )
            sum += vin[i];
        printf("validacio %lld \n",sum);
    }

    MPI_Barrier( MPI_COMM_WORLD );
    MPI_Finalize();
    return 0 ;
}

