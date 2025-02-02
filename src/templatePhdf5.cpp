#include "templateForest.h"
#include "hdf5.h"
#include "templatePhdf5.h"
#define H5FILE_NAME "soln/Pxdmf3d%u.h5"
#define XDMF_NAME "soln/Pxdmf3d%u.xmf"
#define H5FILE "Pxdmf3d%u.h5"

// template <size_t N, typename Nvalue,size_t M,typename Mvalue, class T>
template <size_t N, typename Nvalue, size_t M, typename Mvalue, class T>
// void templatePhdf5<N,Nvalue,M,Mvalue, T>::writePolyvertex(templateForest<N, Nvalue, M, Mvalue, T> &F, uint appx)
void templatePhdf5<N, Nvalue, M, Mvalue, T>::writePolyvertex( TemplateForest<N, Nvalue, M, Mvalue, T> &F, uint appx )
{

    hid_t file_id, dset_id;    /* file and dataset identifiers */
    hid_t filespace, memspace; /* file and memory dataspace identifiers */
    hsize_t count;             /* hyperslab selection parameters */
    hsize_t block;
    hsize_t offset;
    hid_t plist_id; /* property list identifier */
    uint i, j, k, l;
    herr_t status;
    // int         *data=NULL;
    hsize_t total_size;

    /*
     * MPI variables
     */

    int mpi_size, mpi_rank;
    double *xtemp = nullptr;
    double *ytemp = nullptr;
    double *ztemp = nullptr;
    double *qtemp = nullptr;

    //  uint forest.size

    uint partialforestsize = F.getTotalSize()* ( F.npx * F.npy * F.npz ); /*!<the forest size for each processor */

    // do an MPI comunication to get the offset

    xtemp = new double[partialforestsize];
    ytemp = new double[partialforestsize];
    ztemp = new double[partialforestsize];
    qtemp = new double[partialforestsize];

    double Xc;
    double Yc;
    double Zc;

    char str0[50];
    char str1[50];
    char str2[50];
    char str3[50];
    char str[50];

    sprintf( str, H5FILE_NAME, appx );

    CommPoint2Point<uint> com( &partialforestsize, 1 );
    uint offset1 = 0, totalvalue;
    com.getOffset( partialforestsize, &offset1 );

    CommCollective<uint> comc( nullptr, 1 );
    comc.getTotalNumber( &offset1, &partialforestsize, &totalvalue );

    total_size = totalvalue;

    totalnumber = totalvalue;

    //cout << "number of elements" << totalvalue << endl;

    // comc.getTotalNumber(&totalvalue);
    /*
     * Set up file access property list with parallel I/O access
     */

    plist_id = H5Pcreate( H5P_FILE_ACCESS );

    H5Pset_fapl_mpio( plist_id, MPI_COMM_WORLD, MPI_INFO_NULL );

    /*
     * Create a new file collectively and release property list identifier.
     */

    file_id = H5Fcreate( str, H5F_ACC_TRUNC, H5P_DEFAULT, plist_id );
    H5Pclose( plist_id );

    /*
      * Create the dataspace for the dataset.
      */

    /*
     * Each process defines dataset in memory and writes it to the hyperslab
     * in the file.
     */

    //=====================save coordinates to generate grid=======================
    //
    //                               write X
    //
    //=============================================================================

    morton<N> key;
    real xc[3];
    real XYZ[6];
	real Xa;
    real Xb;
    real Ya;
    real Yb;
    real Za;
    real Zb;

    real hx;
    real hy;
    real hz;
    real Xh;
    real Yh;
    real Zh;
    uint L1 = F.npx;
    uint M1 = F.npy;
    uint N1 = F.npz;

    count = 0;

for ( auto it = F.trees.begin(); it != F.trees.end(); it++ )
    {
        for ( auto it2 = ( *it ).begin(); it2 != ( *it ).end(); it2++ )
        {
            key = ( it2->first );
            ( *it ).enclosingBox( key, XYZ );

            //        proc.enclosingBox(key,XYZ);

            Xa = XYZ[0];
            Xb = XYZ[1];
            hx = L1;
            Xh = ( Xb - Xa ) / ( hx );

            //   index = 0;

            for ( uint j = 0; j < L1; j++ )
            {
                for ( uint k = 0; k < M1; k++ )
                {
                    for ( uint l = 0; l < N1; l++ )
                    {
                        xtemp[count] = Xa + Xh * j + 0.5 * Xh;
                       // cout<<xtemp[count]<<endl;
                        count++;
                    }
                }
            }
        }
    }


 count = 0;

    for ( auto it = F.trees.begin(); it != F.trees.end(); it++ )
    {
        for ( auto it2 = ( *it ).begin(); it2 != ( *it ).end(); it2++ )
        {
            key = ( it2->first );
            ( *it ).enclosingBox( key, XYZ );

            //       proc.enclosingBox(key,XYZ);
            //       cout<<XYZ[0]<<" "<<XYZ[1]<<" "<<XYZ[2]<<" "<<XYZ[3]<<" "<<XYZ[4]<<" "<<XYZ[5]<<endl;

            Ya = XYZ[2];
            Yb = XYZ[3];
            hy = M1;
            Yh = ( Yb - Ya ) / ( hy );

            //     index = 0;

            for ( uint j = 0; j < L1; j++ )
            {
                for ( uint k = 0; k < M1; k++ )
                {
                    for ( uint l = 0; l < N1; l++ )
                    {
                        ytemp[count] = Ya + Yh * k + 0.5 * Yh;
                        count++;
                    }
                }
            }
        }
    }

  count = 0;
    for ( auto it = F.trees.begin(); it != F.trees.end(); it++ )
    {
        for ( auto it2 = ( *it ).begin(); it2 != ( *it ).end(); it2++ )
        {
            key = ( it2->first );
            ( *it ).enclosingBox( key, XYZ );

            // proc.enclosingBox(key,XYZ);
            Za = XYZ[4];
            Zb = XYZ[5];
            hz = N1;
            Zh = ( Zb - Za ) / ( hz );

            //         index = 0;

            for ( uint j = 0; j < L1; j++ )
            {
                for ( uint k = 0; k < M1; k++ )
                {
                    for ( uint l = 0; l < N1; l++ )
                    {
                        ztemp[count] = Za + Zh * l + 0.5 * Zh;
                        count++;
                    }
                }
            }
        }
    }


 count = 0;
 int aux;
    for ( auto it = F.trees.begin(); it != F.trees.end(); it++ )
    {
        for ( auto it2 = ( *it ).begin(); it2 != ( *it ).end(); it2++ )
        {
            key = ( it2->first );
            ( *it ).enclosingBox( key, XYZ );

            // proc.enclosingBox(key,XYZ);
                                  
            for ( uint l = 0; l < N1; l++ )
            {
                for ( uint k = 0; k < M1; k++ )
                {
                    for ( uint j = 0; j < L1; j++ )
                    {
                        //qtemp[count] = ( ( it2->second )[( N1 + 2 ) * ( M1 + 2 ) * j + ( N1 + 2 ) * k + l + 1] ).p;
                        aux=1;
                        qtemp[count] += ( ( it2->second )[( N1 + 1 ) * ( M1 + 1 ) * j + ( N1 + 1 ) * k + l + 1] ).p;
                        if(j>1)
                        {
                         qtemp[count] += ( ( it2->second )[( N1 + 1 ) * ( M1 + 1 ) * (j-1) + ( N1 + 1 ) * k + l + 1] ).p;
                         aux++;
                         }
                        if(j<L1-1)
                        {
                         qtemp[count] += ( ( it2->second )[( N1 + 1 ) * ( M1 + 1 ) * (j+1) + ( N1 + 1 ) * k + l + 1] ).p;
                         aux++;
                         }
                        if(k>1)
                        {
                         qtemp[count] += ( ( it2->second )[( N1 + 1 ) * ( M1 + 1 ) * (j) + ( N1 + 1 ) * (k-1) + l + 1] ).p;
                         aux++;
                         }
                        if(k<M1-1)
                        {
                         qtemp[count] += ( ( it2->second )[( N1 + 1 ) * ( M1 + 1 ) * (j) + ( N1 + 1 ) * (k+1) + l + 1] ).p;
                         aux++;
                         }
                        if(l>1)
                        {
                         qtemp[count] += ( ( it2->second )[( N1 + 1 ) * ( M1 + 1 ) * (j) + ( N1 + 1 ) * (k) + l + 1 + 1] ).p;
                         aux++;
                         }
                        if(l<N1-1)
                        {
                         qtemp[count] += ( ( it2->second )[( N1 + 1 ) * ( M1 + 1 ) * (j) + ( N1 + 1 ) * (k) + l +1 - 1] ).p;
                         aux++;
                         }


                      qtemp[count] =  qtemp[count]/(double)aux; 
                        
;
              //          cout<<( ( it2->second )[( N1 + 2 ) * ( M1 + 2 ) * j + ( N1 + 2 ) * k + l + 1] ).p<<endl;

                        // index = (N1+2)*(M1+2)*j+(N1+2)*k+l+1;

                        count++;
                    }
                }
            }
        }
    }



/*

    for ( auto it = F.trees.begin(); it != F.trees.end(); it++ )
    {
        for ( auto it2 = ( *it ).begin(); it2 != ( *it ).end(); it2++ )
        {
            key = ( it2->first );
            ( *it ).centroid( key, xc );
            xtemp[count] = xc[0];
            ytemp[count] = xc[1];
            ztemp[count] = xc[2];
            
//            qtemp[count] = (it2->second)[0].p;

            // qtemp[count] = com.myRank();
            //        coord_index=cube.at(i).centeroid_index;
            //        Xc=XYZ.at(coord_index).x;
            //        xtemp[i]=Xc;
            //          cout<<it2->first<<'\t'<<com.myRank()<<endl;
            count++;
        }
    }

//    count = F.getTotalSize();
*/
  
   offset = offset1;
    block = 1;

    filespace = H5Screate_simple( 1, &total_size, NULL );
    memspace = H5Screate_simple( 1, &count, NULL );

    //================================================================================
    //
    //                               write X
    //
    //================================================================================

    sprintf( str0, "/X" );
    plist_id = H5Pcreate( H5P_DATASET_CREATE );
    dset_id = H5Dcreate( file_id, str0, H5T_NATIVE_DOUBLE, filespace, H5P_DEFAULT, plist_id, H5P_DEFAULT );
    H5Pclose( plist_id );
    H5Sclose( filespace );

    filespace = H5Dget_space( dset_id );
    H5Sselect_hyperslab( filespace, H5S_SELECT_SET, &offset, nullptr, &count, &block );
    plist_id = H5Pcreate( H5P_DATASET_XFER );
    // H5Pset_dxpl_mpio(plist_id, H5FD_MPIO_COLLECTIVE);
    H5Pset_dxpl_mpio( plist_id, H5FD_MPIO_INDEPENDENT );
    // status = H5Dwrite(dset_id, H5T_NATIVE_DOUBLE, memspace,filespace,plist_id, xtemp);

    if ( sizeof( real ) == sizeof( double ) )
    {
        H5Dwrite( dset_id, H5T_NATIVE_DOUBLE, memspace, filespace, plist_id, xtemp );
    }
    else if ( sizeof( real ) == sizeof( float ) )
    {

        H5Dwrite( dset_id, H5T_NATIVE_FLOAT, memspace, filespace, plist_id, xtemp );
    }
    H5Pclose( plist_id );

    //================================================================================
    //
    //                               write Y
    //
    //================================================================================

    sprintf( str0, "/Y" );
    plist_id = H5Pcreate( H5P_DATASET_CREATE );
    dset_id = H5Dcreate( file_id, str0, H5T_NATIVE_DOUBLE, filespace, H5P_DEFAULT, plist_id, H5P_DEFAULT );
    H5Pclose( plist_id );
    H5Sclose( filespace );

    filespace = H5Dget_space( dset_id );

    H5Sselect_hyperslab( filespace, H5S_SELECT_SET, &offset, NULL, &count, &block );
    plist_id = H5Pcreate( H5P_DATASET_XFER );
    // H5Pset_dxpl_mpio(plist_id, H5FD_MPIO_COLLECTIVE);
    H5Pset_dxpl_mpio( plist_id, H5FD_MPIO_INDEPENDENT );
    // status = H5Dwrite(dset_id, H5T_NATIVE_DOUBLE, memspace,filespace,plist_id, xtemp);

    if ( sizeof( real ) == sizeof( double ) )
    {
        H5Dwrite( dset_id, H5T_NATIVE_DOUBLE, memspace, filespace, plist_id, ytemp );
    }
    else if ( sizeof( real ) == sizeof( float ) )
    {

        H5Dwrite( dset_id, H5T_NATIVE_FLOAT, memspace, filespace, plist_id, ytemp );
    }

    H5Pclose( plist_id );

    //================================================================================
    //
    //                               write Z
    //
    //================================================================================

    sprintf( str0, "/Z" );
    plist_id = H5Pcreate( H5P_DATASET_CREATE );
    dset_id = H5Dcreate( file_id, str0, H5T_NATIVE_DOUBLE, filespace, H5P_DEFAULT, plist_id, H5P_DEFAULT );
    H5Pclose( plist_id );
    H5Sclose( filespace );

    filespace = H5Dget_space( dset_id );

    H5Sselect_hyperslab( filespace, H5S_SELECT_SET, &offset, NULL, &count, &block );
    plist_id = H5Pcreate( H5P_DATASET_XFER );
    // H5Pset_dxpl_mpio(plist_id, H5FD_MPIO_COLLECTIVE);
    H5Pset_dxpl_mpio( plist_id, H5FD_MPIO_INDEPENDENT );
    // status = H5Dwrite(dset_id, H5T_NATIVE_DOUBLE, memspace,filespace,plist_id, xtemp);

    if ( sizeof( real ) == sizeof( double ) )
    {
        H5Dwrite( dset_id, H5T_NATIVE_DOUBLE, memspace, filespace, plist_id, ztemp );
    }
    else if ( sizeof( real ) == sizeof( float ) )
    {

        H5Dwrite( dset_id, H5T_NATIVE_FLOAT, memspace, filespace, plist_id, ztemp );
    }

    H5Pclose( plist_id );

    // ask ray to see what is wrong about closing
    // H5Fclose(file_id);

    //================================================================================
    //
    //                               write Z
    //
    //================================================================================

    sprintf( str0, "/Q" );
    plist_id = H5Pcreate( H5P_DATASET_CREATE );
    dset_id = H5Dcreate( file_id, str0, H5T_NATIVE_DOUBLE, filespace, H5P_DEFAULT, plist_id, H5P_DEFAULT );
    H5Pclose( plist_id );
    H5Sclose( filespace );

    filespace = H5Dget_space( dset_id );

    /*
      for(unsigned int i=0;i<cube.size();i++)
        {
               qtemp[i]=cube.at(i).q;
         }
    */

    H5Sselect_hyperslab( filespace, H5S_SELECT_SET, &offset, NULL, &count, &block );
    plist_id = H5Pcreate( H5P_DATASET_XFER );
    // H5Pset_dxpl_mpio(plist_id, H5FD_MPIO_COLLECTIVE);
    H5Pset_dxpl_mpio( plist_id, H5FD_MPIO_INDEPENDENT );
    // status = H5Dwrite(dset_id, H5T_NATIVE_DOUBLE, memspace,filespace,plist_id, xtemp);

    if ( sizeof( real ) == sizeof( double ) )
    {
        H5Dwrite( dset_id, H5T_NATIVE_DOUBLE, memspace, filespace, plist_id, qtemp );
    }
    else if ( sizeof( real ) == sizeof( float ) )
    {

        H5Dwrite( dset_id, H5T_NATIVE_FLOAT, memspace, filespace, plist_id, qtemp );
    }

    H5Pclose( plist_id );
    H5Dclose( dset_id );
    H5Sclose( filespace );
    H5Sclose( memspace );

    delete[] xtemp;
    delete[] ytemp;
    delete[] ztemp;
    delete[] qtemp;

    // XdmfParallelPolyvertex(cube,my_offset,comm,info,ncube_total);

    xdmfPolyvertex( com.myRank(), appx );
}

template <size_t N, typename Nvalue, size_t M, typename Mvalue, class T>
void templatePhdf5<N, Nvalue, M, Mvalue, T>::xdmfPolyvertex( integer myrank, uint appx )
{
    const char *names[] = {"X", "Y", "Z"};

    char str[1000], fname[1000];
    uint ncube_total = totalnumber;

    // FILE *fp=NULL;
    sprintf( str, XDMF_NAME, appx );
    // fp=fopen(str,"w" );

    MPI_File fp;
    MPI_File_open( MPI_COMM_WORLD, str, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fp );

    // printf("%s\n",str);
    // dont need to specify the address here as they are both reside in one folder, that is
    // why I use H5FILE in addition to H5FILE_NAME
    sprintf( fname, H5FILE, appx );
    MPI_Status status;

    if ( myrank == 0 )
    {
        sprintf( str, "<?xml version=\"1.0\" ?>\n" );
        MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
        sprintf( str, "<!DOCTYPE Xdmf SYSTEM \"Xdmf.dtd\" []> \n" );
        MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
        sprintf( str, "<Xdmf Version=\"2.0\">\n" );
        MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
        sprintf( str, "<Domain>\n" );

        MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
        sprintf( str, "   <Grid Name=\"mesh0\" GridType=\"Uniform\">\n" );
        MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
        sprintf( str, "        <Topology TopologyType=\"Polyvertex\" NumberOfElements=\"%d\"/>\n", ncube_total );

        MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
        sprintf( str, "          <Geometry GeometryType=\"X_Y_Z\">  \n" );

        MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
        sprintf( str, "          <DataItem Name=\"X\" Dimensions=\"%d\" NumberType=\"Float\" Precision=\"4\" Format=\"HDF\">\n",
                 ncube_total );
        MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
        sprintf( str, "          %s:/%s\n", fname, names[0] );

        MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
        sprintf( str, "         </DataItem>\n" );
        MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
        sprintf( str, "          <DataItem Name=\"Y\" Dimensions=\"%d\" NumberType=\"Float\" Precision=\"4\" Format=\"HDF\">\n",
                 ncube_total );
        MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
        sprintf( str, "          %s:/%s\n", fname, names[1] );
        MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
        sprintf( str, "         </DataItem>\n" );
        MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
        sprintf( str, "          <DataItem Name=\"Z\" Dimensions=\"%d\" NumberType=\"Float\" Precision=\"4\" Format=\"HDF\">\n",
                 ncube_total );
        MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
        sprintf( str, "          %s:/%s\n", fname, names[2] );
        MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
        sprintf( str, "         </DataItem>\n" );
        MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
        sprintf( str, "      </Geometry>   \n" );
        MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
        sprintf( str, "         <Attribute Name=\"Q\" AttributeType=\"Scalar\" Center=\"Node\"> \n" );
        MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
        sprintf( str, "          <DataItem Name=\"Q\" Dimensions=\"%d \" NumberType=\"Float\" Precision=\"4\" Format=\"HDF\">\n",
                 ncube_total );
        MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
        sprintf( str, "          %s:/Q\n", fname );
        MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
        sprintf( str, "         </DataItem>\n" );
        MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
        sprintf( str, " </Attribute>\n" );
        MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
        sprintf( str, "  </Grid>\n" );
        MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
        sprintf( str, "      </Domain>   \n" );
        MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
        sprintf( str, "  </Xdmf>\n" );
        MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
    }

    // MPI_Barrier(MPI_COMM_WORLD);

    MPI_File_close( &fp );
}

//====================================================================
//               MultiBlock
//====================================================================

template <size_t N, typename Nvalue, size_t M, typename Mvalue, class T>
void templatePhdf5<N, Nvalue, M, Mvalue, T>::writeMultiBlock( TemplateForest<N, Nvalue, M, Mvalue, T> &F, uint appx )
{

    hid_t file_id, dset_id;    /* file and dataset identifiers */
    hid_t filespace, memspace; /* file and memory dataspace identifiers */
    hsize_t dimsf[4];          /* dataset dimensions */
    hsize_t chunk_dims[4];     /* chunk dimensions */
    hsize_t count[4];          /* hyperslab selection parameters */
    hsize_t block[4];
    hsize_t offset[4];
    hid_t plist_id; /* property list identifier */
    uint i, j, k, l;
    herr_t status;
    // int         *data=NULL;

    /*
     * MPI variables
     */
    int mpi_size, mpi_rank;
    real *xtemp = NULL;
    real *ytemp = NULL;
    real *ztemp = NULL;
    real *qtemp = NULL;

    uint L1 = F.npx;
    uint M1 = F.npy;
    uint N1 = F.npz;
    // cout<<L1<<"\t"<<M1<<"\t"<<N1<<endl;

    xtemp = new real[L1 * M1 * N1];
    ytemp = new real[L1 * M1 * N1];
    ztemp = new real[L1 * M1 * N1];
    qtemp = new real[(L1-1) * (M1-1) * (N1-1)];

    real Xa;
    real Xb;
    real Ya;
    real Yb;
    real Za;
    real Zb;

    real hx;
    real hy;
    real hz;
    real Xh;
    real Yh;
    real Zh;

    uint index;

    char str0[50];
    char str1[50];
    char str2[50];
    char str3[50];

    char str[50];

    sprintf( str, H5FILE_NAME, appx );

    uint partialforestsize = F.getTotalSize(); /*!<the forest size for each processor */

    CommPoint2Point<uint> com( &partialforestsize, 1 );
    uint offset1 = 0, totalvalue = 0;
    com.getOffset( partialforestsize, &offset1 );

    CommCollective<uint> comc( nullptr, 1 );
    comc.getTotalNumber( &offset1, &partialforestsize, &totalvalue );

    cout << " partial size " << partialforestsize << endl;

    hsize_t myoffset = offset1;
    //  total_size=totalvalue;

    totalnumber = totalvalue;

    cout << offset1 << endl;

    /*
       * Set up file access property list with parallel I/O access
       */

    plist_id = H5Pcreate( H5P_FILE_ACCESS );
    H5Pset_fapl_mpio( plist_id, MPI_COMM_WORLD, MPI_INFO_NULL );

    /*
     * Create a new file collectively and release property list identifier.
     */

    file_id = H5Fcreate( str, H5F_ACC_TRUNC, H5P_DEFAULT, plist_id );
    H5Pclose( plist_id );

    /*
     * Create the dataspace for the dataset.
     */
    dimsf[0] = L1;
    dimsf[1] = M1;
    dimsf[2] = N1;
    dimsf[3] = totalnumber;

    chunk_dims[0] = L1;
    chunk_dims[1] = M1;
    chunk_dims[2] = N1;
    chunk_dims[3] = 1;

    /*
     * Each process defines dataset in memory and writes it to the hyperslab
     * in the file.
     */

    count[0] = 1;
    count[1] = 1;
    count[2] = 1;
    count[3] = 1;

    block[0] = chunk_dims[0];
    block[1] = chunk_dims[1];
    block[2] = chunk_dims[2];
    block[3] = chunk_dims[3];

    /*
      for loop such that each processor can write all the blocks
    */
    offset[0] = 0;
    offset[1] = 0;
    offset[2] = 0;
    // offset[3] will be set at loop

    //================================================================================
    //
    //                               write X
    //
    //================================================================================
    // the first argument is the dimension which is 4

    filespace = H5Screate_simple( 4, dimsf, NULL );
    memspace = H5Screate_simple( 4, chunk_dims, NULL );

    sprintf( str0, "/X" );
    plist_id = H5Pcreate( H5P_DATASET_CREATE );
    H5Pset_chunk( plist_id, 4, chunk_dims );
    dset_id = H5Dcreate( file_id, str0, H5T_NATIVE_DOUBLE, filespace, H5P_DEFAULT, plist_id, H5P_DEFAULT );

    H5Pclose( plist_id );
    H5Sclose( filespace );

    filespace = H5Dget_space( dset_id );

    unsigned int level, coord_index;

    double dx, dy, dz;
    double denum;
    real XYZ[6];
    morton<N> key;
    hsize_t co = 0;

    for ( auto it = F.trees.begin(); it != F.trees.end(); it++ )
    {
        for ( auto it2 = ( *it ).begin(); it2 != ( *it ).end(); it2++ )
        {
            key = ( it2->first );
            ( *it ).enclosingBox( key, XYZ );

            //        proc.enclosingBox(key,XYZ);

            Xa = XYZ[0];
            Xb = XYZ[1];
            hx = L1 - 1.0;
            Xh = ( Xb - Xa ) / ( hx );

            index = 0;

            for ( uint j = 0; j < L1; j++ )
            {
                for ( uint k = 0; k < M1; k++ )
                {
                    for ( uint l = 0; l < N1; l++ )
                    {
                        xtemp[index] = Xa + Xh * j;
                        index++;
                    }
                }
            }
            // define the offset, only in the fourth dimension

            offset[3] = myoffset + co;
            // printf("my_rank=%d offset=%d\n",my_rank,offset[3]);
            // status = H5Sselect_hyperslab(filespace, H5S_SELECT_SET, offset, NULL, count, block);
            H5Sselect_hyperslab( filespace, H5S_SELECT_SET, offset, NULL, count, block );
            plist_id = H5Pcreate( H5P_DATASET_XFER );
            // H5Pset_dxpl_mpio(plist_id, H5FD_MPIO_COLLECTIVE);
            H5Pset_dxpl_mpio( plist_id, H5FD_MPIO_INDEPENDENT );
            // status = H5Dwrite(dset_id, H5T_NATIVE_DOUBLE, memspace,filespace,plist_id, xtemp);
            //
            if ( sizeof( real ) == sizeof( double ) )
            {
                H5Dwrite( dset_id, H5T_NATIVE_DOUBLE, memspace, filespace, plist_id, xtemp );
            }
            else if ( sizeof( real ) == sizeof( float ) )
            {

                H5Dwrite( dset_id, H5T_NATIVE_FLOAT, memspace, filespace, plist_id, xtemp );
            }

            H5Pclose( plist_id );
            co = co + 1;
        }
    }

    // need to close dset_id since the new variable is going to be Y and Z ...
    // no need to open and close memspace and filespace all the time, simply reuse them

    H5Dclose( dset_id );

    //================================================================================
    //
    //                               write Y
    //
    //================================================================================

    // filespace = H5Screate_simple(4, dimsf, NULL);
    //  memspace  = H5Screate_simple(4, chunk_dims, NULL);

    sprintf( str0, "/Y" );
    plist_id = H5Pcreate( H5P_DATASET_CREATE );
    H5Pset_chunk( plist_id, 4, chunk_dims );
    dset_id = H5Dcreate( file_id, str0, H5T_NATIVE_DOUBLE, filespace, H5P_DEFAULT, plist_id, H5P_DEFAULT );

    H5Pclose( plist_id );
    H5Sclose( filespace );

    filespace = H5Dget_space( dset_id );

    co = 0;

    for ( auto it = F.trees.begin(); it != F.trees.end(); it++ )
    {
        for ( auto it2 = ( *it ).begin(); it2 != ( *it ).end(); it2++ )
        {
            key = ( it2->first );
            ( *it ).enclosingBox( key, XYZ );

            //       proc.enclosingBox(key,XYZ);
            //       cout<<XYZ[0]<<" "<<XYZ[1]<<" "<<XYZ[2]<<" "<<XYZ[3]<<" "<<XYZ[4]<<" "<<XYZ[5]<<endl;

            Ya = XYZ[2];
            Yb = XYZ[3];
            hy = M1 - 1.0;
            Yh = ( Yb - Ya ) / ( hy );

            index = 0;

            for ( uint j = 0; j < L1; j++ )
            {
                for ( uint k = 0; k < M1; k++ )
                {
                    for ( uint l = 0; l < N1; l++ )
                    {
                        ytemp[index] = Ya + Yh * k;
                        index++;
                    }
                }
            }
            // define the offset, only in the fourth dimension

            offset[3] = myoffset + co;
            // printf("my_rank=%d offset=%d\n",my_rank,offset[3]);
            // status = H5Sselect_hyperslab(filespace, H5S_SELECT_SET, offset, NULL, count, block);
            H5Sselect_hyperslab( filespace, H5S_SELECT_SET, offset, NULL, count, block );
            plist_id = H5Pcreate( H5P_DATASET_XFER );
            // H5Pset_dxpl_mpio(plist_id, H5FD_MPIO_COLLECTIVE);
            H5Pset_dxpl_mpio( plist_id, H5FD_MPIO_INDEPENDENT );
            // status = H5Dwrite(dset_id, H5T_NATIVE_DOUBLE, memspace,filespace,plist_id, xtemp);
            if ( sizeof( real ) == sizeof( double ) )
            {
                H5Dwrite( dset_id, H5T_NATIVE_DOUBLE, memspace, filespace, plist_id, ytemp );
            }
            else if ( sizeof( real ) == sizeof( float ) )
            {

                H5Dwrite( dset_id, H5T_NATIVE_FLOAT, memspace, filespace, plist_id, ytemp );
            }

            //      H5Dwrite(dset_id, H5T_NATIVE_DOUBLE, memspace,filespace,plist_id, ytemp);
            H5Pclose( plist_id );
            co = co + 1;
        }
    }

    H5Dclose( dset_id );
    //================================================================================
    //
    //                               write Z
    //
    //================================================================================

    // filespace = H5Screate_simple(4, dimsf, NULL);
    //  memspace  = H5Screate_simple(4, chunk_dims, NULL);

    sprintf( str0, "/Z" );
    plist_id = H5Pcreate( H5P_DATASET_CREATE );
    H5Pset_chunk( plist_id, 4, chunk_dims );
    dset_id = H5Dcreate( file_id, str0, H5T_NATIVE_DOUBLE, filespace, H5P_DEFAULT, plist_id, H5P_DEFAULT );

    H5Pclose( plist_id );
    H5Sclose( filespace );

    filespace = H5Dget_space( dset_id );

    co = 0;

    for ( auto it = F.trees.begin(); it != F.trees.end(); it++ )
    {
        for ( auto it2 = ( *it ).begin(); it2 != ( *it ).end(); it2++ )
        {
            key = ( it2->first );
            ( *it ).enclosingBox( key, XYZ );

            // proc.enclosingBox(key,XYZ);
            Za = XYZ[4];
            Zb = XYZ[5];
            hz = N1 - 1.0;
            Zh = ( Zb - Za ) / ( hz );

            index = 0;

            for ( uint j = 0; j < L1; j++ )
            {
                for ( uint k = 0; k < M1; k++ )
                {
                    for ( uint l = 0; l < N1; l++ )
                    {
                        ztemp[index] = Za + Zh * l;
                        index++;
                    }
                }
            }
            // define the offset, only in the fourth dimension

            offset[3] = myoffset + co;
            // printf("my_rank=%d offset=%d\n",my_rank,offset[3]);
            // status = H5Sselect_hyperslab(filespace, H5S_SELECT_SET, offset, NULL, count, block);
            H5Sselect_hyperslab( filespace, H5S_SELECT_SET, offset, NULL, count, block );
            plist_id = H5Pcreate( H5P_DATASET_XFER );
            // H5Pset_dxpl_mpio(plist_id, H5FD_MPIO_COLLECTIVE);
            H5Pset_dxpl_mpio( plist_id, H5FD_MPIO_INDEPENDENT );
            // status = H5Dwrite(dset_id, H5T_NATIVE_DOUBLE, memspace,filespace,plist_id, xtemp);
            if ( sizeof( real ) == sizeof( double ) )
            {
                H5Dwrite( dset_id, H5T_NATIVE_DOUBLE, memspace, filespace, plist_id, ztemp );
            }
            else if ( sizeof( real ) == sizeof( float ) )
            {
                H5Dwrite( dset_id, H5T_NATIVE_FLOAT, memspace, filespace, plist_id, ztemp );
            }

            //      H5Dwrite(dset_id, H5T_NATIVE_DOUBLE, memspace,filespace,plist_id, ztemp);
            H5Pclose( plist_id );
            co = co + 1;
        }
    }

    H5Dclose( dset_id );

    dimsf[0] = L1-1;
    dimsf[1] = M1-1;
    dimsf[2] = N1-1;
    dimsf[3] = totalnumber;

    chunk_dims[0] = L1-1;
    chunk_dims[1] = M1-1;
    chunk_dims[2] = N1-1;
    chunk_dims[3] = 1;

    /*
     * Each process defines dataset in memory and writes it to the hyperslab
     * in the file.
     */

    count[0] = 1;
    count[1] = 1;
    count[2] = 1;
    count[3] = 1;

    block[0] = chunk_dims[0];
    block[1] = chunk_dims[1];
    block[2] = chunk_dims[2];
    block[3] = chunk_dims[3];

    /*
      for loop such that each processor can write all the blocks
    */
    offset[0] = 0;
    offset[1] = 0;
    offset[2] = 0;
    // offset[3] will be set at loop
/*
     for(int i=0;i<4;i++)
     cout<<chunk_dims[i]<<" "<<endl;
     for(int i=0;i<4;i++)
     cout<<dimsf[i]<<" "<<endl;
*/
    //================================================================================
    //
    //                               write Q
    //
    //================================================================================
      filespace = H5Screate_simple(4, dimsf, NULL);
      memspace  = H5Screate_simple(4, chunk_dims, NULL);

    sprintf( str0, "/Q" );
    plist_id = H5Pcreate( H5P_DATASET_CREATE );
    H5Pset_chunk( plist_id, 4, chunk_dims );
    dset_id = H5Dcreate( file_id, str0, H5T_NATIVE_DOUBLE, filespace, H5P_DEFAULT, plist_id, H5P_DEFAULT );
#if(1)

    H5Pclose( plist_id );
    H5Sclose( filespace );

    filespace = H5Dget_space( dset_id );

    co = 0;

    for ( auto it = F.trees.begin(); it != F.trees.end(); it++ )
    {
        for ( auto it2 = ( *it ).begin(); it2 != ( *it ).end(); it2++ )
        {
            key = ( it2->first );
            ( *it ).enclosingBox( key, XYZ );

            // proc.enclosingBox(key,XYZ);
            index = 0;

      for ( uint l = 1; l < N1; l++ )
                    {         
               for ( uint k = 1; k < M1; k++ )
                {
 				
            for ( uint j = 1; j < L1; j++ )
            {
              	      qtemp[index] = (it2->second[(N1+1)*(M1+1)*j+(N1+1)*k+l]).p;
              	   //  qtemp[index] = 1.0;
              	      //cout<< (it2->second[(N1+1)*(M1+1)*j+(N1+1)*k+l]).p<<endl;
                      //cout<<qtemp[index]<<endl;   
                      //  qtemp[index]=1.0;
                      // cout<<index<<" "<<qtemp[index]<<endl;
                        //index = (N1+2)*(M1+2)*j+(N1+2)*k+l+1;
//                        qtemp[index]=my_rank;
                        index++;
                    }
                }
            }
            // define the offset, only in the fourth dimension

            offset[3] = myoffset + co;
//             printf("my_rank=%d offset=%d\n",offset[3]);
            // status = H5Sselect_hyperslab(filespace, H5S_SELECT_SET, offset, NULL, count, block);
            H5Sselect_hyperslab( filespace, H5S_SELECT_SET, offset, NULL, count, block );
            plist_id = H5Pcreate( H5P_DATASET_XFER );
            // H5Pset_dxpl_mpio(plist_id, H5FD_MPIO_COLLECTIVE);
            H5Pset_dxpl_mpio( plist_id, H5FD_MPIO_INDEPENDENT );
            // status = H5Dwrite(dset_id, H5T_NATIVE_DOUBLE, memspace,filespace,plist_id, xtemp);
            if ( sizeof( real ) == sizeof( double ) )
            {
                H5Dwrite( dset_id, H5T_NATIVE_DOUBLE, memspace, filespace, plist_id, qtemp );
            }
            else if ( sizeof( real ) == sizeof( float ) )
            {

                H5Dwrite( dset_id, H5T_NATIVE_FLOAT, memspace, filespace, plist_id, qtemp );
            }

            H5Pclose( plist_id );
            co = co + 1;
        }
    }

    H5Dclose( dset_id );

    xdmfMultiBlock( F, com.mySize(), com.myRank(), offset1, appx );

#endif
    delete[] xtemp;
    delete[] ytemp;
    delete[] ztemp;
    delete[] qtemp;

#if ( 1 )
#endif
}

static void integer_string( char *strin, int i )
{

    if ( i < 10 )
    {
        sprintf( strin, "00000000000%d", i );
    }
    else if ( i < 100 )
    {
        sprintf( strin, "0000000000%d", i );
    }
    else if ( i < 1000 )
    {
        sprintf( strin, "000000000%d", i );
    }
    else if ( i < 10000 )
    {
        sprintf( strin, "00000000%d", i );
    }
    else if ( i < 100000 )
    {
        sprintf( strin, "0000000%d", i );
    }
    else if ( i < 1000000 )
    {
        sprintf( strin, "000000%d", i );
    }
    else if ( i < 10000000 )
    {
        sprintf( strin, "00000%d", i );
    }
    else if ( i < 100000000 )
    {
        sprintf( strin, "0000%d", i );
    }
    else if ( i < 1000000000 )
    {
        sprintf( strin, "000%d", i );
    }
    else if ( i < 10000000000 )
    {
        sprintf( strin, "00%d", i );
    }
    else if ( i < 100000000000 )
    {
        sprintf( strin, "0%d", i );
    }
    else if ( i < 1000000000000 )
    {
        sprintf( strin, "%d", i );
    }

    else
    {
        printf( "number of cubes larger than 10^12\n" );
        exit( 0 );
    }

    /*
          if(i<10)
            {
          sprintf(strin,"00000%d",i);
        }
          else if(i<100)
            {
          sprintf(strin,"0000%d",i);
        }
          else if(i<1000)
            {
          sprintf(strin,"000%d",i);
        }

          else if(i<10000)
            {
          sprintf(strin,"00%d",i);
        }

          else if(i<100000)
            {
          sprintf(strin,"0%d",i);
        }
          else if(i<1000000)
            {
          sprintf(strin,"%d",i);
        }
          else
            {
          printf("not in the range, offset is too big\n");
          exit(0);
        }
    */
}

//#define offset0 156
//#define offset1 1292 //for L=5
//#define offset1 1861
//#define offset1 2005
// if you have a time dependent that goes beyond 10 need to replace this

template <size_t N, typename Nvalue, size_t M, typename Mvalue, class T>
void templatePhdf5<N, Nvalue, M, Mvalue, T>::xdmfMultiBlock( TemplateForest<N, Nvalue, M, Mvalue, T> &F, integer comsize, integer my_rank,
                                                             uint offset, uint appx )
{
    MPI_File fp;
    int buf[1000], np = comsize;
    MPI_Request request;
    unsigned int i;
    int j;

    MPI_Status status;
    const char *names[] = {"X", "Y", "Z"};

    uint L1 = F.npx;
    uint M1 = F.npy;
    uint N1 = F.npz;

    const int offset0 = 156;
    // const int offset1=2005;
   // const int offset1 = 2044;
    const int offset1 = 2053;

    char strname[1000], fname[1000];
    uint ncube_total = totalnumber;
    sprintf( strname, XDMF_NAME, appx );
    char str[1000];

    //cout << "size str" << strlen( strname ) << endl;

    MPI_File_open( MPI_COMM_WORLD, strname, MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fp );

    // printf("%s\n",str);
    // dont need to specify the address here as they are both reside in one folder, that is
    // why I use H5FILE in addition to H5FILE_NAME
    sprintf( fname, H5FILE, appx );

    char strL[200];
    char strM[200];
    char strN[200];
    char strNcube[1000];
    char stroff[1000];
    int index;

    char strL1[200];
    char strM1[200];
    char strN1[200];
 
    if ( L1 < 10 )
    {
        sprintf( strL, "00%d", L1 );
        sprintf( strM, "00%d", M1 );
        sprintf( strN, "00%d", N1 );
    }
    else if ( L1 < 100 )
    {
        sprintf( strL, "0%d", L1 );
        sprintf( strM, "0%d", M1 );
        sprintf( strN, "0%d", N1 );
    }
    else
    {
        printf( "discretization too big go change xdmf function\n" );
        exit( 0 );
    }


    if ( L1 < 10 )
    {
        sprintf( strL1, "00%d", L1-1 );
        sprintf( strM1, "00%d", M1-1 );
        sprintf( strN1, "00%d", N1-1 );
    }
    else if ( L1 < 100 )
    {
        sprintf( strL1, "0%d", L1-1 );
        sprintf( strM1, "0%d", M1-1 );
        sprintf( strN1, "0%d", N1-1 );
    }
    else
    {
        printf( "discretization too big go change xdmf function\n" );
        exit( 0 );
    }


    /* number of cubes limited to less than a million here upgrade to a trillion
      if(ncube_total<10)
        {
          sprintf(strNcube,"00000%d",ncube_total);
        }
      else if(ncube_total<100)
        {
          sprintf(strNcube,"0000%d",ncube_total);
        }
      else if(ncube_total<1000)
        {
          sprintf(strNcube,"000%d",ncube_total);
        }
      else if(ncube_total<10000)
        {
          sprintf(strNcube,"00%d",ncube_total);
        }
      else  if(ncube_total<100000)
        {
          sprintf(strNcube,"0%d",ncube_total);
        }
      else if(ncube_total<1000000)
        {
          sprintf(strNcube,"%d",ncube_total);
        }
      else
        {
          printf("number of cubes larger than 1000000\n");
          exit(0);
        }
    */

    if ( ncube_total < 10 )
    {
        sprintf( strNcube, "00000000000%d", ncube_total );
    }
    else if ( ncube_total < 100 )
    {
        sprintf( strNcube, "0000000000%d", ncube_total );
    }
    else if ( ncube_total < 1000 )
    {
        sprintf( strNcube, "000000000%d", ncube_total );
    }
    else if ( ncube_total < 10000 )
    {
        sprintf( strNcube, "00000000%d", ncube_total );
    }
    else if ( ncube_total < 100000 )
    {
        sprintf( strNcube, "0000000%d", ncube_total );
    }
    else if ( ncube_total < 1000000 )
    {
        sprintf( strNcube, "000000%d", ncube_total );
    }
    else if ( ncube_total < 10000000 )
    {
        sprintf( strNcube, "00000%d", ncube_total );
    }
    else if ( ncube_total < 100000000 )
    {
        sprintf( strNcube, "0000%d", ncube_total );
    }
    else if ( ncube_total < 1000000000 )
    {
        sprintf( strNcube, "000%d", ncube_total );
    }
    else if ( ncube_total < 10000000000 )
    {
        sprintf( strNcube, "00%d", ncube_total );
    }
    else if ( ncube_total < 100000000000 )
    {
        sprintf( strNcube, "0%d", ncube_total );
    }
    else if ( ncube_total < 1000000000000 )
    {
        sprintf( strNcube, "%d", ncube_total );
    }

    else
    {
        printf( "number of cubes larger than 10^12\n" );
        exit( 0 );
    }

#if(DEBUG)
    cout << ncube_total << endl;
#endif
    uint co = 0;

    // a counts the offset for header whic is only written by process rank 0 and  and b the hyperslab part for each cube
    int a = 0, b = 0;
    if ( my_rank == 0 )
    {
        sprintf( str, "<?xml version=\"1.0\" ?>\n" );
        MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
        a = a + strlen( str );
        sprintf( str, "<!DOCTYPE Xdmf SYSTEM \"Xdmf.dtd\" []> \n" );
        MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
        a = a + strlen( str );
        sprintf( str, "<Xdmf Version=\"2.0\">\n" );
        MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
        a = a + strlen( str );
        sprintf( str, "<Domain>\n" );
        MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
        a = a + strlen( str );
        sprintf( str, "<Grid Name=\"AMR\" GridType=\"Collection\" CollectionType=\"Spatial\">\n" );
        MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
        a = a + strlen( str );
#if(DEBUG)
        printf( "size a=%d\n", a );
#endif
// for(unsigned int i=0;i<cube.size();i++)
#if ( 1 )

        co = 0;
        for ( auto it = F.trees.begin(); it != F.trees.end(); it++ )
        {
            for ( auto it2 = ( *it ).begin(); it2 != ( *it ).end(); it2++ )
            {

                // b calculates the offset for each block, need to be set to zero becasue it is inside the loop
                // but we dont want to accumulate, calculate only for one block
                b = 0;
                sprintf( str, "   <Grid Name=\"mesh0\" GridType=\"Uniform\">\n" );
                MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                b = b + strlen( str );
                sprintf( str, "        <Topology TopologyType=\"3DSMesh\" NumberOfElements=\"%s %s %s\"/>\n", strL, strM, strN );
                // printf("hyperslab =%d\n",strlen(str));
                MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                b = b + strlen( str );
                sprintf( str, "          <Geometry GeometryType=\"X_Y_Z\">  \n" );
                MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                b = b + strlen( str );

                for ( j = 0; j < 3; j++ )
                {
                    sprintf( str, "      <DataItem ItemType=\"HyperSlab\" Dimensions=\"%s %s %s %d\" NumberType=\"Float\" Precision=\"4\"  "
                                  "Type=\"HyperSlab\"> \n",
                             strL, strM, strN, 1 );
                    MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                    b = b + strlen( str );
                    sprintf( str, "         <DataItem Dimensions=\"3 4\" Format=\"XML\" > \n" );
                    MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                    b = b + strlen( str );
                    index = offset + co;
                    integer_string( stroff, index );
                    sprintf( str, "         %d %d %d %s  \n", 0, 0, 0, stroff );
                    MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                    b = b + strlen( str );
                    sprintf( str, "         %d %d %d %d  \n", 1, 1, 1, 1 );
                    MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                    b = b + strlen( str );
                    sprintf( str, "         %s %s %s %d  \n", strL, strM, strN, 1 );
                    MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                    b = b + strlen( str );
                    sprintf( str, "         </DataItem>\n" );
                    MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                    b = b + strlen( str );
                    sprintf(
                    str,
                    "          <DataItem Name=\"%s\" Dimensions=\"%s %s %s %s\" NumberType=\"Float\" Precision=\"4\" Format=\"HDF\">\n",
                    names[j], strL, strM, strN, strNcube );
                    MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                    b = b + strlen( str );
                    sprintf( str, "          %s:/%s\n", fname, names[j] );
                    MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                    b = b + strlen( str );
                    sprintf( str, "         </DataItem>\n" );
                    MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                    b = b + strlen( str );
                    sprintf( str, "         </DataItem>\n" );
                    MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                    b = b + strlen( str );
                }
                sprintf( str, "      </Geometry>   \n" );
                MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                b = b + strlen( str );
                // added because of including solution vector Q
                sprintf( str, "         <Attribute Name=\"Q\" AttributeType=\"Scalar\" Center=\"Cell\"> \n" );
                MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                b = b + strlen( str );
                sprintf( str, "      <DataItem ItemType=\"HyperSlab\" Dimensions=\"%s %s %s %d\" NumberType=\"Float\" Precision=\"4\"  "
                              "Type=\"HyperSlab\"> \n",
                         strL1, strM1, strN1, 1 );
                MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                b = b + strlen( str );
                sprintf( str, "         <DataItem Dimensions=\"3 4\" Format=\"XML\" > \n" );
                MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                b = b + strlen( str );
                index = offset + co;
                integer_string( stroff, index );
                sprintf( str, "         %d %d %d %s  \n", 0, 0, 0, stroff );
                MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                b = b + strlen( str );
                sprintf( str, "         %d %d %d %d  \n", 1, 1, 1, 1 );
                MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                b = b + strlen( str );
                sprintf( str, "         %s %s %s %d  \n", strL1, strM1, strN1, 1 );
                MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                b = b + strlen( str );
                sprintf( str, "         </DataItem>\n" );
                MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                b = b + strlen( str );
                sprintf( str,
                         "          <DataItem Name=\"Q\" Dimensions=\"%s %s %s %s\" NumberType=\"Float\" Precision=\"4\" Format=\"HDF\">\n",
                         strL1, strM1, strN1, strNcube );
                MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                b = b + strlen( str );
                sprintf( str, "          %s:/Q\n", fname );
                MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                b = b + strlen( str );
                sprintf( str, "         </DataItem>\n" );
                MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                b = b + strlen( str );
                sprintf( str, "         </DataItem>\n" );
                MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                b = b + strlen( str );
                sprintf( str, " </Attribute>\n" );
                MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                b = b + strlen( str );
                sprintf( str, "  </Grid>\n" );
                MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                b = b + strlen( str );
                co++;
                // put error here
                if ( a != offset0 || b != offset1 )
                {
                    printf( "??????????????????????????????????????????\n" );
                    printf( "Go fix your offset for Xdmf meta data file a=%d offset0=%d b=%d offset1=%d\n", a, offset0, b, offset1 );
                    printf( "??????????????????????????????????????????\n" );
                    exit( 0 );
                }
                // printf("size length9=%d length11=%d\n",strlen("9"),strlen("11"));
            }
        }
#endif
    }
// made here a bit more conservative, originally there was an else here, but we arenot allowed to put a barried right before else
// 
     MPI_Barrier(MPI_COMM_WORLD);

   if(my_rank!=0) 
    {
// other ranks do something else
// 5 is added because of the header of the file written by  processor 0
#if ( 1 )

        int mpi_offset = 0;
//               mpi_offset = ( ( offset  ) * offset1 + offset0 );
//                MPI_File_seek( fp, mpi_offset, MPI_SEEK_SET );
        co = 0;
        for ( auto it = F.trees.begin(); it != F.trees.end(); it++ )
        {

            for ( auto it2 = ( *it ).begin(); it2 != ( *it ).end(); it2++ )
            {

                mpi_offset = ( ( offset + co ) * offset1 + offset0 );
                mpi_offset = mpi_offset * sizeof( char );
 //               printf("my_rank = %d offset =%d mpi_offset=%d i=%d\n",my_rank,offset,mpi_offset,i);

                MPI_File_seek( fp, mpi_offset, MPI_SEEK_SET );

               //  MPI_File_set_view(fp,mpi_offset,MPI_CHAR,MPI_CHAR, "native", MPI_INFO_NULL );
                sprintf( str, "   <Grid Name=\"mesh0\" GridType=\"Uniform\">\n" );
                MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                sprintf( str, "        <Topology TopologyType=\"3DSMesh\" NumberOfElements=\"%s %s %s\"/>\n", strL, strM, strN );
                MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                sprintf( str, "          <Geometry GeometryType=\"X_Y_Z\">  \n" );
                MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );

                for ( j = 0; j < 3; j++ )
                {
                    sprintf( str, "      <DataItem ItemType=\"HyperSlab\" Dimensions=\"%s %s %s %d\" NumberType=\"Float\" Precision=\"4\"  "
                                  "Type=\"HyperSlab\"> \n",
                             strL, strM, strN, 1 );
                    MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                    sprintf( str, "         <DataItem Dimensions=\"3 4\" Format=\"XML\" > \n" );
                    MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                    index = offset + co;
                    integer_string( stroff, index );
                    sprintf( str, "         %d %d %d %s  \n", 0, 0, 0, stroff );
                    MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                    sprintf( str, "         %d %d %d %d  \n", 1, 1, 1, 1 );
                    MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                    sprintf( str, "         %s %s %s %d  \n", strL, strM, strN, 1 );
                    MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                    sprintf( str, "         </DataItem>\n" );
                    MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                    sprintf(
                    str,
                    "          <DataItem Name=\"%s\" Dimensions=\"%s %s %s %s\" NumberType=\"Float\" Precision=\"4\" Format=\"HDF\">\n",
                    names[j], strL, strM, strN, strNcube );
                    MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                    sprintf( str, "          %s:/%s\n", fname, names[j] );
                    MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                    sprintf( str, "         </DataItem>\n" );
                    MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                    sprintf( str, "         </DataItem>\n" );
                    MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                }
                sprintf( str, "      </Geometry>   \n" );
                MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                // insert here the value of qs

                sprintf( str, "         <Attribute Name=\"Q\" AttributeType=\"Scalar\" Center=\"Cell\"> \n" );
                MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                sprintf( str, "      <DataItem ItemType=\"HyperSlab\" Dimensions=\"%s %s %s %d\" NumberType=\"Float\" Precision=\"4\"  "
                              "Type=\"HyperSlab\"> \n",
                         strL1, strM1, strN1, 1 );
                MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                sprintf( str, "         <DataItem Dimensions=\"3 4\" Format=\"XML\" > \n" );
                MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                index = offset + co;
                integer_string( stroff, index );
                sprintf( str, "         %d %d %d %s  \n", 0, 0, 0, stroff );
                MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                sprintf( str, "         %d %d %d %d  \n", 1, 1, 1, 1 );
                MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                sprintf( str, "         %s %s %s %d  \n", strL1, strM1, strN1, 1 );
                MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                sprintf( str, "         </DataItem>\n" );
                MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                sprintf( str, "          <DataItem Name=\"Q\" Dimensions=\"%s %s %s %s\" NumberType=\"Float\" Precision=\"4\" Format=\"HDF\">\n", strL1, strM1, strN1, strNcube );
                MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                sprintf( str, "          %s:/Q\n", fname );
                MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                sprintf( str, "         </DataItem>\n" );
                MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                sprintf( str, "         </DataItem>\n" );
                MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                sprintf( str, " </Attribute>\n" );
                MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                sprintf( str, "  </Grid>\n" );
                MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
                co++;
            }
            // add the last part of the xdmf text file to close the arguments
        }

#endif
    }

    MPI_Barrier( MPI_COMM_WORLD );
/*
    MPI_File_sync(fp);
    MPI_File_close( &fp );

    MPI_File_open( MPI_COMM_WORLD, strname, MPI_MODE_APPEND | MPI_MODE_WRONLY, MPI_INFO_NULL, &fp );

    //MPI_File_sync(fp);
  */  
    MPI_File_sync(fp);
    if ( my_rank == ( np - 1 ) )
    {
      //  MPI_File_seek( fp, 0, MPI_SEEK_END );
        sprintf( str, "  </Grid>\n" );
        MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
        sprintf( str, "      </Domain>   \n" );
        MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
        sprintf( str, "  </Xdmf>\n" );
        MPI_File_write( fp, str, strlen( str ), MPI_CHAR, &status );
    }

// MPI3.0 Standard :  For writes, however, the MPI_FILE_SYNC routine provides the only guarantee
// that data has been transferred to the storage device, alhough MPI_File_close()
// implicitly calls this function, we experienced some problems on stampede with intel version of MPI
// we decided to explicitly enforce this operation


    MPI_File_sync(fp);
    MPI_File_close( &fp );
}



//template class templatePhdf5<TREESIZE, real, PROCSIZE, uint, Tree<PROCSIZE, uint>>;
template class templatePhdf5<TREESIZE, Q, PROCSIZE, uint, Tree<PROCSIZE, uint>>;
//template class templatePhdf5<TREESIZE, real, WSIZE, uint, FullTree<WSIZE, uint>>;
//template class templatePhdf5<TREESIZE, Q, WSIZE, uint, FullTree<WSIZE, uint>>;
//template class templatePhdf5<TREESIZE, real, PROCSIZE, uint, FullTree<PROCSIZE, uint>>;

