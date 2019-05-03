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
    int i,f,j;
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

void merge2(int* val, int n,int *vo)
{
    int vtmp;
    int i,j,posi,posj; 
    printf ("Merging : %d    %d\n", val, vo);
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
    /* Message TAG */
    int tag = 0;

    /* Status */
    MPI_Status status;

    int ndades,i,m,parts,porcio;
    int *vin,*vout,*vtmp, *recv, *disp;
    long long sum=0;

    assert(nargs == 3);

    ndades = atoi(args[1]);
    assert(ndades <= NN);

    parts = atoi(args[2]);
    if (parts < 2) assert("Han d'haver dues parts com a minim" == 0);
    if (ndades % parts) assert("N ha de ser divisible per parts" == 0);

    /* Init MPI */
    MPI_Init(&nargs, args);
    /* Get the id or rank */
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    /* Get number of processes*/
    MPI_Comm_size(MPI_COMM_WORLD, &n_procc);

    for(i=0;i<ndades;i++) valors[i]=rand()%MAX_INT;

    porcio = ndades/parts;
    sid = id;

    /* Space to recieve data */
    
    recv = (int *) malloc(sizeof(int)*porcio);
    /* Space for available data */
    disp = (int *) malloc(sizeof(int)*porcio*2);

    /* QuickSort */ 
    qs(&valors[id*porcio], porcio);
    printf("%d finished qs\n",id);
    /* Copy for easy use */
    memcpy(disp, &valors[id*porcio], porcio*sizeof(int));
    //printf("%d valors copiats\n",id);
    vout = valors2;
    
    if (id == 0){

        //printf("[%d] before recieve from %d\n", id,id+1);
        MPI_Recv(&disp[porcio], porcio, MPI_INT, id+1, tag, MPI_COMM_WORLD, &status);

        porcio *= 2 ;
        
        printf("[%d] recieve from %d\n", id,id+1);
        merge2(disp, porcio, vout);
        printf("[%d] merged with %d \n", id, id+1);
        memcpy(disp, vout, porcio*sizeof(int));
        //printf("[%d] copied after merged\n", id);
        /* Double the space for disponibility */
        disp = (int *) realloc(disp, sizeof(int)*porcio*2);
        /*
        if(status == MPI_ERROR)
        {
            fprintf(stderr, "Error, no se ha pogut acabar de completar la execusio\n");
            MPI_Finalize();
            exit(0)
        }
        */
        while (1){
            /* Get from sourc in hop 2 */ 
            sid  = sid + 2;
            printf("[%d] SID: %d\n",id, sid );
            if ((parts%sid) || sid >= n_procc){
                printf("[%d] Finishing parllel", id);
                break;
            }

            //printf("[%d] before recieve from %d\n", id,sid);
            MPI_Recv(&disp[porcio], porcio, MPI_INT, sid, tag, MPI_COMM_WORLD, &status);
            printf("[%d] recieve from %d\n", id, sid);
            //memcopy(disp[porcio], recv, porcio);
            porcio = porcio * 2;
            merge2(disp, porcio, vout);
            printf("[%d] merged with %d \n", id, sid);
            memcpy(disp, vout, porcio*sizeof(int));
            //printf("[%d] copied after merged\n", id);
         
            
            /* Double the space */
            disp = (int *) realloc(disp, sizeof(int)*porcio * 2);
        }

        

    }else{
        
        //MPI_Recv(&recv, porcio+1, MPI_INT, id+1, tag, MPI_COMM_WORLD, &status);
        /*
        if(status == MPI_ERROR)
        {
            fprintf(stderr, "Error, no se ha pogut acabar de completar la execusio\n");
            MPI_Finalize();
            exit(0)
        }
        */
        if ((id % 2))
        {
            /* If it is a odd number send the results to previous*/
            MPI_Ssend(disp, porcio, MPI_INT, id-1, tag, MPI_COMM_WORLD);            
            printf("[%d] sent data to %d\n",id, id-1);
        }else{
            //printf("[%d] before recieve from %d\n", id,id+1);
            MPI_Recv(&disp[porcio], porcio, MPI_INT, id+1, tag, MPI_COMM_WORLD, &status);
            
            porcio = porcio * 2;

            printf("[%d] recieve from %d\n", id,id+1);
            merge2(disp, porcio, vout);
            printf("[%d] merged with %d \n", id, id+1);
            memcpy(disp, vout, porcio*sizeof(int));
           // printf("[%d] copied after merged\n", id);
            /* Double the spce */
            
            /* Double the space */
            disp = (int *) realloc(disp, sizeof(int)*porcio * 2);
        
            did = id;
            while(1)
            {
                /* Get from sourc in hop 2 */ 
                sid  = sid + 2;
                //printf("SID in loop %d\n", sid);
                /* Get to destination in 2 hops */
                did = did - 2;
                if ((!(parts%sid) && (parts%id))|| (sid >= n_procc))
                {
                    printf("[%d] sent data to %d\n",id, did);
                    MPI_Ssend(disp, porcio, MPI_INT, did, tag, MPI_COMM_WORLD);
                    break;
                }else{
                
                    MPI_Recv(&disp[porcio], porcio, MPI_INT, sid, tag, MPI_COMM_WORLD, &status);
                    printf("[%d] recieve from %d\n", id,sid);
                    //memcopy(disp[porcio], recv, porcio);
                    porcio = porcio * 2;

                    merge2(disp, porcio, vout);
                    printf("[%d] merged with %d \n", id, sid);
                    memcpy(disp, vout, porcio*sizeof(int));
                    //printf("[%d] copied after merged\n", id);
                    /* Double the spce */
                    
                    /* Double the space */
                    disp = (int *) realloc(disp, sizeof(int)*porcio * 2);
                }
            }
        }

    }

    /*    
    // Quicksort a parts
    for (i=0; i<parts; i++)
        qs(&valors[i*porcio],porcio);

    // Merge en arbre
    vin = valors;
    vout = valors2;
    for (m = 2*porcio; m <= ndades; m *= 2)
    {
        for (i = 0; i < ndades; i += m)
            merge2(&vin[i],m,&vout[i]);
        vtmp=vin;
        vin=vout;
        vout=vtmp;
    }

    // Validacio
    for (i=1;i<ndades;i++) assert(vin[i-1]<=vin[i]);
    for (i=0;i<ndades;i+=100)
        sum += vin[i];
    printf("validacio %lld \n",sum);
    */
    printf ("[%d] Difference  from ndades %d\n", id, ndades-porcio);
    printf("[%d] Esperando en barrera \n", id);
    MPI_Barrier(MPI_COMM_WORLD);
    /* Validate */
    if (id == 0){
        printf("After the barrier");
        printf("Porcio: %d   Ndades: %d \n",porcio, ndades );
        for (i=1;i<ndades;i++) assert(disp[i-1]<=disp[i]);
        for (i=0;i<ndades;i+=100)
            sum += disp[i];
        printf("validacio %lld \n",sum);

        MPI_Finalize();
    }
    return (0);
}

