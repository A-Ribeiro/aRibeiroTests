#ifndef additional_algorithms__h__
#define additional_algorithms__h__
//#include <boost/sort/spreadsort/integer_sort.hpp>
//boost::sort::spreadsort::integer_sort(&job.in[job.normal_sorting_begin],&job.in[job.normal_sorting_end]);

#include <aRibeiroCore/aRibeiroCore.h>
using namespace aRibeiro;

//
// Cormen Recursive Merge Sort Implementation
//
static void Cormen_MergeStep(uint32_t *A, int p, int q, int r, uint32_t* pre_alloc_tmp) {
    int n1 = q-p+1;//p->q
    int n2 = r-q;//q->r

    //uint32_t* L = (uint32_t*)malloc_aligned( (n1+1)*sizeof(uint32_t) );
    //uint32_t* R = (uint32_t*)malloc_aligned( (n2+1)*sizeof(uint32_t) );

    uint32_t* L = pre_alloc_tmp;
    uint32_t* R = pre_alloc_tmp + n1 + 1;

    //for(int i=0;i<n1;i++) L[i] = A[p+i];
    //for(int j=0;j<n2;j++) R[i] = A[q+j+1];

    memcpy(L,A+p,n1*sizeof(uint32_t));
    memcpy(R,A+q+1,n2*sizeof(uint32_t));

    L[n1] = UINT32_MAX;
    R[n2] = UINT32_MAX;

    int i=0;
    int j=0;

    for(int k=p;k<=r;k++){
        if ( L[i]<=R[j] ){
            A[k] = L[i];
            i++;
        } else {
            A[k] = R[j];
            j++;
        }
    }

    //free_aligned(L);
    //free_aligned(R);
}

static void Cormen_MergeRecursiveStep(uint32_t*_array, int p, int r, uint32_t* pre_alloc_tmp) {

    if (p<r){
        int q = (p+r) >> 1;
        Cormen_MergeRecursiveStep(_array, p, q, pre_alloc_tmp);
        Cormen_MergeRecursiveStep(_array, q+1, r, pre_alloc_tmp);
        Cormen_MergeStep(_array, p, q, r, pre_alloc_tmp);
    }

}

static void Cormen_Recursive_MergeSort(uint32_t*_array, int size, uint32_t* pre_alloc_tmp) {
    int p = 0;
    int r = size-1;
    Cormen_MergeRecursiveStep(_array, p, r, pre_alloc_tmp);
}

//
// Non-Recursive Merge Sort Implementation
//
template<typename T>
static void NonRecursive_MergeSort(T*_array, int size, T*pre_alloc_tmp = NULL) {
    T *_aux;
    if (pre_alloc_tmp == NULL)
        _aux = (T *)malloc_aligned( sizeof(T)*size );
    else
        _aux = pre_alloc_tmp;

    T *in = _array;
    T *out = _aux;

    int element_count = 1;

    while (element_count < size){

        //#pragma omp parallel for
        for(int i=0;i<size;i+=element_count<<1){
            int write_index = i;
            int write_max = i + (element_count<<1);
            if (write_max > size)
                write_max = size;

            int a_index = i;
            int b_index = i+element_count;

            int a_max = b_index;
            int b_max = b_index + element_count;

            if (a_max > size)
                a_max = size;
            if (b_max > size)
                b_max = size;

            while (write_index < write_max && 
                a_index < a_max && 
                b_index < b_max) {

                const T& _a = in[a_index];
                const T& _b = in[b_index];

                if (_a > _b){
                    out[write_index] = _b;
                    b_index++;
                } else {
                    out[write_index] = _a;
                    a_index++;
                }
                
                write_index++;
            }

            while (a_index<a_max) {
                out[write_index++] = in[a_index++];
            }
            while (b_index<b_max) {
                out[write_index++] = in[b_index++];
            }
        }

        //swap in/out
        T* aux = in;
        in = out;
        out = aux;

        element_count = element_count << 1;
    }

    if (in != _array)
        memcpy(_array, in, sizeof(T)*size);
    
    if (pre_alloc_tmp == NULL)
        free_aligned(_aux);
}


template<typename T>
static void NonRecursive_MergeSort2(T* _array, int size, T* pre_alloc_tmp = NULL) {
    T* _aux;
    if (pre_alloc_tmp == NULL)
        _aux = (T*)malloc_aligned(sizeof(T) * size);
    else
        _aux = pre_alloc_tmp;

    int element_count = 1;

    while (element_count < size) {

        //#pragma omp parallel for
        for (int i = 0; i < size; i += element_count << 1) {
            int write_index = i;
            int write_max = i + (element_count << 1);
            if (write_max > size)
                write_max = size;

            int a_index = i;
            int b_index = i + element_count;

            int a_max = b_index;
            int b_max = b_index + element_count;

            if (a_max > size)
                a_max = size;
            if (b_max > size)
                b_max = size;

            T* arr_a = _aux;
            T* arr_b = _aux + (a_max - a_index);
            
            if ((a_max - a_index) > 0)
                memcpy(arr_a, _array + a_index, (a_max - a_index) * sizeof(T));
            if ((b_max - b_index)>0)
                memcpy(arr_b, _array + b_index, (b_max - b_index) * sizeof(T));

            a_max = (a_max - a_index);
            b_max = (b_max - b_index);

            a_index = 0;
            b_index = 0;

            while (write_index < write_max &&
                a_index < a_max &&
                b_index < b_max) {

                const T& _a = arr_a[a_index];
                const T& _b = arr_b[b_index];

                if (_a > _b) {
                    _array[write_index] = _b;
                    b_index++;
                }
                else {
                    _array[write_index] = _a;
                    a_index++;
                }

                write_index++;
            }

            while (a_index < a_max) {
                _array[write_index++] = arr_a[a_index++];
            }
            while (b_index < b_max) {
                _array[write_index++] = arr_b[b_index++];
            }
        }

        element_count = element_count << 1;
    }

    if (pre_alloc_tmp == NULL)
        free_aligned(_aux);
}


#endif