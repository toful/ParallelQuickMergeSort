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
    /* Source ID */
    int sid;
    /* Destination ID */
    int did;
    /* logic rank */
    int logic_id = 0;


    int ndades, i, parts, porcio;
    
    int *vin, *vout, *tmp;
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

    //int cntr = parts;
    

    if( id == 0 ){
        valors = malloc(sizeof(int) * ndades);
        assert(valors != NULL);
        for( i=0; i<ndades; i++) valors[i]=rand()%MAX_INT;
        
    }
    

    int * sub_valors = malloc(sizeof(int)*porcio*2);

    MPI_Scatter( valors, porcio, MPI_INT, sub_valors, porcio, MPI_INT, 0, MPI_COMM_WORLD );

    // Quicksort a 
    if (id == 0)
    {
        free(valors);
    }
    // Hacer el qs
    qs( sub_valors, porcio);
    if ((id%2)&& (id != 0))
    {
        MPI_Send(sub_valors, porcio, MPI_INT, id-1, 0,MPI_COMM_WORLD);
        //free(sub_valors);
        
        goto end_jmp;
    }else{
      
        MPI_Recv(&sub_valors[porcio], porcio, MPI_INT, id+1, 0, MPI_COMM_WORLD, &status);
        
        porcio *= 2;
    }
    
    logic_id = id / 2;

    vout = malloc(sizeof(int) * porcio);
    did = ((id -2) < 0) ? 0 : id -2 ;
    sid = id + 2;


    while (ndades/porcio > 1)
    {

        merge2(sub_valors, porcio, vout);

        tmp = sub_valors;
        sub_valors = vout;
        vout = tmp;

        if ( (logic_id%2) || (sid > n_procc)){
            MPI_Send(sub_valors, porcio, MPI_INT,  did, 0, MPI_COMM_WORLD);
            //free(sub_valors);
            //free(vout);
            goto end_jmp;
        }

        porcio *= 2;
        sub_valors = realloc(sub_valors, sizeof(int) * porcio);
        vout = realloc(vout, sizeof(int) * porcio);

        MPI_Recv(&sub_valors[porcio/2], porcio/2, MPI_INT, sid, 0, MPI_COMM_WORLD, &status);
        did -= 2;
        sid += 2;
        logic_id /= 2;
    }

    if( id == 0 ){
        //vout = (int *) malloc(sizeof(int)*porcio);

        merge2( sub_valors, porcio, vout );
        vin = vout;
   
        // Validacio
        for ( i=1; i<ndades; i++ ) assert(vin[i-1]<=vin[i]);
        for ( i=0; i<ndades; i+=100 )
            sum += vin[i];
        printf("validacio %lld \n",sum);
    }

end_jmp:

    MPI_Barrier( MPI_COMM_WORLD );
    MPI_Finalize();
    return 0 ;
}

