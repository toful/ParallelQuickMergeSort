#include <stdio.h>
#include <stdlib.h>
#include <assert.h>  
#include <mpi.h>
#include <string.h>

#define NN 384000000
#define MAX_INT ((int)((unsigned int)(-1)>>1))


int * valors;
//int ranks [128]; 
//int valors2[ NN+1 ];

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
    /* logic rank */
    int logic_id = 0;
    /* index multiplayer */ 
    int ind = 2;

    int ndades, i, parts, porcio;
    
    int *vout, *tmp;
    long long sum=0;

    /* Status */
    MPI_Status status;
    
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
    
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    /* Get number of processes*/
    MPI_Comm_size(MPI_COMM_WORLD, &n_procc);

    int * sub_valors = malloc( sizeof(int) * porcio * 2 );
    assert( sub_valors != NULL );


    for( i=0; i < id * porcio; i++ ) rand();
    for( i=0; i < porcio; i++ ) sub_valors[i] = rand() % MAX_INT;

    // Hacer el qs
    qs( sub_valors, porcio);
    
    // Validate SQ
    for ( i=1; i<porcio; i++ ) assert(sub_valors[i-1]<=sub_valors[i]);
    
    if ( ( id % 2 ) && (id != 0) ){
        MPI_Send(sub_valors, porcio, MPI_INT, id-1, 0,MPI_COMM_WORLD);
        free(sub_valors);
        
        goto end_jmp;
    }
    else{
        MPI_Recv(&sub_valors[porcio], porcio, MPI_INT, id+1, 0, MPI_COMM_WORLD, &status);       
        porcio *= 2;
    }
    
    
    vout = malloc(sizeof(int) * porcio);


    logic_id = id / 2;
    
    
    while ( ndades/porcio > 1 ) {
        merge2( sub_valors, porcio, vout);
        // Assert Limits 
        assert(vout[(porcio/2)-1] <= vout[porcio/2]);
        
        tmp = sub_valors;
        sub_valors = vout;
        vout = tmp;


        if ( (logic_id%2)){
            // DID = id - did
            MPI_Send(sub_valors, porcio, MPI_INT,  id - ind, 0, MPI_COMM_WORLD);
            free( sub_valors );
            free( vout );
            goto end_jmp;
        }

        porcio *= 2;
        sub_valors = realloc(sub_valors, sizeof(int) * porcio);
        vout = realloc(vout, sizeof(int) * porcio);

        // SID = id + ind
        MPI_Recv(&sub_valors[porcio/2], porcio/2, MPI_INT, id+ind , 0, MPI_COMM_WORLD, &status);
        ind = ind*2; 
        logic_id /= 2;
    }

    if( id == 0 ){
        merge2(sub_valors, porcio, vout);
        assert(vout[(porcio/2)-1] <= vout[porcio/2]);
        // Suma 
        for ( i=0; i<ndades; i+=100 )
            sum += vout[i];
        printf("validacio %lld \n",sum);
    }

end_jmp:

    MPI_Barrier( MPI_COMM_WORLD );
    MPI_Finalize();
    return 0 ;
}

